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

#include "UBAbstract.h"

namespace RDK {

class UBASource: public UBAbstract
{
public: // Общедоступные свойства

protected: // Основные свойства

protected: // Временные переменные

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
// Непосредственно задает выходное изображение
bool SetOutputData(int index, const UBitmap &bitmap);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual UBASource* New(void);
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
virtual bool PLACalculate(UBitmap **input, UBitmap **output, int num_inputs=1, int num_outputs=1);

// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool AFDefault(void);

// Сброс процесса счета.
virtual bool AFReset(void);

// Выполняет расчет этого объекта
//virtual bool ACalculate(void);
// --------------------------
};

}
#endif



