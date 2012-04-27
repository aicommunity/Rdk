/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef MDVECTOR_H
#define MDVECTOR_H

namespace RDK {

			   /*
#ifdef MVECTOR_H
typedef MVector<double> MDVector;
#else

class MDVector
{
public: // Данные
double x,y,z; // Координаты в-ра.

public: // Методы
//---------------------------
// Конструкторы и деструкторы
//---------------------------
//  Создаёт в-р нулевой длины.
MDVector(void);

// Создаёт в-р все коорд. которого = ksi.
MDVector(double ksi);

// Создаёт в-р с коорд. xx,yy,zz.
MDVector(double xx,double yy,double zz);

// Создаёт в-р равный в-ру v
MDVector(const MDVector &v);
//---------------------------

//---------------------------
// Сервисные операторы
//---------------------------
// Доступ к коорд. в-ра по их порядк. номеру.
double& operator [](int n);

// Получение длины вектора
double operator !(void) const;

// Функция нормирования вектора.
MDVector& Normalize(void);
//---------------------------

//---------------------------
// Операторы присваивания
//---------------------------
// Присваивание в-ра в-ру.
MDVector& operator = (const MDVector &v);

// Присваивание всем коорд. в-ра значения double
MDVector& operator = (double f);
//---------------------------

//---------------------------
// Логические операторы
//---------------------------
// Проверка на равенство.
bool operator == (const MDVector& v) const;

// Проверка на неравенство.
bool operator != (const MDVector& v) const;

// Проверка "все коорд. строго меньше".
bool operator < (double v);

// Проверка "все коорд. строго больше".
bool operator > (double v);
//---------------------------

//---------------------------
// Арифметическии операторы
//---------------------------
// Смена знака у всех коорд. в-ра.
MDVector operator - (void) const;

// Добавление к вектору вектора.
MDVector& operator += (const MDVector &v);

// Вычитание из вектора вектора.
MDVector& operator -= (const MDVector &v);

// Векторное домножение вектора на вектор.
MDVector& operator ^= (const MDVector &v);

// Умножение в-ра на число
MDVector& operator *= (double f);

// Деление в-ра на число
MDVector& operator /= (double f);

// Почленное деление в-ра на в-р
MDVector& operator /= (const MDVector &v);
//---------------------------
};

// Нормировка
MDVector Normalize(const MDVector &v);

// Сложение векторов.
MDVector operator + (const MDVector &u,const MDVector &v);

// Вычитание векторов.
MDVector operator - (const MDVector &u,const MDVector &v);

// Векторное умножение векторов.
MDVector operator ^ (const MDVector &u,const MDVector &v);

// Скалярное умножение векторов.
double operator * (const MDVector &u,const MDVector &v);

// Умножение вектора и числа справа.
MDVector operator * (const MDVector &v,double f);

// Умножение вектора и числа слева.
MDVector operator * (double f,const MDVector &v);

// Деление вектора и числа.
MDVector operator / (const MDVector &v,double f);

// Покомпонентное деление в-ров
MDVector operator / (const MDVector &u,const MDVector &v);

#endif    */
}
#endif
