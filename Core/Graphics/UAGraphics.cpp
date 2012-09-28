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
 Реализация UAGraphics
*************************************************************************** */

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAGraphics::UAGraphics(void)
{
// Font=0;
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
// Методы доступа к данным
// --------------------------
// Разрешение канвы рисования
int UAGraphics::GetCWidth(void) const
{
 return CWidth;
}

int UAGraphics::GetCHeight(void) const
{
 return CHeight;
}
// --------------------------

// --------------------------
// Методы доступа к атрибутам инструментов рисования
// --------------------------
// Возвращает цвет пера
const UColorT& UAGraphics::GetPenColor(void) const
{
 return PenColor;
}

// Возвращает толщину пера
const int UAGraphics::GetPenWidth(void) const
{
 return PenWidth;
}

// Возвращает X координату пера
const int UAGraphics::GetPenX(void) const
{
 return PenX;
}

// Возвращает Y координату пера
const int UAGraphics::GetPenY(void) const
{
 return PenY;
}

// Устанавливает цвет пера
void UAGraphics::SetPenColor(const UColorT &color)
{
 if(PenColor == color)
  return;

 PenColor=color;
}

// Устанавливает толщину пера
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

// Позиционирует перо в точку x,y
void UAGraphics::SetPenPos(int x, int y)
{
 PenX=x;
 PenY=y;
}
// --------------------------

}
#endif


