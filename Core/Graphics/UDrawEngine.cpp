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

namespace RDK {

// ------------------------
// Конструкторы и деструкторы
// ------------------------
UGEDescription::UGEDescription(void)
{
// Координаты элемента в пикселях
X = -1;
Y = -1;

// Радиус элемента
Radius = 10;

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
}

UGEDescription::~UGEDescription(void)
{
}
// ------------------------
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


 CanvasWidth = 0;
 CanvasHeight = 0;
}

UDrawEngine::~UDrawEngine(void)
{
}
// ---------------------------

// ---------------------------
// Методы доступа к данным
// ---------------------------
// Возвращает указатель на НС
/*UEPtr<UANet> UDrawEngine::GetNet(void)
{
 return Net;
} */
const Serialize::USerStorageXML& UDrawEngine::GetNetXml(void) const
{
 return NetXml;
}


// Возвращает элемент таблицы соответствий
UGEDescription& UDrawEngine::GetDescription(const string &name)
{
 return Descriptions[name];
}

// Возвращает всю таблицу соответсвий
const map<string, UGEDescription>& UDrawEngine::GetDescriptions(void)
{
 return Descriptions;
}

// Возвращает указатель на движок отображения
UEPtr<UAGraphics> UDrawEngine::GetGEngine(void)
{
 return GEngine;
}

// Связывает класс с новой НС
// Если net == 0 то отключает класс от текущей НС и возвращает true
/*bool UDrawEngine::SetNet(UEPtr<UANet> net)
{
 if (Net == net)
  return true;

 Net = net;
 BuildDestinations();
 return true;
} */

bool UDrawEngine::SetNetXml(Serialize::USerStorageXML &net_xml)
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
 map<string, UGEDescription>::iterator I, J;
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
const std::string UDrawEngine::FindComponent(int x, int y)
{
 map<string, UGEDescription>::iterator I, J;
 I = Descriptions.begin();
 J = Descriptions.end();

 for(;I != J;++I)
 {
  double dist=sqrt(double((x-I->second.X)*(x-I->second.X)+(y-I->second.Y)*(y-I->second.Y)));

  if(dist<=I->second.Radius)
   return I->first;
 }

 return string();
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
  if(dI == Descriptions.end())
  {
   UGEDescription &descr=Descriptions[name];
   descr.X=(i+1)*30;
   descr.Y=(i+1)*30;
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

   NetXml.SelectNode("Parameters");
   descr.NumInputs=NetXml.ReadInteger("NumInputs",0);
   descr.NumOutputs=NetXml.ReadInteger("NumOutputs",0);
   NetXml.SelectUp();
  }
  else
  {
   UGEDescription &descr=dI->second;
   descr.X=(i+1)*30;
   descr.Y=(i+1)*30;

   NetXml.SelectNode("Components");
   if(NetXml.GetNumNodes())
	descr.Type=2;
   else
	descr.Type=1;
   NetXml.SelectUp();

   NetXml.SelectNode("Parameters");
   descr.NumInputs=NetXml.ReadInteger("NumInputs",0);
   descr.NumOutputs=NetXml.ReadInteger("NumOutputs",0);
   NetXml.SelectUp();
  }

  NetXml.SelectUp();
 }

 ParseLinks();

