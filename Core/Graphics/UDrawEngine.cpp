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

#include <cmath>
#include "UDrawEngine.h"
#include "../Math/MUXMLSerialize.h"
#include "../Application/UIVisualController.h"

namespace RDK {

// ------------------------
// ������������ � �����������
// ------------------------
UGEDescription::UGEDescription(void)
{
 // ���������� �������� � ��������
 Position=0;

 // ������ ��������
 Width = 80; Height=25;

 // ���� ������� ��������
 ContourColor = UColorT(0, 0, 0, 0);

 // ������� ������� �������� � ��������
 ContourWidth = 1;

 InactiveFontColor=UColorT(100,100,100,0);

 // ���� ������� ��������
 FillColor = UColorT(180, 180, 180, 0);

 InactiveFillColor=UColorT(210,210,210,0);

 LinkColor = UColorT(0, 0, 255, 0);

 // ������� ������� �����
 LinkWidth = 1;

 // ���������� ������ i
 Index = 0;

 // 3D ������ (i,j,k)
 Position=0;

 // ��� ��������
 Type = 0;

 Activity=true;

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
 Activity=copy.Activity;
 NumInputs=copy.NumInputs;
 NumOutputs=copy.NumOutputs;
 Width=copy.Width;
 Height=copy.Height;
 ContourColor=copy.ContourColor;
 ContourWidth=copy.ContourWidth;
 InactiveFillColor=copy.InactiveFillColor;
 FillColor=copy.FillColor;
 LinkColor=copy.LinkColor;
 LinkWidth=copy.LinkWidth;
 Highlight=copy.Highlight;
 Indicators=copy.Indicators;
 InactiveFontColor=copy.InactiveFontColor;

 return *this;
}



/* ******************************************************************* */

/* ******************************************************************* */
// ---------------------------
// ������������ � �����������
// ---------------------------
UDrawEngine::UDrawEngine(void)
{
// Net = 0;
 GEngine = 0;

 // ���� ����
 BackgroundColor = UColorT(255, 255, 255, 255);

 BackgroundLineColor=UColorT(200, 200, 200, 0);

 // ���� ����������� ��������
 SelectedColor = UColorT(128, 128, 128, 0);

 InactiveSelectedColor=UColorT(128, 128, 128, 0);

 // ���� �������� �����
 InputLinksColor=UColorT(255, 0, 0, 0);

 // ���� ��������� �����
 OutputLinksColor=UColorT(0, 0, 255, 0);

 // ���� ����� �� ���������
 DefaultLinksColor=UColorT(180, 180, 180, 0);

 BackgroundLineStep=10;

 CanvasWidth = 0;
 CanvasHeight = 0;

 ZoomCoeff=30;

 Origin=0;

 // ������� �������� � �������� �� ���������
 RectWidth=80;
 RectHeight=25;

 ElementsXRes=1;
 ElementsYRes=1;

 ShowBackgroundLines=true;
}

UDrawEngine::~UDrawEngine(void)
{
}
// ---------------------------

// ---------------------------
// ������ ���������� �����������
// ---------------------------
// ����������� ������������
// ���������� ���������� X,Y=ZoomCoeff*Coord.X,Y
double UDrawEngine::GetZoomCoeff(void) const
{
 return ZoomCoeff;
}

// ������ ��������� � �����
MVector<double,3> UDrawEngine::GetOrigin(void) const
{
 return Origin;
}

// ������� �������� � �������� �� ���������
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

bool UDrawEngine::SetShowBackgroundLines(bool value)
{
 ShowBackgroundLines=value;
 return true;
}

bool UDrawEngine::GetShowBackgroundLines(void) const
{
 return ShowBackgroundLines;
}
// ---------------------------

// ---------------------------
// ������ ������� � ������
// ---------------------------
// ���������� ��������� �� ��
/*UEPtr<UNet> UDrawEngine::GetNet(void)
{
 return Net;
} */
const USerStorageXML& UDrawEngine::GetNetXml(void) const
{
 return NetXml;
}


// ���������� ������� ������� ������������
UGEDescription& UDrawEngine::GetDescription(const string &name)
{
 return Descriptions[name];
}

// ���������� ��� ������� �����������
const UDrawEngine::DescriptionsTableT& UDrawEngine::GetDescriptions(void)
{
 return Descriptions;
}

// ���������� ��������� �� ������ �����������
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

// ��������� ����� � ����� ��
// ���� engine == 0 �� ���������� false � �� ������� ������
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

// ��������� ������� ������������
void UDrawEngine::UpdateDescriptions(void)
{
}

// �������� �������� ���������, ��������� ���� ��������� � ���������
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

// ����� ������� �� �������� ����������� � �����������
std::string UDrawEngine::FindComponent(int x, int y)
{
 DescriptionsTableIteratorT I, J;
 I = Descriptions.begin();
 J = Descriptions.end();
 std::string name;

 for(;I != J;++I)
 {
//  double dist=sqrt(double((x-I->second.Position[0])*(x-I->second.Position[0])+(y-I->second.Position[1])*(y-I->second.Position[1])));

//  name=I->first;
  if(abs(int(x-I->second.Position[0]))<=I->second.Width && abs(int(y-I->second.Position[1]))<=I->second.Height)
//  if(dist<=I->second.Height)// ��������!!
   return I->first;
 }

 return string();
}


// ���������� ���������� � �������� ����������, � ��������� ����� �����������������
void UDrawEngine::MoveComponent(const string &name, int x, int y)
{
 DescriptionsTableIteratorT I=Descriptions.find(name);
 if(I == Descriptions.end())
  return;
 UGEDescription &descr=I->second;

 if(fabs(descr.Position[0]-x)>BackgroundLineStep)
  descr.Position[0]+=(int(x-descr.Position[0])/BackgroundLineStep)*BackgroundLineStep;

 if(fabs(descr.Position[1]-y)>BackgroundLineStep)
  descr.Position[1]+=(int(y-descr.Position[1])/BackgroundLineStep)*BackgroundLineStep;
}

// ��������� ������� ���� ��������� � ������������ � ��������� �� ���������
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


/// ���������� ������������� ������ �����
void UDrawEngine::CalcRecommendSize(int &width, int &height)
{
 if(Descriptions.empty())
  return;

 DescriptionsTableIteratorT I, J;
 I = Descriptions.begin();
 J = Descriptions.end();

 for(;I != J;++I)
 {
  int temp_width=int(I->second.Position(0))+I->second.Width;
  if(width<temp_width)
   width=temp_width;
  int temp_height=int(I->second.Position(1))+I->second.Height;
  if(height<temp_height)
   height=temp_height;
 }
}

/// ������
RDK::UBitmapFontCollection& UDrawEngine::GetFonts(void)
{
 return Fonts;
}

void UDrawEngine::SetFonts(RDK::UBitmapFontCollection& value)
{
 Fonts=value;
}
// ---------------------------

// ---------------------------
// ������ ���������� ��������� �� ������������ ����
// ---------------------------
// ����� ��������������� ������������� ������������ ��������� �� ����
void UDrawEngine::BuildDestinations(void)
{
 Descriptions.clear();
 UpdateDestinations();
}

// ����� ��������������� ������������� ���������������� ���������
// (���������������� �������� ����� ���������� -1,-1)
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


