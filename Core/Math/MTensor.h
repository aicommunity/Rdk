/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 1997.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef MTENSOR_H
#define MTENSOR_H

#include "MDyad.h"
#include <vector>

namespace RDK {

/*
   ����� MTensor ���������� ����� ��� ������ - ������ 2-�� �����.
*/
template<class DataT>
class MTensor
{
public:
// ������ ���� �������.
std::vector<MDyad<DataT> > Dyads;

public:
// ������������ � �����������.
// �������� ������ � ���� ����.
MTensor(void) {};

// �������� ������ � ����� ������.
MTensor(const MDyad<DataT> &D)
{
 Dyads.resize(1);
 Dyads[0]=D;
};

// �������� ������ � ����������� �������, ��������� ��������.
MTensor(const MDyad<DataT> *DL,int quan)
{
 Dyads.resize(quan);
 for(int i=0;i<quan;i++)
  Dyads[i]=DL[i];
};

// �������� ������ � ����������� �������, ��������� ���. ��������.
MTensor(const std::vector<MDyad<DataT> > &DA)
{
 Dyads=DA;
};

// �������� ������ � ����� ������ ��� ������� ������� ����� ���������.
MTensor(const MVector<DataT> &v)
{
 Dyads.ChangeLength(1);
 Dyads[0]=v;
};

// �������� ������ ������ ���������.
MTensor(const MTensor<DataT> &T)
{
 Dyads=T.Dyads;
};

// ����������.
~MTensor(void)
{
 Clear();
};
//--------------------------

//--------------------------
// ��������� �������.
//--------------------------
void Clear(void)
{
 Dyads.clear();
};
//--------------------------

//--------------------------
// ������������� ���������
//--------------------------
// ## ��������� ��������� ##
// ������ � ������ ������� �� �� ������. ������.
MDyad<DataT>& operator [] (int index)
{
 return Dyads[index];
};

// ����� �������.
DataT operator ! (void)
{
 return sqrt( SS(*this,Transpose(*this) ) );
};

// �������� ����������������
MTensor<DataT>& Transpose(void)
{
 for(int i=0;i<Dyads.size();i++)
  Dyads[i].Transpose();
 return *this;
};

// �������� ������ ������������������ �������.
// ������� ����� ������ ��
// ����������� ����� �� ��� ��������: �����
// �� ������������ ������,� ������ �����,
// ���������������� ���������, ��� � ����������.
friend MTensor<DataT> Transpose(const MTensor<DataT> &T);

// ���� �������.
friend DataT Trace(const MTensor<DataT> &T);

// ��������� ���������.
friend MVector<DataT> VectInvar(const MTensor<DataT> &T);
// #########################

// ## ��������� ������������ ##
// ������������ ������� �������.
MTensor<DataT>& operator = (const MTensor<DataT> &T)
{
 Dyads=T.Dyads;
 return *this;
};

// ������������ ������� �����.
MTensor<DataT>& operator = (const MDyad<DataT> &D)
{
 Clear();

 Dyads.resize(1);
 Dyads[0]=D;

 return *this;
};
// ############################

// ## ���������� ��������� ##
// �������� �� ���������.
bool operator == (const MTensor<DataT> &T) const
{
 return (Dyads == T.Dyads);
}

// �������� �� �����������.
bool operator != (const MTensor<DataT> &T) const
{
 return (Dyads != T.Dyads);
}
// ##########################

// ## �������������� ��������� ##
// ����� ����� � ���� ���� �������.
MTensor<DataT> operator - (void)
{
 std::vector< MDyad<DataT> > tmp;
 size_t leng=Dyads.size();

 tmp.resize(leng);
 for(size_t i=0;i<leng;i++)
  tmp[i]=-Dyads[i];

 return MTensor<DataT>(tmp);
};

// ���������� ������� �������.
MTensor<DataT>& operator += (const MTensor<DataT> &T)
{
 Dyads+=T.Dyads;
 return *this;
};

// ���������� ������� �����.
MTensor<DataT>& operator += (const MDyad<DataT> &D)
{
 Dyads.push_back(D);
 return *this;
};

// ��������� �� ������� �������.
MTensor<DataT>& operator -= (const MTensor<DataT> &T)
{
 this+=-T;
 return *this;
};

// ��������� �� ������� �����.
MTensor<DataT>& operator -= (const MDyad<DataT> &D)
{
 this+=-D;
 return *this;
};

// ��������� ���������� � ������� �������.
MTensor<DataT>& operator *= (const MTensor<DataT> &T)
{
 std::vector<MDyad<DataT> > tmp;
 size_t leng,k=0;

 leng=Dyads.size()*T.Dyads.size();
 tmp.resize(leng);
 for(size_t i=0;i<Dyads.size();i++)
  for(size_t j=0;j<T.Dyads.size();j++)
   tmp[k++]=Dyads[i]*T.Dyads[j];

 Dyads=tmp;
 return *this;
};

// ��������� ���������� � ������� �����.
MTensor<DataT>& operator *= (const MDyad<DataT> &D)
{
 for(size_t i=0;i<Dyads.size();i++)
  Dyads[i]*=D;

 return *this;
};

// ���������� ������� �� �����.
MTensor<DataT>& operator *= (DataT DT)
{
 for(size_t i=0;i<Dyads.size();i++)
  Dyads[i]*=DT;

 return *this;
};

// �������� ���� ��������.
friend MTensor<DataT> operator + (const MTensor<DataT> &T1,const MTensor<DataT> &T2);

// �������� ������� � �����.
friend MTensor<DataT> operator + (const MTensor<DataT> &T1,const MDyad<DataT> &T2);

// �������� ����� � �������.
friend MTensor<DataT> operator + (const MDyad<DataT> &T1,const MTensor<DataT> &T2);

// ��������� ���� ��������.
friend MTensor<DataT> operator - (const MTensor<DataT> &T1,const MTensor<DataT> &T2);

// ��������� ������� � �����.
friend MTensor<DataT> operator - (const MTensor<DataT> &T,const MDyad<DataT> &D);

// ��������� ����� � �������.
friend MTensor<DataT> operator - (const MDyad<DataT> &D,const MTensor<DataT> &T);

// ��������� ��������� ���� ��������.
friend MTensor<DataT> operator * (const MTensor<DataT> &T1,const MTensor<DataT> &T2);

// ��������� ��������� ������� � �����.
friend MTensor<DataT> operator * (const MTensor<DataT> &T,const MDyad<DataT> &D);

// ��������� ��������� ����� � �������.
friend MTensor<DataT> operator * (const MDyad<DataT> &D,const MTensor<DataT> &T);

// ��������� ��������� ������� � �������.
friend MVector<DataT> operator * (const MVector<DataT> &v,const MTensor<DataT> &T);

// ��������� ��������� ������� � �������.
friend MVector<DataT> operator * (const MTensor<DataT> &T,const MVector<DataT> &v);

// ��������� ������� � �����.
friend MTensor<DataT> operator * (const MTensor<DataT> &T,DataT d);

// ��������� ����� � �������.
friend MTensor<DataT> operator * (DataT d,const MTensor<DataT> &T);

// ������� ������� � �����.
friend MTensor<DataT> operator / (const MTensor<DataT> &T, DataT d);

// ��������� ��������� ������� � �������.
friend MTensor<DataT> operator ^ (const MVector<DataT> &v,const MTensor<DataT> &T);

// ��������� ��������� ������� � �������.
friend MTensor<DataT> operator ^ (const MTensor<DataT> &T,const MVector<DataT> &v);

// ������� ��������� ������������ ��������
friend DataT SS(const MTensor<DataT> &T1,const MTensor<DataT> &T2);

// ������� ��������� ������������ ��������
friend MTensor<DataT> VV(const MTensor<DataT> &T1,const MTensor<DataT> &T2);

// ������� ��������-��������� ������������ ��������
friend MVector<DataT> VS(const MTensor<DataT> &T1,const MTensor<DataT> &T2);

// ������� ��������-��������� ������������ ��������
friend MVector<DataT> SV(const MTensor<DataT> &T1,const MTensor<DataT> &T2);
// ##############################
// -------------------------------
};



// �������� ������ ������������������ �������.
template<class DataT>
MTensor<DataT> Transpose(const MTensor<DataT> &T)
{
 std::vector< MDyad<DataT> > tmp(T.Dyads);

 for(int i=0;i<T.Dyads.size();i++)
  tmp[i].Transpose();

 return MTensor<DataT>(tmp);
}

// ���� �������.
template<class DataT>
DataT Trace(const MTensor<DataT> &T)
{
 DataT tmp=0;

 for(int i=0;i<T.Dyads.size();i++)
  tmp+=T.Dyads[i].d1*T.Dyads[i].d2;

 return tmp;
}

// ��������� ���������.
template<class DataT>
MVector<DataT> VectInvar(const MTensor<DataT> &T)
{
 MVector<DataT> tmp;

 for(int i=0;i<T.Dyads.size();i++)
  tmp+=T.Dyads[i].d1^T.Dyads[i].d2;

 return MVector<DataT>(tmp);
}
// #########################

// -------------------------------
// �������� ���� ��������.
template<class DataT>
MTensor<DataT> operator + (const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 return MTensor<DataT>(T1.Dyads+T2.Dyads);
}

// �������� ������� � �����.
template<class DataT>
MTensor<DataT> operator + (const MTensor<DataT> &T,const MDyad<DataT> &D)
{
 return MTensor<DataT>(T.Dyads+D);
}

// �������� ����� � �������.
template<class DataT>
MTensor<DataT> operator + (const MDyad<DataT> &D,const MTensor<DataT> &T)
{
 return MTensor<DataT>(T.Dyads+D);
}

// ��������� ���� ��������.
template<class DataT>
MTensor<DataT> operator - (const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 MTensor<DataT>(T1.Dyads+(-T2).Dyads);
}

// ��������� ������� � �����.
template<class DataT>
MTensor<DataT> operator - (const MTensor<DataT> &T,const MDyad<DataT> &D)
{
 return MTensor<DataT>(T.Dyads+(-D));
}

// ��������� ����� � �������.
template<class DataT>
MTensor<DataT> operator - (const MDyad<DataT> &D,const MTensor<DataT> &T)
{
 return MTensor<DataT>(T.Dyads+(-D));
}

// ��������� ��������� ���� ��������.
template<class DataT>
MTensor<DataT> operator * (const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 std::vector< MDyad<DataT> > tmp;
 int leng,k=0;

 leng=T1.Dyads.size()*T2.Dyads.size();
 tmp.ChangeLength(leng);
 for(int i=0;i<T1.Dyads.size();i++)
  for(int j=0;j<T2.Dyads.size();j++)
   tmp[k++]=T1.Dyads[i]*T2.Dyads[j];

 return MTensor<DataT>(tmp);
}

// ��������� ��������� ������� � �����.
template<class DataT>
MTensor<DataT> operator * (const MTensor<DataT> &T,const MDyad<DataT> &D)
{
 std::vector< MDyad<DataT> > tmp;
 int leng=T.Dyads.size();

 tmp.Dyads.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp.Dyads[i]=T.Dyads[i]*D;

 return MTensor<DataT>(tmp);
}

// ��������� ��������� ����� � �������.
template<class DataT>
MTensor<DataT> operator * (const MDyad<DataT> &D,const MTensor<DataT> &T)
{
 std::vector< MDyad<DataT> > tmp;
 int leng=T.Dyads.size();

 tmp.Dyads.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp.Dyads[i]=D*T.Dyads[i];

 return MTensor<DataT>(tmp);
}

// ��������� ��������� ������� � �������.
template<class DataT>
MVector<DataT> operator * (const MVector<DataT> &v,const MTensor<DataT> &T)
{
 MVector<DataT> tmp;

 for(int i=0;i<T.Dyads.size();i++)
  tmp+=v*T.Dyads[i];

 return MVector<DataT>(tmp);
}

// ��������� ��������� ������� � �������.
template<class DataT>
MVector<DataT> operator * (const MTensor<DataT> &T,const MVector<DataT> &v)
{
 MVector<DataT> tmp;

 for(int i=0;i<T.Dyads.size();i++)
  tmp+=T.Dyads[i]*v;

 return MVector<DataT>(tmp);
}

// ��������� ������� � �����.
template<class DataT>
MTensor<DataT> operator * (const MTensor<DataT> &T,DataT d)
{
 std::vector< MDyad<DataT> > tmp;
 int leng=T.Dyads.size();

 tmp.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp[i]=T.Dyads[i]*d;

 return MTensor<DataT>(tmp);
}

// ��������� ����� � �������.
template<class DataT>
MTensor<DataT> operator * (DataT d,const MTensor<DataT>& T)
{
 std::vector< MDyad<DataT> > tmp;
 int leng=T.Dyads.size();

 tmp.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp[i]=d*T.Dyads[i];

 return MTensor<DataT>(tmp);
}

// ������� ������� � �����.
template<class DataT>
MTensor<DataT> operator / (const MTensor<DataT> &T,DataT d)
{
 std::vector< MDyad<DataT> > tmp;
 int leng=T.Dyads.size();

 tmp.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp[i]=T.Dyads[i]/d;

 return MTensor<DataT>(tmp);
}

// ��������� ��������� ������� � �������.
template<class DataT>
MTensor<DataT> operator ^ (const MVector<DataT> &v,const MTensor<DataT> &T)
{
 std::vector< MDyad<DataT> > tmp;
 int leng=T.Dyads.size();

 tmp.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp[i]=v^T.Dyads[i];

 return MTensor<DataT>(tmp);
}

// ��������� ��������� ������� � �������.
template<class DataT>
MTensor<DataT> operator ^ (const MTensor<DataT> &T,const MVector<DataT> &v)
{
 std::vector< MDyad<DataT> > tmp;
 int leng=T.Dyads.size();

 tmp.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp[i]=T.Dyads[i]^v;

 return MTensor<DataT>(tmp);
}

// ������� ��������� ������������ ��������
template<class DataT>
DataT SS(const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 DataT tmp=0;

 for(int i=0;i<T1.Dyads.size();i++)
  for(int j=0;j<T2.Dyads.size();j++)
   tmp+=SS(T1.Dyads[i],T2.Dyads[j]);

 return tmp;
}

// ������� ��������� ������������ ��������
template<class DataT>
MTensor<DataT> VV(const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 std::vector< MDyad<DataT> > tmp;
 int k=0;

 for(int i=0;i<T1.Dyads.size();i++)
  for(int j=0;j<T2.Dyads.size();j++)
   tmp[k++]=VV(T1.Dyads[i],T2.Dyads[j]);

 return MTensor<DataT>(tmp);
}

// ������� ��������-��������� ������������ ��������
template<class DataT>
MVector<DataT> VS(const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 MVector<DataT> tmp;

 for(int i=0;i<T1.Dyads.size();i++)
  for(int j=0;j<T2.Dyads.size();j++)
   tmp+=VS(T1.Dyads[i],T2.Dyads[j]);

 return MVector<DataT>(tmp);
}

// ������� ��������-��������� ������������ ��������
template<class DataT>
MVector<DataT> SV(const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 MVector<DataT> tmp;

 for(int i=0;i<T1.Dyads.size();i++)
  for(int j=0;j<T2.Dyads.size();j++)
   tmp+=SV(T1.Dyads[i],T2.Dyads[j]);

 return MVector<DataT>(tmp);
}
// ##############################
// -------------------------------

}
#endif

