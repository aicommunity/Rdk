#ifndef MCameraH
#define MCameraH

#include "MVector.h"
#include "MDVector.h"
#include "MGeometry.h"

namespace RDK {

template<class T>
class MCamera
{
private: // Данные камеры
// Матрица внешней калибровки
MMatrix<T,4,4> Ecc;

// Обратная матрица внешней калибровки
MMatrix<T,4,4> InvEcc;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MCamera(void);
MCamera(const MCamera& copy);
virtual ~MCamera(void);
// --------------------------

// Управление матрицей внешней калибровки камеры
const MMatrix<T,4,4>& GetEcc(void) const;
const MMatrix<T,4,4>& GetInvEcc(void) const;
bool SetEcc(const MMatrix<T,4,4>& value);
bool SetInvEcc(const MMatrix<T,4,4>& value);

// Преобразование точки в нормальной плоскости в реальную точку (учет дисторсии)
virtual MVector<T,3> CalcPixelPositionFromNormalPosition(const MVector<T,3> &point)=0;

// Вычисляет положение пикселя на кадре с учетом дисторсии, по положению пикселя без учета дисторсии
virtual MVector<T,3> CalcDistortPixelPosition(const MVector<T,3> &undistort_pixel)=0;

// Вычисляет положение пикселя на выпрямленном кадре, по положению пикселя на кадре с дисторсией
virtual MVector<T,3> CalcUndistortPixelPosition(const MVector<T,3> &distort_pixel)=0;

// Вычисляет положение точки на кадре по положению точки в пространстве
virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point)=0;

virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point, int image_width, int image_height, bool &res)=0;

// Вычисляет положение точки в пространстве по положению точки на кадре и заданному расстоянию
virtual MVector<T,4> CalcSpaceByScreenPoint(const MVector<T,3> &screen_point, T distance)=0;

// Вычисляет и возвращает метрическую длину отрезка по отрезку в пикселях при заданном расстоянии
// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
virtual T CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, T distance)=0;

// Вычисляет и возвращает расстояние до отрезка по отрезку в пикселях и заданных метрических размерах
virtual T CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, T segment_length)=0;

// Преобразует 3D геометрию в 2D
virtual void Convert3Dto2DGeometry(const MGeometry<T,4> &geometry_3d, MGeometry<T,3> &geometry_2d);

/// Преобразует 3D геометрию в 2D
/// geometry_3d - N строк по 3 координаты точек
/// geometry_2d - N строк по 2 координаты точек
virtual void Convert3Dto2DGeometry(const MDMatrix<T> &geometry_3d, MDMatrix<T> &geometry_2d);

protected: // Скрытые методы

};

// Стандартная модель камеры с произвольной дисторсией
template<class T>
class MCameraStandard: public MCamera<T>
{
private: // Данные
// Матрица внутренней калибровки
MMatrix<T,3,3> Icc;

// Обратная матрица внутренней калибровки
MMatrix<T,3,3> InvIcc;

// Модель дисторсии
// 0 - Учета дисторсии нет
// 1 - Модель OpenCV
// 2 - Модель Artoolkit
int DistortionMode;

// К-ты дисторсии
MDVector<double> DistortionCoeff;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MCameraStandard(void);
MCameraStandard(const MCameraStandard& copy);
virtual ~MCameraStandard(void);
// --------------------------

// Управление матрицей внутренней калибровки камеры
const MMatrix<T,3,3>& GetIcc(void) const;
const MMatrix<T,3,3>& GetInvIcc(void) const;
bool SetIcc(const MMatrix<T,3,3>& value);
bool SetInvIcc(const MMatrix<T,3,3>& value);

// Модель дисторсии
const int& GetDistortionMode(void) const;
bool SetDistortionMode(const int &value);

// К-ты дисторсии
const MDVector<T>& GetDistortionCoeff(void) const;
bool SetDistortionCoeff(const MDVector<T>& value);

// Преобразование точки в нормальной плоскости в реальную точку (учет дисторсии)
virtual MVector<T,3> CalcPixelPositionFromNormalPosition(const MVector<T,3> &point);

// Вычисляет положение пикселя на кадре с учетом дисторсии, по положению пикселя без учета дисторсии
virtual MVector<T,3> CalcDistortPixelPosition(const MVector<T,3> &undistort_pixel);

// Вычисляет положение пикселя на выпрямленном кадре, по положению пикселя на кадре с дисторсией
virtual MVector<T,3> CalcUndistortPixelPosition(const MVector<T,3> &distort_pixel);

// Вычисляет положение точки на кадре по положению точки в пространстве
virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point);
virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point, int image_width, int image_height, bool &res);

// Вычисляет положение точки в пространстве по положению точки на кадре и заданному расстоянию
virtual MVector<T,4> CalcSpaceByScreenPoint(const MVector<T,3> &screen_point, T distance);

// Вычисляет и возвращает метрическую длину отрезка по отрезку в пикселях при заданном расстоянии
// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
virtual T CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, T distance);

// Вычисляет и возвращает расстояние до отрезка по отрезку в пикселях и заданных метрических размерах
virtual T CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, T segment_length);

};

