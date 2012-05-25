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

#include <vector>
#include "MDyad.h"

namespace RDK {

/*
   Класс MTensor определяет новый тип данных - тензор 2-го ранга.
*/
template<class DataT>
class MTensor
{
public:
// Список диад тензора.
std::vector<MDyad<DataT> > Dyads;

public:
// Конструкторы и деструкторы.
// Создаётся тензор с нулём диад.
MTensor(void) {};

// Создаётся тензор с одной диадой.
MTensor(const MDyad<DataT> &D)
{
 Dyads.resize(1);
 Dyads[0]=D;
};

// Создаётся тензор с несколькими диадами, заданными массивом.
MTensor(const MDyad<DataT> *DL,int quan)
{
 Dyads.resize(quan);
 for(int i=0;i<quan;i++)
  Dyads[i]=DL[i];
};

// Создаётся тензор с несколькими диадами, заданными дин. массивом.
MTensor(const std::vector<MDyad<DataT> > &DA)
{
 Dyads=DA;
};

// Создаётся тензор с одной диадой оба вектора которой равны заданному.
MTensor(const MVector<DataT,3> &v)
{
 Dyads.ChangeLength(1);
 Dyads[0]=v;
};

// Создаётся тензор равный заданному.
MTensor(const MTensor<DataT> &T)
{
 Dyads=T.Dyads;
};

// Деструктор.
~MTensor(void)
{
 Clear();
};
//--------------------------

//--------------------------
// Системные функции.
//--------------------------
void Clear(void)
{
 Dyads.clear();
};
//--------------------------

//--------------------------
// Перегружаемые операторы
//--------------------------
// ## Сервисные операторы ##
// Доступ к диадам тензора по их порядк. номеру.
MDyad<DataT>& operator [] (int index)
{
 return Dyads[index];
};

// Норма тензора.
DataT operator ! (void)
{
 return sqrt( SS(*this,Transpose(*this) ) );
};

// Операция транспонирования
MTensor<DataT>& Transpose(void)
{
 for(int i=0;i<Dyads.size();i++)
  Dyads[i].Transpose();
 return *this;
};

// Операция выдачи транспонированного тензора.
// Отличие этого метода от
// предыдущего видна из его описания: метод
// не модифицирует тензор,а создаёт новый,
// траспонированный экземпляр, его и возвращает.
template<class DataU> friend MTensor<DataU> Transpose(const MTensor<DataU> &T);

// След тензора.
template<class DataU> friend DataU Trace(const MTensor<DataU> &T);

// Векторный инвариант.
template<class DataU> friend MVector<DataU,3> VectInvar(const MTensor<DataU> &T);
// #########################

// ## Операторы присваивания ##
// Присваивание тензора тензору.
MTensor<DataT>& operator = (const MTensor<DataT> &T)
{
 Dyads=T.Dyads;
 return *this;
};

// Присваивание тензору диаду.
MTensor<DataT>& operator = (const MDyad<DataT> &D)
{
 Clear();

 Dyads.resize(1);
 Dyads[0]=D;

 return *this;
};
// ############################

// ## Логические операторы ##
// Проверка на равенство.
bool operator == (const MTensor<DataT> &T) const
{
 return (Dyads == T.Dyads);
}

// Проверка на неравенство.
bool operator != (const MTensor<DataT> &T) const
{
 return (Dyads != T.Dyads);
}
// ##########################

// ## Арифметические операторы ##
// Смена знака у всех диад тензора.
MTensor<DataT> operator - (void)
{
 std::vector< MDyad<DataT> > tmp;
 size_t leng=Dyads.size();

 tmp.resize(leng);
 for(size_t i=0;i<leng;i++)
  tmp[i]=-Dyads[i];

 return MTensor<DataT>(tmp);
};

// Добавление тензору тензора.
MTensor<DataT>& operator += (const MTensor<DataT> &T)
{
 Dyads+=T.Dyads;
 return *this;
};

// Добавление тензору диаду.
MTensor<DataT>& operator += (const MDyad<DataT> &D)
{
 Dyads.push_back(D);
 return *this;
};

// Вычитание из тензора тензора.
MTensor<DataT>& operator -= (const MTensor<DataT> &T)
{
 this+=-T;
 return *this;
};

// Вычитание из тензора диаду.
MTensor<DataT>& operator -= (const MDyad<DataT> &D)
{
 this+=-D;
 return *this;
};

// Скалярное домножение к тензору тензора.
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

// Скалярное домножение к тензору диаду.
MTensor<DataT>& operator *= (const MDyad<DataT> &D)
{
 for(size_t i=0;i<Dyads.size();i++)
  Dyads[i]*=D;

 return *this;
};

// Домножение тензора на число.
MTensor<DataT>& operator *= (DataT DT)
{
 for(size_t i=0;i<Dyads.size();i++)
  Dyads[i]*=DT;

 return *this;
};

// Сложение двух тензоров.
template<class DataU> friend MTensor<DataU> operator + (const MTensor<DataU> &T1,const MTensor<DataU> &T2);

// Сложение тензора и диады.
template<class DataU> friend MTensor<DataU> operator + (const MTensor<DataU> &T1,const MDyad<DataU> &T2);

// Сложение диады и тензора.
template<class DataU> friend MTensor<DataU> operator + (const MDyad<DataU> &T1,const MTensor<DataU> &T2);

// Вычитание двух тензоров.
template<class DataU> friend MTensor<DataU> operator - (const MTensor<DataU> &T1,const MTensor<DataU> &T2);

// Вычитание тензора и диады.
template<class DataU> friend MTensor<DataU> operator - (const MTensor<DataU> &T,const MDyad<DataU> &D);

// Вычитание диады и тензора.
template<class DataU> friend MTensor<DataU> operator - (const MDyad<DataU> &D,const MTensor<DataU> &T);

// Скалярное умножение двух тензоров.
template<class DataU> friend MTensor<DataU> operator * (const MTensor<DataU> &T1,const MTensor<DataU> &T2);

// Скалярное умножение тензора и диады.
template<class DataU> friend MTensor<DataU> operator * (const MTensor<DataU> &T,const MDyad<DataU> &D);

// Скалярное умножение диады и тензора.
template<class DataU> friend MTensor<DataU> operator * (const MDyad<DataU> &D,const MTensor<DataU> &T);

// Скалярное умножение вектора и тензора.
template<class DataU> friend MVector<DataU,3> operator * (const MVector<DataU,3> &v,const MTensor<DataU> &T);

// Скалярное умножение тензора и вектора.
template<class DataU> friend MVector<DataU,3> operator * (const MTensor<DataU> &T,const MVector<DataU,3> &v);

// Умножение тензора и числа.
template<class DataU> friend MTensor<DataU> operator * (const MTensor<DataU> &T,DataU d);

// Умножение числа и тензора.
template<class DataU> friend MTensor<DataU> operator * (DataU d,const MTensor<DataU> &T);

// Деление тензора и числа.
template<class DataU> friend MTensor<DataU> operator / (const MTensor<DataU> &T, DataU d);

// Векторное умножение вектора и тензора.
template<class DataU> friend MTensor<DataU> operator ^ (const MVector<DataU,3> &v,const MTensor<DataU> &T);

// Векторное умножение тензора и вектора.
template<class DataU> friend MTensor<DataU> operator ^ (const MTensor<DataU> &T,const MVector<DataU,3> &v);

// Двойное скалярное произведение тензоров
template<class DataU> friend DataU SS(const MTensor<DataU> &T1,const MTensor<DataU> &T2);

// Двойное векторное произведение тензоров
template<class DataU> friend MTensor<DataU> VV(const MTensor<DataU> &T1,const MTensor<DataU> &T2);

// Двойное векторно-скалярное произведение тензоров
template<class DataU> friend MVector<DataU,3> VS(const MTensor<DataU> &T1,const MTensor<DataU> &T2);

// Двойное скалярно-векторное произведение тензоров
template<class DataU> friend MVector<DataU,3> SV(const MTensor<DataU> &T1,const MTensor<DataU> &T2);
// ##############################
// -------------------------------
};



// Операция выдачи транспонированного тензора.
template<class DataT>
MTensor<DataT> Transpose(const MTensor<DataT> &T)
{
 std::vector< MDyad<DataT> > tmp(T.Dyads);

 for(int i=0;i<T.Dyads.size();i++)
  tmp[i].Transpose();

 return MTensor<DataT>(tmp);
}

// След тензора.
template<class DataT>
DataT Trace(const MTensor<DataT> &T)
{
 DataT tmp=0;

 for(int i=0;i<T.Dyads.size();i++)
  tmp+=T.Dyads[i].d1*T.Dyads[i].d2;

 return tmp;
}

// Векторный инвариант.
template<class DataT>
MVector<DataT,3> VectInvar(const MTensor<DataT> &T)
{
 MVector<DataT,3> tmp;

 for(int i=0;i<T.Dyads.size();i++)
  tmp+=T.Dyads[i].d1^T.Dyads[i].d2;

 return MVector<DataT,3>(tmp);
}
// #########################

// -------------------------------
// Сложение двух тензоров.
template<class DataT>
MTensor<DataT> operator + (const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 return MTensor<DataT>(T1.Dyads+T2.Dyads);
}

// Сложение тензора и диады.
template<class DataT>
MTensor<DataT> operator + (const MTensor<DataT> &T,const MDyad<DataT> &D)
{
 return MTensor<DataT>(T.Dyads+D);
}

// Сложение диады и тензора.
template<class DataT>
MTensor<DataT> operator + (const MDyad<DataT> &D,const MTensor<DataT> &T)
{
 return MTensor<DataT>(T.Dyads+D);
}

// Вычитание двух тензоров.
template<class DataT>
MTensor<DataT> operator - (const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 MTensor<DataT>(T1.Dyads+(-T2).Dyads);
}

// Вычитание тензора и диады.
template<class DataT>
MTensor<DataT> operator - (const MTensor<DataT> &T,const MDyad<DataT> &D)
{
 return MTensor<DataT>(T.Dyads+(-D));
}

// Вычитание диады и тензора.
template<class DataT>
MTensor<DataT> operator - (const MDyad<DataT> &D,const MTensor<DataT> &T)
{
 return MTensor<DataT>(T.Dyads+(-D));
}

// Скалярное умножение двух тензоров.
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

// Скалярное умножение тензора и диады.
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

// Скалярное умножение диады и тензора.
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

// Скалярное умножение вектора и тензора.
template<class DataT>
MVector<DataT,3> operator * (const MVector<DataT,3> &v,const MTensor<DataT> &T)
{
 MVector<DataT,3> tmp;

 for(int i=0;i<T.Dyads.size();i++)
  tmp+=v*T.Dyads[i];

 return MVector<DataT,3>(tmp);
}

// Скалярное умножение тензора и вектора.
template<class DataT>
MVector<DataT,3> operator * (const MTensor<DataT> &T,const MVector<DataT,3> &v)
{
 MVector<DataT,3> tmp;

 for(int i=0;i<T.Dyads.size();i++)
  tmp+=T.Dyads[i]*v;

 return MVector<DataT,3>(tmp);
}

// Умножение тензора и числа.
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

// Умножение числа и тензора.
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

// Деление тензора и числа.
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

// Векторное умножение вектора и тензора.
template<class DataT>
MTensor<DataT> operator ^ (const MVector<DataT,3> &v,const MTensor<DataT> &T)
{
 std::vector< MDyad<DataT> > tmp;
 int leng=T.Dyads.size();

 tmp.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp[i]=v^T.Dyads[i];

 return MTensor<DataT>(tmp);
}

// Векторное умножение тензора и вектора.
template<class DataT>
MTensor<DataT> operator ^ (const MTensor<DataT> &T,const MVector<DataT,3> &v)
{
 std::vector< MDyad<DataT> > tmp;
 int leng=T.Dyads.size();

 tmp.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp[i]=T.Dyads[i]^v;

 return MTensor<DataT>(tmp);
}

// Двойное скалярное произведение тензоров
template<class DataT>
DataT SS(const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 DataT tmp=0;

 for(int i=0;i<T1.Dyads.size();i++)
  for(int j=0;j<T2.Dyads.size();j++)
   tmp+=SS(T1.Dyads[i],T2.Dyads[j]);

 return tmp;
}

// Двойное векторное произведение тензоров
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

// Двойное векторно-скалярное произведение тензоров
template<class DataT>
MVector<DataT,3> VS(const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 MVector<DataT,3> tmp;

 for(int i=0;i<T1.Dyads.size();i++)
  for(int j=0;j<T2.Dyads.size();j++)
   tmp+=VS(T1.Dyads[i],T2.Dyads[j]);

 return MVector<DataT,3>(tmp);
}

// Двойное скалярно-векторное произведение тензоров
template<class DataT>
MVector<DataT,3> SV(const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 MVector<DataT,3> tmp;

 for(int i=0;i<T1.Dyads.size();i++)
  for(int j=0;j<T2.Dyads.size();j++)
   tmp+=SV(T1.Dyads[i],T2.Dyads[j]);

 return MVector<DataT,3>(tmp);
}
// ##############################
// -------------------------------

}
#endif

