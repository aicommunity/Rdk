/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBASOURCE_CPP
#define UBASOURCE_CPP

#include "UBASource.h"

namespace RDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBASource::UBASource(void)
: Output("Output",this)
{
}

UBASource::~UBASource(void)
{
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Непосредственно задает выходное изображение
bool UBASource::SetOutputData(int index, const UBitmap &bitmap)
{
 if(index<0 || index>0)
  return false;

 if(!Build())
  return false;

 *Output=bitmap;

 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
UBASource* UBASource::New(void)
{
 return new UBASource;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UBASource::ADefault(void)
{
 return true;
}

// Сброс процесса счета.
bool UBASource::AReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool UBASource::ACalculate(void)
{
 return true;
}
// --------------------------
}
#endif


