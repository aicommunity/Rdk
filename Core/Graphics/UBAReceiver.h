/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UBARECEIVER_H
#define UBARECEIVER_H

#include "UBAbstract.h"

namespace RDK {

/// Простой класс-приемник изображений
/// копирует входные изображения в выходные
class UBAReceiver: public UBAbstract
{
protected: // Основные свойства

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBAReceiver(void);
virtual ~UBAReceiver(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
/// Выделяет память для новой чистой копии объекта этого класса
virtual UBAReceiver* New(void);
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
/// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool AFDefault(void);

/// Сброс процесса счета.
virtual bool AFReset(void);

/// Выполняет расчет этого объекта
virtual bool AFCalculate(void);
// --------------------------
};

}

#endif



