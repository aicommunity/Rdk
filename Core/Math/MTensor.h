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
MTensor(const MVector<DataT> &v)
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
friend MTensor<DataT> Transpose(const MTensor<DataT> &T);

// След тензора.
friend DataT Trace(const MTensor<DataT> &T);

// Векторный инвариант.
friend MVector<DataT> VectInvar(const MTensor<DataT>&);
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
 std::vector< Dyads<DataT> > tmp;
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
MTensor<DataT>& operator *= (DataT)
{
 for(size_t i=0;i<Dyads.size();i++)
  Dyads[i]*=DT;

 return *this;
};

// Сложение двух тензоров.
friend MTensor<DataT> operator + (const MTensor<DataT>&,const MTensor<DataT>&);

// Сложение тензора и диады.
friend MTensor<DataT> operator + (const MTensor<DataT>&,const MDyad<DataT>&);

// Сложение диады и тензора.
friend MTensor<DataT> operator + (const MDyad<DataT>&,const MTensor<DataT>&);

// Вычитание двух тензоров.
friend MTensor<DataT> operator - (const MTensor<DataT>&,const MTensor<DataT>&);

// Вычитание тензора и диады.
friend MTensor<DataT> operator - (const MTensor<DataT>&,const MDyad<DataT>&);

// Вычитание диады и тензора.
friend MTensor<DataT> operator - (const MDyad<DataT>&,const MTensor<DataT>&);

// Скалярное умножение двух тензоров.
friend MTensor<DataT> operator * (const MTensor<DataT>&,const MTensor<DataT>&);

// Скалярное умножение тензора и диады.
friend MTensor<DataT> operator * (const MTensor<DataT>&,const MDyad<DataT>&);

// Скалярное умножение диады и тензора.
friend MTensor<DataT> operator * (const MDyad<DataT>&,const MTensor<DataT>&);

// Скалярное умножение вектора и тензора.
friend MVector<DataT> operator * (const MVector<DataT>&,const MTensor<DataT>&);

// Скалярное умножение тензора и вектора.
friend MVector<DataT> operator * (const MTensor<DataT>&,const MVector<DataT>&);

// Умножение тензора и числа.
friend MTensor<DataT> operator * (const MTensor<DataT>&,DataT);

// Умножение числа и тензора.
friend MTensor<DataT> operator * (DataT,const MTensor<DataT>&);

// Деление тензора и числа.
friend MTensor<DataT> operator / (const MTensor<DataT>&,DataT);

// Векторное умножение вектора и тензора.
friend MTensor<DataT> operator ^ (const MVector<DataT>&,const MTensor<DataT>&);

// Векторное умножение тензора и вектора.
friend MTensor<DataT> operator ^ (const MTensor<DataT>&,const MVector<DataT>&);

// Двойное скалярное произведение тензоров
friend DataT SS(const MTensor<DataT>&,const MTensor<DataT>&);

// Двойное векторное произведение тензоров
friend MTensor<DataT> VV(const MTensor<DataT>&,const MTensor<DataT>&);

// Двойное векторно-скалярное произведение тензоров
friend MVector<DataT> VS(const MTensor<DataT>&,const MTensor<DataT>&);

// Двойное скалярно-векторное произведение тензоров
friend MVector<DataT> SV(const MTensor<DataT>&,const MTensor<DataT>&);
// ##############################
// -------------------------------
};



// Операция выдачи транспонированного тензора.
template<class DataT>
MTensor<DataT> Transpose(const MTensor<DataT> &T)
{
 std::vector< MDyad<DataT> > tmp(T.Dyads);

 for(int i=0;i<Dyads.size();i++)
  tmp[i].Transpose();

 return MTensor<DataT>(tmp);
}

// След тензора.
template<class DataT>
DataT Trace(const MTensor<DataT> &T)
{
 DataT tmp=0;

 for(int i=0;i<T.Dyads.size();i++)
  tmp+=Dyads[i].d1*Dyads[i].d2;

 return tmp;
}

// Векторный инвариант.
template<class DataT>
MVector<DataT> VectInvar(const MTensor<DataT> &T)
{
 MVector<DataT> tmp;

 for(int i=0;i<T.Dyads.size();i++)
  tmp+=Dyads[i].d1^Dyads[i].d2;

 return MVector<DataT>(tmp);
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
 DArray< MDyad<DataT> > tmp;
 int leng,k=0;

 leng=T1.Dyads.GetLength()*T2.Dyads.GetLength();
 tmp.ChangeLength(leng);
 for(int i=0;i<T1.Dyads.GetLength();i++)
  for(int j=0;j<T2.Dyads.GetLength();j++)
   tmp[k++]=T1.Dyads[i]*T2.Dyads[j];

 return MTensor<DataT>(tmp);
}

