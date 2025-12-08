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
#include <unordered_map>
#include "UEPtr.h"
#include "UContainer.h"
#include "../Serialize/USerStorageXML.h"
#include "UContainerDescription.h"

#include "UComponentFactory.h"


namespace RDK {

/* *********************************************************************** */
class ULibrary;
class URuntimeLibrary;

typedef UEPtr<UComponentAbstractFactory> UClassStorageElement;
typedef std::unordered_map<UId, UClassStorageElement> UClassesStorage;
typedef std::unordered_map<UId, UClassStorageElement>::iterator UClassesStorageIterator;
typedef std::unordered_map<UId, UClassStorageElement>::const_iterator UClassesStorageCIterator;

typedef std::unordered_map<std::string, UEPtr<UContainerDescription> > UClassesDescription;
typedef std::unordered_map<std::string, UEPtr<UContainerDescription> >::iterator UClassesDescriptionIterator;
typedef std::unordered_map<std::string, UEPtr<UContainerDescription> >::const_iterator UClassesDescriptionCIterator;

typedef std::vector<ULibrary*> UClassLibraryList;
// Элемент списка существующих объектов определенного класса
// Элемент списка существующих объектов определенного класса
class RDK_LIB_TYPE UInstancesStorageElement
{
public: // Указатель на объект
// Указатель на объект
UEPtr<UContainer> Object;

// Флаг использования объекта из объекта
bool UseFlag;


public: // --------------------------
// Конструкторы и деструкторы
// --------------------------
// --------------------------
UInstancesStorageElement(void);
UInstancesStorageElement(const UInstancesStorageElement &copy);
UInstancesStorageElement(const UEPtr<UContainer> &object, bool useflag);
virtual ~UInstancesStorageElement(void);
// --------------------------

// Операторы
// --------------------------
// Оператор присваивания
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

typedef std::unordered_map<UId, UInstancesStorage> UObjectsStorage;
typedef std::unordered_map<UId, UInstancesStorage>::iterator UObjectsStorageIterator;
typedef std::unordered_map<UId, UInstancesStorage>::const_iterator UObjectsStorageCIterator;

class UMockUNet;
// согласно описанию USerStorageXML для UMockUNet
// Функция создания USerStorageXML для UMockUNet
typedef bool (*funcCrPropMock)(USerStorageXML*, UMockUNet*);

/* *********************************************************************** */


class RDK_LIB_TYPE UStorage
{
// Системные свойства
protected: // Таблица соответствий имен и Id образцов классов
// Таблица соответствий имен и Id классов класса
std::unordered_map<std::string,UId> ClassesLookupTable;

// Указатель хранилища классов класса
UClassesStorage ClassesStorage;

/// Указатель логгера для логирования
mutable UEPtr<UExceptionLogger> Logger;

protected: // XML описания всех классов хранилища
// XML описания всех классов хранилища
UClassesDescription ClassesDescription;

/// Описание всех классов хранилища
std::unordered_map<std::string, UPropertyDescription> CommonDescriptions;

protected: // Массив доступных библиотек
// Список доступных библиотек
UClassLibraryList CollectionList;


/// имеет вид <имя класса, имя его библиотеки>
//std::map<std::string, std::string> ClassLibraryLookupTable;
//std::map<std::string, std::string> ClassLibraryLookupTable;

// Список всех инициализированных классов
vector<string> CompletedClassNames;

// Список всех не инициализированных классов
vector<string> IncompletedClassNames;

protected: // Список объектов
// Список объектов
UObjectsStorage ObjectsStorage;

// Index map for O(log n) object lookup by UEPtr<UContainer>
// Maps object pointer to iterator in the corresponding UInstancesStorage list
// Using std::map because UEPtr doesn't have hash function for std::unordered_map
std::map<UEPtr<UContainer>, UInstancesStorageIterator> ObjectsIndex;

// Возвращает последний использованный Id классов класса
UId LastClassId;

// 1 -  обычная сборка. Сборка статических библиотек, затем динамических. Все компоненты рабочие (не заглушки)
// 2 -  поочередная сборка. Сборка статических, библиотек-заглушек, затем динамических.
//      Все компоненты, которые смогли собраться рабочими - рабочие.
//      Остальные (которые есть в библиотеках-заглушка, но нет сейчас в хранилище) - заглушки.
// 3 -  сборка только заглушек. Сборка библиотек-заглушек, затем динамических. Все компоненты заглушки (не рабочие)
// 3 -  сборка только заглушек. Сборка библиотек-заглушек, затем динамических. Все компоненты заглушки (не рабочие)
int BuildMode;

// Список функций-создателей Property для MockUNet
std::list<funcCrPropMock> FunctionsCrPropMock;

// Путь к директории библиотек (в данном случае путь к папке для всех MockLibs RTlibs)
std::string LibrariesPath;

// Путь к файлу с описанием классов (в данном случае путь к файлу вида: компоненты\язык (en, ru и т.д.)\компоненты.xml)
std::string ClDesc;

protected: // Указатель компонента


public: // Указатель

// Конструкторы и деструкторы
// --------------------------
// --------------------------
UStorage(void);
virtual ~UStorage(void);
// --------------------------

// Методы управления свойствами
// --------------------------
// Возвращает последний использованный Id классов
// Возвращает последний использованный Id класса
UId GetLastClassId(void) const;
// --------------------------

// Методы доступа к таблицам соотвествий
// --------------------------
// Возвращает Id класса по его имени
// Возвращает Id класса по его имени
const UId& FindClassId(const std::string &name) const;

// Возвращает имя класса по его Id
const NameT FindClassName(const UId &id) const;
// --------------------------

// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище
// Возвращает id класса
// Если classid == ForbiddenId, то id назначается автоматически
// Если classid == ForbiddenId, то id назначается автоматически
virtual UId AddClass(UEPtr<UComponentAbstractFactory> factory, const UId &classid=ForbiddenId);
virtual UId AddClass(UEPtr<UComponentAbstractFactory> factory, const std::string &classname, const UId &classid=ForbiddenId);

// Если 'force' == true то принудительно удаляет из хранилища
// все объекты этого класса
// для удаления всех объектов
virtual void DelClass(const UId &classid, bool force=false);

// Проверяет наличие класса класса класса в хранилище
virtual bool CheckClass(const UId &classid) const;
virtual bool CheckClass(const string &classname) const;

// Возвращает фабрику класса
virtual UEPtr<UComponentAbstractFactory> GetComponentFactory(const UId &classid) const;
virtual UEPtr<UComponentAbstractFactory> GetComponentFactory(const std::string &class_name) const;

// Возвращает число классов
int GetNumClasses(void) const;

// Буфер 'buffer' будет очищен от предыдущих значений
// Буфер 'buffer' будет очищен от предыдущих значений
virtual void GetClassIdList(std::vector<UId> &buffer) const;

// Буфер 'buffer' будет очищен от предыдущих значений
// Буфер 'buffer' будет очищен от предыдущих значений
virtual void GetClassNameList(std::vector<std::string> &buffer) const;

/// Если force == true то удаляет даже если хранилище объектов не пустое
/// и подавляет соответствующее исключение
/// и подавляет соответствующее исключение
virtual void FreeClassesStorage(bool force=false);

/// Если force == true то удаляет даже если хранилище объектов не пустое
/// и подавляет соответствующее исключение
/// и подавляет соответствующее исключение
virtual void ClearClassesStorage(bool force=false);
// --------------------------

// Методы управления хранилищем объектов
// --------------------------
// Извлекает объект из хранилища
// Возвращает указатель на свободный объект по имени класса
// Выбранный объект помечается как занятый в хранилище
// Флаг 'Activity' объекта выставляется в true
// Если свободного объекта не существует он создается и добавляется
// в хранилище
// в хранилище
virtual UEPtr<UComponent> TakeObject(const UId &classid, const UEPtr<UComponent> &prototype=0);
virtual UEPtr<UComponent> TakeObject(const string &classname, const UEPtr<UComponent> &prototype=0);

template<class T>
UEPtr<T> TakeObject(const UId &classid, const UEPtr<UComponent> &prototype=0);

template<class T>
UEPtr<T> TakeObject(const string &classname, const UEPtr<UComponent> &prototype=0);

// Возвращает Id класса, соответствующего объекту 'object'
virtual UId FindClass(UEPtr<UComponent> object) const;

// Проверяет наличие объекта 'object' в хранилище
virtual bool CheckObject(UEPtr<UContainer> object) const;

// Если фабрика, соответствующая виртуальным методам объекта объекта
virtual UVirtualMethodFactory* FindVirualMethodFactory(UEPtr<UContainer> object);

// Вычисляет текущее число объектов в хранилище
virtual int CalcNumObjects(void) const;
virtual int CalcNumObjects(const UId &classid) const;
virtual size_t CalcNumObjects(const string &classname) const;

/// Если force == true то удаляет даже если объекты используются
/// Если force == true то удаляет даже если объекты используются
virtual void FreeObjectsStorage(bool force=false);

// Освобождает память объектов класса класса из хранилища
virtual void FreeObjectsStorageByClass(const UId &classid);

/// Если force == true то удаляет даже если объекты используются
/// Если force == true то удаляет даже если объекты используются
virtual void ClearObjectsStorage(bool force=false);

// Очищает все объекты класса класса из хранилища
virtual void ClearObjectsStorageByClass(const UId &classid);

/// Устанавливает значения по умолчанию для свойств компонента в хранилище по умолчанию
virtual void DefaultObject(UEPtr<UContainer> object);
// --------------------------

// Методы управления описанием классов
// --------------------------
// Установка пути к папке с описаниями классов
// Устанавливает путь к файлу с описанием классов
void SetClDescPath(const std::string& value);

// Возвращает путь к файлу с описанием классов
const std::string GetClDescPath() const;

// Также создаёт необходимые папки
// Возвращает путь к создаваемому файлу
const std::string GetCreateClDescPath(const std::string& class_name);

// Возвращает XML описание класса
const UEPtr<UContainerDescription> GetClassDescription(const std::string &classname, bool no_throw=false) const;

// Класс в хранилище должен существовать
// Класс в хранилище должен существовать
void SetClassDescription(const std::string &classname, const UEPtr<UContainerDescription>& description);

// Загружает описание классов из xml-файла
virtual void LoadClassesDescription();

// Сохраняет описание класса в файл из файла
virtual void SaveClassDescriptionToFile(const std::string &classname);

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

// Сохраняет строку описания всех классов в xml
virtual bool SaveCommonClassesDescription(USerStorageXML &xml);

// Загружает строку описания всех классов из xml
virtual bool LoadCommonClassesDescription(USerStorageXML &xml);
// --------------------------

// Методы управления библиотеками
// --------------------------
// Указатель на логгер
// Указатель на логгер
UEPtr<UExceptionLogger> const GetLogger(void) const;
virtual bool SetLogger(UEPtr<UExceptionLogger> logger);

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

// Нужно перед сборкой
// Очищает все массивы
void ClearAllLibsClassesNameArrays(void);

// Буфер 'buffer' будет очищен от предыдущих значений
// Буфер 'buffer' будет очищен от предыдущих значений
void GetLibsNameListByType(std::string &buffer, int type) const;

//virtual bool AddClass(UContainer *newclass);
//virtual bool AddClass(UContainer *newclass);

// Устанавливает путь к директории библиотек
void SetLibrariesPath(const std::string& value);

// Возвращает путь к директории библиотек
const std::string GetLibrariesPath() const;

/// Инициализация существующих динамических библиотек
/// Вызывается в Engine один раз. Добавляет библиотеки в CollectionList (сборки компонентов нет)
/// Вызывается в Engine один раз. Добавляет библиотеки в CollectionList (сборки компонентов нет)
void InitRTlibs(void);

/// Загружает runtime-коллекцию из файла (для текущего компонента)
virtual bool LoadRuntimeCollection(const std::string &lib_name);

/// Если класс с таким именем существует возможно перезапись при force_replace = true
/// Если класс с таким именем существует возможно перезапись при force_replace = true
virtual bool AddClassToCollection(const std::string &new_class_name, const std::string &new_comp_name, bool force_replace, UContainer *newclass, const std::string &lib_name);

/// Удаляет описание класса из RT коллекции
virtual bool DelClassFromCollection(const std::string &class_name, const std::string &lib_name);

/// Создает новую коллекцию в текущей коллекции
virtual bool CreateRuntimeCollection(const std::string &lib_name);

/// Удаляет runtime-коллекцию класса из коллекции
bool DeleteRuntimeCollection(const std::string &lib_name);

// Если бибилиотека с таким именем уже существует то возвращает false.
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
// Если force_build == true то немедленно осущетсвляет развертывание бибилотеки
// в хранилище
// в хранилище
virtual bool AddCollection(ULibrary *library, bool force_build=false);

// Ответственность за освобождение памяти лежит на вызывающей стороне.
// Освобождает выделенную память для всех компонентов из библиотеки.
virtual bool DelCollection(int index);

// В правильном порядке (порядок опредлен в конкретном файле)
// В правильном порядке (порядок определен в конкретном файле)
bool InitMockLibs(void);

// из статических библиотек
// из статических библиотек
bool CreateMockLibs(void);

// Также сохраняет порядок библиотек в виде отдельного файла
// Также сохраняет порядок библиотек в виде отдельного файла
bool SaveMockLibs(void);

// Ответственность за освобождение памяти лежит на вызывающей стороне.
// Освобождает выделенную память для всех компонентов из библиотеки.
bool DelCollection(const string &name);

// Ответственность за освобождение памяти лежит на вызывающей стороне.
// Освобождает выделенную память для всех компонентов из библиотеки.
virtual bool DelAllCollections(void);

// Устанавливает текущий режим сборки модели
void SetBuildMode(int mode);

// Возвращает текущий режим сборки модели
int GetBuildMode();

// Выполняет построение структуры хранилища
virtual bool BuildStorage(void);

// Тип библиотеки:
// 0 - Внутренняя библиотека (собрана вместе с ядром)
// 1 - Внешняя библиотека (загружена из внешней dll)
// 2 - Библиотека, созданная во время выполнения
// 3 - Библиотека-заглушка (все компоненты-заглушки)
// 3 - Библиотека-заглушка (все компоненты-заглушки)
virtual bool BuildStorage(int lib_type);

/// а также все связанные образцы
/// а также все связанные образцы
virtual void DelAbandonedClasses(void);

/// Возвращает указатель на библиотеку класса по имени класса
virtual UEPtr<ULibrary> FindCollection(const std::string &class_name);
virtual UEPtr<ULibrary> FindCollection(const UId &classid);

/// Метод не очищает переданный список библиотек, а только пополняет его
/// Метод не очищает переданный список библиотек, а только пополняет его
virtual void FindComponentDependencies(const std::string &class_name, std::vector<std::pair<std::string,std::string> > &dependencies);
// --------------------------

// Методы для работы с компонентами-заглушками (UMockUnet)
// --------------------------
// Добавление функции-создателя свойств для UMockUnet в массив в Storage
// Добавляет функцию-создателя свойства для UMockUnet в массив в Storage
bool AddCrPropMockFunc(funcCrPropMock func_ptr);

// Возвращает список функций функции-создателей свойства для UMockUnet
const std::list<funcCrPropMock> &GetFunctionsCrPropMock() const;

// Скрытые методы управления хранилищем объектов
// Выводит уже созданный объект из хранилища и возвращает
// его classid
// --------------------------
// --------------------------
protected:
// Если объект уже принадлежит иному хранилищу то возвращает false
// Если объект уже принадлежит иному хранилищу то возвращает false
virtual void PushObject(const UId &classid, UEPtr<UContainer> object);

public:
// его classid
// В случае ошибки возвращает ForbiddenId
// В случае ошибки возвращает ForbiddenId
virtual UId PopObject(UEPtr<UContainer> object);

protected:
// Возвращает объект в другое хранилище
virtual void MoveObject(UEPtr<UContainer> object, UEPtr<UStorage> newstorage);

public:
// Выбранный объект помечается как свободный в хранилище
// Флаг 'Activity' объекта выставляется в false
// Флаг 'Activity' объекта выставляется в false
virtual void ReturnObject(UEPtr<UComponent> object);

protected:
// В случае ошибки возвращает ForbiddenId
virtual UId PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator);
// --------------------------


// Скрытые методы таблицы соответствий классов
// --------------------------
// --------------------------
protected:
// Добавляет класс в таблицу 'name' по имени соответствия
virtual UId AddLookupClass(const std::string &name);

// Удаляет класс из таблицы 'name' по имени соответствия
virtual void DelLookupClass(const std::string &name);
// --------------------------


// Исключения
// --------------------------
// --------------------------
public:
class IException: public UException {};

// Ошибка класса в таблице по идентификатору classid идентификатору в хранилище
struct EClassIdNotExist: public EIdNotExist
{
explicit EClassIdNotExist(UId id) : EIdNotExist(id) {};
};

// Класс в хранилище класса не существует
struct EClassIdAlreadyExist: public EIdAlreadyExist
{
explicit EClassIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// Ошибка класса в таблице по имени, идентификатору в хранилище
struct EClassNameNotExist: public ENameNotExist
{
explicit EClassNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// Класс в хранилище класса не существует
struct EClassNameAlreadyExist: public ENameAlreadyExist
{
explicit EClassNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// Неправильное имя для класса
struct EInvalidClassName: public ENameError
{
explicit EInvalidClassName(const std::string &name) : ENameError(name) {};
};

// Ошибка объекта в таблице по идентификатору, идентификатору в хранилище
struct EObjectIdNotExist: public EIdNotExist
{
explicit EObjectIdNotExist(UId id) : EIdNotExist(id) {};
};

// Ошибка удаления объекта используемого объекта в таблице, который объект используется в хранилище
struct EObjectStorageNotEmpty: public EIdError
{
explicit EObjectStorageNotEmpty(UId id) : EIdError(id) {};
};

struct EInvalidClassType: public UException
{
 std::string ClassName;
 std::string ExpectedTypeName;

explicit EInvalidClassType(const std::string &expected_type_name, const std::string &class_name) :
 ClassName(class_name), ExpectedTypeName(expected_type_name) {};
virtual ~EInvalidClassType(void) throw() {};

// Создает строку сообщения для ошибки
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
UEPtr<T> UStorage::TakeObject(const UId &classid, const UEPtr<UComponent> &prototype)
{
 UEPtr<T> p;
 UEPtr<UComponent> got_class=TakeObject(classid,prototype);
 p=dynamic_pointer_cast<T>(got_class);
 if(!p)
 {
  ReturnObject(got_class);
  throw EInvalidClassType(FindClassName(classid), typeid(T).name());
 }
 return p;
}

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
// Ошибка класса в таблице по идентификатору classid идентификатору в хранилище
class UStorage::EClassIdNotExist: public EError
{
public: // Ошибочный идентификатор
// Ошибочный идентификатор
UId Id;

public: // --------------------------
// Конструкторы и деструкторы
// --------------------------
// --------------------------
EClassIdNotExist(UId id);
// --------------------------

// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
// Создает строку сообщения для ошибки
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Ошибка класса в таблице по имени, идентификатору в хранилище
class UStorage::EClassNameNotExist: public EError
{
public: // Ошибочный идентификатор
// Ошибочный идентификатор
std::string Name;

public: // --------------------------
// Конструкторы и деструкторы
// --------------------------
// --------------------------
EClassNameNotExist(const std::string &name);
// --------------------------

// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
// Создает строку сообщения для ошибки
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Неправильное имя для класса
class UStorage::EInvalidClassName: public EError
{
public: // Ошибочный идентификатор
// Ошибочный идентификатор
std::string Name;

public: // --------------------------
// Конструкторы и деструкторы
// --------------------------
// --------------------------
EInvalidClassName(const std::string &name);
// --------------------------

// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
// Создает строку сообщения для ошибки
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Класс в хранилище класса не существует
class UStorage::EClassNameAlreadyExist: public EError
{
public: // Ошибочное имя
// Ошибочное имя
std::string Name;

public: // --------------------------
// Конструкторы и деструкторы
// --------------------------
// --------------------------
EClassNameAlreadyExist(const std::string &name);
// --------------------------

// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
// Создает строку сообщения для ошибки
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

                  */
}


#endif

