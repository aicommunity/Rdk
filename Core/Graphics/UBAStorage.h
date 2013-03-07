/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBASTORAGE_H
#define UBASTORAGE_H

#include "UBAbstract.h"
#include "../Engine/UAStorage.h"

namespace RDK {

class UBAStorage: public UAStorage
{
public: // Типы данных

protected: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBAStorage(void);
virtual ~UBAStorage(void);
// --------------------------

// --------------------------
// Методы управления образцами фильров
// --------------------------
// Возвращает заданный фильтр
/*UBAbstract* GetFilter(UId index);

// Добавляет новый фильтр
// Возвращает Id фильтра, или ForbiddenId в случае неудачи
virtual UId AddFilter(const UId &classid, UBAbstract *filter);
virtual UId AddFilter(UBAbstract *filter);

// Удаляет фильтр по индексу
virtual void DelFilter(UId index);

// Удаляет все фильтры
virtual void ClearFilters(void); */
// --------------------------

// --------------------------
// Методы управления экземплярами фильров
// --------------------------
// --------------------------
};

}
#endif


