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


// Абстрактная система координат
template <class T>
class MCSystem
{
public: // Данные
// Базис в 3-мерном пространстве
MVector<T> Basis[3];

// Положение базиса относительно выбранного нуля координат
MVector<T> Location;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
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
// Методы преобразования систем координат
// --------------------------
// Преобразование в заданную систему координат того же типа что текущая
// координаты вектора input в базисе текущей системы координат
// преобразуются в систему координат csystem и возвращается как output
// input и output НЕ могут указывать на один объект
virtual bool ConvertTo(const MCSystem &csystem,
                        const MVector<T> &input, MVector<T> &output) const=0;

// Преобразование в заданную систему координат
// координаты вектора v в базисе текущей системы координат
// преобразуются в систему координат csystem
MVector<T> ConvertTo(const MCSystem &csystem, const MVector<T> &v) const
{
 MVector<T> res;
 ConvertTo(csystem,v,res);
 return res;
};

// Вычисляет вектор смещения этой СК относительно заданной
virtual MVector<T> CalcTranslation(const MCSystem &csystem)=0;

// Вычисляет тензор поворота этой СК относительно заданной
virtual MRotationTensor<T> CalcRotation(const MCSystem &csystem)=0;
// --------------------------

// --------------------------
// Математические операторы
// --------------------------
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
MCSystem<T>& operator = (const MCSystem<T> &copy)
{
 for(int i=0;i<3;i++)
  Basis[i]=copy.Basis[i];

 Location=copy.Location;
 return *this;
};

// Операторы сравнения
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


// Декартова система координат
template <class T>
class MCartesianCSystem: public MCSystem<T>
{
public: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
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
// Методы преобразования систем координат
// --------------------------
// Преобразование в заданную систему координат того же типа что текущая
// координаты вектора input в базисе текущей системы координат
// преобразуются в систему координат csystem и возвращается как output
// input и output НЕ могут указывать на один объект
virtual bool ConvertTo(const MCSystem<T> &csystem,
                        const MVector<T> &input, MVector<T> &output) const
{
 output.x=input*Basis[0];
 output.y=input*Basis[1];
 output.z=input*Basis[2];
 return true;
};

// Вычисляет вектор смещения этой СК относительно заданной
MVector<T> CalcTranslation(const MCSystem<T> &csystem)
{
 return MVector<T>(csystem.Location-Location);
}

// Вычисляет тензор поворота этой СК относительно заданной
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
// Операторы
// --------------------------
// Оператор присваивания
MCartesianCSystem<T>& operator = (const MCartesianCSystem<T> &copy)
{
 static_cast<MCSystem<T>*>(this)->operator = (copy);
 return *this;
};

// Операторы сравнения
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


// Эйлерова система координат
template <class T>
class MEulerCSystem: public MCSystem<T>
{
public: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
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
// Методы преобразования систем координат
// --------------------------
// Преобразование в заданную систему координат того же типа что текущая
// координаты вектора input в базисе текущей системы координат
// преобразуются в систему координат csystem и возвращается как output
// input и output НЕ могут указывать на один объект
virtual bool ConvertTo(const MCSystem<T> &csystem,
                        const MVector<T> &input, MVector<T> &output) const
{
};
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
MEulerCSystem<T>& operator = (const MEulerCSystem<T> &copy)
{
 static_cast<MCSystem<T>*>(this)->operator = (copy);
 return *this;
};

// Операторы сравнения
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


// Полярная система координат
template <class T>
class MPolarCSystem: public MCSystem<T>
{
public: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
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
// Методы преобразования систем координат
// --------------------------
// Преобразование в заданную систему координат того же типа что текущая
// координаты вектора input в базисе текущей системы координат
// преобразуются в систему координат csystem и возвращается как output
// input и output НЕ могут указывать на один объект
virtual bool ConvertTo(const MCSystem<T> &csystem,
                        const MVector<T> &input, MVector<T> &output) const
{
};
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
MPolarCSystem<T>& operator = (const MPolarCSystem<T> &copy)
{
 static_cast<MCSystem<T>*>(this)->operator = (copy);
 return *this;
};

// Операторы сравнения
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
