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
  Класс - MDyad. Определяет новый тип данных - диада.
 Диаду можно создать след. способами:
  1. MDyad<type> d; - Создаётся диада с в-рами нулевой длины.
  2. MDyad<type> v( (MVector<type>)u ,(MVector<type>)v );
      - Создаётся диада в-ра которой равны u и v.
  3. MDyad<type> v( (MDyad<type>)d );
      - Создаётся диада равная диаде d.
 --------------------------------------

 В классе перегружены следующие операции:
   ## Сервисные операторы ##
   MDyad<type> []; - Доступ к в-рам диады по их порядк. номеру.
   !MDyad<type>; - Норма диады.
   MDyad<type>.Transpose(); - Операция транспонирования.
   Transpose(MDyad<type>); - Операция выдачи транспонированной диады.
   Trace(MDyad<DataD>); - След диады.
   VectInvar(MDyad<DataD>); - Векторный инвариант.

   ## Операторы присваивания ##
   MDyad<type> = MDyad<type>; - Присваивание диады диаде.
   MDyad<type> = MVector<type>; - Присваивание обеим в-рам диады значение в-ра.

   ## Логические операторы ##
   MDyad<type> == MDyad<type>; - Проверка на равенство.
   MDyad<type> != MDyad<type>; - Проверка на неравенство.

   ## Арифметические операторы ##
   - MDyad<type>; - Смена знака у всех коорд. левого в-ра диады.
   MDyad<type> *= MDyad<type>; - Скалярное умножение диады на диаду справа.
   MDyad<type> *= type; - Умножение диады на число справа.
   MDyad<type> ^= MVector<type>; - Векторное домножение диады на вектор справа.
   MDyad<type> /= type; - Деление правого в-ра диады на число
   MDyad<type> * MDyad<type>; - Скалярное умножение двух диад.
   MDyad<type> * MVector<type>; - Скалярное умножение  диады и в-ра.
   MVector<type> * MDyad<type>; - Скалярное умножение  в-ра и диады.
   MDyad<type> * type; - Умножение диады и числа.
   type * MDyad<type>; - Умножение числа и диады.
   MDyad<type> ^ MVector<type>; - Векторное умножение диады и вектора.
   MVector<type> ^ MDyad<type>; - Векторное умножение вектора и диады.
   MDyad<type> / type; - Деление диады и числа.
   SS(MDyad<type>,MDyad<type>); - Двойное скалярное произведение диад
   VV(MDyad<type>,MDyad<type>); - Двойное векторное произведение диад
   VS(MDyad<type>,MDyad<type>); - Двойное векторно-скалярное произведение диад
   SV(MDyad<type>,MDyad<type>); - Двойное скалярно-векторное произведение диад
   MVector<type> % MVector<type>; - Диадное умножение двух векторов.

   ## Потоковые операторы ввода-вывода ##
   ostream << MDyad<type>; - Вывод в поток ( как правило cout ), в удобочитаемой форме.
   istream >> MDyad<type>; - Ввод из потока ( как правило cin ), с запросом вида "d1=","d2=".
   fstream << MDyad<type>; - Вывод в файловый поток.
   fstream >> MDyad<type>; - Ввод из файлового потока.
 ----------------------------------------
