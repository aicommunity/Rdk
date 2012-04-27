/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 1997.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MBODY_H
#define MBODY_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"MTheormec.h"

#define FORCE 0
#define MOMENT 1

namespace RDK {


template<class T>
class MBody
{
public:
MVector<T,3> Location;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MBody(void);
MBody(const MBody &copy);
virtual ~MBody(void);
// --------------------------

// --------------------------
// Методы доступа к данными
// --------------------------
const MVector<T,3>& GetLocation(void) const;
bool SetLocation(MVector<T,3> &loc);
// --------------------------


// --------------------------
// Operators
// --------------------------
// Оператор присваивания
MBody& operator = (const MBody &copy);

// Проверка на равенство.
bool operator == (const MBody& b) const;

// Проверка на неравенство.
bool operator != (const MBody& b) const;
// --------------------------
};

//******************************************************************
//******************************************************************
template<class T>
class MKinematicBody: public MBody<T>
{
public:
//  Kinematics data
std::vector<MRotationTensor<T> > Rotation;
MVector<T,3> TranslationSpeed;
MVector<T,3> AngleSpeed;

public:
MKinematicBody(void);
MKinematicBody(const MKinematicBody &copy);
virtual ~MKinematicBody(void){};

// Set startup characteristics
// Устанавливает заданный тензор поворота
void SetRotation(size_t i, MRotationTensor<T> &P);

// Добавляет в конец новый тензор поворота
void AddRotation(MRotationTensor<T> &P);

// Удаляет тензор поворота по индексу
void DelRotation(size_t ix);

// Удаляет все тензоры поворота
void ClearRotations(void);

void SetTranslationSpeed(MVector<T,3> &spd);
void SetAngleSpeed(MVector<T,3> &spd);

// Get mechanical characteristics
// Возвращает число тензоров поворота
size_t GetNumRotations(void) const;

const MRotationTensor<T>& GetRotation(size_t ix) const;
const MVector<T,3>& GetTranslationSpeed(void) const;
const MVector<T,3>& GetAngleSpeed(void) const;

// Run-Time Functions (RTF)
void RTMovement(T time);

// Operators
MKinematicBody& operator = (const MKinematicBody &copy);

// Проверка на равенство.
bool operator == (const MKinematicBody& b) const;

// Проверка на неравенство.
bool operator != (const MKinematicBody& b) const;
};

//******************************************************************
//******************************************************************

template<class T>
class MMechanicalBody: public MKinematicBody<T>
{
public:
// Inertial data
MVector<T,3> MassCenter;
T Mass;
MInertiaTensor<T> CIT; // CIT - Central Inertia Tensor

public:
MMechanicalBody(void);
MMechanicalBody(const MMechanicalBody &copy);
virtual ~MMechanicalBody(void);

// Set startup characteristics
void SetMassCenter(MVector<T,3> &mc);// REM:Mass center is equal Location in mechanical body.
void SetMass(T mass);
void SetAxisInertia(int num,MVector<T,3> &axis);
void SetAxisInertia(MVector<T,3> &axis1,     MVector<T,3> &axis2, MVector<T,3> &axis3);
void SetMomentInertia(int num,T mom);
void SetMomentInertia(T mom1,T mom2,T mom3);
void SetCIT(MInertiaTensor<T> &cit);

// Get mechanical characteristics
MVector<T,3> GetMassCenter(void);
T GetMass(void);
MInertiaTensor<T> GetCIT(void);

T GetKineticEnergy(void); // Кинетическая энергия
MVector<T,3> GetMomentum(void);// Количество движения
MVector<T,3> GetDynamicSpin(void);// Динамический спин тела.
MVector<T,3> GetAngularMomentum(MVector<T,3> &SupportPoint);// Момент кол-ва движения.
MVector<T,3> GetKineticMoment(MVector<T,3> &SupportPoint);// Кинетический момент.

// Run-Time Functions (RTF)
void RTInfluence(int kind,MVector<T,3> influence,T time);

// -----------------
// Operators
// -----------------
// Оператор присваивания
MMechanicalBody& operator = (const MMechanicalBody &copy);

// Проверка на равенство.
bool operator == (const MMechanicalBody& b) const;

// Проверка на неравенство.
bool operator != (const MMechanicalBody& b) const;
// -----------------
};

// **************************************************************************
// **************************************************************************
// Реализация классов
// **************************************************************************
// **************************************************************************
// MBody
// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<class T>
MBody<T>::MBody(void)
{
 Location=0.0;
}

template<class T>
MBody<T>::MBody(const MBody<T> &copy)
{
 *this=copy;
}

template<class T>
MBody<T>::~MBody(void)
{
}
// --------------------------

// --------------------------
// Методы доступа к данными
// --------------------------
template<class T>
const MVector<T,3>& MBody<T>::GetLocation(void) const
{
 return Location;
}

template<class T>
bool MBody<T>::SetLocation(MVector<T,3> &loc)
{
 if(Location == loc)
  return true;

 Location=loc;
 return true;
}
// --------------------------


// --------------------------
// Operators
// --------------------------
// Оператор присваивания
template<class T>
MBody<T>& MBody<T>::operator = (const MBody<T> &copy)
{
 Location=copy.Location;
 return *this;
}

// Проверка на равенство.
template<class T>
bool MBody<T>::operator == (const MBody<T>& b) const
{
 return Location == b.Location;
}

// Проверка на неравенство.
template<class T>
bool MBody<T>::operator != (const MBody<T>& b) const
{
 return Location != b.Location;
}
// --------------------------



// constructor & destructor.
template<class T>
MKinematicBody<T>::MKinematicBody(void)
 : MBody<T>()
{
 Rotation.push_back(MRotationTensor<T>(0.0,0.0));
 TranslationSpeed=0.0;
 AngleSpeed=0.0;
}

template<class T>
MKinematicBody<T>::MKinematicBody(const MKinematicBody<T> &copy)
 : MBody<T>(copy)
{
 *this=copy;
}

// Set startup characteristics
// Устанавливает заданный тензор поворота
template<class T>
void MKinematicBody<T>::SetRotation(size_t i, MRotationTensor<T> &P)
{
 Rotation[i]=P;
}

// Добавляет в конец новый тензор поворота
template<class T>
void MKinematicBody<T>::AddRotation(MRotationTensor<T> &P)
{
 Rotation.push_back(P);
}

// Удаляет тензор поворота по индексу
template<class T>
void MKinematicBody<T>::DelRotation(size_t ix)
{
 Rotation.erase(Rotation.begin()+ix);
}

// Удаляет все тензоры поворота
template<class T>
void MKinematicBody<T>::ClearRotations(void)
{
 Rotation.clear();
}

template<class T>
void MKinematicBody<T>::SetTranslationSpeed(MVector<T,3> &spd)
{
 TranslationSpeed=spd;
}

template<class T>
void MKinematicBody<T>::SetAngleSpeed(MVector<T,3> &spd)
{
 AngleSpeed=spd;
}


// Get mechanical characteristics
// Возвращает число тензоров поворота
template<class T>
size_t MKinematicBody<T>::GetNumRotations(void) const
{
 return Rotation.size();
}

template<class T>
const MRotationTensor<T>& MKinematicBody<T>::GetRotation(size_t ix) const
{
 return Rotation[ix];
}

template<class T>
const MVector<T,3>& MKinematicBody<T>::GetTranslationSpeed(void) const
{
 return TranslationSpeed;
}

template<class T>
const MVector<T,3>& MKinematicBody<T>::GetAngleSpeed(void) const
{
 return AngleSpeed;
}


// Run-Time Functions (RTF)
template<class T>
void MKinematicBody<T>::RTMovement(T time)
{
 T len=!(AngleSpeed);

 this->Location+=TranslationSpeed*time;
 Rotation*=MRotationTensor<T>(AngleSpeed/len,len*time);
}

// Operators
template<class T>
MKinematicBody<T>& MKinematicBody<T>::operator = (const MKinematicBody<T> &copy)
{
 *static_cast<MBody<T>*>(this)=static_cast<const MBody<T>&>(copy);
 Rotation=copy.Rotation;
 TranslationSpeed=copy.TranslationSpeed;
 AngleSpeed=copy.AngleSpeed;

 return *this;
}

// Проверка на равенство.
template<class T>
bool MKinematicBody<T>::operator == (const MKinematicBody<T>& b) const
{
 return static_cast<const MBody<T>*>(this)->operator == (b) & (Rotation == b.Rotation) &
  (TranslationSpeed == b.TranslationSpeed) & (AngleSpeed == b.AngleSpeed);
}

// Проверка на неравенство.
template<class T>
bool MKinematicBody<T>::operator != (const MKinematicBody<T>& b) const
{
 return !operator == (b);
}

// *******************************************************
// constructor & destructor.
template<class T>
MMechanicalBody<T>::MMechanicalBody(void)
{
 MassCenter=0;
 Mass=0;
}

template<class T>
MMechanicalBody<T>::MMechanicalBody(const MMechanicalBody<T> &copy)
 : MKinematicBody<T>(copy)
{
 *this=copy;
}

template<class T>
MMechanicalBody<T>::~MMechanicalBody(void)
{
}

// Set startup characteristics
template<class T>
void MMechanicalBody<T>::SetMassCenter(MVector<T,3> &mc)
{
 MassCenter=this->Location=mc;
}

template<class T>
void MMechanicalBody<T>::SetMass(T mass)
{
 Mass=mass;
}

template<class T>
void MMechanicalBody<T>::SetAxisInertia(int num,MVector<T,3> &axis)
{
 *(&CIT.d1+num)=axis;
}

template<class T>
void MMechanicalBody<T>::SetAxisInertia(MVector<T,3> &axis1, MVector<T,3> &axis2,
                                     MVector<T,3> &axis3)
{
 CIT.d1=axis1;
 CIT.d2=axis2;
 CIT.d3=axis3;
}

template<class T>
void MMechanicalBody<T>::SetMomentInertia(int num,T mom)
{
 *(&CIT.m1+num)=mom;
}

template<class T>
void MMechanicalBody<T>::SetMomentInertia(T mom1,T mom2,T mom3)
{
 CIT.m1=mom1;
 CIT.m2=mom2;
 CIT.m3=mom3;
}

template<class T>
void MMechanicalBody<T>::SetCIT(MInertiaTensor<T> &cit)
{
 CIT=cit;
}


// Get mechanical characteristics
template<class T>
MVector<T,3> MMechanicalBody<T>::GetMassCenter(void)
{
 return MassCenter;
}

template<class T>
T MMechanicalBody<T>::GetMass(void)
{
 return Mass;
}

template<class T>
MInertiaTensor<T> MMechanicalBody<T>::GetCIT(void)
{
 return CIT;
}

template<class T>
T MMechanicalBody<T>::GetKineticEnergy(void) // Кинетическая энергия
{
 T mVV,wCw;

 mVV=Mass*(this->TranslationSpeed*this->TranslationSpeed);
 wCw=this->AngleSpeed*(CIT*this->AngleSpeed);
 return 0.5*mVV+0.5*wCw;
}

template<class T>
MVector<T,3> MMechanicalBody<T>::GetMomentum(void) // Количество движения
{
 return Mass*this->TranslationSpeed;
}

template<class T>
MVector<T,3> MMechanicalBody<T>::GetDynamicSpin(void) // Динамический спин тела.
{
 return CIT*this->AngleSpeed;
}

template<class T>
MVector<T,3> MMechanicalBody<T>::GetAngularMomentum(MVector<T,3> &SupportPoint) // Момент кол-ва движения.
{
 return (MassCenter-SupportPoint)^GetMomentum();
}

template<class T>
MVector<T,3> MMechanicalBody<T>::GetKineticMoment(MVector<T,3> &SupportPoint) // Кинетический момент.
{
 return GetAngularMomentum(SupportPoint)+GetDynamicSpin();
}


// Run-Time Functions (RTF)
template<class T>
void MMechanicalBody<T>::RTInfluence(int kind,MVector<T,3> influence,T time)
{
 MInertiaTensor<T> I;

 switch(kind)
 {
 case FORCE:
	 this->TranslationSpeed+=(influence*time)/Mass;
 break;
 case MOMENT:
  I.d1=CIT.d1; I.d2=CIT.d2; I.d3=CIT.d3;
  I.m1=1./CIT.m1; I.m2=1./CIT.m2; I.m3=1./CIT.m3;
  this->AngleSpeed+=I*(influence*time);
 };
}


// -----------------
// Operators
// -----------------
// Оператор присваивания
template<class T>
MMechanicalBody<T>& MMechanicalBody<T>::operator = (const MMechanicalBody<T> &copy)
{
 *static_cast<MKinematicBody<T>*>(this)=static_cast<const MKinematicBody<T>&>(copy);
 MassCenter=copy.MassCenter;
 Mass=copy.Mass;
 CIT=copy.CIT;
 return *this;
}

// Проверка на равенство.
template<class T>
bool MMechanicalBody<T>::operator == (const MMechanicalBody<T>& b) const
{
 return static_cast<const MKinematicBody<T>* const>(this)->operator ==(b)
 & (MassCenter == b.MassCenter) & (Mass == b.Mass) & (CIT == b.CIT);
}

// Проверка на неравенство.
template<class T>
bool MMechanicalBody<T>::operator != (const MMechanicalBody<T>& b) const
{
 return !operator == (b);
}
// -----------------


}
#endif

