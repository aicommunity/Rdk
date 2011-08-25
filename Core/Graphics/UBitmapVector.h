/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBITMAP_VECTOR_H
#define UBITMAP_VECTOR_H

#include "UBitmap.h"

namespace RDK {

// Одномерный массив указателей на изображения
class UPBitmapVector
{
protected: // Параметры
// Размер массива изображений
int Size;

protected: // Данные
// Массив указателей на изображения
mutable PUBitmap* Buffer;

// Реальный размер массива
int RealSize;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPBitmapVector(void);
UPBitmapVector(const UPBitmapVector &copy);
virtual ~UPBitmapVector(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Возвращает размер массива
int GetSize(void) const;

// Изменяет размер массива с сохранением прежних данных
virtual void Resize(int newsize);

// Очищает массив
virtual void Clear(void);

// Возвращает указатель на начало данных
PUBitmap* GetBuffer(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UPBitmapVector& operator = (const UPBitmapVector &copy);

// Оператор доступа
PUBitmap& operator [] (int index);
const PUBitmap& operator [] (int index) const;
UBitmap& operator () (int index);
const UBitmap& operator () (int index) const;
// --------------------------
};

// Одномерный массив изображений
class UBitmapVector: public UPBitmapVector
{
protected: // Параметры

protected: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBitmapVector(void);
UBitmapVector(const UBitmapVector &copy);
virtual ~UBitmapVector(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Изменяет размер массива с сохранением прежних данных
virtual void Resize(int newsize);

// Очищает массив
virtual void Clear(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UBitmapVector& operator = (const UBitmapVector &copy);

// Оператор доступа
PUBitmap operator [] (int index);
const PUBitmap operator [] (int index) const;
UBitmap& operator () (int index);
const UBitmap& operator () (int index) const;
// --------------------------
};


}
#endif


