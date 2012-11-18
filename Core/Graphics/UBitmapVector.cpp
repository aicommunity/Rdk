/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBITMAP_VECTOR_CPP
#define UBITMAP_VECTOR_CPP

#include "UBitmapVector.h"

namespace RDK {


// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPBitmapVector::UPBitmapVector(void)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;
}

UPBitmapVector::UPBitmapVector(const UPBitmapVector &copy)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;

 *this=copy;
}

UPBitmapVector::~UPBitmapVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Возвращает размер массива
int UPBitmapVector::GetSize(void) const
{
 return Size;
}

// Изменяет размер массива с сохранением прежних данных
void UPBitmapVector::Resize(int newsize)
{
 if(RealSize<newsize || !Buffer)
 {
  PUBitmap *newbuffer=new PUBitmap[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Buffer[i];

  for(int i=Size;i<newsize;i++)
   newbuffer[i]=0;

  if(Buffer)
   delete []Buffer;
  Buffer=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Очищает массив
void UPBitmapVector::Clear(void)
{
 if(Buffer)
 {
  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}

// Возвращает указатель на начало данных
PUBitmap* UPBitmapVector::GetBuffer(void)
{
 return Buffer;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UPBitmapVector& UPBitmapVector::operator = (const UPBitmapVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  *Buffer[i]=*copy.Buffer[i];

 return *this;
}

// Оператор доступа
PUBitmap& UPBitmapVector::operator [] (int index)
{
 return Buffer[index];
}

const PUBitmap& UPBitmapVector::operator [] (int index) const
{
 return Buffer[index];
}

UBitmap& UPBitmapVector::operator () (int index)
{
 return *Buffer[index];
}

const UBitmap& UPBitmapVector::operator () (int index) const
{
 return *Buffer[index];
}
// --------------------------


// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBitmapVector::UBitmapVector(void)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;
}

UBitmapVector::UBitmapVector(const UBitmapVector &copy)
    : UPBitmapVector(copy)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;

 *this=copy;
}

UBitmapVector::~UBitmapVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Изменяет размер массива с сохранением прежних данных
void UBitmapVector::Resize(int newsize)
{
 if(RealSize<newsize || !Buffer)
 {
  PUBitmap *newbuffer=new PUBitmap[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Buffer[i];

  for(int i=Size;i<newsize;i++)
   newbuffer[i]=new UBitmap;

  if(Buffer)
   delete []Buffer;
  Buffer=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Очищает массив
void UBitmapVector::Clear(void)
{
 if(Buffer)
 {
  for(int i=0;i<RealSize;i++)
   delete Buffer[i];

  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UBitmapVector& UBitmapVector::operator = (const UBitmapVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  *Buffer[i]=*copy.Buffer[i];

 return *this;
}

// Оператор доступа
PUBitmap UBitmapVector::operator [] (int index)
{
 return Buffer[index];
}

PUBitmap UBitmapVector::operator [] (int index) const
{
 return Buffer[index];
}

UBitmap& UBitmapVector::operator () (int index)
{
 return *Buffer[index];
}

const UBitmap& UBitmapVector::operator () (int index) const
{
 return *Buffer[index];
}
// --------------------------


}

#endif


