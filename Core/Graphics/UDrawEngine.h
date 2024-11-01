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
#ifndef UDRAW_ENGINE_GRAPHICS_H
#define UDRAW_ENGINE_GRAPHICS_H

//#include "../Engine/UNet.h"
#include "../Serialize/USerStorageXML.h"
#include "../Utilities/USupport.h"
#include "../Engine/UEPtr.h"
#include "../Math/MVector.h"
#include "UAGraphics.h"

namespace RDK {

/*
    ���������, ����������� ������ UContainer ��� ���������� �������
*/
struct RDK_LIB_TYPE UGEDescription
{
// ------------------------
// ����������������� ���������
// ------------------------
// ���������� ������ i
int Index;

// 3D ������
MVector<double, 3> Position;

// �������� ��� ��������
std::string Header;

// ����� ��������
std::string ClassName;

// ��� ��������
// 0 - �� ����������
// 1 - ������� ��� �������� ��������
// 2 - ������� - ���� � ��������� ���������
int Type;

// ����� ������ � �������
int NumInputs, NumOutputs;

// ��������� ����� Activity
bool Activity;
// ------------------------

// ------------------------
// �������������� ���������
// ------------------------
// ������� �������� � ��������
int Width, Height;

// ���� ������� ��������
UColorT ContourColor;

// ������� ������� �������� � ��������
int ContourWidth;

// ���� ������� ��������
UColorT FillColor;

// ���� ������� ����������� ��������
UColorT InactiveFillColor;

// ���� ��������� ������
UColorT LinkColor;

// ���� ������ ����������� ��������
UColorT InactiveFontColor;

// ������� ������� �����
int LinkWidth;

// ���� ��������� ����������
bool Highlight;
// ------------------------

// ------------------------
// ����������
// ------------------------
// ���������� ����������� � %
// (���� -1, �� ��������)
std::vector<double> Indicators;
// ------------------------

// ------------------------
// ������������ � �����������
// ------------------------
UGEDescription(void);
UGEDescription(const UGEDescription &copy);
~UGEDescription(void);
// ------------------------

UGEDescription& operator = (const UGEDescription &copy);
};

/*
    ����� ������������ ����������� ��
*/
class RDK_LIB_TYPE UDrawEngine
{
public: // ���� ������
typedef map<string,UGEDescription> DescriptionsTableT;
typedef map<string,UGEDescription>::iterator DescriptionsTableIteratorT;

typedef map<string,vector<DescriptionsTableIteratorT> > DescriptionsLinksTableT;
typedef map<string,vector<DescriptionsTableIteratorT> >::iterator DescriptionsLinksTableIteratorT;

protected: // ������
// ��������� �� ����
//UEPtr<UNet> Net;
USerStorageXML NetXml;

// ������� ������������ ����� ��������� ���� � �������� ���������� ���������
DescriptionsTableT Descriptions;


// �����
DescriptionsLinksTableT Links;

/// ������
RDK::UBitmapFontCollection Fonts;

// ������ ��� ����������� ����
UEPtr<UAGraphics> GEngine;

protected: // ���������
// ---------------------------
// ��������� �����������
// ---------------------------
// �������� ������ � ������ �����������
int CanvasWidth, CanvasHeight;

// ������� �������� � �������� �� ���������
int RectWidth, RectHeight;

// ����� ��������� � ������
int ElementsXRes;

// ����� ����� ���������
int ElementsYRes;

// ���� ������ �����������
std::vector<UColorT> EmptyIndicatorColors;

// ���� ������ �����������
std::vector<UColorT> FullIndicatorColors;

// ���� ����
UColorT BackgroundColor;

// ���� ����� ����
UColorT BackgroundLineColor;

// ��� ����� ����
int BackgroundLineStep;

// ���� ����������� ��������
UColorT SelectedColor;

// ���� ����������� ��������
UColorT InactiveSelectedColor;

// ���� �������� �����
UColorT InputLinksColor;

// ���� ��������� �����
UColorT OutputLinksColor;

// ���� ����� �� ���������
UColorT DefaultLinksColor;

// ����������� ������������
// ���������� ���������� X,Y=ZoomCoeff*Coord.X,Y
double ZoomCoeff;

// ������ ��������� � �����
MVector<double,3> Origin;

/// ���� ��������� ������ ����� �����
bool ShowBackgroundLines;
// ---------------------------


public: // ������
// ---------------------------
// ������������ � �����������
// ---------------------------
UDrawEngine(void);
~UDrawEngine(void);
// ---------------------------

// ---------------------------
// ������ ���������� �����������
// ---------------------------
// ����������� ������������
// ���������� ���������� X,Y=ZoomCoeff*Coord.X,Y
double GetZoomCoeff(void) const;

// ������ ��������� � �����
MVector<double,3> GetOrigin(void) const;

// ������� �������� � �������� �� ���������
int GetRectWidth(void) const;
bool SetRectWidth(int value);
int GetRectHeight(void) const;
bool SetRectHeight(int value);

bool SetShowBackgroundLines(bool value);
bool GetShowBackgroundLines(void) const;
// ---------------------------

// ---------------------------
// ������ ������� � ������
// ---------------------------
// ���������� ��������� �� ��
//UEPtr<UNet> GetNet(void);
const USerStorageXML& GetNetXml(void) const;

// ���������� ������� ������� ������������
UGEDescription& GetDescription(const string &name);

// ���������� ��� ������� �����������
const UDrawEngine::DescriptionsTableT& GetDescriptions(void);

// ���������� ��������� �� ������ �����������
UEPtr<UAGraphics> GetGEngine(void);

// ��������� ����� � ����� ��
// ���� net == 0 �� ��������� ����� �� ������� �� � ���������� true
//bool SetNet(UEPtr<UNet> net);
bool SetNetXml(USerStorageXML &net_xml);

// ��������� ����� � ����� ��
// ���� engine == 0 �� ���������� false � �� ������� ������
bool SetEngine(UEPtr<UAGraphics> engine);

// ��������� ������� ������������
void UpdateDescriptions(void);

// �������� �������� ���������, ��������� ���� ��������� � ���������
void SelectSingleComponent(const string &name);

// ����� ������� �� �������� ����������� � �����������
std::string FindComponent(int x, int y);

// ���������� ���������� � �������� ����������, � ��������� ����� �����������������
void MoveComponent(const string &name, int x, int y);

// ��������� ������� ���� ��������� � ������������ � ��������� �� ���������
void UpdateAllElementsSize(void);

/// ���������� ������������� ������ �����
void CalcRecommendSize(int &width, int &height);

/// ������
RDK::UBitmapFontCollection& GetFonts(void);
void SetFonts(RDK::UBitmapFontCollection& value);
// ---------------------------

// ---------------------------
// ������ ���������� ��������� �� ������������ ����
// ---------------------------
// ����� ��������������� ������������� ������������ ��������� �� ����
void BuildDestinations(void);

// ����� ��������������� ������������� ���������������� ���������
// (���������������� �������� ����� ���������� -1,-1)
void UpdateDestinations(void);

// ������������� ���������� ���������� �������� id
// ���������� false ���� ������� �� ������ ��� ���������� �����������
// ����� �� ��������� ��������-�� ���������� �������
bool SetDestination(const string &name, int x, int y);

// ��������� ������� ������ ���������
void ParseLinks(void);
// ---------------------------

// ---------------------------
// �������� ������ �����������
// ---------------------------
// ����� ����������� ���� ���������
void Draw(void);

// ����� ���������� ����������� ���������� �������� ����
// ���������� false ���� ������� �� ������
bool Draw(const string &name);

// ����� ����������� ������ ����
// links==true - ���������� ����� ������ �������
// highlight==true - ������������ ��� ���������� ��������� � ����
// (��� ������ �������������� � ������, ���� �� ���� ���� �������� �����)
// contour==true - �������� �������� ���������� ��������� � ����
// (� ������ ����� ������� � �� ��������� ����������)
bool DrawLinks(bool links=true, bool highlight=false, bool contour=false);

// ����� ����������� ����
bool DrawBackground(void);
// ---------------------------

// ---------------------------
// ��������������� ������ �����������
// ---------------------------
// ������������ ������� � �������� �������
void Paint(UGEDescription &ndescr);

// ������������ �������� ������� (NAItem*) � ������� � �������� �������
void PaintItem(UGEDescription &ndescr);

// ������������ �������-���� (UNet*) � ������� � �������� �������
void PaintNet(UGEDescription &ndescr);

// ������������ �����
void PaintLink(UGEDescription &out, UGEDescription &in,
               bool links=true, bool highlight=true, bool contour=true);


// ������������ ��������� � ��������� �����������
// ����������� direction �������� ��������� �������
// 1 - ��������� ����������� ����� �������
// 2 - ��������� ����������� ������ ������
// 3 - ��������� ����������� ������ ����
// 4 - ��������� ����������� ����� �����
void PaintIndicator(int x,int y, int width, int height,
                   double precent, int direction, UColorT full, UColorT empty);

// ������������ ����������
// ---------------------------
};

}
#endif


