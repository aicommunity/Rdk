/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2012.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MKalmanFilterH
#define MKalmanFilterH

#include "MMatrix.h"

namespace RDK {

template<class T, unsigned Size>
class MKalmanFilter
{
public:
typedef MMatrix<T,Size,Size> KMatrix;
typedef MMatrix<T,Size,1> KVector;
protected: // Параметры

public: // Данные
KMatrix FM;
KMatrix BM;
KMatrix QM;
KMatrix HM;
KMatrix RM;

KMatrix Pk1;
MMatrix<T,Size,1> Xk1;
MMatrix<T,Size,1> Uk1;
MMatrix<T,Size,1> Z;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MKalmanFilter(void) : FM(0.0),BM(0.0),QM(0.0),HM(0.0),RM(0.0),Pk1(0.0),Xk1(0.0),Uk1(0.0),Z(0.0) {};
virtual ~MKalmanFilter(void) {};
// --------------------------

// --------------------------
// Методы счета
// --------------------------
// Предсказание
MMatrix<T,Size,1> StatePrediction(const KMatrix &F, const KMatrix &B,
					 const MMatrix<T,Size,1> &xk1, const MMatrix<T,Size,1> &uk1)
{
 return F*xk1+B*uk1;// xkL
}

KMatrix CovariationError(const KMatrix &F, const KMatrix &Pk1,
					 const KMatrix &Q)
{
 return F*Pk1*F.Transpose()+Q; // PkL
}

// Корректировка
KMatrix KalmanGain(const KMatrix &PkL, const KMatrix &H,
					 const KMatrix &R)
{
 return PkL*H.Transpose()*(H*PkL*H.Transpose()+R).Inverse(); // Kk
}

MMatrix<T,Size,1> EstimationUpdate(const MMatrix<T,Size,1> &xkL, const KMatrix &Kk,
					 const MMatrix<T,Size,1> &zk, const KMatrix &H)
{
 return xkL+Kk*(zk-H*xkL); // xk
}

KMatrix CovariationErrorUpdate(const KMatrix &Kk, const KMatrix &H,
					 const KMatrix &PkL)
{
 return (KMatrix::Eye()-Kk*H)*PkL; // Pk
}

// Калман
void KalmanCalculate(int i)
{
 MMatrix<T,Size,1> xkL=StatePrediction(FM,BM,Xk1,Uk1);
 MMatrix<T,Size,Size> PkL;
 if (i>0)
  PkL=CovariationError(FM,Pk1,QM);
 else
  PkL=Pk1;

 MMatrix<T,Size,Size> Kk=KalmanGain(PkL,HM,RM);
 Xk1=EstimationUpdate(xkL,Kk,Z,HM);
 Pk1=CovariationErrorUpdate(Kk,HM,PkL);
}
// --------------------------
};

}
#endif
