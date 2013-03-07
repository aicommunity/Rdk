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
#include "UAGraphics.h"

namespace RDK {

/*
    ���������, ����������� ������ UContainer ��� ���������� �������
*/
struct UGEDescription
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

// ��� ��������
// 0 - �� ����������
// 1 - ������� ��� �������� ��������
// 2 - ������� - ���� � ��������� ���������
int Type;

// ����� ������ � �������
int NumInputs, NumOutputs;
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

// ���� ��������� ������
UColorT LinkColor;

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
class UDrawEngine
{
public: // ���� ������
typedef map<string,UGEDescription> DescriptionsTableT;
typedef map<string,UGEDescription>::iterator DescriptionsTableIteratorT;

typedef map<string,vector<DescriptionsTableIteratorT> > DescriptionsLinksTableT;
typedef map<string,vector<DescriptionsTableIteratorT> >::iterator DescriptionsLinksTableIteratorT;

protected: // ������
// ��������� �� ����
//UEPtr<UNet> Net;
Serialize::USerStorageXML NetXml;

// ������� ������������ ����� ��������� ���� � �������� ���������� ���������
DescriptionsTableT Descriptions;


// �����
DescriptionsLinksTableT Links;


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

// ����������� ������������
// ���������� ���������� X,Y=ZoomCoeff*Coord.X,Y
double ZoomCoeff;

// ������ ��������� � �����
MVector<double,3> Origin;
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
// ---------------------------

// ---------------------------
// ������ ������� � ������
// ---------------------------
// ���������� ��������� �� ��
//UEPtr<UNet> GetNet(void);
const Serialize::USerStorageXML& GetNetXml(void) const;

// ���������� ������� ������� ������������
UGEDescription& GetDescription(const string &name);

// ���������� ��� ������� �����������
const UDrawEngine::DescriptionsTableT& GetDescriptions(void);

// ���������� ��������� �� ������ �����������
UEPtr<UAGraphics> GetGEngine(void);

// ��������� ����� � ����� ��
// ���� net == 0 �� ��������� ����� �� ������� �� � ���������� true
//bool SetNet(UEPtr<UNet> net);
bool SetNetXml(Serialize::USerStorageXML &net_xml);

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