*/
template<class DataD>
class MDyad
{
public: // Данные
MVector<DataD> d1,d2;// Вектора диады.

public: // Методы
// Конструкторы
// Создаётся диада с в-рами нулевой длины.
MDyad(void) {};

// Создаётся диада равная диаде d.
MDyad(const MDyad<DataD> &d)
{ d1=d.d1; d2=d.d2; };


// Создаётся диада в-ра которой равны u и v.
MDyad(const MVector<DataD> &u, const MVector<DataD> &v)
{ d1=u; d2=v; };
//-------------

// Перегружаемые операторы
// ## Сервисные операторы ##
// Доступ к в-рам диады по их порядк. номеру.
MVector<DataD>& operator [] (int index)
{ return *(&d1+index); };

// Норма диады.
DataD operator ! (void)
{ return sqrt( (d2*d2)*(d1*d1) ); };

// Операция транспонирования
MDyad<DataD>& Transpose(void)
{
 MVector<DataD> tmp;

 tmp=d1; d1=d2; d2=d1;
 return *this;
};

// Операция выдачи транспонированной диады.
// (Дурацкая фраза. Отличие этого метода от
// предыдущего видна из его описания: метод
// не модифицирует диаду,а создаёт новый,
// траспонированный экземпляр,его и возвращает).
friend MDyad<DataD> Transpose(const MDyad<DataD>&);

// След диады.
friend DataD Trace(const MDyad<DataD>&);

// Векторный инвариант.
friend MVector<DataD> VectInvar(const MDyad<DataD>&);
// #########################

// ## Операторы присваивания ##
// Присваивание диады диаде.
MDyad<DataD>& operator = (const MDyad<DataD> &D)
{
 d1=D.d1; d2=D.d2;
 return *this;
};

// Присваивание обеим в-рам диады значение в-ра.
MDyad<DataD>& operator = (const MVector<DataD> &v)
{
 d1=d2=v;
 return *this;
};
// ############################

// ## Логические операторы ##
// Проверка на равенство.
bool operator == (const MDyad<DataD> &D) const
{
 return (d1 == D.d1) && (d2 == D.d2);
}

// Проверка на неравенство.
bool operator != (const MDyad<DataD> &D) const
{
 return (d1 != D.d1) || (d2 != D.d2);
}
// ##########################

// ## Арифметические операторы ##
// Смена знака у всех коорд. левого в-ра диады.
MDyad<DataD> operator - (void)
{
 return MDyad<DataD>(d1); // Ошибка?!!!
};

// Скалярное умножение диады на диаду справа.
MDyad<DataD>& operator *=(const MDyad<DataD> &D)
{
 d2=D.d2*(d2*D.d1);
 return *this;
};

// Умножение диады на число справа.
MDyad<DataD>& operator *=(const DataD d)
{
 d2*=d;
 return *this;
};

// Векторное домножение диады на вектор справа.
MDyad<DataD>& operator ^=(const MVector<DataD> &v)
{
 d2^=v;
 return *this;
};

// Деление правого в-ра диады на число
MDyad<DataD>& operator /=(const DataD d)
{
 d2/=d;
 return *this;
};

// Скалярное умножение двух диад.
friend MDyad<DataD> operator * (const MDyad<DataD>&,const MDyad<DataD>&);

// Скалярное умножение  диады и в-ра.
friend MVector<DataD> operator * (const MDyad<DataD>&,const MVector<DataD>&);

// Скалярное умножение  в-ра и диады.
friend MVector<DataD> operator * (const MVector<DataD>&,const MDyad<DataD>&);

// Умножение диады и числа.
friend MDyad<DataD> operator * (const MDyad<DataD>&,const DataD);

// Умножение числа и диады.
friend MDyad<DataD> operator * (const DataD,const MDyad<DataD>&);

// Векторное умножение диады и вектора.
friend MDyad<DataD> operator ^ (const MDyad<DataD>&,const MVector<DataD>&);

// Векторное умножение вектора и диады.
friend MDyad<DataD> operator ^ (const MVector<DataD>&,const MDyad<DataD>&);

// Деление диады и числа.
friend MDyad<DataD> operator / (const MDyad<DataD>&,const DataD);

// Двойное скалярное произведение диад
friend DataD SS(const MDyad<DataD>&,const MDyad<DataD>&);

// Двойное векторное произведение диад
friend MDyad<DataD> VV(const MDyad<DataD>&,const MDyad<DataD>&);

// Двойное векторно-скалярное произведение диад
friend MVector<DataD> VS(const MDyad<DataD>&,const MDyad<DataD>&);

// Двойное скалярно-векторное произведение диад
friend MVector<DataD> SV(const MDyad<DataD>&,const MDyad<DataD>&);

// Диадное умножение двух векторов.
friend MDyad<DataD> operator % (const MVector<DataD>&,const MVector<DataD>&);
// ##############################
// -------------------------------
};