	descr.Activity=NetXml.ReadBool("Activity",true);
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

// ������������� ���������� ���������� �������� id
// ���������� false ���� ������� �� ������ ��� ���������� �����������
// ����� �� ��������� ��������-�� ���������� �������
bool UDrawEngine::SetDestination(const string &name, int x, int y)
{
 return true;
}

// ��������� ������� ������ ���������
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
// �������� ������ �����������
// ---------------------------
// ����� ����������� ���� ���������
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

// ����� ���������� ����������� ���������� �������� ����
// ���������� false ���� ������� �� ������
bool UDrawEngine::Draw(const string &name)
{
 DescriptionsTableIteratorT I = Descriptions.find(name);

 if (I == Descriptions.end())
  return false;

 Paint(I->second);
 return true;
}

// ����� ����������� ������ ����
// links==true - ���������� ����� ������ �������
// highlight==true - ������������ ��� ���������� ��������� � ����
// (��� ������ �������������� � ������, ���� �� ���� ���� �������� �����)
// contour==true - �������� �������� ���������� ��������� � ����
// (� ������ ����� ������� � �� ��������� ����������)
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

// ����� ����������� ����
bool UDrawEngine::DrawBackground(void)
{
 GEngine->SetPenColor(BackgroundColor);
 GEngine->SetPenWidth(1);
 GEngine->Rect(0, 0, CanvasWidth, CanvasHeight, true);

 GEngine->SetPenColor(BackgroundLineColor);
 GEngine->SetPenWidth(1);

 if(ShowBackgroundLines)
 {
  for(int i=0;i<CanvasWidth;i+=BackgroundLineStep)
   GEngine->Line(i,0,i,CanvasHeight-1);

  for(int i=0;i<CanvasHeight;i+=BackgroundLineStep)
   GEngine->Line(0,i,CanvasWidth-1,i);
 }

 GEngine->SetPenColor(BackgroundLineColor);
 GEngine->Rect(0, 0, BackgroundLineStep, CanvasHeight, true);
 GEngine->Rect(0, 0, CanvasWidth, BackgroundLineStep, true);
 GEngine->Rect(CanvasWidth-BackgroundLineStep, 0, CanvasWidth, CanvasHeight, true);
 GEngine->Rect(0, CanvasHeight-BackgroundLineStep, CanvasWidth, CanvasHeight, true);
 return true;
}
// ---------------------------

// ---------------------------
// ��������������� ������ �����������
// ---------------------------
// ������������ ������� � �������� �������
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

