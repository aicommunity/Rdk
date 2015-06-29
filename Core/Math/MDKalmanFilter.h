//---------------------------------------------------------------------------

#ifndef MDKALMAN_FILTER_H
#define MDKALMAN_FILTER_H
//---------------------------------------------------------------------------

#include <cmath>
#include "MDMatrix.h"

namespace RDK {

template<class T>
class MDKalmanFilter
{
protected: // Параметры
// Число состояний системы
int NumStates;

/// Число измеряемых величин
int NumMeasurements;

// Счетчик итераций
int CalcCount;

public: // Данные

MDMatrix<T> FM;  // Матрица динамической модели системы
MDMatrix<T> BM;  // Матрица применения управляющего воздействия
MDMatrix<T> QM;  // Матрица ковариации шума процесса
MDMatrix<T> HM;  // Матрица отношения измерений и состояний ?
MDMatrix<T> RM;  // Матрица ковариации шума измерений

protected:
MDMatrix<T> Pk1; // Матрица ошибки в прошлый момент времени
MDMatrix<T> Xk1; // Вектор состояния системы в прошлый момент времени
MDMatrix<T> Uk1; // Вектор управляющего воздействия в прошлый момент времени
MDMatrix<T> Z;   // Вектор состояния системы в текущий момент времени

public: // Исключения
class EKalmanGainOverflow: public EError {};

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MDKalmanFilter(void) : NumStates(1), NumMeasurements(1), CalcCount(0), FM(NumStates, NumStates, 0.0),BM(NumStates, NumMeasurements, 0.0),QM(NumStates, NumStates, 0.0),HM(NumMeasurements, NumStates, 0.0),RM(NumMeasurements, NumMeasurements, 0.0),Pk1(NumStates, NumStates, 0.0),Xk1(NumStates, 1, 0.0),Uk1(NumStates, 1, 0.0),Z(NumMeasurements, 1, 0.0) {};
virtual ~MDKalmanFilter(void) {};
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Задаем размерность матриц Калмана(кол-во прогнозируемых параметров)
bool SetKalmanSize(int num_states, int num_measurements)
{
 if(NumStates == num_states && NumMeasurements == num_measurements)
  return true;

 KalmanResize(num_states, num_measurements);
 NumMeasurements=num_measurements;
 NumStates=num_states;

 return true;
}

int GetNumStates(void) const
{
 return NumStates;
}

int GetNumMeasurements(void) const
{
 return NumMeasurements;
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

const MDMatrix<T>& GetXk1(void) const
{
 return Xk1;
}

bool SetXk1(const MDMatrix<double> &matrix)
{
 if(matrix==Xk1)
  return true;

 Xk1=matrix;
 return true;
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
bool KalmanResize(int num_states, int num_measurements)
{
 if(NumStates == num_states && NumMeasurements == num_measurements)
  return true;

 FM.Resize(num_states, num_states);
 BM.Resize(num_states, num_measurements);
 QM.Resize(num_states, num_states);
 HM.Resize(num_measurements, num_states);
 RM.Resize(num_measurements, num_measurements);

 Pk1.Resize(num_states, num_states);

 Xk1.Resize(num_states, 1);
 Uk1.Resize(num_states, 1);
 Z.Resize(num_measurements, 1);

 NumMeasurements=num_measurements;
 NumStates=num_states;
 return true;
}

// Сброс всех матриц в нулевые значения
bool KalmanReset(const MDMatrix<T> &xk1, const MDMatrix<T> &pk1)
{
 CalcCount=0;

 Z=Xk1=xk1;
 Pk1=pk1;

 return true;
}

// Предсказание
MDMatrix<T> StatePrediction(const MDMatrix<T> &F, const MDMatrix<T> &B,
					 const MDMatrix<T> &xk1, const MDMatrix<T> &uk1)
{
 MDMatrix<T> M1(F*xk1);
 MDMatrix<T> M2(B*uk1);
 return M1+M2;// xkL
}

MDMatrix<T> CovariationError(const MDMatrix<T> &F, const MDMatrix<T> &Pk1,
					 const MDMatrix<T> &Q)
{
 MDMatrix<T> M1(F*Pk1);
 MDMatrix<T> M2(M1*F.Transpose());

 return M2+Q; // PkL
}

// Корректировка
MDMatrix<T> KalmanGain(const MDMatrix<T> &PkL, const MDMatrix<T> &H,
					 const MDMatrix<T> &R)
{
 try
 {
  MDMatrix<T> M1(PkL*H.Transpose());
  MDMatrix<T> M2(H*PkL);
  MDMatrix<T> M3(M2*H.Transpose());
  MDMatrix<T> M4(M3+R);

  for(int i=0;i<M4.GetCols()*M4.GetRows();i++)
   if(fabs(M4.Data[i])>1e20)
	throw EKalmanGainOverflow();
  MDMatrix<T> M5(M4.Inverse());
  MDMatrix<T> M6(M1*M5);
  return M6;
 }
 catch(EMatrixZeroDet &exception)
 {
   throw EKalmanGainOverflow();
 }
 MDMatrix<T> Dummy;
 return Dummy;
}

MDMatrix<T> EstimationUpdate(const MDMatrix<T> &xkL, const MDMatrix<T> &Kk,
					 const MDMatrix<T> &zk, const MDMatrix<T> &H)
{
 MDMatrix<double> M1(zk-H*xkL);
 MDMatrix<double> M2(Kk*M1);


// return xkL+Kk*(zk-H*xkL); // xk
 return xkL+M2; // xk
}

MDMatrix<T> CovariationErrorUpdate(const MDMatrix<T> &Kk, const MDMatrix<T> &H,
					 const MDMatrix<T> &PkL)
{
 MDMatrix<T> eye_matrix(NumStates, NumStates);
 eye_matrix=eye_matrix.Eye();

 return (eye_matrix-Kk*H)*PkL; // Pk
}

//Калман предсказание без корректировки
void KalmanPredict(int i)
{
 // prediction
 Xk1 = FM * Xk1 + BM*Uk1;
 Pk1 = FM * Pk1 * FM.Transpose()+QM;

 /*
  MDMatrix<T> xkL=StatePrediction(FM,BM,Xk1,Uk1);
 MDMatrix<T> PkL;

 if (i>0)
  PkL=CovariationError(FM,Pk1,QM);
 else
  PkL=Pk1;

 Xk1=xkL;
 Pk1=PkL;
   */
 CalcCount++;
}
// Калман
void KalmanCalculate(int i)
{
 try
 {
  MDMatrix<double> test;

  // prediction
  Xk1 = FM * Xk1 + BM*Uk1;
  Pk1 = FM * Pk1 * FM.Transpose()+QM;

  test=Pk1;

  MDMatrix<double> y(Z - (HM * Xk1));
  MDMatrix<double> S(HM * Pk1 * HM.Transpose() + RM);
  for(int i=0;i<S.GetCols()*S.GetRows();i++)
   if(fabs(S.Data[i])>1e20)
	throw EKalmanGainOverflow();
  MDMatrix<double> K(Pk1 * HM.Transpose() * S.Inverse());
  Xk1 = Xk1 + (K * y);
  MDMatrix<double> eye(Pk1.GetRows(),Pk1.GetCols());
  eye=eye.Eye();
  test=Pk1;
  Pk1 = (eye - (K * HM)) * Pk1;

  test=Pk1;
 }
 catch(EMatrixZeroDet &exception)
 {
   throw EKalmanGainOverflow();
 }

	/*
 MDMatrix<T> Kk=KalmanGain(Pk1,HM,RM);
 Xk1=EstimationUpdate(Xk1,Kk,Z,HM);
 Pk1=CovariationErrorUpdate(Kk,HM,Pk1);
 Xk1=StatePrediction(FM,BM,Xk1,Uk1);
 Pk1=CovariationError(FM,Pk1,QM);
	  */
 CalcCount++;
}
// --------------------------
};

}



#endif
