/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef MCSystemH
#define MCSystemH

#include "MVector.h"
#include "MTheormec.h"

namespace RDK {


// ����������� ������� ���������
template <class T>
class MCSystem
{
public: // ������
// ����� � 3-������ ������������
MVector<T,3> Basis[3];

// ��������� ������ ������������ ���������� ���� ���������
MVector<T,3> Location;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MCSystem(void)
{
 Basis[0].x=1; Basis[0].y=0; Basis[0].z=0;
 Basis[1].x=0; Basis[1].y=1; Basis[1].z=0;
 Basis[2].x=0; Basis[2].y=0; Basis[2].z=1;
 Location=0.0;
};

MCSystem(const MCSystem<T> &copy)
{
 *this=copy;
};

MCSystem(const MVector<T,3> &b1, const MVector<T,3> &b2, const MVector<T,3> &b3,
        const MVector<T,3> &loc=0)
{
 Basis[0]=b1; Basis[1]=b2; Basis[2]=b3;
 Location=loc;
};

MCSystem(const MVector<T,3> *basis, const MVector<T,3> *loc=0)
{
 MCSystem();
 if(!basis)
  return;
 for(int i=0;i<3;i++)
  Basis[i]=basis[i];

 if(!loc)
  Location=0;
 else
  Location=*loc;
};

virtual ~MCSystem(void)
{
};
// --------------------------

// --------------------------
// ������ �������������� ������ ���������
// --------------------------
// �������������� � �������� ������� ��������� ���� �� ���� ��� �������
// ���������� ������� input � ������ ������� ������� ���������
// ������������� � ������� ��������� csystem � ������������ ��� output
// input � output �� ����� ��������� �� ���� ������
virtual bool ConvertTo(const MCSystem &csystem,
                        const MVector<T,3> &input, MVector<T,3> &output) const=0;

// �������������� � �������� ������� ���������
// ���������� ������� v � ������ ������� ������� ���������
// ������������� � ������� ��������� csystem
MVector<T,3> ConvertTo(const MCSystem &csystem, const MVector<T,3> &v) const
{
 MVector<T,3> res;
 ConvertTo(csystem,v,res);
 return res;
};

// ��������� ������ �������� ���� �� ������������ ��������
virtual MVector<T,3> CalcTranslation(const MCSystem &csystem)=0;

// ��������� ������ �������� ���� �� ������������ ��������
virtual MRotationTensor<T> CalcRotation(const MCSystem &csystem)=0;
// --------------------------

// --------------------------
// �������������� ���������
// --------------------------
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
MCSystem<T>& operator = (const MCSystem<T> &copy)
{
 for(int i=0;i<3;i++)
  Basis[i]=copy.Basis[i];

 Location=copy.Location;
 return *this;
};

// ��������� ���������
bool operator == (const MCSystem<T> &copy) const
{
 return (Basis[0] == copy.Basis[0]) & (Basis[1] == copy.Basis[1]) &
         (Basis[2] == copy.Basis[2]) & (Location == copy.Location);
};

bool operator != (const MCSystem<T> &copy) const
{
 return !operator ==(copy);
};
// --------------------------
};


// ��������� ������� ���������
template <class T>
class MCartesianCSystem: public MCSystem<T>
{
public: // ������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MCartesianCSystem(void)
{
};

MCartesianCSystem(const MCartesianCSystem<T> &copy)
{
 *this=copy;
};

virtual ~MCartesianCSystem(void)
{
};
// --------------------------

// --------------------------
// ������ �������������� ������ ���������
// --------------------------
// �������������� � �������� ������� ��������� ���� �� ���� ��� �������
// ���������� ������� input � ������ ������� ������� ���������
// ������������� � ������� ��������� csystem � ������������ ��� output
// input � output �� ����� ��������� �� ���� ������
virtual bool ConvertTo(const MCSystem<T> &csystem,
                        const MVector<T,3> &input, MVector<T,3> &output) const
{
 output.x=input*this->Basis[0];
 output.y=input*this->Basis[1];
 output.z=input*this->Basis[2];
 return true;
};

// ��������� ������ �������� ���� �� ������������ ��������
MVector<T,3> CalcTranslation(const MCSystem<T> &csystem)
{
 return MVector<T,3>(csystem.Location-this->Location);
}
/*
// ��������� ������ �������� ���� �� ������������ ��������
MRotationTensor<T> CalcRotation(const MCSystem<T> &csystem)
{
 MRotationTensor<T> rotation;
 MTensor<T> temp;
 MDyad<T> dyad;

 for(int i=0;i<3;i++)
 {
  dyad.d1=this->Basis[i];
  dyad.d2=csystem.Basis[i];
  temp+=dyad;
 }

 rotation=temp;

 return rotation;
};*/

// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
MCartesianCSystem<T>& operator = (const MCartesianCSystem<T> &copy)
{
 static_cast<MCSystem<T>*>(this)->operator = (copy);
 return *this;
};

// ��������� ���������
bool operator == (const MCartesianCSystem<T> &copy) const
{
 return static_cast<const MCSystem<T>* const>(this)->operator == (copy);
};

bool operator != (const MCartesianCSystem<T> &copy) const
{
 return static_cast<const MCSystem<T>* const>(this)->operator != (copy);
};
// --------------------------
};


// �������� ������� ���������
template <class T>
class MEulerCSystem: public MCSystem<T>
{
public: // ������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MEulerCSystem(void)
{
};

MEulerCSystem(const MEulerCSystem<T> &copy)
{
 *this=copy;
};

virtual ~MEulerCSystem(void)
{
};
// --------------------------

// --------------------------
// ������ �������������� ������ ���������
// --------------------------
// �������������� � �������� ������� ��������� ���� �� ���� ��� �������
// ���������� ������� input � ������ ������� ������� ���������
// ������������� � ������� ��������� csystem � ������������ ��� output
// input � output �� ����� ��������� �� ���� ������
virtual bool ConvertTo(const MCSystem<T> &csystem,
                        const MVector<T,3> &input, MVector<T,3> &output) const
{
 return true;
};
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
MEulerCSystem<T>& operator = (const MEulerCSystem<T> &copy)
{
 static_cast<MCSystem<T>*>(this)->operator = (copy);
 return *this;
};

// ��������� ���������
bool operator == (const MEulerCSystem<T> &copy) const
{
 return static_cast<const MCSystem<T>* const>(this)->operator == (copy);
};

bool operator != (const MEulerCSystem<T> &copy) const
{
 return static_cast<const MCSystem<T>* const>(this)->operator != (copy);
};
// --------------------------
};


// �������� ������� ���������
template <class T>
class MPolarCSystem: public MCSystem<T>
{
public: // ������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MPolarCSystem(void)
{
};

MPolarCSystem(const MPolarCSystem<T> &copy)
{
 *this=copy;
};

virtual ~MPolarCSystem(void)
{
};
// --------------------------

// --------------------------
// ������ �������������� ������ ���������
// --------------------------
// �������������� � �������� ������� ��������� ���� �� ���� ��� �������
// ���������� ������� input � ������ ������� ������� ���������
// ������������� � ������� ��������� csystem � ������������ ��� output
// input � output �� ����� ��������� �� ���� ������
virtual bool ConvertTo(const MCSystem<T> &csystem,
                        const MVector<T,3> &input, MVector<T,3> &output) const
{
 return true;
};
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
MPolarCSystem<T>& operator = (const MPolarCSystem<T> &copy)
{
 static_cast<MCSystem<T>*>(this)->operator = (copy);
 return *this;
};

// ��������� ���������
bool operator == (const MPolarCSystem<T> &copy) const
{
 return static_cast<const MCSystem<T>* const>(this)->operator == (copy);
};

bool operator != (const MPolarCSystem<T> &copy) const
{
 return static_cast<const MCSystem<T>* const>(this)->operator != (copy);
};
// --------------------------
};


}
#endif
