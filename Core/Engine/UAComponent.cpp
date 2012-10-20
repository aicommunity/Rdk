/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UACOMPONENT_CPP
#define UACOMPONENT_CPP

#include "UAComponent.h"
#include "UAStorage.h"
#include "UComponentDescription.h"

namespace RDK {

//class UAComponent
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAComponent::UAComponent(void)
{
// Owner=0;

// MainOwner=0;

// Storage=0;

 Class=ForbiddenId;
}

UAComponent::~UAComponent(void)
{
}
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возвращает владелца этого объекта.
UEPtr<UAComponent> const UAComponent::GetOwner(void) const
{
 return Owner;
}

bool UAComponent::SetOwner(UEPtr<UAComponent> owner)
{
 if(Owner == owner)
  return true;

 Owner=owner;
 return true;
}


// Возвращает указатель на главного владельца этим объектом
UEPtr<UAComponent> const UAComponent::GetMainOwner(void) const
{
 return MainOwner;
}

void UAComponent::SetMainOwner(UEPtr<UAComponent> mainowner)
{
 if(mainowner != MainOwner)
  MainOwner=mainowner;
}

// Возвращает хранилище компонент этого объекта
UEPtr<UAStorage> const UAComponent::GetStorage(void) const
{
 return Storage;
}

bool UAComponent::SetStorage(UEPtr<UAStorage> storage)
{
 if(Storage == storage)
  return true;

 Storage=storage;
 return true;
}
// --------------------------


// --------------------------
// Методы управления данными
// --------------------------
// Идентификатор класса
UId UAComponent::GetClass(void) const
{
 return Class;
}

bool UAComponent::SetClass(UId value)
{
 if(Class == value)
  return true;

 Class=value;
 return true;
}
// --------------------------



// --------------------------
// Методы управления счетом
// --------------------------
// Создает экземпляр описания класса
UComponentDescription* UAComponent::NewDescription(void)
{
 UComponentDescription* result=new UComponentDescription;

 return ANewDescription(result);
}

UComponentDescription* UAComponent::ANewDescription(UComponentDescription* description)
{
 return description;
}

// Уничтожение этого объекта
void UAComponent::Free(void)
{
 delete this;
}
// --------------------------

}

#endif


