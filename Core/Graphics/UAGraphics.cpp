/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UAGRAPHICS_CPP
#define UAGRAPHICS_CPP

#include "UAGraphics.h"
#include <cmath>
#include <cstdlib>
//#include <list>

namespace RDK {

using namespace std;

/* ***************************************************************************
 ���������� UAGraphics
*************************************************************************** */

// --------------------------
// ������������ � �����������
// --------------------------
UAGraphics::UAGraphics(void)
{
 Font=0;
 CData=0;
 PenColor.c=0;
 PenWidth=1;
 HalfPenWidth=0;
 PenX=PenY=0;

 CWidth=CHeight=0;
}

UAGraphics::~UAGraphics(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ����� ���������
int UAGraphics::GetCWidth(void) const
{
 return CWidth;
}

int UAGraphics::GetCHeight(void) const
{
 return CHeight;
}

// ������������� �����
bool UAGraphics::SetFont(UAFont* font)
{
 Font=font;
 return true;
}

// ���������� ������� �����
UAFont* UAGraphics::GetFont(void)
{
 return Font;
}
// --------------------------

// --------------------------
// ������ ������� � ��������� ������������ ���������
// --------------------------
// ���������� ���� ����
const UColorT& UAGraphics::GetPenColor(void) const
{
 return PenColor;
}

// ���������� ������� ����
int UAGraphics::GetPenWidth(void) const
{
 return PenWidth;
}

// ���������� X ���������� ����
int UAGraphics::GetPenX(void) const
{
 return PenX;
}

// ���������� Y ���������� ����
int UAGraphics::GetPenY(void) const
{
 return PenY;
}

// ������������� ���� ����
void UAGraphics::SetPenColor(const UColorT &color)
{
 if(PenColor == color)
  return;

 PenColor=color;
}

// ������������� ������� ����
bool UAGraphics::SetPenWidth(int width)
{
 if(PenWidth == width)
  return true;

 if(width <=0)
  return false;

 PenWidth=width;
 HalfPenWidth=PenWidth>>1;
 return true;
}

// ������������� ���� � ����� x,y
void UAGraphics::SetPenPos(int x, int y)
{
 PenX=x;
 PenY=y;
}
// --------------------------

}
#endif


