/* ***********************************************************
@Copyright @COPYRIGHT_NAME@, @COPYRIGHT_YEAR@.
E-mail:        @COPYRIGHT_EMAIL@
Url:           @COPYRIGHT_URL@

This file is part of the project: @COPYRIGHT_PROJECT_NAME@

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef @FILE_NAME@_CPP
#define @FILE_NAME@_CPP

#include "@FILE_NAME@.h"

namespace @NAMESPACE_NAME@ {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
@CLASS_NAME@::@CLASS_NAME@(void)
{
}

@CLASS_NAME@::~@CLASS_NAME@(void)
{
}
// --------------------------    


// ---------------------
// Методы управления параметрами
// ---------------------
// ---------------------

// ---------------------
// Методы управления переменными состояния
// ---------------------
// ---------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
@CLASS_NAME@* @CLASS_NAME@::New(void)
{
 return new @CLASS_NAME@;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом 
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool @CLASS_NAME@::ADefault(void)
{           
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool @CLASS_NAME@::ABuild(void)
{
 return true;
}

// Сброс процесса счета без потери настроек
bool @CLASS_NAME@::AReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool @CLASS_NAME@::ACalculate(void)
{
 return true;
}
// --------------------------

}
#endif



