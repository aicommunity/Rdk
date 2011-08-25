/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UMODULE_CPP
#define UMODULE_CPP

#include "UModule.h"

namespace RDK {


// --------------------------
// Конструкторы и деструкторы
// --------------------------
UModule::UModule(void)
{
 Ready=false;
}

UModule::~UModule(void)
{
}
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Флаг готовности объекта к счету
bool UModule::IsReady(void) const
{
 return Ready;
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UModule::Default(void)
{
 Ready=false;
 if(!ADefault())
  return false;

 return Build();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UModule::Build(void)
{
 if(IsReady())
  return true;

 if(ABuild())
 {
  Ready=true;
  return Reset();
 }

 Ready=false;
 return false;
}

// Сброс процесса счета.
bool UModule::Reset(void)
{
 if(!IsReady())
  return Build();

 if(!AReset())
  return false;

 return true;
}

// Выполняет расчет этого объекта
bool UModule::Calculate(void)
{
 if(!IsReady())
  if(!Build())
   return false;

 return ACalculate();
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UModule::ADefault(void)
{
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UModule::ABuild(void)
{
 return true;
}

// Сброс процесса счета.
bool UModule::AReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool UModule::ACalculate(void)
{
 return true;
}
// --------------------------

}

#endif