// Скалярное умножение тензора и диады.
template<class DataT>
MTensor<DataT> operator * (const MTensor<DataT> &T,const MDyad<DataT> &D)
{
 DArray< MDyad<DataT> > tmp;
 int leng=T.Dyads.GetLength();

 tmp.Dyads.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp.Dyads[i]=T.Dyads[i]*D;

 return MTensor<DataT>(tmp);
}

// Скалярное умножение диады и тензора.
template<class DataT>
MTensor<DataT> operator * (const MDyad<DataT> &D,const MTensor<DataT> &T)
{
 DArray< MDyad<DataT> > tmp;
 int leng=T.Dyads.GetLength();

 tmp.Dyads.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp.Dyads[i]=D*T.Dyads[i];

 return MTensor<DataT>(tmp);
}

// Скалярное умножение вектора и тензора.
template<class DataT>
MVector<DataT> operator * (const MVector<DataT> &v,const MTensor<DataT> &T)
{
 MVector<DataT> tmp;

 for(int i=0;i<T.Dyads.GetLength();i++)
  tmp+=v*T.Dyads[i];

 return MVector<DataT>(tmp);
}

// Скалярное умножение тензора и вектора.
template<class DataT>
MVector<DataT> operator * (const MTensor<DataT> &T,const MVector<DataT> &v)
{
 MVector<DataT> tmp;

 for(int i=0;i<T.Dyads.GetLength();i++)
  tmp+=T.Dyads[i]*v;

 return MVector<DataT>(tmp);
}

// Умножение тензора и числа.
template<class DataT>
MTensor<DataT> operator * (const MTensor<DataT> &T,DataT d)
{
 DArray< Dyads<DataT> > tmp;
 int leng=T.Dyads.GetLength();

 tmp.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp[i]=T.Dyads[i]*d;

 return MTensor<DataT>(tmp);
}

// Умножение числа и тензора.
template<class DataT>
MTensor<DataT> operator * (DataT,const MTensor<DataT>&)
{
 DArray< Dyads<DataT> > tmp;
 int leng=T.Dyads.GetLength();

 tmp.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp[i]=d*T.Dyads[i];

 return MTensor<DataT>(tmp);
}

// Деление тензора и числа.
template<class DataT>
MTensor<DataT> operator / (const MTensor<DataT> &T,DataT d)
{
 DArray< Dyads<DataT> > tmp;
 int leng=T.Dyads.GetLength();

 tmp.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp[i]=T.Dyads[i]/d;

 return MTensor<DataT>(tmp);
}

// Векторное умножение вектора и тензора.
template<class DataT>
MTensor<DataT> operator ^ (const MVector<DataT> &v,const MTensor<DataT> &T)
{
 DArray< Dyads<DataT> > tmp;
 int leng=T.Dyads.GetLength();

 tmp.ChangeLength(leng);
 for(int i=0;i<leng;i++)
  tmp[i]=v^T.Dyads[i];

 return MTensor<DataT>(tmp);
}

// Векторное умножение тензора и вектора.
template<class DataT>
MTensor<DataT> operator ^ (const MTensor<DataT> &T,const MVector<DataT> &v)
{
 DArray< Dyads<DataT> > tmp;
 int leng=T.Dyads.GetLength();

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

 for(int i=0;i<T1.Dyads.GetLength();i++)
  for(int j=0;j<T2.Dyads.GetLength();j++)
   tmp+=SS(T1.Dyads[i],T2.Dyads[j]);

 return tmp;
}

// Двойное векторное произведение тензоров
template<class DataT>
MTensor<DataT> VV(const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 DArray< MDyad<DataT> > tmp;
 int k=0;

 for(int i=0;i<T1.Dyads.GetLength();i++)
  for(int j=0;j<T2.Dyads.GetLength();j++)
   tmp[k++]=VV(T1.Dyads[i],T2.Dyads[j]);

 return MTensor<DataT>(tmp);
}

// Двойное векторно-скалярное произведение тензоров
template<class DataT>
MVector<DataT> VS(const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 MVector<DataT> tmp;

 for(int i=0;i<T1.Dyads.GetLength();i++)
  for(int j=0;j<T2.Dyads.GetLength();j++)
   tmp+=VS(T1.Dyads[i],T2.Dyads[j]);

 return MVector<DataT>(tmp);
}

// Двойное скалярно-векторное произведение тензоров
template<class DataT>
MVector<DataT> SV(const MTensor<DataT> &T1,const MTensor<DataT> &T2)
{
 MVector<DataT> tmp;

 for(int i=0;i<T1.Dyads.GetLength();i++)
  for(int j=0;j<T2.Dyads.GetLength();j++)
   tmp+=SV(T1.Dyads[i],T2.Dyads[j]);

 return MVector<DataT>(tmp);
}
// ##############################
// -------------------------------

}
#endif