// if (!NetXml)
//  return;
	/*
	size_t numcomps=Net->GetComponentsList().size();
	map<string,UId>::const_iterator I,J;
    map<UId,UGEDescription>::iterator dI;
    I=Net->GetComponentsList().begin();
    J=Net->GetComponentsList().end();

    // Вычисляем допустимое число элементов по горизонтали и вертикали.
    // Пытаемся разместить все объекты квадратом
    int xres=static_cast<int>(ceil(sqrt(float(numcomps))));
    if(xres<=0)
    xres=1;
    ElementsXRes=xres;
	ElementsYRes=numcomps/xres;
    if(ElementsXRes*ElementsYRes<int(numcomps))
    ++ElementsXRes;

    int i=0,j=0,l=0;
    while(I != J)
    {
    dI=Descriptions.find(I->second);
    if(dI == Descriptions.end())
    {
    Descriptions[I->second].Index=l;
    Descriptions[I->second].I=i;
    Descriptions[I->second].J=j;
    Descriptions[I->second].X=(CanvasWidth/ElementsXRes)*i+(CanvasWidth/ElementsXRes)/2;
    Descriptions[I->second].Y=(CanvasHeight/ElementsYRes)*j+(CanvasHeight/ElementsYRes)/2;
    if(dynamic_cast<NAItem*>(Net->GetComponent(I->second)))
    {
    Descriptions[I->second].Radius=10;
    Descriptions[I->second].Type=1;
    }
    else
    if(dynamic_cast<UANet*>(Net->GetComponent(I->second)))
    {
    Descriptions[I->second].Radius=15;
    Descriptions[I->second].Type=2;
    }

    Descriptions[I->second].Name=Net->GetComponent(I->second)->Name;


    ++l; ++i;
    if(i >= ElementsXRes)
    {
    i=0;
    ++j;
    }
    }
    ++I;
    } */
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
 map<string,UGEDescription>::iterator I,J;

 NetXml.SelectRoot();
 NetXml.SelectNode(0);
 if(!NetXml.SelectNode("Links"))
  return;

 int num_outs=NetXml.GetNumNodes();

 for (int i = 0; i < num_outs; i++)
 {
   NetXml.SelectNode(i);
   int num_inps=NetXml.GetNumNodes()-1;

   std::string item_name=NetXml.ReadString("Item","");
   I=Descriptions.find(item_name);
   if(I == Descriptions.end())
   {
    NetXml.SelectUp();
	continue;
   }
   UGEDescription &out=I->second;
   Links[I->first].clear();
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
 map<string, UGEDescription>::iterator I, J;
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
 map<string, UGEDescription>::iterator I = Descriptions.find(name);

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
 map<string,vector<map<string,UGEDescription>::iterator> >::iterator I,J;

 I=Links.begin();
 J=Links.end();

 for(;I!=J;++I)
 {
  map<string,UGEDescription>::iterator K=Descriptions.find(I->first);
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

 for(int i=0;i<CanvasWidth;i+=10)
  GEngine->Line(i,0,i,CanvasHeight-1);

 for(int i=0;i<CanvasHeight;i+=10)
  GEngine->Line(0,i,CanvasWidth-1,i);
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
  PaintIndicator(ndescr.X - ndescr.Radius,
		ndescr.Y + ndescr.Radius + ndescr.ContourWidth, ndescr.Radius * 2,
		ndescr.Radius / 2, ndescr.Indicators[i], 1, (i<FullIndicatorColors.size())?FullIndicatorColors[i]:BackgroundColor,
		(i<EmptyIndicatorColors.size())?EmptyIndicatorColors[i]:BackgroundColor);

 // Отрисовываем имя
 GEngine->SetPenColor(ndescr.ContourColor);
 // GEngine->Text(ndescr.Name,ndescr.X-ndescr.Radius*3,static_cast<int>(ndescr.Y-ndescr.Radius*3.5));
}

// Отрисовывает конечный элемент (NAItem*) с центром в заданной позиции
void UDrawEngine::PaintItem(UGEDescription &ndescr)
{
 UBRect rect;
 int shift=0;

 if(ndescr.ContourWidth % 2)
  shift=std::ceil(ndescr.ContourWidth/2.0);
 else
  shift=std::ceil(ndescr.ContourWidth/2.0+1);

 GEngine->SetPenWidth(ndescr.ContourWidth);
 if (ndescr.ContourColor == ndescr.FillColor)
 {
        GEngine->SetPenColor(ndescr.ContourColor);
		GEngine->Rect(ndescr.X-ndescr.Radius, ndescr.Y-ndescr.Radius, ndescr.X+ndescr.Radius, ndescr.Y+ndescr.Radius, true);
 }
 else
 {
  if (ndescr.Highlight)
  {
   GEngine->Rect(ndescr.X-ndescr.Radius, ndescr.Y-ndescr.Radius, ndescr.X+ndescr.Radius, ndescr.Y+ndescr.Radius, false);
   GEngine->SetPenColor(SelectedColor);
   GEngine->Rect(ndescr.X-ndescr.Radius+shift, ndescr.Y-ndescr.Radius+shift, ndescr.X+ndescr.Radius-ndescr.ContourWidth, ndescr.Y+ndescr.Radius-ndescr.ContourWidth, true);
  }
  else
  {
   GEngine->SetPenColor(ndescr.ContourColor);
   GEngine->Rect(ndescr.X-ndescr.Radius, ndescr.Y-ndescr.Radius, ndescr.X+ndescr.Radius, ndescr.Y+ndescr.Radius, false);
   GEngine->SetPenColor(ndescr.FillColor);
   GEngine->Rect(ndescr.X-ndescr.Radius+shift, ndescr.Y-ndescr.Radius+shift, ndescr.X+ndescr.Radius-ndescr.ContourWidth, ndescr.Y+ndescr.Radius-ndescr.ContourWidth, true);
  }
 }
}

// Отрисовывает элемент-сеть (UANet*) с центром в заданной позиции
void UDrawEngine::PaintNet(UGEDescription &ndescr)
{
 int shift=0;

 if((ndescr.ContourWidth+1) % 2)
  shift=std::ceil((ndescr.ContourWidth+1)/2.0);
 else
  shift=std::ceil((ndescr.ContourWidth+1)/2.0+1);

 GEngine->SetPenWidth(ndescr.ContourWidth+1);
 if (ndescr.ContourColor == ndescr.FillColor)
 {
  GEngine->SetPenColor(ndescr.ContourColor);
  GEngine->Rect(ndescr.X-ndescr.Radius, ndescr.Y-ndescr.Radius, ndescr.X+ndescr.Radius, ndescr.Y+ndescr.Radius, true);
 }
 else
 {
  if (ndescr.Highlight)
  {
   GEngine->Rect(ndescr.X-ndescr.Radius, ndescr.Y-ndescr.Radius, ndescr.X+ndescr.Radius, ndescr.Y+ndescr.Radius, false);
   GEngine->SetPenColor(SelectedColor);
   GEngine->Rect(ndescr.X-ndescr.Radius+shift, ndescr.Y-ndescr.Radius+shift, ndescr.X+ndescr.Radius-ndescr.ContourWidth, ndescr.Y+ndescr.Radius-ndescr.ContourWidth, true);
  }
  else
  {
   GEngine->SetPenColor(ndescr.ContourColor);
   GEngine->Rect(ndescr.X-ndescr.Radius, ndescr.Y-ndescr.Radius, ndescr.X+ndescr.Radius, ndescr.Y+ndescr.Radius, false);
   GEngine->SetPenColor(ndescr.FillColor);
   GEngine->Rect(ndescr.X-ndescr.Radius+shift, ndescr.Y-ndescr.Radius+shift, ndescr.X+ndescr.Radius-ndescr.ContourWidth, ndescr.Y+ndescr.Radius-ndescr.ContourWidth, true);
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
  GEngine->Line(out.X + out.Radius, out.Y, in.X - in.Radius, in.Y);
  GEngine->Rect(in.X - in.Radius - out.LinkWidth,
            in.Y - out.LinkWidth,
            in.X - in.Radius + out.LinkWidth, in.Y + out.LinkWidth, true);
 }

 if (highlight)
 {
  PaintItem(out);
  PaintItem(in);
 }

 if (contour)
 {
 }

}
// Отрисовывает окружность
// ---------------------------

}
#endif

