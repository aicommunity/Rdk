/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UACONTAINER_STORAGE_H
#define UACONTAINER_STORAGE_H

#include "UAStorage.h"
#include "UAContainer.h"

namespace RDK {
/* *********************************************************************** */
/* *********************************************************************** */
// Элемент списка существующих объектов определенного класса
class UInstancesStorageElement
{
public: // Данные
// Указатель на объект
UEPtr<UAContainer> Object;

// Признак того свободен ли объект
bool UseFlag;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UInstancesStorageElement(void);
UInstancesStorageElement(const UInstancesStorageElement &copy);
UInstancesStorageElement(const UEPtr<UAContainer> &object, bool useflag);
virtual ~UInstancesStorageElement(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UInstancesStorageElement& operator = (const UInstancesStorageElement &copy);

// Операторы сравнения
bool operator < (const UInstancesStorageElement &value);
bool operator > (const UInstancesStorageElement &value);
bool operator <= (const UInstancesStorageElement &value);
bool operator >= (const UInstancesStorageElement &value);
bool operator == (const UInstancesStorageElement &value);
bool operator != (const UInstancesStorageElement &value);
// --------------------------
};

typedef UInstancesStorageElement* PUInstancesStorageElement;
typedef list<UInstancesStorageElement> UInstancesStorage;
typedef list<UInstancesStorageElement>::iterator UInstancesStorageIterator;

typedef pair<UId,UInstancesStorage> UObjectStorageElement;

typedef map<UId, UInstancesStorage> UObjectsStorage;
typedef map<UId, UInstancesStorage>::iterator UObjectsStorageIterator;
typedef map<UId, UInstancesStorage>::const_iterator UObjectsStorageCIterator;

/* *********************************************************************** */
/* *********************************************************************** */
class UAContainerStorage: public UAStorage
{
friend class UAContainer;
protected: // Основные свойства
// Список объектов
UObjectsStorage ObjectsStorage;

protected: // Временные переменные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAContainerStorage(void);
virtual ~UAContainerStorage(void);
// --------------------------

// --------------------------
// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище
// Возвращает id класса
//protected:
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const UId &classid=ForbiddenId);
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const string &classname, const UId &classid=ForbiddenId);
//public:
// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
virtual void DelClass(const UId &classid);

// Удаляет все не используемые образцы классов из хранилища
virtual void FreeClassesStorage(void);

// Удаляет все образцы классов из хранилища
// Возвращает false если в хранилище присутствуют объекты
virtual void ClearClassesStorage(void);
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
virtual UEPtr<UAComponent> TakeObject(const UId &classid, const UEPtr<UAComponent> prototype=0);
virtual UEPtr<UAComponent> TakeObject(const string &classname, const UEPtr<UAComponent> prototype=0);

// Проверяет существует ли объект 'object' в хранилище
virtual bool CheckObject(UEPtr<UAContainer> object) const;

// Вычисляет суммарное число объектов в хранилище
virtual int CalcNumObjects(void) const;
virtual int CalcNumObjects(const UId &classid) const;
virtual size_t CalcNumObjects(const string &classname) const;

// Удалаяет все свободные объекты из хранилища
virtual void FreeObjectsStorage(void);

// Удаляет все объекты из хранилища
virtual void ClearObjectsStorage(void);
// --------------------------

// --------------------------
// Скрытые методы управления хранилищем объектов
// Выводит уже созданный объект из хранилища и возвращает
// его classid
// --------------------------
protected:
// Добавляет уже созданный объект в хранилище
// Если объект уже принадлежит иному хранилищу то возвращает false
virtual void PushObject(const UId &classid, UEPtr<UAContainer> object);

// Выводит уже созданный объект из хранилища и возвращает
// его classid
// В случае ошибки возвращает ForbiddenId
virtual UId PopObject(UEPtr<UAContainer> object);

// Перемещает объект в другое хранилище
virtual void MoveObject(UEPtr<UAContainer> object, UEPtr<UAContainerStorage> newstorage);

// Возвращает объект в хранилище
// Выбранный объект помечается как свободный в хранилище
// Флаг 'Activity' объекта выставляется в false
// Если объект не существует в хранилище - возвращается false
virtual void ReturnObject(UEPtr<UAComponent> object);

// В случае ошибки возвращает ForbiddenId
virtual UId PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator);
// --------------------------

// --------------------------
// Исключения
// --------------------------
public: // Классы описания исключений
class IException: public UException {};

public:
// Попытка работы с классом по идентификатору, отсутствующему в хранилище
struct EObjectIdNotExist: public EIdNotExist
{
EObjectIdNotExist(UId id) : EIdNotExist(id) {};
};

// Попытка выполнения разрушающих действий к классом, объекты которого присутствуют в хранилище
struct EObjectStorageNotEmpty: public EIdError
{
EObjectStorageNotEmpty(UId id) : EIdError(id) {};
};
// --------------------------
};
     /*
// Попытка работы с классом по идентификатору, отсутствующему в хранилище
class UAContainerStorage::EObjectIdNotExist: public EError
{
public: // Данные
// Ошибочный идентификатор
UId Id;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EObjectIdNotExist(UId id);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};         */
				  /*
// Попытка выполнения разрушающих действий к классом, объекты которого присутствуют в хранилище
class UAContainerStorage::EObjectStorageNotEmpty: public UAStorage::EClassIdNotExist
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EObjectStorageNotEmpty(UId id) : UAStorage::EClassIdNotExist(id) {};
// --------------------------
};
              */

}

#endif

