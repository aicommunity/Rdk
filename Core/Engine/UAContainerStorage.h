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

#include <map>
#include <string>
#include <vector>
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
protected: // Системные свойства
// Таблица соответствий имен и Id образцов классов
map<string,UId> ClassesLookupTable;

protected: // Основные свойства
// Список объектов
UObjectsStorage ObjectsStorage;

// Последний использованный Id образцов классов
UId LastClassId;

protected: // Временные переменные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAContainerStorage(void);
virtual ~UAContainerStorage(void);
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
// Возвращает Id класса по его имени
const UId& GetClassId(const string &name) const;

// Возвращает имя класса по его Id
const NameT GetClassName(const UId &id) const;
// --------------------------

// --------------------------
// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище
// Возвращает id класса
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const string &classname, const UId &classid=ForbiddenId);

protected:
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const UId &classid=ForbiddenId);
public:
// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
virtual void DelClass(const UId &classid);

// Возвращает список идентификаторов всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
virtual void GetClassIdList(std::vector<UId> &buffer) const;
//virtual void GetClassIdList(UIdVector &buffer) const;

// Возвращает список имен всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
virtual void GetClassNameList(vector<string> &buffer) const;

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
// Методы управления описанием классов
// --------------------------
public:
// Сохраняет общее описание всех классов в xml
virtual bool SaveCommonClassesDescription(Serialize::USerStorageXML &xml);

// Загружает общее описание всех классов из xml
virtual bool LoadCommonClassesDescription(Serialize::USerStorageXML &xml);
// --------------------------

// --------------------------
// Скрытые методы управления описанием классов
// --------------------------
// Формирует прототип XML описания для заданного класса
// Класс в хранилище должен существовать
// Очищает уже созданные поля описания
//virtual bool GenerateClassDescription(UClassesStorageIterator classI,
//										Serialize::USerStorageXML &xml);
// --------------------------

// --------------------------
// Скрытые методы таблицы соответствий классов
// --------------------------
protected:
// Добавляет класс с именем 'name' в таблицу соответствий
virtual UId AddLookupClass(const string &name);

// Удаляет класс с именем 'name' из таблицы соотвествий
virtual void DelLookupClass(const string &name);
// --------------------------

// --------------------------
// Исключения
// --------------------------
public:
// Попытка работы с классом по имени, отсутствующему в хранилище
class EClassNameDontExist;

// Некорректное имя класса
class EInvalidClassName;

// Попытка работы с классом по идентификатору, отсутствующему в хранилище
class EObjectIdDontExist;

// Попытка выполнения разрушающих действий к классом, объекты которого присутствуют в хранилище
class EObjectStorageNotEmpty;

// Класс с заданным именем уже существует
class EClassNameAlreadyExist;
// --------------------------
};

// Попытка работы с классом по имени, отсутствующему в хранилище
class UAContainerStorage::EClassNameDontExist: public EError
{
public: // Данные
// Ошибочный идентификатор
std::string Name;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EClassNameDontExist(const std::string &name);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Некорректное имя класса
class UAContainerStorage::EInvalidClassName: public EError
{
public: // Данные
// Ошибочный идентификатор
std::string Name;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EInvalidClassName(const std::string &name);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Попытка работы с классом по идентификатору, отсутствующему в хранилище
class UAContainerStorage::EObjectIdDontExist: public EError
{
public: // Данные
// Ошибочный идентификатор
UId Id;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EObjectIdDontExist(UId id);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Попытка выполнения разрушающих действий к классом, объекты которого присутствуют в хранилище
class UAContainerStorage::EObjectStorageNotEmpty: public UAStorage::EClassIdDontExist
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EObjectStorageNotEmpty(UId id) : UAStorage::EClassIdDontExist(id) {};
// --------------------------
};

// Класс с заданным именем уже существует
class UAContainerStorage::EClassNameAlreadyExist: public EError
{
public: // Данные
// Ошибочное имя
std::string Name;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EClassNameAlreadyExist(const std::string &name);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};



}

#endif

