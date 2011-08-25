/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MCAMERA_CPP
#define MCAMERA_CPP

#include <math.h>
#include "MCamera.h"

namespace RDK {

//class MCamera: public UAComponent
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MCamera::MCamera(void)
{
}

MCamera::MCamera(const MCamera &copy)
{
}

MCamera::~MCamera(void)
{
}
// --------------------------

// --------------------------
// Методы управления пространственными характеристиками
// --------------------------
// Положение центра матрицы камеры в пространстве (м)
const MDVector& MCamera::GetPosition(void) const
{
 return Position;
}

bool MCamera::SetPosition(const MDVector& value)
{
 if(Position == value)
  return true;

 Ready=false;
 Position=value;
 return true;
}

// Единичный вектор направления камеры
const MDVector& MCamera::GetDirection(void) const
{
 return Direction;
}

bool MCamera::SetDirection(const MDVector& value)
{
 if(Direction == value)
  return true;

 Ready=false;
 Direction=value;
 return true;
}
// --------------------------

// --------------------------
// Методы управления параметрами камеры
// --------------------------
// Разрешение изображения в пикселах
int MCamera::GetImageWidth(void) const
{
 return ImageWidth;
}

int MCamera::GetImageHeight(void) const
{
 return ImageHeight;
}

bool MCamera::SetImageRes(int width, int height)
{
 if(ImageWidth == width && ImageHeight == height)
  return true;

 Ready=false;
 ImageWidth=width;
 ImageHeight=height;
 return true;
}

// Размер матрицы
double MCamera::GetMatrixWidth(void) const
{
 return MatrixWidth;
}

double MCamera::GetMatrixHeight(void) const
{
 return MatrixHeight;
}

bool MCamera::SetMatrixRes(double width, double height)
{
 if(MatrixWidth == width && MatrixHeight == height)
  return true;

 Ready=false;
 MatrixWidth=width;
 MatrixHeight=height;
 return true;
}

// Фокусное расстояние
double MCamera::GetFocalLength(void) const
{
 return FocalLength;
}

bool MCamera::SetFocalLength(double value)
{
 if(FocalLength == value)
  return true;

 Ready=false;
 FocalLength=value;
 return true;
}

// Углы зрения по осям
double MCamera::GetHOpticalAngle(void) const
{
 return HOpticalAngle;
}

double MCamera::GetVOpticalAngle(void) const
{
 return VOpticalAngle;
}

bool MCamera::SetOpticalAngles(double hangle, double vangle)
{
 if(HOpticalAngle == hangle && VOpticalAngle == vangle)
  return true;

 Ready=false;
 HOpticalAngle=hangle;
 VOpticalAngle=vangle;
 return true;
}

// Принципиальная точка
double MCamera::GetPrincipleX(void) const
{
 return PrincipleX;
}

double MCamera::GetPrincipleY(void) const
{
 return PrincipleY;
}

bool MCamera::SetPriciplePoint(double x, double y)
{
 if(PrincipleX == x && PrincipleY == y)
  return true;

 Ready=false;
 PrincipleX=x;
 PrincipleY=y;
 return true;
}

// Дисторсия
double MCamera::GetDistorionX(void) const
{
 return DistorionX;
}

double MCamera::GetDistorionY(void) const
{
 return DistorionY;
}

bool MCamera::SetDistorion(double x, double y)
{
 if(DistorionX == x && DistorionY == y)
  return true;

 Ready=false;
 DistorionX=x;
 DistorionY=y;
 return true;
}
// --------------------------

// --------------------------
// Методы калибровки
// --------------------------
// Калибровка
bool MCamera::Calibration()
{
 return true;
}
// --------------------------

// --------------------------
// Методы счета
// --------------------------
// Вычисляет положение точки на кадре по положению точки в пространстве
UBPoint MCamera::CalcScreenBySpacePoint(const MDVector &space_point)
{
 UBPoint point;

 if(!Build())
  return point;


 return point;
}

// Вычисляет положение точки в пространстве по положению точки на кадре и заданному расстоянию
MDVector MCamera::CalcSpaceByScreenPoint(const UBPoint &screen_point, double distance)
{
 MDVector point;

 if(!Build())
  return point;

 return point;
}

// Вычисляет и возвращает метрическую длину отрезка по отрезку в пикселях при заданном расстоянии
// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
double MCamera::CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, double distance)
{
 if(!Build())
  return 0;

 double hx=0,hy=0;
 double divisor=1;
 if(screen_segment.Width != 0)
 {
  hx=screen_segment.Width*distance*MatrixWidth;
  divisor=FocalLength*ImageWidth;
  if(divisor>0)
   hx/=divisor;
 }
 else
 if(screen_segment.Height != 0)
 {
  hy=screen_segment.Height*distance*MatrixHeight;
  divisor=FocalLength*ImageHeight;
  if(divisor>0)
   hx/=divisor;
 }

 return sqrt(hx*hx+hy*hy);
}

// Вычисляет и возвращает расстояние до отрезка по отрезку в пикселях и заданных метрических размерах
double MCamera::CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, double segment_length)
{
 if(!Build())
  return 0;

 return 0;
}
// --------------------------

// --------------------------
// Вспомогательные методы счета
// --------------------------
// Расчет вспомогательных параметров
bool MCamera::CalcAdditionalParameters(void)
{
 if(!MatrixWidth || !MatrixHeight || !ImageWidth || !ImageHeight)
  return false;

 // Метрические размеры пиксела
 PixelSizeX=ImageWidth/MatrixWidth;
 PixelSizeY=ImageHeight/MatrixHeight;

 // Углы ориентации камеры в пространстве (м)
// CosPhi,SinPhi,CosPsi,SinPsi,CosTheta,SinTheta;

 // Отношение фокусного расстояния к размеру пикселя
 FocalPixelX=FocalLength/PixelSizeX;
 FocalPixelY=FocalLength/PixelSizeY;

 // Уголы зрения по осям
// HOpticalAngle, VOpticalAngle;

// Левая и правая Y координаты линии горизонта на кадре в пикселях
//int HorizontY1, HorizontY2;

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool MCamera::ADefault(void)
{
 // Вектор направления камеры
 Position=0;

 // Разрешение изображения в пикселах
 ImageWidth=720;
 ImageHeight=576;

 // Размер матрицы
 MatrixWidth=3;
 MatrixHeight=2;

 // Фокусное расстояние
 FocalLength=4;

 // Принципиальная точка
 PrincipleX=0;
 PrincipleY=0;

 // Дисторсия
 DistorionX=0;
 DistorionY=0;
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool MCamera::ABuild(void)
{
 return CalcAdditionalParameters();
}

// Сброс процесса счета.
bool MCamera::AReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool MCamera::ACalculate(void)
{
 return true;
}
// --------------------------

}
#endif

