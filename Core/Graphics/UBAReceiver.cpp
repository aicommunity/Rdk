/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBARECEIVER_CPP
#define UBANRECEIVER_CPP

#include "UBAReceiver.h"

namespace RDK {


// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBAReceiver::UBAReceiver(void)
{
}

UBAReceiver::~UBAReceiver(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
/// Выделяет память для новой чистой копии объекта этого класса
UBAReceiver* UBAReceiver::New(void)
{
 return new UBAReceiver;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
/// Восстановление настроек по умолчанию и сброс процесса счета
bool UBAReceiver::AFDefault(void)
{
 return true;
}

/// Сброс процесса счета.
bool UBAReceiver::AFReset(void)
{
 return true;
}

/// Выполняет расчет этого объекта
bool UBAReceiver::AFCalculate(void)
{
 int min=(Inputs.GetSize()<Outputs.GetSize())?Inputs.GetSize():Outputs.GetSize();
 for(int i=0;i<min;i++)
 {
  *Outputs[i]=*Inputs[i];
 }

 return true;
}
// --------------------------

}
#endif