 // ������������ ����������
 for(size_t i=0;i<ndescr.Indicators.size();i++)
  PaintIndicator(int(ndescr.Position[0]) - ndescr.Width,
		int(ndescr.Position[1]) + ndescr.Height + ndescr.ContourWidth, ndescr.Width * 2,
		ndescr.Height / 2, ndescr.Indicators[i], 1, (i<FullIndicatorColors.size())?FullIndicatorColors[i]:BackgroundColor,
		(i<EmptyIndicatorColors.size())?EmptyIndicatorColors[i]:BackgroundColor);

 // ������������ ���
 UColorT font_color;
 if(ndescr.Activity)
  font_color=ndescr.ContourColor;
 else
  font_color=ndescr.InactiveFontColor;

 GEngine->SetPenColor(font_color);
// GEngine->Text(ndescr.Header,int(ndescr.Position[0]+ndescr.Radius*1.5),static_cast<int>(int(ndescr.Position[1])-ndescr.Radius/2));
 UBRect rect;
 rect.X=int(ndescr.Position[0])-(ndescr.Width-2);
 rect.Y=int(ndescr.Position[1])-ndescr.Height;
 rect.Width=(ndescr.Width-2)*2;
 rect.Height=ndescr.Height;
 UAFont *font=GEngine->GetFont();
 GEngine->TextRect(ndescr.Header,rect,alCenter);
 rect.Y+=int(ndescr.Height*1.2);
 UAFont *class_font=Fonts.GetFont(font->GetName(),font->GetSize()-4);
 if(class_font)
 {
  GEngine->SetFont(class_font);
  GEngine->TextRect(ndescr.ClassName,rect,alRight);
  GEngine->SetFont(font);
 }
}

// ������������ �������� ������� (NAItem*) � ������� � �������� �������
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
  GEngine->Rect(int(int(ndescr.Position[0]))-ndescr.Width, int(int(ndescr.Position[1]))-ndescr.Height, int(int(ndescr.Position[0]))+ndescr.Width, int(int(ndescr.Position[1]))+ndescr.Height, true);
 }
 else
 {
  UColorT common_color;
  UColorT selected_color;
  if(ndescr.Activity)
  {
   common_color=ndescr.FillColor;
   selected_color=SelectedColor;
  }
  else
  {
   common_color=ndescr.InactiveFillColor;
   selected_color=InactiveSelectedColor;
  }

  if (ndescr.Highlight)
  {
   GEngine->Rect(int(int(ndescr.Position[0]))-ndescr.Width, int(int(ndescr.Position[1]))-ndescr.Height, int(int(ndescr.Position[0]))+ndescr.Width, int(int(ndescr.Position[1]))+ndescr.Height, false);
   GEngine->SetPenColor(selected_color);
   GEngine->Rect(int(int(ndescr.Position[0]))-ndescr.Width+shift, int(int(ndescr.Position[1]))-ndescr.Height+shift, int(int(ndescr.Position[0]))+ndescr.Width-ndescr.ContourWidth, int(int(ndescr.Position[1]))+ndescr.Height-ndescr.ContourWidth, true);
  }
  else
  {
   GEngine->SetPenColor(ndescr.ContourColor);
   GEngine->Rect(int(int(ndescr.Position[0]))-ndescr.Width, int(int(ndescr.Position[1]))-ndescr.Height, int(int(ndescr.Position[0]))+ndescr.Width, int(int(ndescr.Position[1]))+ndescr.Height, false);
   GEngine->SetPenColor(common_color);
   GEngine->Rect(int(int(ndescr.Position[0]))-ndescr.Width+shift, int(int(ndescr.Position[1]))-ndescr.Height+shift, int(int(ndescr.Position[0]))+ndescr.Width-ndescr.ContourWidth, int(ndescr.Position[1])+ndescr.Height-ndescr.ContourWidth, true);
  }
 }
}