// -----------------------------------------------------------------------------
// Реализация
// -----------------------------------------------------------------------------
// MCamera
// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<class T>
MCamera<T>::MCamera(void)
{
 SetEcc(MMatrix<T,4,4>::Eye());
}

template<class T>
MCamera<T>::MCamera(const MCamera<T>& copy)
{
 SetEcc(copy.GetEcc());
}

template<class T>
MCamera<T>::~MCamera(void)
{

}
// --------------------------


// Управление матрицей внешней калибровки камеры
template<class T>
const MMatrix<T,4,4>& MCamera<T>::GetEcc(void) const
{
 return Ecc;
}

template<class T>
const MMatrix<T,4,4>& MCamera<T>::GetInvEcc(void) const
{
 return InvEcc;
}

template<class T>
bool MCamera<T>::SetEcc(const MMatrix<T,4,4>& value)
{
 Ecc=value;
 InverseEcc(Ecc,InvEcc);
 InvEcc=Ecc.Inverse();
 return true;
}

template<class T>
bool MCamera<T>::SetInvEcc(const MMatrix<T,4,4>& value)
{
 InvEcc=value;
 InverseEcc(InvEcc,Ecc);
 return true;
}


// Преобразует 3D геометрию в 2D
template<class T>
void MCamera<T>::Convert3Dto2DGeometry(const MGeometry<T,4> &geometry_3d, MGeometry<T,3> &geometry_2d)
{
 geometry_2d.Clear();
 geometry_2d.SetNumVertices(geometry_3d.GetNumVertices());
 geometry_2d.SetNumBorders(geometry_3d.GetNumBorders());
 for(size_t i=0;i<geometry_2d.GetNumVertices();i++)
 {
  MVector<T,4> v=geometry_3d.Vertex(i);
  geometry_2d.Vertex(i)=CalcScreenBySpacePoint(v);
 }
 for(size_t i=0;i<geometry_2d.GetNumBorders();i++)
  geometry_2d.Border(i)=geometry_3d.Border(i);
}


/// Преобразует 3D геометрию в 2D
/// geometry_3d - N строк по 3 координаты точек
/// geometry_2d - N строк по 2 координаты точек
template<class T>
void MCamera<T>::Convert3Dto2DGeometry(const MDMatrix<T> &geometry_3d, MDMatrix<T> &geometry_2d)
{
 geometry_2d.Resize(geometry_3d.GetRows(),2);
 for(int i=0;i<geometry_3d.GetRows();i++)
 {
  MVector<T,4> v;
  for(int j=0;j<3;j++)
   v(j)=geometry_3d(i,j);
  v(3)=1;
  MVector<T,3> res;
  res=CalcScreenBySpacePoint(v);
  for(int j=0;j<2;j++)
   geometry_2d(i,j)=res(j);
 }
}


// ****************************************************************************
// MCameraStandard
// ****************************************************************************
// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<class T>
MCameraStandard<T>::MCameraStandard(void)
: MCamera<T>(), DistortionMode(0)
{
 SetIcc(MMatrix<T,3,3>::Eye());
}

template<class T>
MCameraStandard<T>::MCameraStandard(const MCameraStandard& copy)
: MCamera<T>(copy), DistortionMode(0)
{
 SetIcc(copy.GetIcc());
}

template<class T>
MCameraStandard<T>::~MCameraStandard(void)
{

}
// --------------------------

// Управление матрицей внутренней калибровки камеры
template<class T>
const MMatrix<T,3,3>& MCameraStandard<T>::GetIcc(void) const
{
 return Icc;
}

template<class T>
const MMatrix<T,3,3>& MCameraStandard<T>::GetInvIcc(void) const
{
 return InvIcc;
}

template<class T>
bool MCameraStandard<T>::SetIcc(const MMatrix<T,3,3>& value)
{
 Icc=value;
 InvIcc=Icc.Inverse();
 return true;
}

template<class T>
bool MCameraStandard<T>::SetInvIcc(const MMatrix<T,3,3>& value)
{
 InvIcc=value;
 Icc=InvIcc.Inverse();
 return true;
}

// Модель дисторсии
template<class T>
const int& MCameraStandard<T>::GetDistortionMode(void) const
{
 return DistortionMode;
}

template<class T>
bool MCameraStandard<T>::SetDistortionMode(const int &value)
{
 DistortionMode=value;
 return true;
}

// К-ты дисторсии
template<class T>
const MDVector<T>& MCameraStandard<T>::GetDistortionCoeff(void) const
{
 return DistortionCoeff;
}

template<class T>
bool MCameraStandard<T>::SetDistortionCoeff(const MDVector<T>& value)
{
 DistortionCoeff.Resize(value.GetSize());
 DistortionCoeff=value;
 return true;
}

