#ifndef MCameraH
#define MCameraH

#include "MVector.h"
#include "MDVector.h"
#include "MGeometry.h"
#include "../Graphics/UBitmap.h"

namespace RDK {

template<class T>
class MCamera
{
private: // Данные камеры
/// Матрица внешней калибровки
MMatrix<T,4,4> Ecc;

/// Обратная матрица внешней калибровки
MMatrix<T,4,4> InvEcc;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MCamera(void);
MCamera(const MCamera& copy);
virtual ~MCamera(void);
// --------------------------

/// Управление матрицей внешней калибровки камеры
const MMatrix<T,4,4>& GetEcc(void) const;
const MMatrix<T,4,4>& GetInvEcc(void) const;
bool SetEcc(const MMatrix<T,4,4>& value);
bool SetInvEcc(const MMatrix<T,4,4>& value);

/// Преобразование точки в нормальной плоскости в реальную точку (учет дисторсии)
virtual MVector<T,3> CalcPixelPositionFromNormalPosition(const MVector<T,3> &point)=0;

/// Вычисляет положение пикселя на кадре с учетом дисторсии, по положению пикселя без учета дисторсии
virtual MVector<T,3> CalcDistortPixelPosition(const MVector<T,3> &undistort_pixel)=0;

/// Вычисляет положение пикселя на выпрямленном кадре, по положению пикселя на кадре с дисторсией
virtual MVector<T,3> CalcUndistortPixelPosition(const MVector<T,3> &distort_pixel)=0;

/// Вычисляет положение точки на кадре по положению точки в пространстве
virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point)=0;
virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point, int image_width, int image_height, bool &res)=0;
virtual MVector<T,3> CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point)=0;
virtual MVector<T,3> CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point, int image_width, int image_height, bool &res)=0;

/// Вычисляет положение точки в пространстве по положению точки на кадре и заданному расстоянию
virtual MVector<T,4> CalcSpaceByScreenPoint(const MVector<T,3> &screen_point, T distance)=0;

/// Вычисляет и возвращает метрическую длину отрезка по отрезку в пикселях при заданном расстоянии
/// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
virtual T CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, T distance)=0;

/// Вычисляет и возвращает расстояние до отрезка по отрезку в пикселях и заданных метрических размерах
virtual T CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, T segment_length)=0;

/// Преобразует 3D геометрию в 2D
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
/// Матрица внутренней калибровки
MMatrix<T,3,3> Icc;

/// Обратная матрица внутренней калибровки
MMatrix<T,3,3> InvIcc;

// Модель дисторсии
// 0 - Учета дисторсии нет
// 1 - Модель OpenCV
// 2 - Модель Artoolkit
// 3 - Модель Цая (DistortionCoeff(0) = k1, DistortionCoeff(1) = f (фокусное расстояние в мм), DistortionCoeff(2) = s (маштабирующий коэффициент)
int DistortionMode;

/// Модель камеры
/// 0 - обычная (pinhole)
/// 1 - fisheye (opencv)
int CameraMode;

/// К-ты дисторсии
MDVector<double> DistortionCoeff;

/// Ширина изображения на котором проводилась калибровка
int CalibrationWidth;

/// Высота изображения на котором проводилась калибровка
int CalibrationHeight;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MCameraStandard(void);
MCameraStandard(const MCameraStandard& copy);
virtual ~MCameraStandard(void);
// --------------------------

/// Управление матрицей внутренней калибровки камеры
const MMatrix<T,3,3>& GetIcc(void) const;
const MMatrix<T,3,3>& GetInvIcc(void) const;
virtual bool SetIcc(const MMatrix<T,3,3>& value);
virtual bool SetInvIcc(const MMatrix<T,3,3>& value);

/// Модель дисторсии
const int& GetDistortionMode(void) const;
virtual bool SetDistortionMode(const int &value);

/// Модель камеры
/// 0 - обычная (pinhole)
/// 1 - fisheye (opencv)
const int& GetCameraMode(void) const;
virtual bool SetCameraMode(const int& value);

/// К-ты дисторсии
const MDVector<T>& GetDistortionCoeff(void) const;
virtual bool SetDistortionCoeff(const MDVector<T>& value);

/// Ширина изображения на котором проводилась калибровка
const int& GetCalibrationWidth(void) const;
virtual bool SetCalibrationWidth(const int& value);

