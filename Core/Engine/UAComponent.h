/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UACOMPONENT_H
#define UACOMPONENT_H

#include "UModule.h"
#include "UAEnvSupport.h"

namespace RDK {

class UAStorage;

//typedef long int UId;
//extern UId ForbiddenId;

class UAComponent: public UModule
{
friend UAStorage;
protected: // Основные свойства
// Указатель на владельца этим объектом
UAComponent *Owner;

// Указатель на главного владельца этим объектом
// Автоматически устанавливается для всех дочерних объектов
UAComponent *MainOwner;

// Указатель на хранилище компонент этого объекта
UAStorage *Storage;

// Последний использованный Id компонент
UId LastId;

protected: // Данные
// Идентификатор класса
UId Class;

// Идентификатор экземпляра объекта
//UId Id;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAComponent(void);
virtual ~UAComponent(void);
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возвращает владелца этого объекта
UAComponent* const GetOwner(void) const;
virtual bool SetOwner(UAComponent* owner);

// Возвращает указатель на главного владельца этим объектом
UAComponent* const GetMainOwner(void) const;
virtual bool SetMainOwner(UAComponent* const mainowner);

// Возвращает хранилище компонент этого объекта
UAStorage* const GetStorage(void) const;
virtual bool SetStorage(UAStorage* const storage);

// Возвращает последний использованный Id компонент
UId GetLastId(void) const;
// --------------------------


// --------------------------
// Методы управления данными
// --------------------------
// Идентификатор класса
UId GetClass(void) const;
bool SetClass(UId value);

// Идентификатор экземпляра
/*UId GetId(void) const;
bool SetId(UId value);          */
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Создает экземпляр этого класса
virtual UAComponent* New(void)=0;

// Уничтожение этого объекта
void Free(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// --------------------------
};

}

#endif