// Преобразование точки в нормальной плоскости в реальную точку (учет дисторсии)
template<class T>
MVector<T,3> MCameraStandard<T>::CalcPixelPositionFromNormalPosition(const MVector<T,3> &point)
{
 if(DistortionMode == 0)
  return point;

 if(DistortionMode == 1)
 {
  if(DistortionCoeff.GetSize()<5)
   return point;

  MVector<T,3> res;
  T r=point.x*point.x+point.y*point.y;
  T m1=(1.0+DistortionCoeff[0]*r+DistortionCoeff[1]*r*r+DistortionCoeff[4]*r*r*r);
  res.x=m1*point.x;
  res.y=m1*point.y;
  res.z=1;

  res.x+=2*DistortionCoeff[2]*point.x*point.y+DistortionCoeff[3]*(r+2*point.x*point.x);
  res.y+=DistortionCoeff[2]*(r+2*point.y*point.y)+2*DistortionCoeff[3]*point.x*point.y;
  return res;
 }

 if(DistortionMode == 2)
 {
  if(DistortionCoeff.GetSize()<4)
   return point;

  MVector<T,3> res;
  T x=DistortionCoeff[3]*(point.x-DistortionCoeff[0]);
  T y=DistortionCoeff[3]*(point.y-DistortionCoeff[1]);
  T d=x*x+y*y;
  T p=1.0-(DistortionCoeff[2]*d)/100000000.0;
  res.x=p*x+DistortionCoeff[0];
  res.y=p*y+DistortionCoeff[1];
  res.z=1;
  return res;
 }

 return point;
}

// Вычисляет положение пикселя на кадре с учетом дисторсии, по положению пикселя без учета дисторсии
template<class T>
MVector<T,3> MCameraStandard<T>::CalcDistortPixelPosition(const MVector<T,3> &undistort_pixel)
{
 MVector<T,3> normal_point=InvIcc*undistort_pixel;
 MVector<T,3> dist_point=CalcPixelPositionFromNormalPosition(normal_point);
 MVector<T,3> result=Icc*dist_point;

 return result;
}

// Вычисляет положение пикселя на выпрямленном кадре, по положению пикселя на кадре с дисторсией
template<class T>
MVector<T,3> MCameraStandard<T>::CalcUndistortPixelPosition(const MVector<T,3> &distort_pixel)
{
 MVector<T,3> result;

 MVector<T,3> dist_iteration=CalcPixelPositionFromNormalPosition(distort_pixel);

 return result;
}

// Вычисляет положение точки на кадре по положению точки в пространстве
template<class T>
MVector<T,3> MCameraStandard<T>::CalcScreenBySpacePoint(const MVector<T,4> &space_point)
{
 MVector<T,3> normalpoint, distpoint, screenpoint;
 MVector<T,4> temp4;
 MVector<T,3> cameraspacepoint;
 MMatrix<T,3,4> E=MMatrix<T,3,4>::Eye();

 cameraspacepoint=E*(MCamera<T>::GetEcc()*space_point);
 if(fabs(cameraspacepoint.z)>10e-7)
 {
  normalpoint.x=cameraspacepoint.x/cameraspacepoint.z;
  normalpoint.y=cameraspacepoint.y/cameraspacepoint.z;
  normalpoint.z=1;
 }
 else
  return screenpoint;

 distpoint=CalcPixelPositionFromNormalPosition(normalpoint);
 screenpoint=Icc*distpoint;

 return screenpoint;
}

// Вычисляет положение точки на кадре по положению точки в пространстве
template<class T>
MVector<T,3> MCameraStandard<T>::CalcScreenBySpacePoint(const MVector<T,4> &space_point, int image_width, int image_height, bool &res)
{
 res=false;
 MVector<T,3> normalpoint, distpoint, screenpoint;
 MVector<T,4> temp4;
 MVector<T,3> cameraspacepoint;
 MMatrix<T,3,4> E=MMatrix<T,3,4>::Eye();

 cameraspacepoint=E*(MCamera<T>::GetEcc()*space_point);
 if(fabs(cameraspacepoint.z)>10e-7)
 {
  normalpoint.x=cameraspacepoint.x/cameraspacepoint.z;
  normalpoint.y=cameraspacepoint.y/cameraspacepoint.z;
  normalpoint.z=1;
 }
 else
  return screenpoint;

 screenpoint=Icc*normalpoint;
 if(screenpoint.x<0 || screenpoint.x>=image_width || screenpoint.y<0 || screenpoint.y>=image_height)
 {
  return screenpoint;
 }
 distpoint=CalcPixelPositionFromNormalPosition(normalpoint);
 screenpoint=Icc*distpoint;
 res=true;

 return screenpoint;
}

// Вычисляет положение точки в пространстве по положению точки на кадре и заданному расстоянию
template<class T>
MVector<T,4> MCameraStandard<T>::CalcSpaceByScreenPoint(const MVector<T,3> &screen_point, T distance)
{
 MVector<T,4> res;

 return res;
}

// Вычисляет и возвращает метрическую длину отрезка по отрезку в пикселях при заданном расстоянии
// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
template<class T>
T MCameraStandard<T>::CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, T distance)
{
 return 0;
}

// Вычисляет и возвращает расстояние до отрезка по отрезку в пикселях и заданных метрических размерах
template<class T>
T MCameraStandard<T>::CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, T segment_length)
{
 return 0;
}

}
#endif

