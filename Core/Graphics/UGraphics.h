/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UGRAPHICS_H
#define UGRAPHICS_H

#include "UAGraphics.h"

namespace RDK {

class UGraphics: public UAGraphics
{
protected: // ������
// ����� ���������
UBitmap* Canvas;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UGraphics(void);
UGraphics(UBitmap *canvas);
virtual ~UGraphics(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������� ����� ���������
UBitmap* GetCanvas(void);

// ������ ����� ���������
bool SetCanvas(UBitmap *canvas);
// --------------------------

// --------------------------
// ������������� ���������
// --------------------------
// �������� ������������
UGraphics& operator = (UGraphics &tool);
// --------------------------

// --------------------------
// ����������� ���������
// --------------------------
// ���������� �������
// ���� ispos == true ������������� ���� � �������� �������
void Pixel(int x, int y, bool ispos=true);

// ���������� ����� �� ����������� ������
void Line(int x1, int y1, int x2, int y2);

// ���������� ����� ������������ ������� ����
// ���������� ���� � ������� x,y
void LineTo(int x, int y);

// ���������� ���������� � ������� x,y � �������� r
// ���� fill == true - �� ������ � ��������
void Circle(int x, int y, int r, bool fill=false);

// ���������� �������� ������ � ������� x,y � �������� r
// ������� ������� fi, �������� �� ���� teta 
// ������ ������� ������� �� �������������� ���
// ���� �������� � ��������
void Sector(int x, int y, int r, float fi, float teta, bool fill=false);

// ���������� ������ � ������� x,y � ��������� hor, vert
// ���� fill == true - �� ������ � ��������
void Ellipse(int x, int y, int hor, int vert, bool fill=false);

// ���������� ������������� � ������������
// x1,y1 - �������� ������ ����
// x2,y2 - ������� ������� ����
// ���� fill == true - �� ������ � ��������
void Rect(int x1, int y1, int x2, int y2, bool fill=false);

// ���������� ������������� � ������������ ������
// � �������� ������ color
// ���� fill == true - �� ������ � ��������
void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, bool fill=false);

// ���������� ������� ������������ �������
void Fill(int x, int y, UColorT BorderColor);

// ������� ����������� � ������� � �������� �������
// ���� transparency == 0 ���������� ��� ������������
// ���� transparency == 1 ���������� c �������� ������������ ������������
// ���� transparency == 2 ���������� ��� �����, � ������ ������ ��� �������� ����
void Bitmap(int x, int y, UBitmap &bmp, int transparency=0, UColorT transp=UColorT(255,255,255));
// --------------------------

protected: // ��������������� ������
// --------------------------
// ��������������� ����������� ���������
// --------------------------
// ������������ ������� � ����������� x,y ��� ��������
void DrawPixel(int x, int y);
// --------------------------
};

}
#endif