// Операция выдачи транспонированной диады.
template<class DataD>
inline MDyad<DataD> Transpose(const MDyad<DataD> &D)
{
 return MDyad<DataD>(D.d2,D.d1);
}

// След диады.
template<class DataD>
inline DataD Trace(const MDyad<DataD> &D)
{
 return D.d1*D.d2;
}

// Векторный инвариант.
template<class DataD>
inline MVector<DataD> VectInvar(const MDyad<DataD> &D)
{
 return MVector<DataD>(D.d1^D.d2);
}

// Скалярное умножение двух диад.
template<class DataD>
inline MDyad<DataD> operator * (const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MDyad<DataD>( MVector<DataD>(D1.d1),MVector<DataD>(D2.d2*(D1.d2*D2.d1)) );
}

// Скалярное умножение  диады и в-ра.
template<class DataD>
inline MVector<DataD> operator * (const MDyad<DataD> &D,const MVector<DataD> &v)
{
 return MVector<DataD>( D.d1*(D.d2*v) );
}

// Скалярное умножение  в-ра и диады.
template<class DataD>
inline MVector<DataD> operator * (const MVector<DataD> &v,const MDyad<DataD> &D)
{
 return MVector<DataD>( D.d2*(D.d1*v) );
}

// Умножение диады и числа.
template<class DataD>
inline MDyad<DataD> operator * (const MDyad<DataD> &D,const DataD d)
{
 return MDyad<DataD>( MVector<DataD>(D.d1),MVector<DataD>(D.d2*d) );
}

// Умножение числа и диады.
template<class DataD>
inline MDyad<DataD> operator * (const DataD d,const MDyad<DataD> &D)
{
 return MDyad<DataD>( MVector<DataD>(D.d1*d),MVector<DataD>(D.d2) );
}

// Векторное умножение диады и вектора.
template<class DataD>
inline MDyad<DataD> operator ^ (const MDyad<DataD> &D,const MVector<DataD> &v)
{
 return MDyad<DataD>( MVector<DataD>(D.d1),MVector<DataD>(D.d2^v) );
}

// Векторное умножение вектора и диады.
template<class DataD>
inline MDyad<DataD> operator ^ (const MVector<DataD>&,const MDyad<DataD>&)
{
 return MDyad<DataD>( MVector<DataD>(v^D.d1),MVector<DataD>(D.d2) );
}

// Деление диады и числа.
template<class DataD>
inline MDyad<DataD> operator / (const MDyad<DataD>&,const DataD)
{
 return MDyad<DataD>( MVector<DataD>(D.d1),MVector<DataD>(D.d2/d) );
}

// Двойное скалярное произведение диад
template<class DataD>
inline DataD SS(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return (D1.d2*D2.d1)*(D1.d1*D2.d2);
}

// Двойное векторное произведение диад
template<class DataD>
inline MDyad<DataD> VV(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MDyad<DataD>( D1.d2^D2.d1,D1.d1^D2.d2 );
}

// Двойное векторно-скалярное произведение диад
template<class DataD>
inline MVector<DataD> VS(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MVector<DataD>( (D1.d2^D2.d1)*(D1.d1*D2.d2) );
}

// Двойное скалярно-векторное произведение диад
template<class DataD>
inline MVector<DataD> SV(const MDyad<DataD> &D1,const MDyad<DataD> &D2)
{
 return MVector<DataD>( (D1.d2*D2.d1)*(D1.d1^D2.d2) );
}


// Диадное умножение двух векторов.
template<class DataD>
inline MDyad<DataD> operator % (const MVector<DataD> &u,const MVector<DataD> &v)
{
 return MDyad<DataD>( MVector<DataD>(u),MVector<DataD>(v) );
}
// ##############################

}
#endif
