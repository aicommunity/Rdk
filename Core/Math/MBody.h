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
MVector<T> Location;

public:
// --------------------------
// ������������ � �����������
// --------------------------
MBody(void);
MBody(const MBody &copy);
virtual ~MBody(void);
// --------------------------

// --------------------------
// ������ ������� � �������
// --------------------------
const MVector<T>& GetLocation(void) const;
bool SetLocation(MVector<T> &loc);
// --------------------------


// --------------------------
// Operators
// --------------------------
// �������� ������������
MBody& operator = (const MBody &copy);

// �������� �� ���������.
bool operator == (const MBody& b) const;

// �������� �� �����������.
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
MVector<T> TranslationSpeed;
MVector<T> AngleSpeed;

public:
MKinematicBody(void);
MKinematicBody(const MKinematicBody &copy);
virtual ~MKinematicBody(void){};

// Set startup characteristics
// ������������� �������� ������ ��������
void SetRotation(size_t i, MRotationTensor<T> &P);

// ��������� � ����� ����� ������ ��������
void AddRotation(MRotationTensor<T> &P);

// ������� ������ �������� �� �������
void DelRotation(size_t ix);

// ������� ��� ������� ��������
void ClearRotations(void);

void SetTranslationSpeed(MVector<T> &spd);
void SetAngleSpeed(MVector<T> &spd);

// Get mechanical characteristics
// ���������� ����� �������� ��������
size_t GetNumRotations(void) const;

const MRotationTensor<T>& GetRotation(size_t ix) const;
const MVector<T>& GetTranslationSpeed(void) const;
const MVector<T>& GetAngleSpeed(void) const;

// Run-Time Functions (RTF)
void RTMovement(T time);

// Operators
MKinematicBody& operator = (const MKinematicBody &copy);

// �������� �� ���������.
bool operator == (const MKinematicBody& b) const;

// �������� �� �����������.
bool operator != (const MKinematicBody& b) const;
};

//******************************************************************
//******************************************************************

template<class T>
class MMechanicalBody:virtual public MKinematicBody<T>
{
public:
// Inertial data
MVector<T> MassCenter;
T Mass;
MInertiaTensor<T> CIT; // CIT - Central Inertia Tensor

public:
MMechanicalBody(void);
MMechanicalBody(const MMechanicalBody &copy);
virtual ~MMechanicalBody(void);

// Set startup characteristics
void SetMassCenter(MVector<T> &mc);// REM:Mass center is equal Location in mechanical body.
void SetMass(T mass);
void SetAxisInertia(int num,MVector<T> &axis);
void SetAxisInertia(MVector<T> &axis1,     MVector<T> &axis2, MVector<T> &axis3);
void SetMomentInertia(int num,T mom);
void SetMomentInertia(T mom1,T mom2,T mom3);
void SetCIT(MInertiaTensor<T> &cit);

// Get mechanical characteristics
MVector<T> GetMassCenter(void);
T GetMass(void);
MInertiaTensor<T> GetCIT(void);

T GetKineticEnergy(void); // ������������ �������
MVector<T> GetMomentum(void);// ���������� ��������
MVector<T> GetDynamicSpin(void);// ������������ ���� ����.
MVector<T> GetAngularMomentum(MVector<T> &SupportPoint);// ������ ���-�� ��������.
MVector<T> GetKineticMoment(MVector<T> &SupportPoint);// ������������ ������.

// Run-Time Functions (RTF)
void RTInfluence(int kind,MVector<T> influence,T time);

// -----------------
// Operators
// -----------------
// �������� ������������
MMechanicalBody& operator = (const MMechanicalBody &copy);

// �������� �� ���������.
bool operator == (const MMechanicalBody& b) const;

// �������� �� �����������.
bool operator != (const MMechanicalBody& b) const;
// -----------------
};

// **************************************************************************
// **************************************************************************
// ���������� �������
// **************************************************************************
// **************************************************************************
// MBody
// --------------------------
// ������������ � �����������
// --------------------------
template<class T>
MBody<T>::MBody(void)
{
 Location=0;
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
// ������ ������� � �������
// --------------------------
template<class T>
const MVector<T>& MBody<T>::GetLocation(void) const
{
 return Location;
}

template<class T>
bool MBody<T>::SetLocation(MVector<T> &loc)
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
// �������� ������������
template<class T>
MBody<T>& MBody<T>::operator = (const MBody<T> &copy)
{
 Location=copy.Location;
 return *this;
}

// �������� �� ���������.
template<class T>
bool MBody<T>::operator == (const MBody<T>& b) const
{
 return Location == b.Location;
}

// �������� �� �����������.
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
 Rotation.push_back(MRotationTensor<T>(0,0));
 TranslationSpeed=0;
 AngleSpeed=0;
}

template<class T>
MKinematicBody<T>::MKinematicBody(const MKinematicBody<T> &copy)
 : MBody<T>(copy)
{
 *this=copy;
}

// Set startup characteristics
// ������������� �������� ������ ��������
template<class T>
void MKinematicBody<T>::SetRotation(size_t i, MRotationTensor<T> &P)
{
 Rotation[i]=P;
}

// ��������� � ����� ����� ������ ��������
template<class T>
void MKinematicBody<T>::AddRotation(MRotationTensor<T> &P)
{
 Rotation.push_back(P);
}

