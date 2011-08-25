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
MVector<T> Basis[3];

// ��������� ������ ������������ ���������� ���� ���������
MVector<T> Location;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MCSystem(void)
{
 Basis[0].x=1; Basis[0].y=0; Basis[0].z=0;
 Basis[1].x=0; Basis[1].y=1; Basis[1].z=0;
 Basis[2].x=0; Basis[2].y=0; Basis[2].z=1;
 Location=0;
};

MCSystem(const MCSystem<T> &copy)
{
 *this=copy;
};

MCSystem(const MVector<T> &b1, const MVector<T> &b2, const MVector<T> &b3,
        const MVector<T> &loc=0)
{
 Basis[0]=b1; Basis[1]=b2; Basis[2]=b3;
 Location=loc;
};

MCSystem(const MVector<T> *basis, const MVector<T> *loc=0)
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
                        const MVector<T> &input, MVector<T> &output) const=0;

// �������������� � �������� ������� ���������
// ���������� ������� v � ������ ������� ������� ���������
// ������������� � ������� ��������� csystem
MVector<T> ConvertTo(const MCSystem &csystem, const MVector<T> &v) const
{
 MVector<T> res;
 ConvertTo(csystem,v,res);
 return res;
};

// ��������� ������ �������� ���� �� ������������ ��������
virtual MVector<T> CalcTranslation(const MCSystem &csystem)=0;

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
                        const MVector<T> &input, MVector<T> &output) const
{
 output.x=input*Basis[0];
 output.y=input*Basis[1];
 output.z=input*Basis[2];
 return true;
};

// ��������� ������ �������� ���� �� ������������ ��������
MVector<T> CalcTranslation(const MCSystem<T> &csystem)
{
 return MVector<T>(csystem.Location-Location);
}

// ��������� ������ �������� ���� �� ������������ ��������
MRotationTensor<T> CalcRotation(const MCSystem<T> &csystem)
{
 MRotationTensor<T> rotation;
 MTensor<T> temp;
 MDyad<T> dyad;

 for(int i=0;i<3;i++)
 {
  dyad.d1=Basis[i];
  dyad.d2=csystem.Basis[i];
  temp+=dyad;
 }

 rotation=temp;

 return rotation;
}

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
                        const MVector<T> &input, MVector<T> &output) const
{
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
                        const MVector<T> &input, MVector<T> &output) const
{
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
