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
MVector<DataD,3> d1,d2;// ������� �����.

public: // ������
// ������������
// �������� ����� � �-���� ������� �����.
MDyad(void) {};

// �������� ����� ������ ����� d.
MDyad(const MDyad<DataD> &d)
{ d1=d.d1; d2=d.d2; };


// �������� ����� �-�� ������� ����� u � v.
MDyad(const MVector<DataD,3> &u, const MVector<DataD,3> &v)
{ d1=u; d2=v; };
//-------------

// ������������� ���������
// ## ��������� ��������� ##
// ������ � �-��� ����� �� �� ������. ������.
MVector<DataD,3>& operator [] (int index)
{ return *(&d1+index); };

// ����� �����.
DataD operator ! (void)
{ return sqrt( (d2*d2)*(d1*d1) ); };

// �������� ����������������
MDyad<DataD>& Transpose(void)
{
 MVector<DataD,3> tmp;

 tmp=d1; d1=d2; d2=d1;
 return *this;
};

// �������� ������ ����������������� �����.
// (�������� �����. ������� ����� ������ ��
// ����������� ����� �� ��� ��������: �����
// �� ������������ �����,� ������ �����,
// ���������������� ���������,��� � ����������).
//template<class DataU> friend MDyad<DataU> Transpose(const MDyad<DataU> &D);

// ���� �����.
//template<class DataU> friend DataU Trace(const MDyad<DataU> &D);

// ��������� ���������.
//template<class DataU> friend MVector<DataU> VectInvar(const MDyad<DataU> &D);
// #########################

// ## ��������� ������������ ##
// ������������ ����� �����.
MDyad<DataD>& operator = (const MDyad<DataD> &D)
{
 d1=D.d1; d2=D.d2;
 return *this;
};

// ������������ ����� �-��� ����� �������� �-��.
MDyad<DataD>& operator = (const MVector<DataD,3> &v)
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
MDyad<DataD>& operator ^=(const MVector<DataD,3> &v)
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
template<class DataU> friend MDyad<DataU> operator * (const MDyad<DataU> &D,const MDyad<DataU>&);

// ��������� ���������  ����� � �-��.
template<class DataU> friend MVector<DataU,3> operator * (const MDyad<DataU> &D1,const MVector<DataU,3> &D2);

// ��������� ���������  �-�� � �����.
template<class DataU> friend MVector<DataU,3> operator * (const MVector<DataU,3> &D1,const MDyad<DataU> &D2);

// ��������� ����� � �����.
template<class DataU> friend MDyad<DataU> operator * (const MDyad<DataU> &D,const DataU d);

// ��������� ����� � �����.
template<class DataU> friend MDyad<DataU> operator * (const DataU d,const MDyad<DataU> &D);

// ��������� ��������� ����� � �������.
//template<class DataU> friend MDyad<DataU> operator ^ (const MDyad<DataU> &D1,const MVector<DataU,3> &D2);

// ��������� ��������� ������� � �����.
//template<class DataU> friend MDyad<DataU> operator ^ (const MVector<DataU,3> &v,const MDyad<DataU> &D);

// ������� ����� � �����.
template<class DataU> friend MDyad<DataU> operator / (const MDyad<DataU> &D,const DataU d);

// ������� ��������� ������������ ����
//template<class DataU> friend DataU SS(const MDyad<DataU> &D1,const MDyad<DataU> &D2);

// ������� ��������� ������������ ����
//template<class DataU> friend MDyad<DataU> VV(const MDyad<DataU> &D1,const MDyad<DataU> &D2);

// ������� ��������-��������� ������������ ����
//template<class DataU> friend MVector<DataU,3> VS(const MDyad<DataU> &D1,const MDyad<DataU> &D2);

// ������� ��������-��������� ������������ ����
//template<class DataU> friend MVector<DataU,3> SV(const MDyad<DataU> &D1,const MDyad<DataU> &D2);

