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
#include "UBASource.h"
#include "UBAReceiver.h"

namespace RDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBAModel::UBAModel(void)
// : Sources(this,"Sources"),
//   Receivers(this,"Receivers")
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

bool UBAModel::PLACalculate(UBitmap **input, UBitmap **output, int num_inputs, int num_outputs)
{
 return true;
}
// --------------------------



}
#endif


