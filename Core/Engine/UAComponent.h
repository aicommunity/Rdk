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
#include "UEPtr.h"

namespace RDK {

class UAStorage;
class UComponentDescription;

//typedef long int UId;
//extern UId ForbiddenId;

class UAComponent: public UModule
{
friend class UAStorage;
public: // Классы описания исключений
class IException: public UException {};

protected: // Основные свойства
// Указатель на владельца этим объектом
UEPtr<UAComponent> Owner;

// Указатель на главного владельца этим объектом
// Автоматически устанавливается для всех дочерних объектов
UEPtr<UAComponent> MainOwner;

// Указатель на хранилище компонент этого объекта
UEPtr<UAStorage> Storage;

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
UEPtr<UAComponent> const GetOwner(void) const;
virtual bool SetOwner(UEPtr<UAComponent> owner);

// Возвращает указатель на главного владельца этим объектом
UEPtr<UAComponent> const GetMainOwner(void) const;
virtual void SetMainOwner(UEPtr<UAComponent> mainowner);

// Возвращает хранилище компонент этого объекта
UEPtr<UAStorage> const GetStorage(void) const;
virtual bool SetStorage(UEPtr<UAStorage> storage);
// --------------------------


// --------------------------
// Методы управления данными
// --------------------------
// Идентификатор класса
UId GetClass(void) const;
bool SetClass(UId value);
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Создает экземпляр этого класса
virtual UAComponent* New(void)=0;

// Создает экземпляр описания класса
virtual UComponentDescription* NewDescription(void);

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

