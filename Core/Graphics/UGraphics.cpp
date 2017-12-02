/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UGRAPHICS_CPP
#define UGRAPHICS_CPP

#include <cmath>
#include <cstdlib>
#include <string.h>
//#include <list>
#include "UGraphics.h"
#include "UFont.h"

#ifndef M_PI
#define M_PI 3.1416
#endif

#define UG_EDGE_CODE_NONE 0
#define UG_EDGE_CODE_LEFT 1
#define UG_EDGE_CODE_RIGHT 2
#define UG_EDGE_CODE_BOTTOM 4
#define UG_EDGE_CODE_TOP 8

namespace RDK {

int round(double number)
{
 return (number < 0.0) ? ceil(number - 0.5) : floor(number + 0.5);
}

// Процедура вычисления кодов для точки(конца/начала отрезка)
int CompOutCode(const UBPoint &point, const UBRect &rect)
{
 int code=UG_EDGE_CODE_NONE;
 if(point.Y < rect.Y2())
  code=UG_EDGE_CODE_TOP;
 else
 if(point.Y > rect.Y1())
  code=UG_EDGE_CODE_BOTTOM;
 if(point.X > rect.X2())
  code=code + UG_EDGE_CODE_RIGHT;
 else
 if(point.X < rect.X1())
  code = code + UG_EDGE_CODE_LEFT;
 return code;
}

// Процедура отсечения отрезка
// взято отсюда: http://grafika.me/node/694
void ClipLine(const UBPoint &point1, const UBPoint &point2, const UBRect &rect, UBPoint &out_point1, UBPoint &out_point2)
{
 int outcodeOut=UG_EDGE_CODE_NONE;
 bool accept = false;
 bool done = false;
 int x0(point1.X), y0(point1.Y), x1(point2.X), y1(point2.Y);
 double x,y;
 int counter=100000;

 out_point1=point1;
 out_point2=point2;

 int outcode0=CompOutCode (point1,rect);
 int outcode1=CompOutCode (point2,rect);
 do
 {
  if((outcode0==UG_EDGE_CODE_NONE) && (outcode1==UG_EDGE_CODE_NONE))
  {
   // Отрезок целиком лежит внутри окна
   accept = true; done=true;
  }
  else
  if( (outcode0*outcode1) != UG_EDGE_CODE_NONE)
   //Отрезок лежит за пределами окна и не будет отрисован
   done = true;
  else // Часть отрезка лежит внутри прямоугольника
  {
   if(outcode0 != UG_EDGE_CODE_NONE) // Если начальная точка лежит вне прямоугольника
	outcodeOut = outcode0;
   else
    outcodeOut = outcode1;

   // Найдём точку пересечения отрезка с границей прямоугольника
   if(UG_EDGE_CODE_TOP & outcodeOut)
   {
	x = x0 + (x1 - x0) * (rect.Y2() - y0) / (y1 - y0);
	y = rect.Y1();
   }

   if(UG_EDGE_CODE_BOTTOM & outcodeOut)
   {
	x = x0 + (x1 - x0) * (rect.Y1() - y0) / (y1 - y0);
	y = rect.Y2();
   }
   else
   if(UG_EDGE_CODE_RIGHT & outcodeOut)
   {
	y = y0 + (y1 - y0) * (rect.X2() - x0) / (x1 - x0);
	x = rect.X2();
   }
   else
   if(UG_EDGE_CODE_LEFT & outcodeOut)
   {
	y = y0 + (y1 - y0) * (rect.X1() - x0) / (x1 - x0);
	x = rect.X1();
   }

   // Переместили внешнюю точку в точку пересечения
   if (outcodeOut == outcode0)
   {
	x0 = round(x); y0 = round(y);
	outcode0=CompOutCode(UBPoint(x0,y0),rect);
   }
   else
   {
	x1 = round(x);
	y1 = round(y);
	outcode1=CompOutCode(UBPoint(x1,y1),rect);
   }
  }
  --counter;
 } while(!done || counter == 0);

 if(accept)  // Рисуем видимую часть отрезка
 {
  out_point1.X=x0;
  out_point1.Y=y0;
  out_point2.X=x1;
  out_point2.Y=y1;
 }
}

using namespace std;

/* ***************************************************************************
 Реализация UGraphics
*************************************************************************** */

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UGraphics::UGraphics(void)
{
 Canvas=0;
 Font=0;
 PenColor.c=0;
 PenWidth=1;
 HalfPenWidth=0;
 PenX=PenY=0;
}

UGraphics::UGraphics(UBitmap *canvas)
{
 Canvas=canvas;
 PenColor.c=0;
 PenWidth=1;
 HalfPenWidth=0;
 PenX=PenY=0;
}

UGraphics::~UGraphics(void)
{
}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает текущую канву рисования
UBitmap* UGraphics::GetCanvas(void)
{
 return Canvas;
}

// Задает канву рисования
bool UGraphics::SetCanvas(UBitmap *canvas)
{
 if(!canvas)
  return false;

 Canvas=canvas;
 CWidth=Canvas->GetWidth();
 CHeight=Canvas->GetHeight();
 return true;
}
// --------------------------

// --------------------------
// Перегруженные операторы
// --------------------------
// Оператор присваивания
UGraphics& UGraphics::operator = (UGraphics &tool)
{
 PenColor=tool.PenColor;
 PenWidth=tool.PenWidth;
 return *this;
}
// --------------------------

// --------------------------
// Графические примитивы
// --------------------------
// Отображает пиксель
// если ispos == true позиционирует перо в заданную позицию
void UGraphics::Pixel(int x, int y, bool ispos)
{
 CWidth=Canvas->GetWidth();
 CHeight=Canvas->GetHeight();

 DrawPixel(x,y);

 if(ispos)
  {
   PenX=x;
   PenY=y;
  }
}

// Отображает линию по координатам концов
void UGraphics::Line(int x1, int y1, int x2, int y2)
{
 int tmp;
 int dx,dy,y,x;
 int old_x1=x1, old_y1=y1;

 CWidth=Canvas->GetWidth();
 CHeight=Canvas->GetHeight();

 dx=x2-x1;
 dy=y2-y1;

 if(!dx && !dy)
  {
   DrawPixel(x1,y1);
   return;
  }
  /*
 if(x1>x2 && y1>y2)
  {
   tmp=x1; x1=x2; x2=tmp;
   tmp=y1; y1=y2; y2=tmp;
  }     */

 UBPoint p1,p2;
 ClipLine(UBPoint(x1,y1), UBPoint(x2,y2),UBRect(0,0,CWidth,CHeight),p1,p2);
 x1=p1.X; y1=p1.Y;
 x2=p2.X; y2=p2.Y;

 dx=x2-x1;
 dy=y2-y1;

 if(!dx)
  {
   CData=Canvas->GetData()+y1*CWidth+x1;
   if(y1<y2)
    for(int i=y1;i<=y2;i++)
     DrawPixel(x1,i);
   else
    for(int i=y1;i>=y2;i--)
     DrawPixel(x1,i);
  }
 else
 if(!dy)
  {
   CData=Canvas->GetData()+y1*CWidth+x1;
   if(x1<x2)
    for(int i=x1;i<=x2;i++)
     DrawPixel(i,y1);
   else
    for(int i=x1;i>=x2;i--)
     DrawPixel(i,y1);
  }
 else
 if(abs(dy)<abs(dx))
  {
   if(x1<x2)
    for(int i=x1;i<=x2;i++)
    {
	 y=(dy*(i-old_x1))/dx+old_y1;
	 if(y >= 0 && y < CHeight)
	  DrawPixel(i,y);
	}
   else
	for(int i=x1;i>=x2;i--)
	{
	 y=(dy*(i-old_x1))/dx+old_y1;
	 if(y >= 0 && y < CHeight)
	  DrawPixel(i,y);
	}
  }
 else
  {
   if(y1<y2)
	for(int i=y1;i<=y2;i++)
	{
	 x=(dx*(i-old_y1))/dy+old_x1;
	 if(x >= 0 && x < CWidth)
	  DrawPixel(x,i);
	}
   else
	for(int i=y1;i>=y2;i--)
	{
	 x=(dx*(i-old_y1))/dy+old_x1;
	 if(x >= 0 && x < CWidth)
	  DrawPixel(x,i);
	}

  }
}

// Отображает линию относительно позиции пера
// Перемещает перо в позицию x,y
void UGraphics::LineTo(int x, int y)
{
 Line(PenX,PenY,x,y);
 PenX=x;
 PenY=y;
}

// Отображает окружность с центром x,y и радиусом r
// Если fill == true - то рисуем с заливкой
void UGraphics::Circle(int x, int y, int r, bool fill)
{
 int r2=r*r, r4=static_cast<int>(r*1.414/2.0);
 int x0,y0;
 int x1,x2,y1;

 CWidth=Canvas->GetWidth();
 CHeight=Canvas->GetHeight();

 if(!fill)
  {
   for(int i=-r4-1; i <=r4+1; i++)
    {
     y0=static_cast<int>(sqrt((float)r2-i*i));
     DrawPixel(x+i,y+y0);
     DrawPixel(x+i,y-y0);
     DrawPixel(x+y0,y+i);
     DrawPixel(x-y0,y+i);
    }
  }
 else
  {
   for(int i=-r; i <=r; i++)
    {
     x0=static_cast<int>(sqrt((float)r2-i*i));
     x1=x-x0; x2=x+x0;
     y1=y+i;

     if(y1>=0 && y1 <CHeight)
      {
       if(x1<0) x1=0;
       if(x2>=CWidth) x2=CWidth-1;

       CData=Canvas->GetData()+y1*CWidth*3;
       for(int j=x1;j<=x2;j++)
       {
        *(CData+j*3)=PenColor.rgb.b;
        *(CData+j*3+1)=PenColor.rgb.g;
        *(CData+j*3+2)=PenColor.rgb.r;
       }
      }
    }
  }
}

// Отображает круговой сектор с центром x,y и радиусом r
// Раствор сектора fi, повернут на угол teta
// против часовой стрелки от горизонтальной оси
// углы задаются в градусах
void UGraphics::Sector(int x, int y, int r, float fi, float teta, bool fill)
{
 int x1,y1;
 float Pi2=static_cast<float>(M_PI/180.0);
 float Alpha=static_cast<float>(asin(1.0/r));
 float Teta=teta*Pi2,Fi=fi*Pi2;

 CWidth=Canvas->GetWidth();
 CHeight=Canvas->GetHeight();

 x1=x+static_cast<int>(r*cos(Teta));
 y1=y+static_cast<int>(r*sin(Teta));
 Line(x,y,x1,y1);
 x1=x+static_cast<int>(r*cos(Teta+Fi));
 y1=y+static_cast<int>(r*sin(Teta+Fi));
 Line(x,y,x1,y1);
 for(float i=Teta;i<Teta+Fi;i+=Alpha)
  DrawPixel(x+static_cast<int>(r*cos(i)),y+static_cast<int>(r*sin(i)));

 if(fill)
 {
  x1=x+static_cast<int>(r*cos(Teta+Fi/2)/2.0);
  y1=y+static_cast<int>(r*sin(Teta+Fi/2)/2.0);
  Fill(x1, y1, PenColor);
 }
}

// Отображает эллипс с центром x,y и радиусами hor, vert
// Если fill == true - то рисуем с заливкой
void UGraphics::Ellipse(int x, int y, int hor, int vert, bool fill)
{
 int hor2=hor*hor;
 int vert2=vert*vert;
 int x0,y0;
 int x1,x2,y1;

 CWidth=Canvas->GetWidth();
 CHeight=Canvas->GetHeight();

 if(!fill)
  {
   for(int i=-hor; i <=hor; i++)
    {
     y0=static_cast<int>(sqrt((float)vert2-i*i*vert2/hor2));
     DrawPixel(x+i,y+y0);
     DrawPixel(x+i,y-y0);
    }

   for(int i=-vert; i <=vert; i++)
    {
     x0=static_cast<int>(sqrt((float)hor2-i*i*hor2/vert2));
     DrawPixel(x+x0,y+i);
     DrawPixel(x-x0,y+i);
    }
  }
 else
  {
   for(int i=-vert; i <=vert; i++)
    {
     x0=static_cast<int>(sqrt((float)hor2-i*i*hor2/vert2));
     x1=x-x0; x2=x+x0;
     y1=y+i;

     if(y1>=0 && y1 <CHeight)
      {
       if(x1<0) x1=0;
       if(x2>=CWidth) x2=CWidth-1;

       CData=Canvas->GetData()+y1*CWidth*3;
       for(int j=x1;j<=x2;j++)
       {
        *(CData+j*3)=PenColor.rgb.b;
        *(CData+j*3+1)=PenColor.rgb.g;
        *(CData+j*3+2)=PenColor.rgb.r;
       }
      }
    }
  }
}

// Отображает прямоугольник с координатами
// x1,y1 - верхнего левого угла
// x2,y2 - правого нижнего угла
// Если fill == true - то рисуем с заливкой
void UGraphics::Rect(int x1, int y1, int x2, int y2, bool fill)
{
 int width;
 int dx,dy;

 if(!fill)
  {
   Line(x1,y1,x1,y2);
   Line(x1,y1,x2,y1);
   Line(x2,y1,x2,y2);
   Line(x1,y2,x2,y2);
  }
 else
  {
   CWidth=Canvas->GetWidth();
   CHeight=Canvas->GetHeight();


   if(x2<x1)
    {
     dx=x1; x1=x2; x2=dx;
    }

   if(y2<y1)
    {
     dy=y1; y1=y2; y2=dy;
    }

   if(x1>=CWidth || y1 >=CHeight || x2<0 || y2 < 0)
    return;

   if(x1<0) x1=0;
   if(y1<0) y1=0;
   if(x2>=CWidth) x2=CWidth-1;
   if(y2>=CHeight) y2=CHeight-1;

   dx=x2-x1+1;
   dy=y2-y1+1;
   width=PenWidth;
   SetPenWidth(1);

   CData=Canvas->GetData()+y1*CWidth*3+x1*3;
   for(int i=0;i<dx;i++)
   {
    *(CData+i*3)=PenColor.rgb.b;
    *(CData+i*3+1)=PenColor.rgb.g;
    *(CData+i*3+2)=PenColor.rgb.r;
   }

   for(int i=0;i<dy;i++)
    memcpy(CData+i*CWidth*3, CData, dx*3);

   SetPenWidth(width);
  }
}

// Отображает прямоугольник с координатами вершин
// Если fill == true - то рисуем с заливкой
void UGraphics::Triangle(int x1, int y1, int x2, int y2, int x3, int y3,
                         bool fill)
{
 Line(x1,y1,x2,y2);
 Line(x1,y1,x3,y3);
 Line(x2,y2,x3,y3);

 if(fill)
 {
  Fill((x1+x2+x3)/3, (y1+y2+y3)/3, PenColor);
 }

}


// Простейшая заливка произвольной области
void UGraphics::Fill(int x, int y, UColorT BorderColor)
{
 CWidth=Canvas->GetWidth();
 CHeight=Canvas->GetHeight();

 if(!CWidth || !CHeight)
  return;

 list<int> points;
 int pos=y*CWidth+x;

 if(pos<0 || pos>=CWidth*CHeight)
  return;

 CData=Canvas->GetData()+pos*3;
 *CData++=PenColor.rgb.b;
 *CData++=PenColor.rgb.g;
 *CData=PenColor.rgb.r;
 points.push_back(pos);

 do{
  pos=points.front();
  // Поиск соседних точек
  int y=pos/CWidth;
  int x=pos-y*CWidth;

  if(x-1>=0)
  {
   CData=Canvas->GetData()+(pos-1)*3;
   if(memcmp(CData,&BorderColor,3))
   {
    *CData++=PenColor.rgb.b;
    *CData++=PenColor.rgb.g;
    *CData=PenColor.rgb.r;
    points.push_back(pos-1);
   }
  }
  if(x+1<CWidth)
  {
   CData=Canvas->GetData()+(pos+1)*3;
   if(memcmp(CData,&BorderColor,3))
   {
    *CData++=PenColor.rgb.b;
    *CData++=PenColor.rgb.g;
    *CData=PenColor.rgb.r;
    points.push_back(pos+1);
   }
  }
  if(y+1<CHeight)
  {
   CData=Canvas->GetData()+(pos+CWidth)*3;
   if(memcmp(CData,&BorderColor,3))
   {
    *CData++=PenColor.rgb.b;
    *CData++=PenColor.rgb.g;
    *CData=PenColor.rgb.r;
    points.push_back(pos+CWidth);
   }
  }
  if(y-1>=0)
  {
   CData=Canvas->GetData()+(pos-CWidth)*3;
   if(memcmp(CData,&BorderColor,3))
   {
    *CData++=PenColor.rgb.b;
    *CData++=PenColor.rgb.g;
    *CData=PenColor.rgb.r;
    points.push_back(pos-CWidth);
   }
  }

  points.pop_front();
 }while(!points.empty());
}

// Выводит изображение с началом в заданной позиции
// Если transparency == 0 копируется без прозрачности
// Если transparency == 1 копируется c эффектом прозрачности
// Если transparency == 2 копируется как маска, с цветом данных как текущего пера
void UGraphics::Bitmap(int x, int y, UBitmap &bmp, int transparency, UColorT transp)
{
 switch(transparency)
 {
 case 0:
  bmp.CopyTo(x, y, *Canvas);
 break;

 case 1:
  bmp.CopyTo(x, y, *Canvas, transp);
 break;

 case 2:
  bmp.MaskTo(x, y, *Canvas, transp,PenColor);
 break;
 }
}
// --------------------------


// --------------------------
// Вывод текста
// --------------------------
// Вычисление длины и высоты строки текста
void UGraphics::CalcTextSize(const wstring &str, int &width, int &height)
{
 if(!Font)
  return;

 Font->CalcTextSize(str, width, height);
}

void UGraphics::CalcTextSize(const string &str, int &width, int &height)
{
 if(!Font)
  return;

 Font->CalcTextSize(str, width, height);
}

// Вычисление, сколько символов строки, начиная с символа index, войдет по
// ширине в заданное число пикселей
int UGraphics::CalcTextLength(const string &str, int index, int width)
{
 if(!Font)
  return 0;

 return Font->CalcTextLength(str, index, width);
}

int UGraphics::CalcTextLength(const wstring &str, int index, int width)
{
 if(!Font)
  return 0;

 return Font->CalcTextLength(str, index, width);
}

// Выводит текст str
void UGraphics::Text(const wstring &str, int x, int y)
{
 if(!Font)
  return;

 SetPenPos(x,y);
 Font->Draw(str,this);
}

void UGraphics::Text(const string &str, int x, int y)
{
 if(!Font)
  return;

 SetPenPos(x,y);
 Font->Draw(str,this);
}

void UGraphics::TextRect(const wstring &str, const UBRect &rect, int align)
{
 if(!Font)
  return;

 Font->DrawRect(str,rect,align,this);
}

void UGraphics::TextRect(const string &str, const UBRect &rect, int align)
{
 if(!Font)
  return;

 Font->DrawRect(str,rect,align,this);
}
// --------------------------



// Вспомогательные методы
// --------------------------
// Вспомогательные графические примитивы
// --------------------------
// Отрисовывает пиксель в координатах x,y без проверок
void UGraphics::DrawPixel(int x, int y)
{
 int x1,y1,x2,y2,dx,dy;

 if(PenWidth == 1)
  {
   if(x<0 || x>=CWidth || y<0 || y>=CHeight)
    return;
   CData=Canvas->GetData()+(y*CWidth+x)*3;
   *CData++=PenColor.rgb.b;
   *CData++=PenColor.rgb.g;
   *CData=PenColor.rgb.r;
  }
 else
  {
   if(PenWidth % 2)
    {
     x1=x-HalfPenWidth;
     y1=y-HalfPenWidth;
     x2=x+HalfPenWidth+1;
     y2=y+HalfPenWidth+1;
    }
   else
    {
     x1=x-HalfPenWidth+1;
     y1=y-HalfPenWidth+1;
     x2=x+HalfPenWidth+1;
     y2=y+HalfPenWidth+1;
    }

   if(x1<0) x1=0;
   if(y1<0) y1=0;
   if(x2>=CWidth) x2=CWidth-1;
   if(y2>=CHeight) y2=CHeight-1;

   CData=Canvas->GetData()+(y1*CWidth+x1)*3;
   UBColor* data=CData;

   dx=x2-x1;
   dy=y2-y1;

   if(PenWidth > 4)
    {
     for(int i=0;i<dx;i++)
     {
      *CData++=PenColor.rgb.b;
      *CData++=PenColor.rgb.g;
      *CData++=PenColor.rgb.r;
     }

     --dy;
     CData+=(CWidth-dx)*3;
     for(int j=0;j<dy;j++,CData+=CWidth*3)
      memcpy(CData,data,dx*3);
    }
   else
    {
     for(int j=0;j<dy;j++,CData+=(CWidth-dx)*3)
      for(int i=0;i<dx;i++)
      {
       *CData++=PenColor.rgb.b;
       *CData++=PenColor.rgb.g;
       *CData++=PenColor.rgb.r;
      }
    }
  }
}
// --------------------------

}
#endif