// ������� ������ �������� �� �������
template<class T>
void MKinematicBody<T>::DelRotation(size_t ix)
{
 Rotation.erase(Rotation.begin()+ix);
}

// ������� ��� ������� ��������
template<class T>
void MKinematicBody<T>::ClearRotations(void)
{
 Rotation.clear();
}

template<class T>
void MKinematicBody<T>::SetTranslationSpeed(MVector<T> &spd)
{
 TranslationSpeed=spd;
}

template<class T>
void MKinematicBody<T>::SetAngleSpeed(MVector<T> &spd)
{
 AngleSpeed=spd;
}


// Get mechanical characteristics
// ���������� ����� �������� ��������
template<class T>
size_t MKinematicBody<T>::GetNumRotations(void) const
{
 return Rotation.size();
}

template<class T>
inline const MRotationTensor<T>& MKinematicBody<T>::GetRotation(size_t ix) const
{
 return Rotation[ix];
}

template<class T>
inline const MVector<T>& MKinematicBody<T>::GetTranslationSpeed(void) const
{
 return TranslationSpeed;
}

template<class T>
inline const MVector<T>& MKinematicBody<T>::GetAngleSpeed(void) const
{
 return AngleSpeed;
}


// Run-Time Functions (RTF)
template<class T>
void MKinematicBody<T>::RTMovement(T time)
{
 T len=!(AngleSpeed);

 Location+=TranslationSpeed*time;
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

// �������� �� ���������.
template<class T>
bool MKinematicBody<T>::operator == (const MKinematicBody<T>& b) const
{
 return static_cast<const MBody<T>*>(this)->operator == (b) & (Rotation == b.Rotation) &
  (TranslationSpeed == b.TranslationSpeed) & (AngleSpeed == b.AngleSpeed);
}

// �������� �� �����������.
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
void MMechanicalBody<T>::SetMassCenter(MVector<T> &mc)
{
 MassCenter=Location=mc;
}

template<class T>
void MMechanicalBody<T>::SetMass(T mass)
{
 Mass=mass;
}

template<class T>
void MMechanicalBody<T>::SetAxisInertia(int num,MVector<T> &axis)
{
 *(&CIT.d1+num)=axis;
}

template<class T>
void MMechanicalBody<T>::SetAxisInertia(MVector<T> &axis1, MVector<T> &axis2,
                                     MVector<T> &axis3)
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
inline MVector<T> MMechanicalBody<T>::GetMassCenter(void)
{
 return MassCenter;
}

template<class T>
inline T MMechanicalBody<T>::GetMass(void)
{
 return Mass;
}

template<class T>
inline MInertiaTensor<T> MMechanicalBody<T>::GetCIT(void)
{
 return CIT;
}

template<class T>
inline T MMechanicalBody<T>::GetKineticEnergy(void) // ������������ �������
{
 T mVV,wCw;

 mVV=Mass*(TranslationSpeed*TranslationSpeed);
 wCw=AngleSpeed*(CIT*AngleSpeed);
 return 0.5*mVV+0.5*wCw;
}

template<class T>
inline MVector<T> MMechanicalBody<T>::GetMomentum(void) // ���������� ��������
{
 return Mass*TranslationSpeed;
}

template<class T>
inline MVector<T> MMechanicalBody<T>::GetDynamicSpin(void) // ������������ ���� ����.
{
 return CIT*AngleSpeed;
}

template<class T>
inline MVector<T> MMechanicalBody<T>::GetAngularMomentum(MVector<T> &SupportPoint) // ������ ���-�� ��������.
{
 return (MassCenter-SupportPoint)^GetMomentum();
}

template<class T>
inline MVector<T> MMechanicalBody<T>::GetKineticMoment(MVector<T> &SupportPoint) // ������������ ������.
{
 return GetAngularMomentum(SupportPoint)+GetDynamicSpin();
}


// Run-Time Functions (RTF)
template<class T>
void MMechanicalBody<T>::RTInfluence(int kind,MVector<T> influence,T time)
{
 MInertiaTensor<T> I;

 switch(kind)
 {
 case FORCE:
  TranslationSpeed+=(influence*time)/Mass;
 break;
 case MOMENT:
  I.d1=CIT.d1; I.d2=CIT.d2; I.d3=CIT.d3;
  I.m1=1./CIT.m1; I.m2=1./CIT.m2; I.m3=1./CIT.m3;
  AngleSpeed+=I*(influence*time);
 };
}


// -----------------
// Operators
// -----------------
// �������� ������������
template<class T>
MMechanicalBody<T>& MMechanicalBody<T>::operator = (const MMechanicalBody<T> &copy)
{
 *static_cast<MKinematicBody<T>*>(this)=static_cast<const MKinematicBody<T>&>(copy);
 MassCenter=copy.MassCenter;
 Mass=copy.Mass;
 CIT=copy.CIT;
 return *this;
}

// �������� �� ���������.
template<class T>
bool MMechanicalBody<T>::operator == (const MMechanicalBody<T>& b) const
{
 return static_cast<const MKinematicBody<T>* const>(this)->operator ==(b)
 & (MassCenter == b.MassCenter) & (Mass == b.Mass) & (CIT == b.CIT);
}

// �������� �� �����������.
template<class T>
bool MMechanicalBody<T>::operator != (const MMechanicalBody<T>& b) const
{
 return !operator == (b);
}
// -----------------


}
#endif

