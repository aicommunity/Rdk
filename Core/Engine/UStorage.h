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
#include "UEPtr.h"
#include "UContainer.h"
#include "../Serialize/USerStorageXML.h"
#include "UContainerDescription.h"

namespace RDK {

/* *********************************************************************** */
class ULibrary;
class URuntimeLibrary;

typedef UEPtr<UComponent> UClassStorageElement;
typedef std::map<UId, UClassStorageElement> UClassesStorage;
typedef std::map<UId, UClassStorageElement>::iterator UClassesStorageIterator;
typedef std::map<UId, UClassStorageElement>::const_iterator UClassesStorageCIterator;

typedef std::map<std::string, UEPtr<UContainerDescription> > UClassesDescription;
typedef std::map<std::string, UEPtr<UContainerDescription> >::iterator UClassesDescriptionIterator;
typedef std::map<std::string, UEPtr<UContainerDescription> >::const_iterator UClassesDescriptionCIterator;

typedef std::vector<ULibrary*> UClassLibraryList;
/* *********************************************************************** */
// Элемент списка существующих объектов определенного класса
class RDK_LIB_TYPE UInstancesStorageElement
{
public: // Данные
// Указатель на объект
UEPtr<UContainer> Object;

// Признак того свободен ли объект
bool UseFlag;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UInstancesStorageElement(void);
UInstancesStorageElement(const UInstancesStorageElement &copy);
UInstancesStorageElement(const UEPtr<UContainer> &object, bool useflag);
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

class RDK_LIB_TYPE UStorage
{
friend class UContainer;
protected: // Системные свойства
// Таблица соответствий имен и Id образцов классов
std::map<std::string,UId> ClassesLookupTable;

// Хранилище образцов классов
UClassesStorage ClassesStorage;

/// Экземпляр класса для логирования
mutable UEPtr<ULoggerEnv> Logger;

protected: // Описания классов
// XML описания всех классов хранилища
UClassesDescription ClassesDescription;

/// Описания общих свойств компонент
std::map<std::string, UPropertyDescription> CommonDescriptions;

protected: // Описания библиотек
// Массив доступных библиотек
UClassLibraryList CollectionList;

/// Таблица соответствий между именами классов и библиотек
/// имеет вид <имя класса, имя его библиотеки>
//std::map<std::string, std::string> ClassLibraryLookupTable;

// Массив имен загруженных классов
vector<string> CompletedClassNames;

// Массив имен не загруженных классов
vector<string> IncompletedClassNames;

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
UStorage(void);
virtual ~UStorage(void);
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
// Возвращает Id класса по его имени
const UId& FindClassId(const std::string &name) const;

// Возвращает имя класса по его Id
const NameT FindClassName(const UId &id) const;
// --------------------------

// --------------------------
// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище
// Возвращает id класса
// Если classid == ForbiddenId, то id назначается автоматически
virtual UId AddClass(UEPtr<UComponent> classtemplate, const UId &classid=ForbiddenId);
virtual UId AddClass(UEPtr<UComponent> classtemplate, const std::string &classname, const UId &classid=ForbiddenId);

// Удаляет образец класса объекта из хранилища
// Если 'force' == true то принудительно удаляет из хранилища
// все объекты этого класса
virtual void DelClass(const UId &classid, bool force=false);

// Проверяет наличие образца класса объекта в хранилище
virtual bool CheckClass(const UId &classid) const;
virtual bool CheckClass(const string &classname) const;

// Возвращает образец класса
virtual UEPtr<UComponent> GetClass(const UId &classid) const;
virtual UEPtr<UComponent> GetClass(const std::string &class_name) const;

// Возвращает число классов
int GetNumClasses(void) const;

// Возвращает список идентификаторов всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
virtual void GetClassIdList(std::vector<UId> &buffer) const;

// Возвращает список имен всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
virtual void GetClassNameList(std::vector<std::string> &buffer) const;

// Удаляет все не используемые образцы классов из хранилища
virtual void FreeClassesStorage(void);

// Удаляет все образцы классов из хранилища
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
virtual UEPtr<UComponent> TakeObject(const UId &classid, const UEPtr<UComponent> &prototype=0);
virtual UEPtr<UComponent> TakeObject(const string &classname, const UEPtr<UComponent> &prototype=0);

template<class T>
UEPtr<T> TakeObject(const string &classname, const UEPtr<UComponent> &prototype=0);

// Возвращает Id класса, отвечающий объекту 'object'
virtual UId FindClass(UEPtr<UComponent> object) const;

// Проверяет существует ли объект 'object' в хранилище
virtual bool CheckObject(UEPtr<UContainer> object) const;

// Вычисляет суммарное число объектов в хранилище
virtual int CalcNumObjects(void) const;
virtual int CalcNumObjects(const UId &classid) const;
virtual size_t CalcNumObjects(const string &classname) const;

// Удалаяет все свободные объекты из хранилища
virtual void FreeObjectsStorage(void);

// Удаляет все объекты из хранилища
virtual void ClearObjectsStorage(void);

// Удалаяет все объекты заданного класса из хранилища
virtual void ClearObjectsStorageByClass(const UId &classid);
// --------------------------

// --------------------------
// Методы управления описанием классов
// --------------------------
// Возвращает XML описание класса
const UEPtr<UContainerDescription> GetClassDescription(const std::string &classname) const;

// Устанавливает XML описание класса
// Класс в хранилище должен существовать
void SetClassDescription(const std::string &classname, const UEPtr<UContainerDescription>& description);

// Сохраняет описание класса в xml
virtual void SaveClassDescription(const std::string &classname,
										USerStorageXML &xml);

// Загружает описание класса из xml
virtual void LoadClassDescription(const std::string &classname,
										USerStorageXML &xml);

// Сохраняет описание всех классов в xml
virtual void SaveClassesDescription(USerStorageXML &xml);

// Загружает описание всех классов из xml
virtual void LoadClassesDescription(USerStorageXML &xml);

// Сохраняет общее описание всех классов в xml
virtual bool SaveCommonClassesDescription(USerStorageXML &xml);

// Загружает общее описание всех классов из xml
virtual bool LoadCommonClassesDescription(USerStorageXML &xml);
// --------------------------

// --------------------------
// Методы управления библиотеками
// --------------------------
// Указатель на логгер
UEPtr<ULoggerEnv> const GetLogger(void) const;
virtual bool SetLogger(UEPtr<ULoggerEnv> logger);

// Возвращает библиотеку по индексу
UEPtr<ULibrary> GetCollection(int index);

// Возвращает число библиотек
int GetNumCollections(void) const;

// Возвращает библиотеку по имени
UEPtr<ULibrary> GetCollection(const string &name);

// Возвращает имя библиотеки по индексу
const string& GetCollectionName(int index);

// Возвращает версию библиотеки по индексу
const string& GetCollectionVersion(int index);

// Непосредственно добавялет новый образец класса в хранилище
//virtual bool AddClass(UContainer *newclass);

/// Добавялет новый образец класса в коллекцию
virtual bool AddClassToCollection(const std::string &new_class_name, UContainer *newclass, URuntimeLibrary *library);

/// Создает новую библиотеку с заданным именем
virtual bool CreateRuntimeCollection(const std::string &lib_name);

/// Загружает runtime-библиотеку из строки
virtual bool LoadRuntimeCollection(const std::string &buffer, bool force_build=false);

/// Сохраняет runtime-библиотеку в строку
virtual bool SaveRuntimeCollection(const std::string &lib_name, std::string &buffer);
virtual bool SaveRuntimeCollection(URuntimeLibrary *library, std::string &buffer);

// Подключает динамическую библиотеку с набором образцов классов.
// Если бибилиотека с таким именем уже существует то возвращает false.
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
// Если force_build == true то немедленно осущетсвляет развертывание бибилотеки
// в хранилище
virtual bool AddCollection(ULibrary *library, bool force_build=false);

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool DelCollection(int index);

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool DelCollection(const string &name);

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool DelAllCollections(void);

// Заполняет хранилище данными библиотек
virtual bool BuildStorage(void);

/// Удаляет все образцы классов, для которых нет библиотек
/// а также все связанные образцы
virtual void DelAbandonedClasses(void);

/// Возвращает указатель на библиотеку класса по имени класса
virtual UEPtr<ULibrary> FindCollection(const std::string &class_name);
virtual UEPtr<ULibrary> FindCollection(const UId &classid);

/// Формирует список зависимостей класса компонента от библиотек
/// Метод не очищает переданный список библиотек, а только пополняет его
virtual void FindComponentDependencies(const std::string &class_name, std::vector<std::pair<std::string,std::string> > &dependencies);
// --------------------------

// --------------------------
// Скрытые методы управления хранилищем объектов
// Выводит уже созданный объект из хранилища и возвращает
// его classid
// --------------------------
protected:
// Добавляет уже созданный объект в хранилище
// Если объект уже принадлежит иному хранилищу то возвращает false
virtual void PushObject(const UId &classid, UEPtr<UContainer> object);

// Выводит уже созданный объект из хранилища и возвращает
// его classid
// В случае ошибки возвращает ForbiddenId
virtual UId PopObject(UEPtr<UContainer> object);

// Перемещает объект в другое хранилище
virtual void MoveObject(UEPtr<UContainer> object, UEPtr<UStorage> newstorage);

public:
// Возвращает объект в хранилище
// Выбранный объект помечается как свободный в хранилище
// Флаг 'Activity' объекта выставляется в false
virtual void ReturnObject(UEPtr<UComponent> object);

protected:
// В случае ошибки возвращает ForbiddenId
virtual UId PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator);
// --------------------------


// --------------------------
// Скрытые методы таблицы соответствий классов
// --------------------------
protected:
// Добавляет класс с именем 'name' в таблицу соответствий
virtual UId AddLookupClass(const std::string &name);

// Удаляет класс с именем 'name' из таблицы соотвествий
virtual void DelLookupClass(const std::string &name);
// --------------------------


// --------------------------
// Исключения
// --------------------------
public:
class IException: public UException {};

// Попытка работы с классом по идентификатору classid отсутствующим в хранилище
struct EClassIdNotExist: public EIdNotExist
{
explicit EClassIdNotExist(UId id) : EIdNotExist(id) {};
};

// Класс с заданным именем уже существует
struct EClassIdAlreadyExist: public EIdAlreadyExist
{
explicit EClassIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// Попытка работы с классом по имени, отсутствующему в хранилище
struct EClassNameNotExist: public ENameNotExist
{
explicit EClassNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// Класс с заданным именем уже существует
struct EClassNameAlreadyExist: public ENameAlreadyExist
{
explicit EClassNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// Некорректное имя класса
struct EInvalidClassName: public ENameError
{
explicit EInvalidClassName(const std::string &name) : ENameError(name) {};
};

// Попытка работы с классом по идентификатору, отсутствующему в хранилище
struct EObjectIdNotExist: public EIdNotExist
{
explicit EObjectIdNotExist(UId id) : EIdNotExist(id) {};
};

// Попытка выполнения разрушающих действий к классом, объекты которого присутствуют в хранилище
struct EObjectStorageNotEmpty: public EIdError
{
explicit EObjectStorageNotEmpty(UId id) : EIdError(id) {};
};

struct EInvalidClassType: public IException
{
 std::string ClassName;
 std::string ExpectedTypeName;

explicit EInvalidClassType(const std::string &expected_type_name, const std::string &class_name) :
 ClassName(class_name), ExpectedTypeName(expected_type_name) {};

// Формирует строку лога об исключении
std::string CreateLogMessage(void) const
{
 return UException::CreateLogMessage()+std::string(" ClassName=")+ClassName+
  std::string(" ExpectedTypeName=")+ExpectedTypeName;
};
};
// --------------------------

// --------------------------
};

template<class T>
UEPtr<T> UStorage::TakeObject(const string &classname, const UEPtr<UComponent> &prototype)
{
 UEPtr<T> p;
 UEPtr<UComponent> got_class=TakeObject(classname,prototype);
 p=dynamic_pointer_cast<T>(got_class);
 if(!p)
 {
  ReturnObject(got_class);
  throw EInvalidClassType(classname, typeid(T).name());
 }
 return p;
}

	 /*
// Попытка работы с классом по идентификатору classid отсутствующим в хранилище
class UStorage::EClassIdNotExist: public EError
{
public: // Данные
// Ошибочный идентификатор
UId Id;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EClassIdNotExist(UId id);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Попытка работы с классом по имени, отсутствующему в хранилище
class UStorage::EClassNameNotExist: public EError
{
public: // Данные
// Ошибочный идентификатор
std::string Name;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EClassNameNotExist(const std::string &name);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Некорректное имя класса
class UStorage::EInvalidClassName: public EError
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

// Класс с заданным именем уже существует
class UStorage::EClassNameAlreadyExist: public EError
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

                  */
}


#endif

