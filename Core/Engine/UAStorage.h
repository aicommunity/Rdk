/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UASTORAGE_H
#define UASTORAGE_H

#include <map>
#include "../Utilities/UPtr.h"
#include "UAComponent.h"

namespace RDK {

typedef USharedPtr<UAComponent> UClassStorageElement;
typedef std::map<UId, UClassStorageElement> UClassesStorage;
typedef std::map<UId, UClassStorageElement>::iterator UClassesStorageIterator;
typedef std::map<UId, UClassStorageElement>::const_iterator UClassesStorageCIterator;
/*
// Единица хранилища образцов классов
class UClassStorageElement: public USharedPtr<UAComponent>
{
public:
// Id класса
UId Id;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UClassStorageElement(void);
UClassStorageElement(UAComponent *comp, UId id);
// --------------------------
};

// Хранилище образцов классов
class UClassesStorage
{
protected: // Data
// Classes list
UClassStorageElement* Classes;

// Number of classes into storage
int Size;

// real number of allocated elements
int RealSize;

public: // Methods
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UClassesStorage(void);
UClassesStorage(const UClassesStorage &copy);
virtual ~UClassesStorage(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает хранилище
void Clear(void);

// Изменяет размер хранилища
// Метод сохраняет старые данные
void Resize(int newsize);

// Возвращает размер массива
int GetSize(void) const;

// Ищет класс по Id
USharedPtr<UAComponent> Find(const UId &id) const;

// Ищет класс по Id и удаляет его из массива
// Возвращает указатель на удаленный класс
USharedPtr<UAComponent> Erase(const UId &id);

// Добавляет новый элемент в конец хранилища
void PushBack(const UClassStorageElement &classelement);
void PushBack(const UId &id, UAComponent *component);

// Возвращает указатель на массив классов
UClassStorageElement* GetClasses(void) const;
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UClassesStorage& operator = (const UClassesStorage &copy);

// Оператор доступа
UClassStorageElement& operator [] (int i);
// --------------------------
};    */

class UAStorage
{
protected: // Системные свойства
UClassesStorage ClassesStorage;

protected: // Основные свойства
// Последний использованный Id образцов классов
UId LastClassId;

protected: // Временные переменные


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAStorage(void);
virtual ~UAStorage(void);
// --------------------------

// --------------------------
// Методы управления свойствами
// --------------------------
// Возвращает последний использованный Id классов
UId GetLastClassId(void) const;
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
// --------------------------

// --------------------------
// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище
// Возвращает id класса
virtual UId AddClass(UAComponent *classtemplate, const UId &classid=ForbiddenId);

// Добавляет образец класса объекта в хранилище с автоматическим назначением id
// Возвращает id класса
//virtual UId AddClass(UAComponent *classtemplate);

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
virtual bool DelClass(const UId &classid);

// Проверяет наличие образца класса объекта в хранилище
virtual bool CheckClass(const UId &classid) const;

// Возвращает образец класса
virtual USharedPtr<UAComponent> GetClass(const UId &classid) const;

// Возвращает число классов
int GetNumClasses(void) const;

// Возвращает список идентификаторов всех классов хранилища
// max_num_classes - максимальная длина массива
virtual void GetClassIdList(UId* buffer, int max_num_classes) const;

// Удаляет все образцы классов из хранилища
virtual bool ClearClassesStorage(void);
// --------------------------

// --------------------------
// Методы управления хранилищем объектов
// --------------------------
// Извлекает объект из хранилища
// Возвращает указатель на свободный объект по имени класса
// Выбранный объект помечается как занятый в хранилище
// Флаг 'Activity' объекта выставляется в true
// Если свободного объекта не существует он создается и добавляется
// в хранилище
virtual UAComponent* TakeObject(const UId &classid, const UAComponent *prototype=0);

// Возвращает объект в хранилище
// В текущей реализации всегда удаляет объект и возвращает true
virtual bool ReturnObject(UAComponent *object);

// Возвращает Id класса, отвечающий объекту 'object'
virtual UId FindClass(const UAComponent *object) const;
// --------------------------

// --------------------------
// Скрытые методы таблицы соответствий классов
// --------------------------
// --------------------------
};

}


#endif

