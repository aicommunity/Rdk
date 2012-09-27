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

//#include "../Engine/UANet.h"
#include "../Serialize/USerStorageXML.h"
#include "../Utilities/USupport.h"
#include "../Engine/UEPtr.h"
#include "UAGraphics.h"

namespace RDK {

/*
    ���������, ����������� ������ UAContainer ��� ���������� �������
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
// ���������� �������� � ��������
int X,Y;

// ������ �������� � ��������
int Radius;

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
~UGEDescription(void);
// ------------------------
};

/*
    ����� ������������ ����������� ��
*/
class UDrawEngine
{
protected: // ������
// ��������� �� ����
//UEPtr<UANet> Net;
Serialize::USerStorageXML NetXml;

// ������� ������������ ����� ��������� ���� � �������� ���������� ���������
map<string,UGEDescription> Descriptions;


// �����
map<string,vector<map<string,UGEDescription>::iterator> > Links;


// ������ ��� ����������� ����
UEPtr<UAGraphics> GEngine;

protected: // ���������
// ---------------------------
// ��������� �����������
// ---------------------------
// �������� ������ � ������ �����������
int CanvasWidth, CanvasHeight;

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

// ���� ����������� ��������
UColorT SelectedColor;
// ---------------------------


public: // ������
// ---------------------------
// ������������ � �����������
// ---------------------------
UDrawEngine(void);
~UDrawEngine(void);
// ---------------------------

// ---------------------------
// ������ ������� � ������
// ---------------------------
// ���������� ��������� �� ��
//UEPtr<UANet> GetNet(void);
const Serialize::USerStorageXML& GetNetXml(void) const;

// ���������� ������� ������� ������������
UGEDescription& GetDescription(const string &name);

// ���������� ��� ������� �����������
const map<string,UGEDescription>& GetDescriptions(void);

// ���������� ��������� �� ������ �����������
UEPtr<UAGraphics> GetGEngine(void);

// ��������� ����� � ����� ��
// ���� net == 0 �� ��������� ����� �� ������� �� � ���������� true
//bool SetNet(UEPtr<UANet> net);
bool SetNetXml(Serialize::USerStorageXML &net_xml);

// ��������� ����� � ����� ��
// ���� engine == 0 �� ���������� false � �� ������� ������
bool SetEngine(UEPtr<UAGraphics> engine);

// ��������� ������� ������������
void UpdateDescriptions(void);

// �������� �������� ���������, ��������� ���� ��������� � ���������
void SelectSingleComponent(const string &name);

// ����� ������� �� �������� ����������� � �����������
const std::string FindComponent(int x, int y);
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

// ������������ �������-���� (UANet*) � ������� � �������� �������
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


