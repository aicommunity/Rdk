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
#include "UEnvSupport.h"
#include "UEPtr.h"

namespace RDK {

class UStorage;
class UComponentDescription;

//typedef long int UId;
//extern UId ForbiddenId;

class UComponent: public UModule
{
friend class UStorage;
public: // Классы описания исключений
class IException: public UException {};

protected: // Основные свойства
// Указатель на владельца этим объектом
UEPtr<UComponent> Owner;

// Указатель на главного владельца этим объектом
// Автоматически устанавливается для всех дочерних объектов
UEPtr<UComponent> MainOwner;

// Указатель на хранилище компонент этого объекта
UEPtr<UStorage> Storage;

protected: // Данные
// Идентификатор класса
UId Class;

// Идентификатор экземпляра объекта
//UId Id;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UComponent(void);
virtual ~UComponent(void);
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возвращает владелца этого объекта
UEPtr<UComponent> const GetOwner(void) const;
virtual bool SetOwner(UEPtr<UComponent> owner);

// Возвращает указатель на главного владельца этим объектом
UEPtr<UComponent> const GetMainOwner(void) const;
virtual void SetMainOwner(UEPtr<UComponent> mainowner);

// Возвращает хранилище компонент этого объекта
UEPtr<UStorage> const GetStorage(void) const;
virtual bool SetStorage(UEPtr<UStorage> storage);
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
virtual UComponent* New(void)=0;

// Создает экземпляр описания класса
virtual UComponentDescription* NewDescription(void);
virtual UComponentDescription* ANewDescription(UComponentDescription* description);

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

