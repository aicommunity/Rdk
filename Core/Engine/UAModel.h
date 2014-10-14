/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UAMODEL_H
#define UAMODEL_H

#include "UNet.h"

namespace RDK {
/*
class UASource: public UASimpleNet
{

};

class UAReceiver: public UASimpleNet
{

};*/

class RDK_LIB_TYPE UAModel: public UNet
{
protected: // Основные свойства

protected: // Указатели на источники и приемники сигналов модели
// Массив источников
UCPointer<UNet, UAModel> Sources;

// Массив приемников
UCPointer<UNet, UAModel> Receivers;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAModel(void);
virtual ~UAModel(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual UAModel* New(void);
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// --------------------------
};


}

#endif



