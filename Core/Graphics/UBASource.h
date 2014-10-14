/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBASOURCE_H
#define UBASOURCE_H

#include "../Engine/UNet.h"
#include "UBitmap.h"

namespace RDK {

/// Простой класс-источник изображений
class RDK_LIB_TYPE UBASource: public UNet
{
protected:
UPropertyOutputData<UBitmap, UBASource> Output;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBASource(void);
virtual ~UBASource(void);
// --------------------------

protected:
// --------------------------
// Методы управления данными
// --------------------------
/// Непосредственно задает выходное изображение
bool SetOutputData(int index, const UBitmap &bitmap);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
/// Выделяет память для новой чистой копии объекта этого класса
virtual UBASource* New(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
/// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

/// Сброс процесса счета.
virtual bool AReset(void);

/// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

}
#endif



