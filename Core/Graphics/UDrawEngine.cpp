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
// ������������ � �����������
// ------------------------
UGEDescription::UGEDescription(void)
{
// ���������� �������� � ��������
Position=0;

// ������ ��������
Radius = 10;

// ���� ������� ��������
ContourColor = UColorT(0, 0, 0, 0);

// ������� ������� �������� � ��������
ContourWidth = 1;

// ���� ������� ��������
FillColor = UColorT(180, 180, 180, 0);

LinkColor = UColorT(0, 0, 255, 0);

// ������� ������� �����
LinkWidth = 1;

// ���������� ������ i
Index = 0;

// 3D ������ (i,j,k)
Position=0;

// ��� ��������
Type = 0;
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
Type=copy.Type;
NumInputs=copy.NumInputs;
NumOutputs=copy.NumOutputs;
Radius=copy.Radius;
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

 BackgroundLineStep=10;

 CanvasWidth = 0;
 CanvasHeight = 0;

 ZoomCoeff=30;

 Origin=0;
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
// ---------------------------

// ---------------------------
// ������ ������� � ������
// ---------------------------
// ���������� ��������� �� ��
/*UEPtr<UANet> UDrawEngine::GetNet(void)
{
 return Net;
} */
const Serialize::USerStorageXML& UDrawEngine::GetNetXml(void) const
{
 return NetXml;
}


// ���������� ������� ������� ������������
UGEDescription& UDrawEngine::GetDescription(const string &name)
{
 return Descriptions[name];
}

// ���������� ��� ������� �����������
const map<string, UGEDescription>& UDrawEngine::GetDescriptions(void)
{
 return Descriptions;
}

// ���������� ��������� �� ������ �����������
UEPtr<UAGraphics> UDrawEngine::GetGEngine(void)
{
 return GEngine;
}

bool UDrawEngine::SetNetXml(Serialize::USerStorageXML &net_xml)
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

// ����� ������� �� �������� ����������� � �����������
std::string UDrawEngine::FindComponent(int x, int y)
{
 map<string, UGEDescription>::iterator I, J;
 I = Descriptions.begin();
 J = Descriptions.end();
 std::string name;

 for(;I != J;++I)
 {
  double dist=sqrt(double((x-I->second.Position.x)*(x-I->second.Position.x)+(y-I->second.Position.y)*(y-I->second.Position.y)));

  name=I->first;
  if(dist<=I->second.Radius)
   return I->first;
 }

 return string();
}


// ���������� ���������� � �������� ����������, � ��������� ����� �����������������
void UDrawEngine::MoveComponent(const string &name, int x, int y)
{
 map<string, UGEDescription>::iterator I=Descriptions.find(name);
 if(I == Descriptions.end())
  return;
 UGEDescription &descr=I->second;

 if(fabs(descr.Position.x-x)>BackgroundLineStep)
  descr.Position.x+=(int(x-descr.Position.x)/BackgroundLineStep)*BackgroundLineStep;

 if(fabs(descr.Position.y-y)>BackgroundLineStep)
  descr.Position.y+=(int(y-descr.Position.y)/BackgroundLineStep)*BackgroundLineStep;
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

   NetXml.SelectNode("Parameters");
   descr.NumInputs=NetXml.ReadInteger("NumInputs",0);
   descr.NumOutputs=NetXml.ReadInteger("NumOutputs",0);

   if(NetXml.SelectNode("Coord"))
   {
	RDK::MVector<double,3> coord(1);
	RDK::Serialize::operator >> (NetXml,coord);
	NetXml.SelectUp();

	if(!coord>0)
	 descr.Position=coord*ZoomCoeff+Origin;
	else
	 descr.Position=i*30.0;
   }

   descr.Header=dI->first;

   NetXml.SelectUp();
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
// �������� ������ �����������
// ---------------------------
// ����� ����������� ���� ���������
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

// ����� ���������� ����������� ���������� �������� ����
// ���������� false ���� ������� �� ������
bool UDrawEngine::Draw(const string &name)
{
 map<string, UGEDescription>::iterator I = Descriptions.find(name);

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

// ����� ����������� ����
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
  PaintIndicator(ndescr.Position.x - ndescr.Radius,
		ndescr.Position.y + ndescr.Radius + ndescr.ContourWidth, ndescr.Radius * 2,
		ndescr.Radius / 2, ndescr.Indicators[i], 1, (i<FullIndicatorColors.size())?FullIndicatorColors[i]:BackgroundColor,
		(i<EmptyIndicatorColors.size())?EmptyIndicatorColors[i]:BackgroundColor);

 // ������������ ���
 GEngine->SetPenColor(ndescr.ContourColor);
  GEngine->Text(ndescr.Header,ndescr.Position.x+ndescr.Radius*1.5,static_cast<int>(ndescr.Position.y-ndescr.Radius/2));
}

// ������������ �������� ������� (NAItem*) � ������� � �������� �������
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
		GEngine->Rect(ndescr.Position.x-ndescr.Radius, ndescr.Position.y-ndescr.Radius, ndescr.Position.x+ndescr.Radius, ndescr.Position.y+ndescr.Radius, true);
 }
 else
 {
  if (ndescr.Highlight)
  {
   GEngine->Rect(ndescr.Position.x-ndescr.Radius, ndescr.Position.y-ndescr.Radius, ndescr.Position.x+ndescr.Radius, ndescr.Position.y+ndescr.Radius, false);
   GEngine->SetPenColor(SelectedColor);
   GEngine->Rect(ndescr.Position.x-ndescr.Radius+shift, ndescr.Position.y-ndescr.Radius+shift, ndescr.Position.x+ndescr.Radius-ndescr.ContourWidth, ndescr.Position.y+ndescr.Radius-ndescr.ContourWidth, true);
  }
  else
  {
   GEngine->SetPenColor(ndescr.ContourColor);
   GEngine->Rect(ndescr.Position.x-ndescr.Radius, ndescr.Position.y-ndescr.Radius, ndescr.Position.x+ndescr.Radius, ndescr.Position.y+ndescr.Radius, false);
   GEngine->SetPenColor(ndescr.FillColor);
   GEngine->Rect(ndescr.Position.x-ndescr.Radius+shift, ndescr.Position.y-ndescr.Radius+shift, ndescr.Position.x+ndescr.Radius-ndescr.ContourWidth, ndescr.Position.y+ndescr.Radius-ndescr.ContourWidth, true);
  }
 }
}

