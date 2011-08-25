/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 1997.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef MDYAD_H
#define MDYAD_H

#include "MVector.h"

namespace RDK {


/*
  ����� - MDyad. ���������� ����� ��� ������ - �����.
 ����� ����� ������� ����. ���������:
  1. MDyad<type> d; - �������� ����� � �-���� ������� �����.
  2. MDyad<type> v( (MVector<type>)u ,(MVector<type>)v );
      - �������� ����� �-�� ������� ����� u � v.
  3. MDyad<type> v( (MDyad<type>)d );
      - �������� ����� ������ ����� d.
 --------------------------------------

 � ������ ����������� ��������� ��������:
   ## ��������� ��������� ##
   MDyad<type> []; - ������ � �-��� ����� �� �� ������. ������.
   !MDyad<type>; - ����� �����.
   MDyad<type>.Transpose(); - �������� ����������������.
   Transpose(MDyad<type>); - �������� ������ ����������������� �����.
   Trace(MDyad<DataD>); - ���� �����.
   VectInvar(MDyad<DataD>); - ��������� ���������.

   ## ��������� ������������ ##
   MDyad<type> = MDyad<type>; - ������������ ����� �����.
   MDyad<type> = MVector<type>; - ������������ ����� �-��� ����� �������� �-��.

   ## ���������� ��������� ##
   MDyad<type> == MDyad<type>; - �������� �� ���������.
   MDyad<type> != MDyad<type>; - �������� �� �����������.

   ## �������������� ��������� ##
   - MDyad<type>; - ����� ����� � ���� �����. ������ �-�� �����.
   MDyad<type> *= MDyad<type>; - ��������� ��������� ����� �� ����� ������.
   MDyad<type> *= type; - ��������� ����� �� ����� ������.
   MDyad<type> ^= MVector<type>; - ��������� ���������� ����� �� ������ ������.
   MDyad<type> /= type; - ������� ������� �-�� ����� �� �����
   MDyad<type> * MDyad<type>; - ��������� ��������� ���� ����.
   MDyad<type> * MVector<type>; - ��������� ���������  ����� � �-��.
   MVector<type> * MDyad<type>; - ��������� ���������  �-�� � �����.
   MDyad<type> * type; - ��������� ����� � �����.
   type * MDyad<type>; - ��������� ����� � �����.
   MDyad<type> ^ MVector<type>; - ��������� ��������� ����� � �������.
   MVector<type> ^ MDyad<type>; - ��������� ��������� ������� � �����.
   MDyad<type> / type; - ������� ����� � �����.
   SS(MDyad<type>,MDyad<type>); - ������� ��������� ������������ ����
   VV(MDyad<type>,MDyad<type>); - ������� ��������� ������������ ����
   VS(MDyad<type>,MDyad<type>); - ������� ��������-��������� ������������ ����
   SV(MDyad<type>,MDyad<type>); - ������� ��������-��������� ������������ ����
   MVector<type> % MVector<type>; - ������� ��������� ���� ��������.

   ## ��������� ��������� �����-������ ##
   ostream << MDyad<type>; - ����� � ����� ( ��� ������� cout ), � ������������� �����.
   istream >> MDyad<type>; - ���� �� ������ ( ��� ������� cin ), � �������� ���� "d1=","d2=".
   fstream << MDyad<type>; - ����� � �������� �����.
   fstream >> MDyad<type>; - ���� �� ��������� ������.
 ----------------------------------------
*/
template<class DataD>
class MDyad
{
public: // ������
MVector<DataD> d1,d2;// ������� �����.

public: // ������
// ������������
// �������� ����� � �-���� ������� �����.
MDyad(void) {};

// �������� ����� ������ ����� d.
MDyad(const MDyad<DataD> &d)
{ d1=d.d1; d2=d.d2; };


// �������� ����� �-�� ������� ����� u � v.
MDyad(const MVector<DataD> &u, const MVector<DataD> &v)
{ d1=u; d2=v; };
//-------------

// ������������� ���������
// ## ��������� ��������� ##
// ������ � �-��� ����� �� �� ������. ������.
MVector<DataD>& operator [] (int index)
{ return *(&d1+index); };

// ����� �����.
DataD operator ! (void)
{ return sqrt( (d2*d2)*(d1*d1) ); };

// �������� ����������������
MDyad<DataD>& Transpose(void)
{
 MVector<DataD> tmp;

 tmp=d1; d1=d2; d2=d1;
 return *this;
};

// �������� ������ ����������������� �����.
// (�������� �����. ������� ����� ������ ��
// ����������� ����� �� ��� ��������: �����
// �� ������������ �����,� ������ �����,
// ���������������� ���������,��� � ����������).
friend MDyad<DataD> Transpose(const MDyad<DataD>&);

// ���� �����.
friend DataD Trace(const MDyad<DataD>&);

// ��������� ���������.
friend MVector<DataD> VectInvar(const MDyad<DataD>&);
// #########################

// ## ��������� ������������ ##
// ������������ ����� �����.
MDyad<DataD>& operator = (const MDyad<DataD> &D)
{
 d1=D.d1; d2=D.d2;
 return *this;
};

// ������������ ����� �-��� ����� �������� �-��.
MDyad<DataD>& operator = (const MVector<DataD> &v)
{
 d1=d2=v;
 return *this;
};
// ############################

// ## ���������� ��������� ##
// �������� �� ���������.
bool operator == (const MDyad<DataD> &D) const
{
 return (d1 == D.d1) && (d2 == D.d2);
}

// �������� �� �����������.
bool operator != (const MDyad<DataD> &D) const
{
 return (d1 != D.d1) || (d2 != D.d2);
}
// ##########################

// ## �������������� ��������� ##
// ����� ����� � ���� �����. ������ �-�� �����.
MDyad<DataD> operator - (void)
{
 return MDyad<DataD>(d1); // ������?!!!
};

// ��������� ��������� ����� �� ����� ������.
MDyad<DataD>& operator *=(const MDyad<DataD> &D)
{
 d2=D.d2*(d2*D.d1);
 return *this;
};

// ��������� ����� �� ����� ������.
MDyad<DataD>& operator *=(const DataD d)
{
 d2*=d;
 return *this;
};

// ��������� ���������� ����� �� ������ ������.
MDyad<DataD>& operator ^=(const MVector<DataD> &v)
{
 d2^=v;
 return *this;
};

// ������� ������� �-�� ����� �� �����
MDyad<DataD>& operator /=(const DataD d)
{
 d2/=d;
 return *this;
};

// ��������� ��������� ���� ����.
friend MDyad<DataD> operator * (const MDyad<DataD>&,const MDyad<DataD>&);

// ��������� ���������  ����� � �-��.
friend MVector<DataD> operator * (const MDyad<DataD>&,const MVector<DataD>&);

// ��������� ���������  �-�� � �����.
friend MVector<DataD> operator * (const MVector<DataD>&,const MDyad<DataD>&);

// ��������� ����� � �����.
friend MDyad<DataD> operator * (const MDyad<DataD>&,const DataD);

// ��������� ����� � �����.
friend MDyad<DataD> operator * (const DataD,const MDyad<DataD>&);

// ��������� ��������� ����� � �������.
friend MDyad<DataD> operator ^ (const MDyad<DataD>&,const MVector<DataD>&);

// ��������� ��������� ������� � �����.
friend MDyad<DataD> operator ^ (const MVector<DataD>&,const MDyad<DataD>&);

// ������� ����� � �����.
friend MDyad<DataD> operator / (const MDyad<DataD>&,const DataD);

// ������� ��������� ������������ ����
friend DataD SS(const MDyad<DataD>&,const MDyad<DataD>&);

// ������� ��������� ������������ ����
friend MDyad<DataD> VV(const MDyad<DataD>&,const MDyad<DataD>&);

// ������� ��������-��������� ������������ ����
friend MVector<DataD> VS(const MDyad<DataD>&,const MDyad<DataD>&);

// ������� ��������-��������� ������������ ����
friend MVector<DataD> SV(const MDyad<DataD>&,const MDyad<DataD>&);

// ������� ��������� ���� ��������.
friend MDyad<DataD> operator % (const MVector<DataD>&,const MVector<DataD>&);
// ##############################
// -------------------------------
};


// �������� ������ ����������������� �����.
template<class DataD>
inline MDyad<DataD> Transpose(const MDyad<DataD> &D)
{
 return MDyad<DataD>(D.d2,D.d1);
}

// ���� �����.
template<class DataD>
inline DataD Trace(const MDyad<DataD> &D)
{
 return D.d1*D.d2;
}

// ��������� ���������.
template<class DataD>
inline MVector<DataD> VectInvar(const MDyad<DataD> &D)
{
 return MVector<DataD>(D.d1^D.d2);
}

// ��������� ��������� ���� ����.
template<class DataD>
inline MDyad<DataD> operator * (const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MDyad<DataD>( MVector<DataD>(D1.d1),MVector<DataD>(D2.d2*(D1.d2*D2.d1)) );
}

// ��������� ���������  ����� � �-��.
template<class DataD>
inline MVector<DataD> operator * (const MDyad<DataD> &D,const MVector<DataD> &v)
{
 return MVector<DataD>( D.d1*(D.d2*v) );
}

// ��������� ���������  �-�� � �����.
template<class DataD>
inline MVector<DataD> operator * (const MVector<DataD> &v,const MDyad<DataD> &D)
{
 return MVector<DataD>( D.d2*(D.d1*v) );
}

// ��������� ����� � �����.
template<class DataD>
inline MDyad<DataD> operator * (const MDyad<DataD> &D,const DataD d)
{
 return MDyad<DataD>( MVector<DataD>(D.d1),MVector<DataD>(D.d2*d) );
}

// ��������� ����� � �����.
template<class DataD>
inline MDyad<DataD> operator * (const DataD d,const MDyad<DataD> &D)
{
 return MDyad<DataD>( MVector<DataD>(D.d1*d),MVector<DataD>(D.d2) );
}

// ��������� ��������� ����� � �������.
template<class DataD>
inline MDyad<DataD> operator ^ (const MDyad<DataD> &D,const MVector<DataD> &v)
{
 return MDyad<DataD>( MVector<DataD>(D.d1),MVector<DataD>(D.d2^v) );
}

// ��������� ��������� ������� � �����.
template<class DataD>
inline MDyad<DataD> operator ^ (const MVector<DataD>&,const MDyad<DataD>&)
{
 return MDyad<DataD>( MVector<DataD>(v^D.d1),MVector<DataD>(D.d2) );
}

// ������� ����� � �����.
template<class DataD>
inline MDyad<DataD> operator / (const MDyad<DataD>&,const DataD)
{
 return MDyad<DataD>( MVector<DataD>(D.d1),MVector<DataD>(D.d2/d) );
}

// ������� ��������� ������������ ����
template<class DataD>
inline DataD SS(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return (D1.d2*D2.d1)*(D1.d1*D2.d2);
}

// ������� ��������� ������������ ����
template<class DataD>
inline MDyad<DataD> VV(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MDyad<DataD>( D1.d2^D2.d1,D1.d1^D2.d2 );
}

// ������� ��������-��������� ������������ ����
template<class DataD>
inline MVector<DataD> VS(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MVector<DataD>( (D1.d2^D2.d1)*(D1.d1*D2.d2) );
}

// ������� ��������-��������� ������������ ����
template<class DataD>
inline MVector<DataD> SV(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MVector<DataD>( (D1.d2*D2.d1)*(D1.d1^D2.d2) );
}


// ������� ��������� ���� ��������.
template<class DataD>
inline MDyad<DataD> operator % (const MVector<DataD> &u,const MVector<DataD> &v)
{
 return MDyad<DataD>( MVector<DataD>(u),MVector<DataD>(v) );
}
// ##############################

}
#endif