/// Высота изображения на котором проводилась калибровка
const int& GetCalibrationHeight(void) const;
virtual bool SetCalibrationHeight(const int& value);

/// Преобразование точки в нормальной плоскости в реальную точку (учет дисторсии)
virtual MVector<T,3> CalcPixelPositionFromNormalPosition(const MVector<T,3> &point);

/// Вычисляет положение пикселя на кадре с учетом дисторсии, по положению пикселя без учета дисторсии
virtual MVector<T,3> CalcDistortPixelPosition(const MVector<T,3> &undistort_pixel);

/// Вычисляет положение пикселя на выпрямленном кадре, по положению пикселя на кадре с дисторсией
virtual MVector<T,3> CalcUndistortPixelPosition(const MVector<T,3> &distort_pixel);

/// Вычисляет положение точки на кадре по положению точки в пространстве
virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point);
virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point, int image_width, int image_height, bool &res);
virtual MVector<T,3> CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point);
virtual MVector<T,3> CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point, int image_width, int image_height, bool &res);

/// Вычисляет положение точки в пространстве по положению точки на кадре и заданному расстоянию
virtual MVector<T,4> CalcSpaceByScreenPoint(const MVector<T,3> &screen_point, T distance);

/// Вычисляет и возвращает метрическую длину отрезка по отрезку в пикселях при заданном расстоянии
/// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
virtual T CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, T distance);

/// Вычисляет и возвращает расстояние до отрезка по отрезку в пикселях и заданных метрических размерах
virtual T CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, T segment_length);

/// Вычисляет угловое значение пикселя в модели камеры
virtual T CalcAngleX(int pixel) const;
virtual T CalcAngleY(int pixel) const;

/// Вычисляет пиксельное значение угла в модели камеры
virtual T CalcPixelXByAngle(T angle) const;
virtual T CalcPixelYByAngle(T angle) const;

/// Вычисляет матрицу внутренней калибровки по известным полям зрения
virtual bool CalcIccByVisualAngle(T angle_x, T angle_y, T principle_x, T principle_y, int image_width, int image_height, MMatrix<T,3,3> &icc, MMatrix<T,3,3> &norm_icc);

/// Вычисляет матрицу внутренней калибровки по известным полям зрения
virtual bool CalcVisualAnglesByIcc(const MMatrix<T,3,3> &icc, T &angle_x, T &angle_y, T &principle_x, T &principle_y, int image_width, int image_height);

// Получение мировых координат точки в пространстве по её экранным координатам
virtual int GetWorldPoint(double Xfd, double Yfd, double yw, double &xw, double &zw);

protected: // Скрытые методы

virtual int image_coord_to_world_coord (double Xfd, double Yfd, double zw,
					  double &xw, double &yw);

void distorted_to_undistorted_sensor_coord (double Xd, double Yd, double &Xu, double &Yu);


/// Функция вычисления дисторсии для fisheye thetaD=theta+k1*theta^3+k2*theta^5+k3*theta^7+k4*theta^9  (theta+k1*theta^3+k2*theta^5+k3*theta^7+k4*theta^9 - thetaD = 0)
T FuncTheta(T thetaD, T Xthet) const;

/// Метод хорд для нахождения численного решения уравнения относительно theta
T ChordMethod(T Cx1, T Cx2, double epsilon, T thetaD) const;


MCameraStandard& operator = (const MCameraStandard& copy);
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
  MVector<T,4> v=geometry_3d.Vertex(int(i));
  geometry_2d.Vertex(int(i))=CalcScreenBySpacePoint(v);
 }
 for(size_t i=0;i<geometry_2d.GetNumBorders();i++)
  geometry_2d.Border(int(i))=geometry_3d.Border(int(i));
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
: MCamera<T>(), DistortionMode(0), CameraMode(0), CalibrationWidth(0), CalibrationHeight(0)
{
 Icc=MMatrix<T,3,3>::Eye();
 InvIcc=MMatrix<T,3,3>::Eye();
}

template<class T>
MCameraStandard<T>::MCameraStandard(const MCameraStandard& copy)
: MCamera<T>(copy), DistortionMode(copy.DistortionMode), CameraMode(copy.CameraMode), DistortionCoeff(copy.DistortionCoeff)
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

