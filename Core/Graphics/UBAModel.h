/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBAMODEL_H
#define UBAMODEL_H

#include "UBAbstract.h"

namespace RDK {

class UBAModel: public UBAbstract
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBAModel(void);
virtual ~UBAModel(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual UBAModel* New(void);
// --------------------------
};

}

#endif



