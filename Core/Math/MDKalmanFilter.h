//---------------------------------------------------------------------------

#ifndef MDKALMAN_FILTER_H
#define MDKALMAN_FILTER_H
//---------------------------------------------------------------------------

#include "MDMatrix.h"

namespace RDK {

template<class T>
class MDKalmanFilter
{
public:
//typedef MMatrix<T,Size,Size> KMatrix;
//typedef MMatrix<T,Size,1> KVector;


protected: // Параметры
// Размерность матриц
int Size;

// Счетчик итераций
int CalcCount;

public: // Данные

MDMatrix<T> FM;  // Матрица динамической модели системы
MDMatrix<T> BM;  // Матрица применения управляющего воздействия
MDMatrix<T> QM;  // Матрица ковариации шума процесса
MDMatrix<T> HM;  // Матрица отношения измерений и состояний ?
MDMatrix<T> RM;  // Матрица ковариации шума измерений

MDMatrix<T> Pk1; // Матрица ошибки в прошлый момент времени
MDMatrix<T> Xk1; // Вектор состояния системы в прошлый момент времени
MDMatrix<T> Uk1; // Вектор управляющего воздействия в прошлый момент времени
MDMatrix<T> Z;   // Вектор состояния системы в текущий момент времени


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MDKalmanFilter(void) : Size(1), CalcCount(0), FM(Size, Size, 0.0),BM(Size, Size, 0.0),QM(Size, Size, 0.0),HM(Size, Size, 0.0),RM(Size, Size, 0.0),Pk1(Size, Size, 0.0),Xk1(Size, 1, 0.0),Uk1(Size, 1, 0.0),Z(Size, 1, 0.0) {};
virtual ~MDKalmanFilter(void) {};
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Задаем размерность матриц Калмана(кол-во прогнозируемых параметров)
bool SetKalmanSize(int value)
{
 if(Size==value)
  return true;

 KalmanResize(value);
 Size=value;

 return true;
}

int GetKalmanSize(void) const
{
 return Size;
}
// Методы доступа к счетчику итераций
bool SetCalcCount(int value)
{
 if(CalcCount==value)
  return true;

 CalcCount=value;
 return true;
}

int GetCalcCount(void) const
{
 return CalcCount;
}

// Задаем начальные параметры Фильтра Калмана
const MDMatrix<double>& GetFM(void) const
{
 return FM;
}

bool SetFM(const MDMatrix<double> &matrix)
{
 if(matrix==FM)
  return true;

 FM=matrix;
 return true;
}

const MDMatrix<double>& GetBM(void) const
{
 return BM;
}

bool SetBM(const MDMatrix<double> &matrix)
{
 if(matrix==BM)
  return true;

 BM=matrix;
 return true;
}

const MDMatrix<double>& GetQM(void) const
{
 return QM;
}

bool SetQM(const MDMatrix<double> &matrix)
{
 if(matrix==QM)
  return true;

 QM=matrix;
 return true;
}

const MDMatrix<double>& GetHM(void) const
{
 return HM;
}

bool SetHM(const MDMatrix<double> &matrix)
{
 if(matrix==HM)
  return true;

 HM=matrix;
 return true;
}

const MDMatrix<double>& GetRM(void) const
{
 return RM;
}

bool SetRM(const MDMatrix<double> &matrix)
{
 if(matrix==RM)
  return true;

 RM=matrix;
 return true;
}

const MDMatrix<double>& GetPk1(void) const
{
 return Pk1;
}

bool SetPk1(const MDMatrix<double> &matrix)
{
 if(matrix==Pk1)
  return true;

 Pk1=matrix;
 return true;
}

const MDMatrix<double>& GetUk1(void) const
{
 return Uk1;
}

bool SetUk1(const MDMatrix<double> &matrix)
{
 if(matrix==Uk1)
  return true;

 Uk1=matrix;
 return true;
}

const MDMatrix<double>& GetZ(void) const
{
 return Z;
}

bool SetZ(const MDMatrix<double> &matrix)
{
 if(matrix==Z)
  return true;

 Z=matrix;
 return true;
}
// --------------------------
// Методы счета
// --------------------------
// Изменение размера матриц
bool KalmanResize(int value)
{
 if(value==Size)
  return true;

 FM.Resize(value, value);
 BM.Resize(value, value);
 QM.Resize(value, value);
 HM.Resize(value, value);
 RM.Resize(value, value);

 Pk1.Resize(value, value);

 Xk1.Resize(value, 1);
 Uk1.Resize(value, 1);
 Z.Resize(value, 1);

 Size=value;
 return true;
}

// Сброс всех матриц в нулевые значения
bool KalmanReset(void)
{
 CalcCount=0;

 return true;
}

// Предсказание
MDMatrix<T> StatePrediction(const MDMatrix<T> &F, const MDMatrix<T> &B,
					 const MDMatrix<T> &xk1, const MDMatrix<T> &uk1)
{
 return F*xk1+B*uk1;// xkL
}

MDMatrix<T> CovariationError(const MDMatrix<T> &F, const MDMatrix<T> &Pk1,
					 const MDMatrix<T> &Q)
{
 return F*Pk1*F.Transpose()+Q; // PkL
}

// Корректировка
MDMatrix<T> KalmanGain(const MDMatrix<T> &PkL, const MDMatrix<T> &H,
					 const MDMatrix<T> &R)
{
 return PkL*H.Transpose()*(H*PkL*H.Transpose()+R).Inverse(); // Kk Ошибка на итерации 211
}

MDMatrix<T> EstimationUpdate(const MDMatrix<T> &xkL, const MDMatrix<T> &Kk,
					 const MDMatrix<T> &zk, const MDMatrix<T> &H)
{
 return xkL+Kk*(zk-H*xkL); // xk
}

MDMatrix<T> CovariationErrorUpdate(const MDMatrix<T> &Kk, const MDMatrix<T> &H,
					 const MDMatrix<T> &PkL)
{
 MDMatrix<T> eye_matrix(Size, Size);
 eye_matrix.Eye();

 return (eye_matrix-Kk*H)*PkL; // Pk
}

//Калман предсказание без корректировки
void KalmanPredict(int i)
{
 MDMatrix<T> xkL=StatePrediction(FM,BM,Xk1,Uk1);
 MDMatrix<T> PkL;

 if (i>0)
  PkL=CovariationError(FM,Pk1,QM);
 else
  PkL=Pk1;

 Xk1=xkL;
 Pk1=PkL;

 //MDMatrix<T> Kk=KalmanGain(PkL,HM,RM);
 CalcCount++;
}
// Калман
void KalmanCalculate(int i)
{
 MDMatrix<T> xkL=StatePrediction(FM,BM,Xk1,Uk1);
 MDMatrix<T> PkL;

 if (i>0)
  PkL=CovariationError(FM,Pk1,QM);
 else
  PkL=Pk1;

 MDMatrix<T> Kk=KalmanGain(PkL,HM,RM);
 Xk1=EstimationUpdate(xkL,Kk,Z,HM);
 Pk1=CovariationErrorUpdate(Kk,HM,PkL);
 CalcCount++;
}
// --------------------------
};

}



#endif
