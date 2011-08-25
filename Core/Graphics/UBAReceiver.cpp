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
//: NADItem(name)//NAConnector(name)
{
}

UBAReceiver::~UBAReceiver(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
UBAReceiver* UBAReceiver::New(void)
{
 return new UBAReceiver;
}
// --------------------------

bool UBAReceiver::PLACalculate(UBitmap **input, UBitmap **output, int num_inputs, int num_outputs)
{
 int min=(num_inputs<num_outputs)?num_inputs:num_outputs;
 for(int i=0;i<min;i++)
 {
  *output[i]=*input[i];
 }

 return true;
}


}
#endif


