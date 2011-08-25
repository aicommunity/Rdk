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

//#include <map>
//#include <string>
//#include <vector>
#include "UAStorage.h"
#include "UAContainer.h"

namespace RDK {

/* *********************************************************************** */
/* *********************************************************************** */
// Элемент списка существующих объектов определенного класса
class UInstancesStorageElement
{
friend class UInstancesStorage;
public: // Служебные данные
// Указатель на предыдущий элемент списка
UInstancesStorageElement *Prev;

// Указатель на следующий элемент списка
UInstancesStorageElement *Next;

public: // Данные контейнера
// Указатель на объект
UAContainer* Object;

// Признак того свободен ли объект
bool UseFlag;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UInstancesStorageElement(void);
UInstancesStorageElement(const UInstancesStorageElement &copy);
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


// Список существующих объектов определенного класса с флагом использования
// <экземпляр объекта, флаг true если занят или false если свободен>
class UInstancesStorage
{
protected: // Системные данные
// Указатель на первый элемент списка
UInstancesStorageElement* First;

// Указатель на последний элемент списка
UInstancesStorageElement* Last;

// Размер списка
int Size;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UInstancesStorage(void);
UInstancesStorage(const UInstancesStorage& copy);
virtual ~UInstancesStorage(void);
// --------------------------

// --------------------------
// Методы управления списком
// --------------------------
// Очищает список
void Clear(void);

// Возвращает размер списка
int GetSize(void) const;

// Добавляет элемент в список и возвращает указатель на добавленый элемент
// Элементы автоматически сортируются по object
// Метод проверят объект на уникальность и обновляет существующую запись если
// такой объект уже существует
UInstancesStorageElement* Add(UInstancesStorageElement* value);
UInstancesStorageElement* Add(UAContainer* object, bool useflag);

// Удаляет элемент из списка
void Del(UAContainer* object);

// Удаляет элемент из списка только в том случае, если value является элементом списка
void Del(UInstancesStorageElement* value);

// Ищет заданный объект и возвращает указатель на элемент списка с этим объектом
UInstancesStorageElement* Find(const UAContainer* object);
const UInstancesStorageElement* Find(const UAContainer* object) const;

// Ищет "свободный" и возвращает указатель на элемент списка с этим объектом
UInstancesStorageElement* FindFree(void);

// Указатель на первый элемент списка
UInstancesStorageElement* GetFirst(void);
const UInstancesStorageElement* GetFirst(void) const;

// Указатель на последний элемент списка
UInstancesStorageElement* GetLast(void);
const UInstancesStorageElement* GetLast(void) const;
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UInstancesStorage& operator = (const UInstancesStorage &copy);
// --------------------------

};
/* *********************************************************************** */
/* *********************************************************************** */

/* *********************************************************************** */
/* *********************************************************************** */
// Единица хранилища образцов классов
struct UObjectStorageElement
{
UId Id;
UInstancesStorage Objects;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UObjectStorageElement(void);
UObjectStorageElement(const UObjectStorageElement &copy);
virtual ~UObjectStorageElement(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UObjectStorageElement& operator = (const UObjectStorageElement &copy);
// --------------------------
};

// Хранилище объектов разбитое на группы по Id классов
class UObjectsStorage
{
protected: // Data
// Objects list
UObjectStorageElement* Objects;

// Number of classes into storage
int Size;

// real number of allocated elements
int RealSize;

public: // Methods
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UObjectsStorage(void);
UObjectsStorage(const UObjectsStorage &copy);
virtual ~UObjectsStorage(void);
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

// Ищет объект по Id
UInstancesStorage* Find(const UId &id);
const UInstancesStorage* Find(const UId &id) const;

// Ищет класс по Id и удаляет его из массива
void Erase(const UId &id);

// Добавляет новый элемент в конец хранилища
UObjectStorageElement* PushBack(const UObjectStorageElement &classelement);
UObjectStorageElement* PushBack(const UId &id, UInstancesStorage *instance);

// Создает новый элемент в хранилище с заданным id и возвращает указатель на него
// Если такой элемент уже существует то возвращает указатель на этот элемент
UInstancesStorage* Create(const UId &id);

// Возвращает указатель на массив классов
UObjectStorageElement* GetObjects(void) const;
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UObjectsStorage& operator = (const UObjectsStorage &copy);

// Оператор доступа
UObjectStorageElement& operator [] (int i);
const UObjectStorageElement& operator [] (int i) const;
// --------------------------
};
/* *********************************************************************** */
/* *********************************************************************** */


/* *********************************************************************** */
/* *********************************************************************** */
class UAContainerStorage: public UAStorage
{
protected: // Системные свойства
// Таблица соответствий имен и Id образцов классов
map<string,UId> ClassesLookupTable;

protected: // Основные свойства
// Список доступных классов объектов
//ClassesStorageT ClassesStorage;

// Список объектов
UObjectsStorage ObjectsStorage;

// Последний использованный Id образцов классов
UId LastClassId;

protected: // Временные переменные
// Таблица итераторов на свободные объекты классов
//UFreeObjectsTable FreeObjectsTable;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAContainerStorage(void);
virtual ~UAContainerStorage(void);
// --------------------------

// --------------------------
// Методы управления свойствами
// --------------------------
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
// Добавляет образец класса объекта в хранилище с автоматическим назначением id
// Возвращает id класса
//virtual UId AddClass(UAComponent *classtemplate);

// Добавляет образец класса объекта в хранилище
// Возвращает id класса
virtual UId AddClass(UAComponent *classtemplate, const string &classname, const UId &classid=ForbiddenId);

// Возвращает id класса
//virtual UId AddClass(const string &classname, UAComponent *classtemplate);

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
virtual bool DelClass(const UId &classid);

// Возвращает список идентификаторов всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
virtual void GetClassIdList(UId* buffer, int max_num_classes) const;
virtual void GetClassIdList(UIdVector &buffer) const;

// Возвращает список имен всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
virtual void GetClassNameList(vector<string> &buffer) const;

// Удаляет все не используемые образцы классов из хранилища
virtual void FreeClassesStorage(void);

// Удаляет все образцы классов из хранилища
// Возвращает false если в хранилище присутствуют объекты
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
virtual UAContainer* TakeObject(const UId &classid, const UAComponent *prototype=0);
virtual UAComponent* TakeObject(const string &classname, const UAComponent *prototype=0);

// Возвращает объект в хранилище
// Выбранный объект помечается как свободный в хранилище
// Флаг 'Activity' объекта выставляется в false
// Если объект не существует в хранилище - возвращается false
virtual bool ReturnObject(UAComponent *object);

// Добавляет уже созданный объект в хранилище
// Если объект уже принадлежит иному хранилищу то возвращает false
virtual bool PushObject(const UId &classid, UAContainer *object);

// Выводит уже созданный объект из хранилища и возвращает
// его classid
// В случае ошибки возвращает ForbiddenId
virtual UId PopObject(UAContainer *object);

// Перемещает объект в другое хранилище
virtual bool MoveObject(UAContainer *object, UAContainerStorage *newstorage);

// Проверяет существует ли объект 'object' в хранилище
virtual bool CheckObject(const UAContainer *object) const;

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
// Скрытые методы таблицы соответствий классов
// --------------------------
protected:
// Добавляет класс с именем 'name' в таблицу соответствий
virtual UId AddLookupClass(const string &name);

// Удаляет класс с именем 'name' из таблицы соотвествий
virtual bool DelLookupClass(const string &name);
// --------------------------
};

}

#endif