// ������������ �������-���� (UNet*) � ������� � �������� �������
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
  GEngine->Rect(int(int(ndescr.Position[0]))-ndescr.Width, int(int(ndescr.Position[1]))-ndescr.Height, int(int(ndescr.Position[0]))+ndescr.Width, int(int(ndescr.Position[1]))+ndescr.Height, true);
 }
 else
 {
  UColorT common_color;
  UColorT selected_color;
  if(ndescr.Activity)
  {
   common_color=ndescr.FillColor;
   selected_color=SelectedColor;
  }
  else
  {
   common_color=ndescr.InactiveFillColor;
   selected_color=InactiveSelectedColor;
  }

  if (ndescr.Highlight)
  {
   GEngine->Rect(int(ndescr.Position[0])-ndescr.Width, int(ndescr.Position[1])-ndescr.Height, int(ndescr.Position[0])+ndescr.Width, int(ndescr.Position[1])+ndescr.Height, false);
   GEngine->SetPenColor(SelectedColor);
   GEngine->Rect(int(ndescr.Position[0])-ndescr.Width+shift, int(ndescr.Position[1])-ndescr.Height+shift, int(ndescr.Position[0])+ndescr.Width-ndescr.ContourWidth, int(ndescr.Position[1])+ndescr.Height-ndescr.ContourWidth, true);
  }
  else
  {
   GEngine->SetPenColor(ndescr.ContourColor);
   GEngine->Rect(int(ndescr.Position[0])-ndescr.Width, int(ndescr.Position[1])-ndescr.Height, int(ndescr.Position[0])+ndescr.Width, int(ndescr.Position[1])+ndescr.Height, false);
   GEngine->SetPenColor(ndescr.FillColor);
   GEngine->Rect(int(ndescr.Position[0])-ndescr.Width+shift, int(ndescr.Position[1])-ndescr.Height+shift, int(ndescr.Position[0])+ndescr.Width-ndescr.ContourWidth, int(ndescr.Position[1])+ndescr.Height-ndescr.ContourWidth, true);
  }
 }
}

// ������������ ��������� � ��������� �����������
// ����������� direction �������� ��������� �������
// 1 - ��������� ����������� ����� �������
// 2 - ��������� ����������� ������ ������
// 3 - ��������� ����������� ������ ����
// 4 - ��������� ����������� ����� �����
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

// ������������ �����
void UDrawEngine::PaintLink(UGEDescription &out, UGEDescription &in,
    bool links, bool highlight, bool contour)
{
 if(!GEngine)
  return;

 if (links)
 {
  if(out.Highlight)
  {
   GEngine->SetPenColor(OutputLinksColor);
  }
  else
  if(in.Highlight)
  {
   GEngine->SetPenColor(InputLinksColor);
  }
  else
  {
   GEngine->SetPenColor(DefaultLinksColor);
  }



  if(!out.Highlight && !in.Highlight)
   GEngine->SetPenColor(DefaultLinksColor);

  GEngine->SetPenWidth(out.LinkWidth);
  RDK::MVector<double,3> &c_out=out.Position;
  RDK::MVector<double,3> &c_in=in.Position;
  double x_diff=fabs(c_out[0]-c_in[0]), y_diff=fabs(c_out[1]-c_in[1]);
  if(c_out[0]<c_in[0] && x_diff>y_diff)
  {
   GEngine->Line(int(c_out[0]) + out.Width, int(c_out[1]), int(c_in[0]) - in.Width, int(c_in[1]));
   GEngine->Rect(int(in.Position[0]) - in.Width - out.LinkWidth,
			int(c_in[1]) - out.LinkWidth,
			int(c_in[0]) - in.Width + out.LinkWidth, int(c_in[1]) + out.LinkWidth, true);
  }
  else
  if(c_out[1]<c_in[1] && x_diff<=y_diff)
  {
   GEngine->Line(int(c_out[0]), int(c_out[1]) + out.Height, int(c_in[0]), int(c_in[1]) - in.Height);
   GEngine->Rect(int(in.Position[0]) - out.LinkWidth,
			int(c_in[1]) - in.Height - out.LinkWidth,
			int(c_in[0]) + out.LinkWidth, int(c_in[1]) - in.Height + out.LinkWidth, true);
  }
  else
  if(c_out[0]>=c_in[0] && x_diff>y_diff)
  {
   GEngine->Line(int(c_out[0]) - out.Width, int(c_out[1]), int(c_in[0]) + in.Width, int(c_in[1]));
   GEngine->Rect(int(in.Position[0]) + in.Width - out.LinkWidth,
			int(c_in[1]) - out.LinkWidth,
			int(c_in[0]) + in.Width + out.LinkWidth, int(c_in[1]) + out.LinkWidth, true);
  }
  else
  if(c_out[1]>=c_in[1] && x_diff<=y_diff)
  {
   GEngine->Line(int(c_out[0]), int(c_out[1]) - out.Height, int(c_in[0]), int(c_in[1]) + in.Height);
   GEngine->Rect(int(in.Position[0]) - out.LinkWidth,
			int(c_in[1]) + in.Height - out.LinkWidth,
			int(c_in[0]) + out.LinkWidth, int(c_in[1]) + in.Height + out.LinkWidth, true);
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
// ������������ ����������
// ---------------------------

}
#endif

