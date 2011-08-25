/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UAMODEL_CPP
#define UAMODEL_CPP

#include "UAModel.h"

namespace RDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAModel::UAModel(void)
 : Sources(this,"Sources"),
   Receivers(this,"Receivers")
{
}

UAModel::~UAModel(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
UAModel* UAModel::New(void)
{
 return new UAModel;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// --------------------------


}
#endif


