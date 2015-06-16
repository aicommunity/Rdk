// ===========================================================
// Version:        2.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2009.
E-mail:        alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: RDK

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
 *********************************************************** */
#ifndef UDRAW_ENGINE_GRAPHICS_CPP
#define UDRAW_ENGINE_GRAPHICS_CPP

#include "UDrawEngine.h"
#include "../Math/MUXMLSerialize.h"
#include "../Application/UIVisualController.h"

namespace RDK {

// ------------------------
// Конструкторы и деструкторы
// ------------------------
UGEDescription::UGEDescription(void)
{
// Координаты элемента в пикселях
Position=0;

// Радиус элемента
Width = 80; Height=25;

// Цвет контура элемента
ContourColor = UColorT(0, 0, 0, 0);

// Толщина контура элемента в пикселях
ContourWidth = 1;

// Цвет заливки элемента
FillColor = UColorT(180, 180, 180, 0);

LinkColor = UColorT(0, 0, 255, 0);

// Толщина контура связи
LinkWidth = 1;

// Одномерный индекс i
Index = 0;

// 3D индекс (i,j,k)
Position=0;

// Тип элемента
Type = 0;

 NumInputs=0;
 NumOutputs=0;
 Highlight=false;

}

UGEDescription::UGEDescription(const UGEDescription &copy)
{
 *this=copy;
}


UGEDescription::~UGEDescription(void)
{
}
// ------------------------

UGEDescription& UGEDescription::operator = (const UGEDescription &copy)
{
Index=copy.Index;
Position=copy.Position;
Header=copy.Header;
ClassName=copy.ClassName;
Type=copy.Type;
NumInputs=copy.NumInputs;
NumOutputs=copy.NumOutputs;
Width=copy.Width;
Height=copy.Height;
ContourColor=copy.ContourColor;
ContourWidth=copy.ContourWidth;
FillColor=copy.FillColor;
LinkColor=copy.LinkColor;
LinkWidth=copy.LinkWidth;
Highlight=copy.Highlight;
Indicators=copy.Indicators;

 return *this;
}



/* ******************************************************************* */

/* ******************************************************************* */
// ---------------------------
// Конструкторы и деструкторы
// ---------------------------
UDrawEngine::UDrawEngine(void)
{
// Net = 0;
 GEngine = 0;

 // Цвет фона
 BackgroundColor = UColorT(255, 255, 255, 255);

 BackgroundLineColor=UColorT(200, 200, 200, 0);

 // Цвет выделяемого элемента
 SelectedColor = UColorT(128, 128, 128, 0);

 BackgroundLineStep=10;

 CanvasWidth = 0;
 CanvasHeight = 0;

 ZoomCoeff=30;

 Origin=0;

 // Размеры элемента в пикселях по умолчанию
 RectWidth=80;
 RectHeight=25;

 ElementsXRes=1;
 ElementsYRes=1;
}

UDrawEngine::~UDrawEngine(void)
{
}
// ---------------------------

// ---------------------------
// Методы управления параметрами
// ---------------------------
// Коэффициент зуммирования
// Пиксельная координата X,Y=ZoomCoeff*Coord.X,Y
double UDrawEngine::GetZoomCoeff(void) const
{
 return ZoomCoeff;
}

// Начало координат в канве
MVector<double,3> UDrawEngine::GetOrigin(void) const
{
 return Origin;
}

// Размеры элемента в пикселях по умолчанию
int UDrawEngine::GetRectWidth(void) const
{
 return RectWidth;
}

bool UDrawEngine::SetRectWidth(int value)
{
 RectWidth=value;
 return true;
}

int UDrawEngine::GetRectHeight(void) const
{
 return RectHeight;
}

bool UDrawEngine::SetRectHeight(int value)
{
 RectHeight=value;
 return true;
}
// ---------------------------

// ---------------------------
// Методы доступа к данным
// ---------------------------
// Возвращает указатель на НС
/*UEPtr<UNet> UDrawEngine::GetNet(void)
{
 return Net;
} */
const USerStorageXML& UDrawEngine::GetNetXml(void) const
{
 return NetXml;
}


// Возвращает элемент таблицы соответствий
UGEDescription& UDrawEngine::GetDescription(const string &name)
{
 return Descriptions[name];
}

// Возвращает всю таблицу соответсвий
const UDrawEngine::DescriptionsTableT& UDrawEngine::GetDescriptions(void)
{
 return Descriptions;
}

// Возвращает указатель на движок отображения
UEPtr<UAGraphics> UDrawEngine::GetGEngine(void)
{
 return GEngine;
}

bool UDrawEngine::SetNetXml(USerStorageXML &net_xml)
{
 NetXml = net_xml;
 BuildDestinations();
 return true;
}

// Связывает класс с новой НС
// Если engine == 0 то возвращает false и не делеает ничего
bool UDrawEngine::SetEngine(UEPtr<UAGraphics> engine)
{
 if (GEngine == engine)
  return true;

 GEngine = engine;
 CanvasWidth = GEngine->GetCWidth();
 CanvasHeight = GEngine->GetCHeight();

 Draw();
 return true;
}

// Обновляет таблицу соответствий
void UDrawEngine::UpdateDescriptions(void)
{
}

// Выделяет заданный компонент, сбрасывая флаг выделения с остальных
void UDrawEngine::SelectSingleComponent(const string &name)
{
 DescriptionsTableIteratorT I, J;
 I = Descriptions.begin();
 J = Descriptions.end();

 for(;I != J;++I)
 {
  if(I->first != name)
   I->second.Highlight=false;
  else
   I->second.Highlight=true;
 }
}

// Поиск объекта по заданным координатам в изображении
std::string UDrawEngine::FindComponent(int x, int y)
{
 DescriptionsTableIteratorT I, J;
 I = Descriptions.begin();
 J = Descriptions.end();
 std::string name;

 for(;I != J;++I)
 {
//  double dist=sqrt(double((x-I->second.Position.x)*(x-I->second.Position.x)+(y-I->second.Position.y)*(y-I->second.Position.y)));

//  name=I->first;
  if(abs(int(x-I->second.Position.x))<=I->second.Width && abs(int(y-I->second.Position.y))<=I->second.Height)
//  if(dist<=I->second.Height)// Заглушка!!
   return I->first;
 }

 return string();
}


// Перемещает компонента в заданные координаты, с некоторой зоной нечуствительности
void UDrawEngine::MoveComponent(const string &name, int x, int y)
{
 DescriptionsTableIteratorT I=Descriptions.find(name);
 if(I == Descriptions.end())
  return;
 UGEDescription &descr=I->second;

 if(fabs(descr.Position.x-x)>BackgroundLineStep)
  descr.Position.x+=(int(x-descr.Position.x)/BackgroundLineStep)*BackgroundLineStep;

 if(fabs(descr.Position.y-y)>BackgroundLineStep)
  descr.Position.y+=(int(y-descr.Position.y)/BackgroundLineStep)*BackgroundLineStep;
}

// Обновляет размеры всех элементов в соответствии с размерами по умолчанию
void UDrawEngine::UpdateAllElementsSize(void)
{
 DescriptionsTableIteratorT I, J;
 I = Descriptions.begin();
 J = Descriptions.end();

 for(;I != J;++I)
 {
  I->second.Width=RectWidth;
  I->second.Height=RectHeight;
 }
}
// ---------------------------

// ---------------------------
// Методы размещения элементов по графическому полю
// ---------------------------
// Метод автоматического распределения существующих элементов по полю
void UDrawEngine::BuildDestinations(void)
{
 Descriptions.clear();
 UpdateDestinations();
}

// Метод автоматического распределения нераспределенных элементов
// (нераспределенные элементы имеют координаты -1,-1)
void UDrawEngine::UpdateDestinations(void)
{
 NetXml.SelectRoot();
 NetXml.SelectNode(0);
 if(!NetXml.SelectNode("Components"))
  return;

 int num_components=NetXml.GetNumNodes();

 map<string,UGEDescription>::iterator dI;
 for(int i=0;i<num_components;i++)
 {
  NetXml.SelectNode(i);

  std::string name=NetXml.GetNodeName();
  dI=Descriptions.find(name);
  UGEDescription descr;
  if(dI == Descriptions.end())
  {
   descr=Descriptions[name];
   dI=Descriptions.find(name);
  }
  else
  {
   descr=dI->second;
  }

   if(!NetXml.SelectNode("Components"))
	descr.Type=1;
   else
   {
	if(NetXml.GetNumNodes())
	 descr.Type=2;
	else
	 descr.Type=1;
	NetXml.SelectUp();
   }

   if(NetXml.SelectNode("Parameters"))
   {
	descr.NumInputs=NetXml.ReadInteger("NumInputs",0);
	descr.NumOutputs=NetXml.ReadInteger("NumOutputs",0);

	if(NetXml.SelectNode("Coord"))
	{
	 RDK::MVector<double,3> coord(1);
	 RDK::operator >> (NetXml,coord);
	 NetXml.SelectUp();

	 if((!coord)>0)
	  descr.Position=coord*ZoomCoeff+Origin;
	 else
	  descr.Position=i*30.0;
	}

	NetXml.SelectUp();
   }

   descr.ClassName=NetXml.GetNodeAttribute("Class");
   descr.Header=dI->first;
   descr.Width=RectWidth;
   descr.Height=RectHeight;
   dI->second=descr;

  NetXml.SelectUp();
 }

 ParseLinks();
}

// Устанавливает координаты размещения элемента id
// Возвращает false если элемент не найден или координаты некорректны
// Метод не проверяет свободна-ли занимаемая область
bool UDrawEngine::SetDestination(const string &name, int x, int y)
{
 return true;
}

// Заполняет массивы связей компонент
void UDrawEngine::ParseLinks(void)
{
 DescriptionsTableIteratorT I,J;

 NetXml.SelectRoot();
 NetXml.SelectNode(0);
 if(!NetXml.SelectNode("Links"))
  return;

 int num_outs=NetXml.GetNumNodes();

 Links.clear();
 for (int i = 0; i < num_outs; i++)
 {
   NetXml.SelectNode(i);
   int num_inps=NetXml.GetNumNodes()-1;

   std::string item_name=NetXml.ReadString("Item","");
   I=Descriptions.find(item_name.substr(0,item_name.find_first_of(".")));
   if(I == Descriptions.end())
   {
    NetXml.SelectUp();
	continue;
   }

//   Links[I->first].clear();
   for(int j=0;j<num_inps;j++)
   {
	std::string conn_name=NetXml.ReadString("Connector",j,"");
	J=Descriptions.find(conn_name.substr(0,conn_name.find_first_of(".")));
	if(J == Descriptions.end())
	 continue;
	Links[I->first].push_back(J);
   }

   NetXml.SelectUp();
 }
}
// ---------------------------

// ---------------------------
// Основные методы отображения
// ---------------------------
// Метод перерисовки всех элементов
void UDrawEngine::Draw(void)
{
 CanvasWidth=GEngine->GetCWidth();
 CanvasHeight=GEngine->GetCHeight();

 DrawBackground();
 DescriptionsTableIteratorT I, J;
 I = Descriptions.begin();
 J = Descriptions.end();

 while (I != J)
 {
  if (!Draw(I->first))
   return;
  ++I;
 }
 if (!DrawLinks())
  return;
}

// Метод обновления изображения отдельного элемента сети
// Возвращает false если элемент не найден
bool UDrawEngine::Draw(const string &name)
{
 DescriptionsTableIteratorT I = Descriptions.find(name);

 if (I == Descriptions.end())
  return false;

 Paint(I->second);
 return true;
}

// Метод отображения связей сети
// links==true - отображает схему связей линиями
// highlight==true - подсвечивает все компоненты связанные с этим
// (сам нейрон подсвечивается в случае, если на него есть обратные связи)
// contour==true - окружает контуром компоненты связанные с этим
// (в контур могут попасть и не связанные компоненты)
bool UDrawEngine::DrawLinks(bool links, bool highlight,
	bool contour)
{
 DescriptionsLinksTableIteratorT I,J;

 I=Links.begin();
 J=Links.end();

 for(;I!=J;++I)
 {
  DescriptionsTableIteratorT K=Descriptions.find(I->first);
  if(K == Descriptions.end())
   continue;
  UGEDescription &out=K->second;
  for(size_t i=0;i<I->second.size();i++)
  {
   UGEDescription &in=I->second[i]->second;
   PaintLink(out,in,links,highlight,contour);

  }
 }
 return true;
}

// Метод отображения фона
bool UDrawEngine::DrawBackground(void)
{
 GEngine->SetPenColor(BackgroundColor);
 GEngine->SetPenWidth(1);
 GEngine->Rect(0, 0, CanvasWidth, CanvasHeight, true);

 GEngine->SetPenColor(BackgroundLineColor);
 GEngine->SetPenWidth(1);

 for(int i=0;i<CanvasWidth;i+=BackgroundLineStep)
  GEngine->Line(i,0,i,CanvasHeight-1);

 for(int i=0;i<CanvasHeight;i+=BackgroundLineStep)
  GEngine->Line(0,i,CanvasWidth-1,i);

 GEngine->SetPenColor(BackgroundLineColor);
 GEngine->Rect(0, 0, BackgroundLineStep, CanvasHeight, true);
 GEngine->Rect(0, 0, CanvasWidth, BackgroundLineStep, true);
 GEngine->Rect(CanvasWidth-BackgroundLineStep, 0, CanvasWidth, CanvasHeight, true);
 GEngine->Rect(0, CanvasHeight-BackgroundLineStep, CanvasWidth, CanvasHeight, true);
 return true;
}
// ---------------------------

// ---------------------------
// Вспомогательные методы отображения
// ---------------------------
// Отрисовывает элемент в заданной позиции
void UDrawEngine::Paint(UGEDescription &ndescr)
{
 if (!GEngine)
  return;

 switch(ndescr.Type)
 {
 case 1:
  PaintItem(ndescr);
 break;

 case 2:
  PaintNet(ndescr);
 break;
 }

 // Отрисовываем индикаторы
 for(size_t i=0;i<ndescr.Indicators.size();i++)
  PaintIndicator(int(ndescr.Position.x) - ndescr.Width,
		int(ndescr.Position.y) + ndescr.Height + ndescr.ContourWidth, ndescr.Width * 2,
		ndescr.Height / 2, ndescr.Indicators[i], 1, (i<FullIndicatorColors.size())?FullIndicatorColors[i]:BackgroundColor,
		(i<EmptyIndicatorColors.size())?EmptyIndicatorColors[i]:BackgroundColor);

 // Отрисовываем имя
 GEngine->SetPenColor(ndescr.ContourColor);
// GEngine->Text(ndescr.Header,int(ndescr.Position.x+ndescr.Radius*1.5),static_cast<int>(int(ndescr.Position.y)-ndescr.Radius/2));
 UBRect rect;
 rect.X=int(ndescr.Position.x)-(ndescr.Width-2);
 rect.Y=int(ndescr.Position.y)-ndescr.Height;
 rect.Width=(ndescr.Width-2)*2;
 rect.Height=ndescr.Height;
 UAFont *font=GEngine->GetFont();
 GEngine->TextRect(ndescr.Header,rect,alCenter);
 rect.Y+=int(ndescr.Height*1.2);
 UAFont *class_font=RDK::GlobalFonts.GetFont(font->GetName(),font->GetSize()-4);
 if(class_font)
 {
  GEngine->SetFont(class_font);
  GEngine->TextRect(ndescr.ClassName,rect,alRight);
  GEngine->SetFont(font);
 }
}

// Отрисовывает конечный элемент (NAItem*) с центром в заданной позиции
void UDrawEngine::PaintItem(UGEDescription &ndescr)
{
 UBRect rect;
 int shift=0;

 if(ndescr.ContourWidth % 2)
  shift=int(std::ceil(ndescr.ContourWidth/2.0));
 else
  shift=int(std::ceil(ndescr.ContourWidth/2.0+1));

 GEngine->SetPenWidth(ndescr.ContourWidth);
 if (ndescr.ContourColor == ndescr.FillColor)
 {
        GEngine->SetPenColor(ndescr.ContourColor);
		GEngine->Rect(int(int(ndescr.Position.x))-ndescr.Width, int(int(ndescr.Position.y))-ndescr.Height, int(int(ndescr.Position.x))+ndescr.Width, int(int(ndescr.Position.y))+ndescr.Height, true);
 }
 else
 {
  if (ndescr.Highlight)
  {
   GEngine->Rect(int(int(ndescr.Position.x))-ndescr.Width, int(int(ndescr.Position.y))-ndescr.Height, int(int(ndescr.Position.x))+ndescr.Width, int(int(ndescr.Position.y))+ndescr.Height, false);
   GEngine->SetPenColor(SelectedColor);
   GEngine->Rect(int(int(ndescr.Position.x))-ndescr.Width+shift, int(int(ndescr.Position.y))-ndescr.Height+shift, int(int(ndescr.Position.x))+ndescr.Width-ndescr.ContourWidth, int(int(ndescr.Position.y))+ndescr.Height-ndescr.ContourWidth, true);
  }
  else
  {
   GEngine->SetPenColor(ndescr.ContourColor);
   GEngine->Rect(int(int(ndescr.Position.x))-ndescr.Width, int(int(ndescr.Position.y))-ndescr.Height, int(int(ndescr.Position.x))+ndescr.Width, int(int(ndescr.Position.y))+ndescr.Height, false);
   GEngine->SetPenColor(ndescr.FillColor);
   GEngine->Rect(int(int(ndescr.Position.x))-ndescr.Width+shift, int(int(ndescr.Position.y))-ndescr.Height+shift, int(int(ndescr.Position.x))+ndescr.Width-ndescr.ContourWidth, int(ndescr.Position.y)+ndescr.Height-ndescr.ContourWidth, true);
  }
 }
}

// Отрисовывает элемент-сеть (UNet*) с центром в заданной позиции
void UDrawEngine::PaintNet(UGEDescription &ndescr)
{
 int shift=0;

 if((ndescr.ContourWidth+1) % 2)
  shift=int(std::ceil((ndescr.ContourWidth+1)/2.0));
 else
  shift=int(std::ceil((ndescr.ContourWidth+1)/2.0+1));

 GEngine->SetPenWidth(ndescr.ContourWidth+1);
 if (ndescr.ContourColor == ndescr.FillColor)
 {
  GEngine->SetPenColor(ndescr.ContourColor);
  GEngine->Rect(int(int(ndescr.Position.x))-ndescr.Width, int(int(ndescr.Position.y))-ndescr.Height, int(int(ndescr.Position.x))+ndescr.Width, int(int(ndescr.Position.y))+ndescr.Height, true);
 }
 else
 {
  if (ndescr.Highlight)
  {
   GEngine->Rect(int(ndescr.Position.x)-ndescr.Width, int(ndescr.Position.y)-ndescr.Height, int(ndescr.Position.x)+ndescr.Width, int(ndescr.Position.y)+ndescr.Height, false);
   GEngine->SetPenColor(SelectedColor);
   GEngine->Rect(int(ndescr.Position.x)-ndescr.Width+shift, int(ndescr.Position.y)-ndescr.Height+shift, int(ndescr.Position.x)+ndescr.Width-ndescr.ContourWidth, int(ndescr.Position.y)+ndescr.Height-ndescr.ContourWidth, true);
  }
  else
  {
   GEngine->SetPenColor(ndescr.ContourColor);
   GEngine->Rect(int(ndescr.Position.x)-ndescr.Width, int(ndescr.Position.y)-ndescr.Height, int(ndescr.Position.x)+ndescr.Width, int(ndescr.Position.y)+ndescr.Height, false);
   GEngine->SetPenColor(ndescr.FillColor);
   GEngine->Rect(int(ndescr.Position.x)-ndescr.Width+shift, int(ndescr.Position.y)-ndescr.Height+shift, int(ndescr.Position.x)+ndescr.Width-ndescr.ContourWidth, int(ndescr.Position.y)+ndescr.Height-ndescr.ContourWidth, true);
  }
 }
}

// Отрисовывает индикатор с заданными параметрами
// Направление direction задается следующим образом
// 1 - индикатор заполняется слева направо
// 2 - индикатор заполняется справа налево
// 3 - индикатор заполняется сверху вниз
// 4 - индикатор заполняется снизу вверх
void UDrawEngine::PaintIndicator(int x, int y, int width, int height,
    double precent, int direction, UColorT full, UColorT empty)
{
 if (precent < 0)
  return;

 GEngine->SetPenWidth(1);
 GEngine->SetPenColor(empty);
 GEngine->Rect(x, y, x + width, y + height, true);
 GEngine->SetPenColor(full);
 GEngine->Rect(x, y, x + width, y + height, false);

 switch(direction)
 {
 case 1:
     GEngine->Rect(x, y, x +static_cast<int>((width * precent) / 100.0),
         y + height, true);
 break;

 case 2:
     GEngine->Rect
         (x +static_cast<int>((width - (width * precent) / 100.0)),
         y, x + width, y + height, true);
 break;

 case 3:
     GEngine->Rect(x, y, x + width,
         y +static_cast<int>((height * precent) / 100.0), true);
 break;

 case 4:
     GEngine->Rect(x,
         y +static_cast<int>((height - (height * precent) / 100.0)),
         x + width, y + height, true);
 break;
 }
}

// Отрисовывает связь
void UDrawEngine::PaintLink(UGEDescription &out, UGEDescription &in,
    bool links, bool highlight, bool contour)
{
 if(!GEngine)
  return;

 if (links)
 {
  GEngine->SetPenColor(out.LinkColor);
  GEngine->SetPenWidth(out.LinkWidth);
  RDK::MVector<double,3> &c_out=out.Position;
  RDK::MVector<double,3> &c_in=in.Position;
  double x_diff=fabs(c_out.x-c_in.x), y_diff=fabs(c_out.y-c_in.y);
  if(c_out.x<c_in.x && x_diff>y_diff)
  {
   GEngine->Line(int(c_out.x) + out.Width, int(c_out.y), int(c_in.x) - in.Width, int(c_in.y));
   GEngine->Rect(int(in.Position.x) - in.Width - out.LinkWidth,
			int(c_in.y) - out.LinkWidth,
			int(c_in.x) - in.Width + out.LinkWidth, int(c_in.y) + out.LinkWidth, true);
  }
  else
  if(c_out.y<c_in.y && x_diff<=y_diff)
  {
   GEngine->Line(int(c_out.x), int(c_out.y) + out.Height, int(c_in.x), int(c_in.y) - in.Height);
   GEngine->Rect(int(in.Position.x) - out.LinkWidth,
			int(c_in.y) - in.Height - out.LinkWidth,
			int(c_in.x) + out.LinkWidth, int(c_in.y) - in.Height + out.LinkWidth, true);
  }
  else
  if(c_out.x>=c_in.x && x_diff>y_diff)
  {
   GEngine->Line(int(c_out.x) - out.Width, int(c_out.y), int(c_in.x) + in.Width, int(c_in.y));
   GEngine->Rect(int(in.Position.x) + in.Width - out.LinkWidth,
			int(c_in.y) - out.LinkWidth,
			int(c_in.x) + in.Width + out.LinkWidth, int(c_in.y) + out.LinkWidth, true);
  }
  else
  if(c_out.y>=c_in.y && x_diff<=y_diff)
  {
   GEngine->Line(int(c_out.x), int(c_out.y) - out.Height, int(c_in.x), int(c_in.y) + in.Height);
   GEngine->Rect(int(in.Position.x) - out.LinkWidth,
			int(c_in.y) + in.Height - out.LinkWidth,
			int(c_in.x) + out.LinkWidth, int(c_in.y) + in.Height + out.LinkWidth, true);
  }
 }
/*
 if (highlight)
 {
  PaintItem(out);
  PaintItem(in);
 }

 if (contour)
 {
 }
  */
}
// Отрисовывает окружность
// ---------------------------

}
#endif

