/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBASTORAGE_CPP
#define UBASTORAGE_CPP

#include "UBAStorage.h"


namespace RDK{


// class UBAStorage
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBAStorage::UBAStorage(void)
{
}

UBAStorage::~UBAStorage(void)
{
// ClearFilters();
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Возвращает заданный фильтр
/*UBAbstract* UBAStorage::GetFilter(UId index)
{
 return static_cast<UBAbstract*>(GetClass(index));
}

// Добавляет новый фильтр
UId UBAStorage::AddFilter(const UId &classid, UBAbstract *filter)
{
 if(!filter)
  return false;

 if(!AddClass(classid, filter))
  return false;

 return filter->GetClass();
}

UId UBAStorage::AddFilter(UBAbstract *filter)
{
 if(!filter)
  return false;

 if(!AddClass(filter))
  return false;

 return filter->GetClass();
}
// Удаляет фильтр по индексу
void UBAStorage::DelFilter(UId index)
{
 DelClass(index);
}

// Удаляет все фильтры
void UBAStorage::ClearFilters(void)
{
 ClearClassesStorage();
}         */
// --------------------------


// --------------------------
// Методы управления экземплярами фильров
// --------------------------
// --------------------------


}

#endif


