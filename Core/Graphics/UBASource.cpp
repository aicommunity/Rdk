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
 if(index<0 || index>=NumOutputs)
  return false;

 if(!Build())
  return false;

 Outputs(index)=bitmap;

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
bool UBASource::PLACalculate(UBitmap **input, UBitmap **output, int num_inputs, int num_outputs)
{
 return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool UBASource::AFDefault(void)
{
 return true;
}

// Сброс процесса счета.
bool UBASource::AFReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
/*bool UBASource::ACalculate(void)
{
 if(ActionCounter.v>0)
 {
  ActionCounter-=TimeStep.v;
  if(ActionCounter.v <= 0) // Выключаем импульс и включаем ожидание
  {
   ActionCounter=0;
   Activity=false;
  }
 }

 return true;
}      */
// --------------------------
}
#endif