/// Модель камеры
/// 0 - обычная (pinhole)
/// 1 - fisheye (opencv)
template<class T>
const int& MCameraStandard<T>::GetCameraMode(void) const
{
 return CameraMode;
}

template<class T>
bool MCameraStandard<T>::SetCameraMode(const int& value)
{
 CameraMode=value;
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


/// Ширина изображения на котором проводилась калибровка
template<class T>
const int& MCameraStandard<T>::GetCalibrationWidth(void) const
{
 return CalibrationWidth;
}

template<class T>
bool MCameraStandard<T>::SetCalibrationWidth(const int& value)
{
 CalibrationWidth=value;
 return true;
}

/// Высота изображения на котором проводилась калибровка
template<class T>
const int& MCameraStandard<T>::GetCalibrationHeight(void) const
{
 return CalibrationHeight;
}

template<class T>
bool MCameraStandard<T>::SetCalibrationHeight(const int& value)
{
 CalibrationHeight=value;
 return true;
}

// Преобразование точки в нормальной плоскости в реальную точку (учет дисторсии)
template<class T>
MVector<T,3> MCameraStandard<T>::CalcPixelPositionFromNormalPosition(const MVector<T,3> &point)
{
 switch(CameraMode)
 {
 case 0:
 {
  if(DistortionMode == 0)
   return point;

  if(DistortionMode == 1)
  {
   if(DistortionCoeff.GetSize()<1)
	return point;

   MVector<T,3> res;
   res=point;
   if(DistortionCoeff.GetSize() == 1)
   {
	T r=point(0)*point(0)+point(1)*point(1);
	T m1=(1.0+DistortionCoeff[0]*r);
	res(0)=m1*point(0);
	res(1)=m1*point(1);
	res(2)=1;
   }
   else
   if(DistortionCoeff.GetSize() == 5)
   {
	T r=point(0)*point(0)+point(1)*point(1);
	T m1=(1.0+DistortionCoeff[0]*r+DistortionCoeff[1]*r*r+DistortionCoeff[4]*r*r*r);
	res(0)=m1*point(0);
	res(1)=m1*point(1);
	res(2)=1;

	res(0)+=2*DistortionCoeff[2]*point(0)*point(1)+DistortionCoeff[3]*(r+2*point(0)*point(0));
	res(1)+=DistortionCoeff[2]*(r+2*point(1)*point(1))+2*DistortionCoeff[3]*point(0)*point(1);
   }
   else
   if(DistortionCoeff.GetSize() == 8)
   {
	T r2=point(0)*point(0)+point(1)*point(1);
	T r4=r2*r2;
	T r6=r4*r2;
	T dividend=1.0+DistortionCoeff[0]*r2+DistortionCoeff[1]*r4+DistortionCoeff[4]*r6;
	T divider=1.0+DistortionCoeff[5]*r2+DistortionCoeff[6]*r4+DistortionCoeff[7]*r6;
	T m1=dividend/divider;
	res(0)=m1*point(0);
	res(1)=m1*point(1);
	res(2)=1;

	res(0)+=2*DistortionCoeff[2]*point(0)*point(1)+DistortionCoeff[3]*(r2+2*point(0)*point(0));
	res(1)+=DistortionCoeff[2]*(r2+2*point(1)*point(1))+2*DistortionCoeff[3]*point(0)*point(1);
   }
   else
   if(DistortionCoeff.GetSize() == 10)
   {
	T r2=point(0)*point(0)+point(1)*point(1);
	T r4=r2*r2;
	T r6=r4*r2;
	T dividend=1.0+DistortionCoeff[0]*r2+DistortionCoeff[1]*r4+DistortionCoeff[4]*r6;
	T divider=1.0+DistortionCoeff[5]*r2+DistortionCoeff[6]*r4+DistortionCoeff[7]*r6;
	T m1=dividend/divider;
	res(0)=m1*point(0);
	res(1)=m1*point(1);
	res(2)=1;

	res(0)+=2*DistortionCoeff[2]*point(0)*point(1)+DistortionCoeff[3]*(r2+2*point(0)*point(0));
	res(1)+=DistortionCoeff[2]*(r2+2*point(1)*point(1))+2*DistortionCoeff[3]*point(0)*point(1);

	res(0)+=DistortionCoeff[8]*r2+DistortionCoeff[9]*r4;
	res(1)+=DistortionCoeff[8]*r2+DistortionCoeff[9]*r4;
   }
   else
   if(DistortionCoeff.GetSize() == 12)
   {
	T r2=point(0)*point(0)+point(1)*point(1);
	T r4=r2*r2;
	T r6=r4*r2;
	T dividend=1.0+DistortionCoeff[0]*r2+DistortionCoeff[1]*r4+DistortionCoeff[4]*r6;
	T divider=1.0+DistortionCoeff[5]*r2+DistortionCoeff[6]*r4+DistortionCoeff[7]*r6;
	T m1=dividend/divider;
	res(0)=m1*point(0);
	res(1)=m1*point(1);
	res(2)=1;

	res(0)+=2*DistortionCoeff[2]*point(0)*point(1)+DistortionCoeff[3]*(r2+2*point(0)*point(0));
	res(1)+=DistortionCoeff[2]*(r2+2*point(1)*point(1))+2*DistortionCoeff[3]*point(0)*point(1);

	res(0)+=DistortionCoeff[8]*r2+DistortionCoeff[9]*r4;
	res(1)+=DistortionCoeff[10]*r2+DistortionCoeff[11]*r4;
   }
   return res;
  }

  if(DistortionMode == 2)
  {
   if(DistortionCoeff.GetSize()<4)
	return point;

   MVector<T,3> res;
   T x=DistortionCoeff[3]*(point(0)-DistortionCoeff[0]);
   T y=DistortionCoeff[3]*(point(1)-DistortionCoeff[1]);
   T d=x*x+y*y;
   T p=1.0-(DistortionCoeff[2]*d)/100000000.0;
   res(0)=p*x+DistortionCoeff[0];
   res(1)=p*y+DistortionCoeff[1];
   res(2)=1;
   return res;
  }

  // 3 - Модель Цая (DistortionCoeff(0) = k1, DistortionCoeff(1) = f (фокусное расстояние в мм), DistortionCoeff(2) = s (маштабирующий коэффициент)
  if(DistortionMode == 3)
  {
   if(DistortionCoeff.GetSize() != 3)
	return point;

   MVector<T,3> res;

   // Tsai/cal_tran.cpp
   //void      Tsai::undistorted_to_distorted_sensor_coord (double Xu, double Yu, double *Xd, double *Yd)
   double Ru, Rd, lambda, c, d, Q, R, D, S, TT, sinT, cosT;
   double Xu, Yu;	// a, b, x', y' - Здесь и ниже в комментах обозначения OpenCv
   double kappa1;	// k1 (?)

   Xu = DistortionCoeff(1)*point(0);	// a
   Yu = DistortionCoeff(1)*point(1);   // b
   kappa1 = DistortionCoeff(0);	// k1

   if(((Xu == 0) && (Yu == 0)) || kappa1 == 0)
   {
	res(0) = Xu;
	res(1) = Yu;
	res(2) = 1;

	res(0) = res(0) / DistortionCoeff(1);
    res(1) = res(1) / DistortionCoeff(1);

	return res;
   }

   Ru = sqrt(Xu*Xu + Yu*Yu); // r

   c = 1.0 / kappa1;
   d = -c * Ru;

   Q = c / 3.0;
   R = -d / 2.0;
   D = Q*Q*Q + R*R;

   if(D >= 0) 		// one real root
   {
	D = sqrt(D);

	if(R+D == 0)
	 S = 0;
	else if(R+D > 0)
	 S = pow((R+D), (double) 1.0 / 3.0);
	else
	 S = -pow(-(R+D), (double) 1.0 / 3.0);

	if(R-D == 0)
	 TT = 0;
	else if(R-D > 0)
	 TT = pow((R-D), (double) 1.0 / 3.0);
	else
	 TT = -pow(-(R-D), (double) 1.0 / 3.0);

	Rd = S + TT; // TETAd

	if(Rd < 0)
	 Rd = sqrt(-1.0 / (3.0 * kappa1));
   }
   else        // three real roots
   {
	D = sqrt(-D);

	double hypot = sqrt(R*R + D*D);
	if(hypot == 0)
	 S = 0;
	else if(hypot > 0)
	 S = pow((hypot), (double) 1.0 / 3.0);
	else
	 S = -pow(-(hypot), (double) 1.0 / 3.0);

	TT = atan2(D, R) / 3.0;
	sinT = sin(TT);
	cosT = cos(TT);

	// the larger positive root is    2*S*cos(T)
	// the smaller positive root is   -S*cos(T) + SQRT(3)*S*sin(T)
	// the negative root is           -S*cos(T) - SQRT(3)*S*sin(T)

	// Корень из трёх
	double SQRT3 = 1.732050807568877293527446341505872366943;
	Rd = -S * cosT + SQRT3 * S * sinT;	// use the smaller positive root
   }

   lambda = Rd / Ru;

   res(0) = Xu * lambda; // x' = a * TETAd / r
   res(1) = Yu * lambda; // x' = b * TETAd / r
   res(2) = 1.0;

   res(0) = (res(0) / DistortionCoeff(1)) * DistortionCoeff(2);
   res(1) = res(1) / DistortionCoeff(1);

   return res;
  }
 }
 break;

 case 1:
 {
  if(DistortionMode == 1)
  {
   if(DistortionCoeff.GetSize()<1)
	return point;

   MVector<T,3> res;
   res=point;

   if(DistortionCoeff.GetSize() == 4)
   {
	T r2=point(0)*point(0)+point(1)*point(1);
	T r=sqrt(r2);
	T teta=atan(r);
	T teta2=teta*teta;
	T teta4=teta2*teta2;
	T teta6=teta2*teta;
	T teta8=teta4*teta4;
	T teta_d=teta*(1.0+DistortionCoeff[0]*teta2+DistortionCoeff[1]*teta4+DistortionCoeff[2]*teta6+DistortionCoeff[3]*teta8);
	res(0)=(teta_d*point(0))/r;
	res(1)=(teta_d*point(1))/r;
	res(2)=1;
   }
   return res;
  }
 }
 break;
 }

 return point;
}

// Вычисляет положение пикселя на кадре с учетом дисторсии, по положению пикселя без учета дисторсии
template<class T>
MVector<T,3> MCameraStandard<T>::CalcDistortPixelPosition(const MVector<T,3> &undistort_pixel)
{
 MVector<T,3> normal_point(InvIcc*undistort_pixel);
 MVector<T,3> dist_point(CalcPixelPositionFromNormalPosition(normal_point));
 MVector<T,3> result(Icc*dist_point);

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
 if(fabs(cameraspacepoint(2))>10e-7)
 {
  normalpoint(0)=cameraspacepoint(0)/cameraspacepoint(2);
  normalpoint(1)=cameraspacepoint(1)/cameraspacepoint(2);
  normalpoint(2)=1;
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
 if(fabs(cameraspacepoint(2))>10e-7)
 {
  normalpoint(0)=cameraspacepoint(0)/cameraspacepoint(2);
  normalpoint(1)=cameraspacepoint(1)/cameraspacepoint(2);
  normalpoint(2)=1;
 }
 else
  return screenpoint;

 screenpoint=Icc*normalpoint;
 if(screenpoint(0)<0 || screenpoint(0)>=image_width || screenpoint(1)<0 || screenpoint(1)>=image_height)
 {
  return screenpoint;
 }
 distpoint=CalcPixelPositionFromNormalPosition(normalpoint);
 screenpoint=Icc*distpoint;
 res=true;

 return screenpoint;
}

template<class T>
MVector<T,3> MCameraStandard<T>::CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point)
{
 MVector<T,3> normalpoint, distpoint, screenpoint;
 MVector<T,4> temp4;
 MVector<T,3> cameraspacepoint;
 MMatrix<T,3,4> E=MMatrix<T,3,4>::Eye();

 cameraspacepoint=E*(MCamera<T>::GetEcc()*space_point);
 if(fabs(cameraspacepoint(2))>10e-7)
 {
  normalpoint(0)=cameraspacepoint(0)/cameraspacepoint(2);
  normalpoint(1)=cameraspacepoint(1)/cameraspacepoint(2);
  normalpoint(2)=1;
 }
 else
  return screenpoint;

 screenpoint=Icc*normalpoint;

 return screenpoint;
}

template<class T>
MVector<T,3> MCameraStandard<T>::CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point, int image_width, int image_height, bool &res)
{
 res=false;
 MVector<T,3> normalpoint, distpoint, screenpoint;
 MVector<T,4> temp4;
 MVector<T,3> cameraspacepoint;
 MMatrix<T,3,4> E=MMatrix<T,3,4>::Eye();

 cameraspacepoint=E*(MCamera<T>::GetEcc()*space_point);
 if(fabs(cameraspacepoint(2))>10e-7)
 {
  normalpoint(0)=cameraspacepoint(0)/cameraspacepoint(2);
  normalpoint(1)=cameraspacepoint(1)/cameraspacepoint(2);
  normalpoint(2)=1;
 }
 else
  return screenpoint;

 screenpoint=Icc*normalpoint;
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


/// Вычисляет угловое значение пикселя в модели камеры
template<class T>
T MCameraStandard<T>::CalcAngleX(int pixel) const
{
 if(GetIcc()(0,0) < 1e-5 || GetIcc()(1,1)< 1e-5)
  return 0; // TODO: тут исключение
 switch(CameraMode)
 {
 case 0:
 {
  T center=pixel-GetIcc()(0,2);
  T tg_res=center/GetIcc()(0,0);
  T res=atan(tg_res);
  return res;
 }
 break;

 case 1:
 {
  T x_diff=(pixel-GetIcc()(0,2))/GetIcc()(0,0);
  T y_diff=(0-GetIcc()(0,1))/GetIcc()(1,1);
  T teta=sqrt(x_diff*x_diff+y_diff*y_diff);
  teta=ChordMethod(0,M_PI,1e-5,teta); // учет дисторсии
  T beta=atan2(y_diff,x_diff);
  T res=atan(tan(teta)*cos(beta));
  return res;
 }
 break;
 }
 return 0.0;
}

template<class T>
T MCameraStandard<T>::CalcAngleY(int pixel) const
{
 if(GetIcc()(0,0) < 1e-5 || GetIcc()(1,1)< 1e-5)
  return 0; // TODO: тут исключение

 switch(CameraMode)
 {
 case 0:
 {
  T center=pixel-GetIcc()(1,2);
  T tg_res=center/GetIcc()(1,1);
  T res=atan(tg_res);
  return res;
 }
 break;

 case 1:
 {
  T x_diff=(0-GetIcc()(0,2))/GetIcc()(0,0);
  T y_diff=(pixel-GetIcc()(0,1))/GetIcc()(1,1);
  T teta=sqrt(x_diff*x_diff+y_diff*y_diff);
  teta=ChordMethod(0,M_PI,1e-5,teta); // учет дисторсии
  T beta=atan2(x_diff,y_diff);
  T res=atan(tan(teta)*cos(beta));
  return res;
 }
 break;
 }
 return 0.0;
}

/// Вычисляет пиксельное значение угла в модели камеры
template<class T>
T MCameraStandard<T>::CalcPixelXByAngle(T angle) const
{
 switch(CameraMode)
 {
 case 0:
 {
  T tg_angle=tan(angle);
  T res=tg_angle*GetIcc()(0,0)+GetIcc()(0,2);
  return res;
 }
 break;

 case 1:
 {
  T tg_angle=tan(angle);
  T res=tg_angle*GetIcc()(0,0)+GetIcc()(0,2);
  return res;
 }
 break;
 }
 return 0.0;
}

template<class T>
T MCameraStandard<T>::CalcPixelYByAngle(T angle) const
{
 switch(CameraMode)
 {
 case 0:
 {
  T tg_angle=tan(angle);
  T res=tg_angle*GetIcc()(1,1)+GetIcc()(1,2);
  return res;
 }
 break;

 case 1:
 {
  T tg_angle=tan(angle);
  T res=tg_angle*GetIcc()(1,1)+GetIcc()(1,2);
  return res;
 }
 break;
 }
 return 0.0;
}


/// Вычисляет матрицу внутренней калибровки по известным полям зрения
template<class T>
bool MCameraStandard<T>::CalcIccByVisualAngle(T angle_x, T angle_y, T principle_x, T principle_y, int image_width, int image_height, MMatrix<T,3,3> &icc, MMatrix<T,3,3> &norm_icc)
{
 switch(CameraMode)
 {
 case 0:
 {
  T h_angle((angle_x*M_PI/180.0)/2.0);
  T w_angle((angle_y*M_PI/180.0)/2.0);

  icc=icc.Zero();

  if(image_width && h_angle)
   icc(0,0)=(image_width/2.0)/tan(h_angle);
  else
   return false;

  if(image_height && w_angle)
   icc(1,1)=(image_height/2.0)/tan(w_angle);
  else
   return false;

  icc(0,2)=principle_x*image_width;
  icc(1,2)=principle_y*image_height;
  icc(2,2)=1.0;
 }
 break;

 case 1:
 {
  icc=icc.Zero();
  icc(0,0)=image_width/angle_x;
  icc(1,1)=image_height/angle_y;
  icc(0,2)=principle_x*image_width;
  icc(1,2)=principle_y*image_height;
  icc(2,2)=1.0;
 }
 break;
 }

 // Вычисляем нормированную ICC
 norm_icc(0,0)/=image_width;
 norm_icc(1,1)/=image_height;
 norm_icc(0,2)/=image_width;
 norm_icc(1,2)/=image_height;

 return true;
}

/// Вычисляет матрицу внутренней калибровки по известным полям зрения
template<class T>
bool MCameraStandard<T>::CalcVisualAnglesByIcc(const MMatrix<T,3,3> &icc, T &angle_x, T &angle_y, T &principle_x, T &principle_y, int image_width, int image_height)
{
 switch(CameraMode)
 {
 case 0:
 {
  if(icc(0,0) > 1e-5)
   angle_x=(atan(icc(0,2)/icc(0,0))+atan((image_width-icc(0,2))/icc(0,0)))*180.0/M_PI;
  else
   return false;

  if(icc(1,1) > 1e-5)
   angle_y=(atan(icc(1,2)/icc(1,1))+atan((image_height-icc(1,2))/icc(1,1)))*180.0/M_PI;
  else
   return false;

  if(image_width)
   principle_x=icc(0,2)/image_width;
  else
   principle_x=0;

  if(image_height)
   principle_y=icc(1,2)/image_height;
  else
   principle_y=0;
 }
 break;

 case 1:
 {
  if(icc(0,0) > 1e-5)
   angle_x=image_width/icc(0,0);
  else
   return false;

  if(icc(1,1) > 1e-5)
   angle_y=image_height/icc(1,1);
  else
   return false;

  if(image_width)
   principle_x=icc(0,2)/image_width;
  else
   principle_x=0;

  if(image_height)
   principle_y=icc(1,2)/image_height;
  else
   principle_y=0;
 }
 break;
 }
 return true;
}



// Получение мировых координат точки в пространстве по её экранным координатам
template<class T>
int MCameraStandard<T>::GetWorldPoint(double Xfd, double Yfd, double yw, double &xw, double &zw)
{
 if(DistortionMode != 3)
  return 10;

// if(IsInitialized < 2 && fabs(zw) > 0.0000001)
//  return -2;

 return image_coord_to_world_coord(Xfd,Yfd,-yw,xw,zw);
}

template<class T>
int MCameraStandard<T>::image_coord_to_world_coord (double Xfd, double Yfd, double zw, double &xw, double &yw)
{
    double    Xd,
              Yd,
              Xu,
              Yu,
			  common_denominator;

	if(DistortionCoeff.GetSize()<2)
	 return 1;

	if(fabs(DistortionCoeff(2))<0.00001)
	 return 2;

	if(fabs(Icc(0,0))<0.00001)
	 return 3;

	if(fabs(Icc(1,1))<0.00001)
	 return 4;

	/* convert from image to distorted sensor coordinates */
	Xd = (DistortionCoeff(1)/Icc(0,0)) * (Xfd - Icc(0,2)) / DistortionCoeff(2);
	Yd = (DistortionCoeff(1)/Icc(1,1)) * (Yfd - Icc(1,2));

    /* convert from distorted sensor to undistorted sensor plane coordinates */
	distorted_to_undistorted_sensor_coord (Xd, Yd, Xu, Yu);

    /* calculate the corresponding xw and yw world coordinates	 */
    /* (these equations were derived by simply inverting	 */
	/* the perspective projection equations using Macsyma)	 */

	const MMatrix<T,4,4>& ecc=GetEcc();

	double r1=ecc(0,0), r2=ecc(0,1), r3=ecc(0,2),
		   r4=ecc(1,0), r5=ecc(1,1), r6=ecc(1,2),
		   r7=ecc(2,0), r8=ecc(2,1), r9=ecc(2,2);
	double f=DistortionCoeff(1);

	// Положение Tsai
	RDK::MVector<double,6> tsai_position;

	CalcObjectAnglesAndShifts(ecc,tsai_position,6);

  //	mat_tsai_cam_to_scene = RDK::CalcObjectPositionMatrix(tsai_position,6);

//	double Tz=Ecc(3,2), Tx=Ecc(3,0), Ty=Ecc(3,1);
	double Tx=tsai_position(0), Ty=tsai_position(1), Tz=tsai_position(2);

	common_denominator = ((r1 * r8 - r2 * r7) * Yu +
			  (r5 * r7 - r4 * r8) * Xu -
			  f * r1 * r5 + f * r2 * r4);

	if(fabs(common_denominator)<0.00001)
	 return 5;

	xw = (((r2 * r9 - r3 * r8) * Yu +
		(r6 * r8 - r5 * r9) * Xu -
		f * r2 * r6 + f * r3 * r5) * zw +
	   (r2 * Tz - r8 * Tx) * Yu +
	   (r8 * Ty - r5 * Tz) * Xu -
	   f * r2 * Ty + f * r5 * Tx) / common_denominator;

	yw = -(((r1 * r9 - r3 * r7) * Yu +
		 (r6 * r7 - r4 * r9) * Xu -
		 f * r1 * r6 + f * r3 * r4) * zw +
		(r1 * Tz - r7 * Tx) * Yu +
		(r7 * Ty - r4 * Tz) * Xu -
		f * r1 * Ty + f * r4 * Tx) / common_denominator;

 return 0;
}

template<class T>
void MCameraStandard<T>::distorted_to_undistorted_sensor_coord (double Xd, double Yd, double &Xu, double &Yu)
{
	double    distortion_factor;

	double kappa1 = DistortionCoeff(0);

    /* convert from distorted to undistorted sensor plane coordinates */
    distortion_factor = 1 + kappa1 * (Xd*Xd + Yd*Yd);
	Xu = Xd * distortion_factor;
    Yu = Yd * distortion_factor;
}


// Функция вычисления дисторсии для fisheye thetaD=theta+k1*theta^3+k2*theta^5+k3*theta^7+k4*theta^9  (theta+k1*theta^3+k2*theta^5+k3*theta^7+k4*theta^9 - thetaD = 0)
template<class T>
T MCameraStandard<T>::FuncTheta(T thetaD, T Xthet) const
{
	if(DistortionCoeff.GetSize()<4) return thetaD;

	T Xthet3=Xthet*Xthet*Xthet;
	T Xthet5=Xthet3*Xthet*Xthet;
	T Xthet7=Xthet5*Xthet*Xthet;
	T Xthet9=Xthet7*Xthet*Xthet;
	T res= Xthet+DistortionCoeff(0)*Xthet3+DistortionCoeff(1)*Xthet5+DistortionCoeff(2)*Xthet7+DistortionCoeff(3)*Xthet9-thetaD;

	return res;
}

// Метод хорд для нахождения численного решения уравнения относительно theta
template<class T>
T MCameraStandard<T>::ChordMethod(T Cx1, T Cx2, double epsilon, T thetaD) const
{
	if( fabs(Cx2-Cx1)< epsilon) return (Cx1+Cx1)/2;

	T Cy1 = FuncTheta(thetaD,Cx1);
	T Cy2 = FuncTheta(thetaD,Cx2);

	T Cx3 = Cx1-(Cx2-Cx1)*Cy1/(Cy2-Cy1);

	return ChordMethod(Cx2, Cx3, epsilon, thetaD);
}

template<class T>
MCameraStandard<T>& MCameraStandard<T>::operator = (const MCameraStandard<T>& copy)
{
 *static_cast<MCamera<T>*>(this)=copy;
 DistortionMode=copy.DistortionMode;
 CameraMode=copy.CameraMode; 
 DistortionCoeff=copy.DistortionCoeff;
 SetIcc(copy.GetIcc());
	
 return *this;
}

}
#endif

