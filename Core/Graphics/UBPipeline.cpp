/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBPIPELINE_CPP
#define UBPIPELINE_CPP

#include <string.h>
#include "UBPipeline.h"


namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBPipeline::UBPipeline(void)
{
}

UBPipeline::~UBPipeline(void)
{
}
// --------------------------

// ---------------------
// Методы управления конвеером
// ---------------------
// ---------------------


// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool UBPipeline::CheckComponentType(UEPtr<UContainer> comp) const
{
 return true;
}
// --------------------------

// ---------------------
// Методы счета
// ---------------------
// Создание новой копии этого объекта
UBPipeline* UBPipeline::New(void)
{
 return new UBPipeline;
}
// ---------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
// Может быть передан указатель на локальную переменную
bool UBPipeline::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool UBPipeline::ADelComponent(UEPtr<UContainer> comp)
{
 return true;
}
// --------------------------

// ---------------------
// Скрытые методы счета
// ---------------------
// ---------------------




// class UBParallelPipeline
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBParallelPipeline::UBParallelPipeline(void)
{
}

UBParallelPipeline::~UBParallelPipeline(void)
{
}
// --------------------------

// ---------------------
// Методы счета
// ---------------------
// Создание новой копии этого объекта
UBParallelPipeline* UBParallelPipeline::New(void)
{
 return new UBParallelPipeline;
}
// ---------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UBParallelPipeline::ADefault(void)
{
 return true;
}

// Сброс процесса счета.
bool UBParallelPipeline::AReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool UBParallelPipeline::ACalculate(void)
{
 return true;
}
// --------------------------
}

#endif


