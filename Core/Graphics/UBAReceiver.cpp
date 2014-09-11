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
: Input("Input",this),
  Output("Output",this)
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
bool UBAReceiver::ADefault(void)
{
 return true;
}

/// Сброс процесса счета.
bool UBAReceiver::AReset(void)
{
 return true;
}

/// Выполняет расчет этого объекта
bool UBAReceiver::ACalculate(void)
{
 *Output=*Input;
 return true;
}
// --------------------------

}
#endif


