/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAGRAPHICS_H
#define UAGRAPHICS_H

#include <string>
#include "UBitmap.h"
//#include "UFont.h"

namespace RDK {

using namespace std;

//class UAFont;

class UAGraphics
{
protected: // ������
//UAFont *Font;

protected: // �������� ������������ ���������
// ���� ����
UColorT PenColor;

// ������� ����
int PenWidth;

// ���������� ����
int PenX, PenY;

protected: // ��������� ����������
// ������ ��������� �� ������ ����� ���������
UBColor *CData;

// ���������� ����� ���������
int CWidth, CHeight;

// �������� ������� ����
int HalfPenWidth;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAGraphics(void);
virtual ~UAGraphics(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ����� ���������
int GetCWidth(void) const;
int GetCHeight(void) const;

// ���������� ������� �����
//UAFont* GetFont(void);

// ������������� ������� �����
//bool SetFont(UAFont* font);
// --------------------------

// --------------------------
// ������ ������� � ��������� ������������ ���������
// --------------------------
// ���������� ���� ����
const UColorT& GetPenColor(void) const;

// ���������� ������� ����
const int GetPenWidth(void) const;

// ���������� X ���������� ����
const int GetPenX(void) const;

// ���������� Y ���������� ����
const int GetPenY(void) const;

// ������������� ���� ����
void SetPenColor(const UColorT &color);

// ������������� ������� ����
bool SetPenWidth(int width);

// ������������� ���� � ����� x,y
void SetPenPos(int x, int y);
// --------------------------

// --------------------------
// ����������� ���������
// --------------------------
// ���������� �������
// ���� ispos == true ������������� ���� � �������� �������
virtual void Pixel(int x, int y, bool ispos=true)=0;

// ���������� ����� �� ����������� ������
virtual void Line(int x1, int y1, int x2, int y2)=0;

// ���������� ����� ������������ ������� ����
// ���������� ���� � ������� x,y
virtual void LineTo(int x, int y)=0;

// ���������� ���������� � ������� x,y � �������� r
// ���� fill == true - �� ������ � ��������
virtual void Circle(int x, int y, int r, bool fill=false)=0;

// ���������� �������� ������ � ������� x,y � �������� r
// ������� ������� fi, �������� �� ���� teta
// ������ ������� ������� �� �������������� ���
// ���� �������� � ��������
virtual void Sector(int x, int y, int r, float fi, float teta, bool fill=false)=0;

// ���������� ������ � ������� x,y � ��������� hor, vert
// ���� fill == true - �� ������ � ��������
virtual void Ellipse(int x, int y, int hor, int vert, bool fill=false)=0;

// ���������� ������������� � ������������
// x1,y1 - �������� ������ ����
// x2,y2 - ������� ������� ����
// ���� fill == true - �� ������ � ��������
virtual void Rect(int x1, int y1, int x2, int y2, bool fill=false)=0;

// ���������� ������������� � ������������ ������
// � �������� ������ color
// ���� fill == true - �� ������ � ��������
virtual void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, bool fill=false)=0;

// ���������� ������� ������������ �������
virtual void Fill(int x, int y, UColorT BorderColor)=0;

// ������� ����������� � ������� � �������� �������
// ���� transparency == 0 ���������� ��� ������������
// ���� transparency == 1 ���������� c �������� ������������ ������������
// ���� transparency == 2 ���������� ��� �����, � ������ ������ ��� �������� ����
virtual void Bitmap(int x, int y, UBitmap &bmp, int transparency=0, UColorT transp=UColorT(255,255,255))=0;
// --------------------------

// --------------------------
// ����� ������
// --------------------------
// ������� ����� str
//virtual void Text(const wstring &str, int x, int y);
//virtual void Text(const string &str, int x, int y);
// --------------------------

protected: // ��������������� ������
// --------------------------
// ��������������� ����������� ���������
// --------------------------
// ������������ ������� � ����������� x,y ��� ��������
virtual void DrawPixel(int x, int y)=0;
// --------------------------
};

}
#endif

