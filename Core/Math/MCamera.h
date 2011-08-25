/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MCameraH
#define MCameraH

#include "../Engine/UAComponent.h"
#include "../Graphics/UBitmap.h"
#include "MDVector.h"

namespace RDK {

class MCamera: public UAComponent
{
protected: // Пространственные характеристики
// Положение центра матрицы камеры в пространстве (м)
MDVector Position;

// Единичный вектор направления камеры
MDVector Direction;

protected: // Параметры камеры
// Разрешение изображения в пикселах
int ImageWidth, ImageHeight;

// Размер матрицы
double MatrixWidth, MatrixHeight;

// Фокусное расстояние
double FocalLength;

// Углы зрения по осям
double HOpticalAngle, VOpticalAngle;

// Принципиальная точка
double PrincipleX, PrincipleY;

// Дисторсия
double DistorionX, DistorionY;

protected: //... какие еще параметры реально должны быть или могут получатся из этих?

protected: // Вспомогательные параметры
// Метрические размеры пиксела
double PixelSizeX, PixelSizeY;

// Отношение фокусного расстояния к метрическому размеру пикселя
double FocalPixelX, FocalPixelY;

// Углы ориентации камеры в пространстве
double CosPhi,SinPhi,CosPsi,SinPsi,CosTheta,SinTheta;

// Левая и правая Y координаты линии горизонта на кадре в пикселях
int HorizontY1, HorizontY2;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MCamera(void);
MCamera(const MCamera &copy);
virtual ~MCamera(void);
// --------------------------

// --------------------------
// Методы управления пространственными характеристиками
// --------------------------
// Положение центра матрицы камеры в пространстве (м)
const MDVector& GetPosition(void) const;
bool SetPosition(const MDVector& value);

// Единичный вектор направления камеры
const MDVector& GetDirection(void) const;
bool SetDirection(const MDVector& value);
// --------------------------

// --------------------------
// Методы управления параметрами камеры
// --------------------------
// Разрешение изображения в пикселах
int GetImageWidth(void) const;
int GetImageHeight(void) const;
bool SetImageRes(int width, int height);

// Размер матрицы
double GetMatrixWidth(void) const;
double GetMatrixHeight(void) const;
bool SetMatrixRes(double width, double height);

// Фокусное расстояние
double GetFocalLength(void) const;
bool SetFocalLength(double value);

// Углы зрения по осям
double GetHOpticalAngle(void) const;
double GetVOpticalAngle(void) const;
bool SetOpticalAngles(double hangle, double vangle);

// Принципиальная точка
double GetPrincipleX(void) const;
double GetPrincipleY(void) const;
bool SetPriciplePoint(double x, double y);

// Дисторсия
double GetDistorionX(void) const;
double GetDistorionY(void) const;
bool SetDistorion(double x, double y);
// --------------------------

// --------------------------
// Методы калибровки
// --------------------------
// Калибровка
bool Calibration();
// --------------------------

// --------------------------
// Методы счета
// --------------------------
// Вычисляет положение точки на кадре по положению точки в пространстве
UBPoint CalcScreenBySpacePoint(const MDVector &space_point);

// Вычисляет положение точки в пространстве по положению точки на кадре и заданному расстоянию
MDVector CalcSpaceByScreenPoint(const UBPoint &screen_point, double distance);

// Вычисляет и возвращает метрическую длину отрезка по отрезку в пикселях при заданном расстоянии
// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
double CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, double distance);

// Вычисляет и возвращает расстояние до отрезка по отрезку в пикселях и заданных метрических размерах
double CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, double segment_length);
// --------------------------

// --------------------------
// Вспомогательные методы счета
// --------------------------
protected:
// Расчет вспомогательных параметров
bool CalcAdditionalParameters(void);
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool ABuild(void);

// Сброс процесса счета.
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

}
#endif