// ������������ �������-���� (UANet*) � ������� � �������� �������
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
  GEngine->Rect(ndescr.Position.x-ndescr.Radius, ndescr.Position.y-ndescr.Radius, ndescr.Position.x+ndescr.Radius, ndescr.Position.y+ndescr.Radius, true);
 }
 else
 {
  if (ndescr.Highlight)
  {
   GEngine->Rect(ndescr.Position.x-ndescr.Radius, ndescr.Position.y-ndescr.Radius, ndescr.Position.x+ndescr.Radius, ndescr.Position.y+ndescr.Radius, false);
   GEngine->SetPenColor(SelectedColor);
   GEngine->Rect(ndescr.Position.x-ndescr.Radius+shift, ndescr.Position.y-ndescr.Radius+shift, ndescr.Position.x+ndescr.Radius-ndescr.ContourWidth, ndescr.Position.y+ndescr.Radius-ndescr.ContourWidth, true);
  }
  else
  {
   GEngine->SetPenColor(ndescr.ContourColor);
   GEngine->Rect(ndescr.Position.x-ndescr.Radius, ndescr.Position.y-ndescr.Radius, ndescr.Position.x+ndescr.Radius, ndescr.Position.y+ndescr.Radius, false);
   GEngine->SetPenColor(ndescr.FillColor);
   GEngine->Rect(ndescr.Position.x-ndescr.Radius+shift, ndescr.Position.y-ndescr.Radius+shift, ndescr.Position.x+ndescr.Radius-ndescr.ContourWidth, ndescr.Position.y+ndescr.Radius-ndescr.ContourWidth, true);
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
  GEngine->SetPenColor(out.LinkColor);
  GEngine->SetPenWidth(out.LinkWidth);
  RDK::MVector<double,3> &c_out=out.Position;
  RDK::MVector<double,3> &c_in=in.Position;
  int x_diff=fabs(c_out.x-c_in.x), y_diff=fabs(c_out.y-c_in.y);
  if(c_out.x<c_in.x && x_diff>y_diff)
  {
   GEngine->Line(c_out.x + out.Radius, c_out.y, c_in.x - in.Radius, c_in.y);
   GEngine->Rect(in.Position.x - in.Radius - out.LinkWidth,
			c_in.y - out.LinkWidth,
			c_in.x - in.Radius + out.LinkWidth, c_in.y + out.LinkWidth, true);
  }
  else
  if(c_out.y<c_in.y && x_diff<=y_diff)
  {
   GEngine->Line(c_out.x, c_out.y + out.Radius, c_in.x, c_in.y - in.Radius);
   GEngine->Rect(in.Position.x - out.LinkWidth,
			c_in.y - in.Radius - out.LinkWidth,
			c_in.x + out.LinkWidth, c_in.y - in.Radius + out.LinkWidth, true);
  }
  else
  if(c_out.x>=c_in.x && x_diff>y_diff)
  {
   GEngine->Line(c_out.x - out.Radius, c_out.y, c_in.x + in.Radius, c_in.y);
   GEngine->Rect(in.Position.x + in.Radius - out.LinkWidth,
			c_in.y - out.LinkWidth,
			c_in.x + in.Radius + out.LinkWidth, c_in.y + out.LinkWidth, true);
  }
  else
  if(c_out.y>=c_in.y && x_diff<=y_diff)
  {
   GEngine->Line(c_out.x, c_out.y - out.Radius, c_in.x, c_in.y + in.Radius);
   GEngine->Rect(in.Position.x - out.LinkWidth,
			c_in.y + in.Radius - out.LinkWidth,
			c_in.x + out.LinkWidth, c_in.y + in.Radius + out.LinkWidth, true);
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