// ������� ��������� ���� ��������.
//template<class DataU> friend MDyad<DataU> operator % (const MVector<DataU,3> &D,const MVector<DataU,3> &v);
// ##############################
// -------------------------------
};


// �������� ������ ����������������� �����.
template<class DataD>
 MDyad<DataD> Transpose(const MDyad<DataD> &D)
{
 return MDyad<DataD>(D.d2,D.d1);
}

// ���� �����.
template<class DataD>
 DataD Trace(const MDyad<DataD> &D)
{
 return D.d1*D.d2;
}

// ��������� ���������.
template<class DataD>
 MVector<DataD,3> VectInvar(const MDyad<DataD> &D)
{
 return MVector<DataD,3>(D.d1^D.d2);
}

// ��������� ��������� ���� ����.
template<class DataD>
 MDyad<DataD> operator * (const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MDyad<DataD>( MVector<DataD,3>(D1.d1),MVector<DataD,3>(D2.d2*(D1.d2*D2.d1)) );
}

// ��������� ���������  ����� � �-��.
template<class DataD>
 MVector<DataD,3> operator * (const MDyad<DataD> &D,const MVector<DataD,3> &v)
{
 return MVector<DataD,3>( D.d1*(D.d2*v) );
}

// ��������� ���������  �-�� � �����.
template<class DataD>
 MVector<DataD,3> operator * (const MVector<DataD,3> &v,const MDyad<DataD> &D)
{
 return MVector<DataD,3>( D.d2*(D.d1*v) );
}

// ��������� ����� � �����.
template<class DataD>
 MDyad<DataD> operator * (const MDyad<DataD> &D,const DataD d)
{
 return MDyad<DataD>( MVector<DataD,3>(D.d1),MVector<DataD,3>(D.d2*d) );
}

// ��������� ����� � �����.
template<class DataD>
 MDyad<DataD> operator * (const DataD d,const MDyad<DataD> &D)
{
 return MDyad<DataD>( MVector<DataD,3>(D.d1*d),MVector<DataD,3>(D.d2) );
}

// ��������� ��������� ����� � �������.
template<class DataD>
 MDyad<DataD> operator ^ (const MDyad<DataD> &D,const MVector<DataD,3> &v)
{
 return MDyad<DataD>( MVector<DataD,3>(D.d1),MVector<DataD,3>(D.d2^v) );
}

// ��������� ��������� ������� � �����.
template<class DataD>
 MDyad<DataD> operator ^ (const MVector<DataD,3> &v,const MDyad<DataD> &D)
{
 return MDyad<DataD>( MVector<DataD,3>(v^D.d1),MVector<DataD,3>(D.d2) );
}

// ������� ����� � �����.
template<class DataD>
 MDyad<DataD> operator / (const MDyad<DataD> &D,const DataD d)
{
 return MDyad<DataD>( MVector<DataD,3>(D.d1),MVector<DataD,3>(D.d2/d) );
}

// ������� ��������� ������������ ����
template<class DataD>
 DataD SS(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return (D1.d2*D2.d1)*(D1.d1*D2.d2);
}

// ������� ��������� ������������ ����
template<class DataD>
 MDyad<DataD> VV(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MDyad<DataD>( D1.d2^D2.d1,D1.d1^D2.d2 );
}

// ������� ��������-��������� ������������ ����
template<class DataD>
 MVector<DataD,3> VS(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MVector<DataD,3>( (D1.d2^D2.d1)*(D1.d1*D2.d2) );
}

// ������� ��������-��������� ������������ ����
template<class DataD>
 MVector<DataD,3> SV(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MVector<DataD,3>( (D1.d2*D2.d1)*(D1.d1^D2.d2) );
}

 /*
// ������� ��������� ���� ��������.
template<class DataD>
 MDyad<DataD> operator % (const MVector<DataD,3> &u,const MVector<DataD,3> &v)
{
 return MDyad<DataD>( MVector<DataD,3>(u),MVector<DataD,3>(v) );
}   */
// ##############################

}
#endif
