/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBAMODEL_CPP
#define UBAMODEL_CPP

#include "UBAModel.h"

namespace RDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBAModel::UBAModel(void)
 : Output("Output",this),
   WorkWidth("WorkWidth",this),
   WorkHeight("WorkHeight",this)
{
}

UBAModel::~UBAModel(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
UBAModel* UBAModel::New(void)
{
 return new UBAModel;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UBAModel::ADefault(void)
{
 return AMDefault();
}

bool UBAModel::AMDefault(void)
{
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UBAModel::ABuild(void)
{
 return AMBuild();
}

bool UBAModel::AMBuild(void)
{
 return true;
}

// Сброс процесса счета.
bool UBAModel::AReset(void)
{
 *WorkWidth=Output->GetWidth();
 *WorkHeight=Output->GetHeight();
 return AMReset();
}

bool UBAModel::AMReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool UBAModel::ACalculate(void)
{
 *WorkWidth=Output->GetWidth();
 *WorkHeight=Output->GetHeight();
 return AMCalculate();
}

bool UBAModel::AMCalculate(void)
{
 return true;
}
// --------------------------


}
#endif


