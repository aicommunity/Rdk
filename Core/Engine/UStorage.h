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
// ������� ������ ������������ �������� ������������� ������
class RDK_LIB_TYPE UInstancesStorageElement
{
public: // Указатель на объект
// ��������� �� ������
UEPtr<UContainer> Object;

// ������� ���� �������� �� ������
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
// �������� ������������
UInstancesStorageElement& operator = (const UInstancesStorageElement &copy);

// ��������� ���������
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
// �������� �������� USerStorageXML ��� UMockUNet
typedef bool (*funcCrPropMock)(USerStorageXML*, UMockUNet*);

/* *********************************************************************** */


class RDK_LIB_TYPE UStorage
{
// Системные свойства
protected: // Таблица соответствий имен и Id образцов классов
// ������� ������������ ���� � Id �������� �������
std::unordered_map<std::string,UId> ClassesLookupTable;

// ��������� �������� �������
UClassesStorage ClassesStorage;

/// ��������� ������ ��� �����������
mutable UEPtr<UExceptionLogger> Logger;

protected: // XML описания всех классов хранилища
// XML �������� ���� ������� ���������
UClassesDescription ClassesDescription;

/// �������� ����� ������� ���������
std::unordered_map<std::string, UPropertyDescription> CommonDescriptions;

protected: // Массив доступных библиотек
// ������ ��������� ���������
UClassLibraryList CollectionList;


/// имеет вид <имя класса, имя его библиотеки>
//std::map<std::string, std::string> ClassLibraryLookupTable;
//std::map<std::string, std::string> ClassLibraryLookupTable;

// ������ ���� ����������� �������
vector<string> CompletedClassNames;

// ������ ���� �� ����������� �������
vector<string> IncompletedClassNames;

protected: // Список объектов
// ������ ��������
UObjectsStorage ObjectsStorage;

// Index map for O(log n) object lookup by UEPtr<UContainer>
// Maps object pointer to iterator in the corresponding UInstancesStorage list
// Using std::map because UEPtr doesn't have hash function for std::unordered_map
std::map<UEPtr<UContainer>, UInstancesStorageIterator> ObjectsIndex;

// ��������� �������������� Id �������� �������
UId LastClassId;

// 1 -  обычная сборка. Сборка статических библиотек, затем динамических. Все компоненты рабочие (не заглушки)
// 2 -  поочередная сборка. Сборка статических, библиотек-заглушек, затем динамических.
//      Все компоненты, которые смогли собраться рабочими - рабочие.
//      Остальные (которые есть в библиотеках-заглушка, но нет сейчас в хранилище) - заглушки.
// 3 -  сборка только заглушек. Сборка библиотек-заглушек, затем динамических. Все компоненты заглушки (не рабочие)
// 3 -  ������ ������ ��������. ������ ���������-��������, ����� ������������. ��� ���������� �������� (�� �������)
int BuildMode;

// ������ �������-���������� Property ��� MockUNet
std::list<funcCrPropMock> FunctionsCrPropMock;

// ���� � ������ ��������� (� ������ ���������� ����� ��� ����� MockLibs RTlibs)
std::string LibrariesPath;

// ���� � ����� � ���������� ������� (� ������ ���������� ����� ����: ����������\������ (en, ru ���)\�������������.xml)
std::string ClDesc;

protected: // ��������� ����������


public: // ������

// Конструкторы и деструкторы
// --------------------------
// --------------------------
UStorage(void);
virtual ~UStorage(void);
// --------------------------

// Методы управления свойствами
// --------------------------
// Возвращает последний использованный Id классов
// ���������� ��������� �������������� Id �������
UId GetLastClassId(void) const;
// --------------------------

// Методы доступа к таблицам соотвествий
// --------------------------
// Возвращает Id класса по его имени
// ���������� Id ������ �� ��� �����
const UId& FindClassId(const std::string &name) const;

// ���������� ��� ������ �� ��� Id
const NameT FindClassName(const UId &id) const;
// --------------------------

// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище
// Возвращает id класса
// Если classid == ForbiddenId, то id назначается автоматически
// ���� classid == ForbiddenId, �� id ����������� �������������
virtual UId AddClass(UEPtr<UComponentAbstractFactory> factory, const UId &classid=ForbiddenId);
virtual UId AddClass(UEPtr<UComponentAbstractFactory> factory, const std::string &classname, const UId &classid=ForbiddenId);

// Если 'force' == true то принудительно удаляет из хранилища
// все объекты этого класса
// ��� ������� ����� ������
virtual void DelClass(const UId &classid, bool force=false);

// ��������� ������� ������� ������ ������� � ���������
virtual bool CheckClass(const UId &classid) const;
virtual bool CheckClass(const string &classname) const;

// ���������� ������� ������
virtual UEPtr<UComponentAbstractFactory> GetComponentFactory(const UId &classid) const;
virtual UEPtr<UComponentAbstractFactory> GetComponentFactory(const std::string &class_name) const;

// ���������� ����� �������
int GetNumClasses(void) const;

// Буфер 'buffer' будет очищен от предыдущих значений
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassIdList(std::vector<UId> &buffer) const;

// Буфер 'buffer' будет очищен от предыдущих значений
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassNameList(std::vector<std::string> &buffer) const;

/// Если force == true то удаляет даже если хранилище объектов не пустое
/// и подавляет соответствующее исключение
/// � ��������� ��������������� ����������
virtual void FreeClassesStorage(bool force=false);

/// Если force == true то удаляет даже если хранилище объектов не пустое
/// и подавляет соответствующее исключение
/// � ��������� ��������������� ����������
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
// � ���������
virtual UEPtr<UComponent> TakeObject(const UId &classid, const UEPtr<UComponent> &prototype=0);
virtual UEPtr<UComponent> TakeObject(const string &classname, const UEPtr<UComponent> &prototype=0);

template<class T>
UEPtr<T> TakeObject(const UId &classid, const UEPtr<UComponent> &prototype=0);

template<class T>
UEPtr<T> TakeObject(const string &classname, const UEPtr<UComponent> &prototype=0);

// ���������� Id ������, ���������� ������� 'object'
virtual UId FindClass(UEPtr<UComponent> object) const;

// ��������� ���������� �� ������ 'object' � ���������
virtual bool CheckObject(UEPtr<UContainer> object) const;

// ���� �������, ��������������� �������� �������� ���������
virtual UVirtualMethodFactory* FindVirualMethodFactory(UEPtr<UContainer> object);

// ��������� ��������� ����� �������� � ���������
virtual int CalcNumObjects(void) const;
virtual int CalcNumObjects(const UId &classid) const;
virtual size_t CalcNumObjects(const string &classname) const;

/// Если force == true то удаляет даже если объекты используются
/// ���� force == true �� ������� ���� ���� ������� ������������
virtual void FreeObjectsStorage(bool force=false);

// ������� ��� ��������� ������� ��������� ������ �� ���������
virtual void FreeObjectsStorageByClass(const UId &classid);

/// Если force == true то удаляет даже если объекты используются
/// ���� force == true �� ������� ���� ���� ������� ������������
virtual void ClearObjectsStorage(bool force=false);

// ������� ��� ������� ��������� ������ �� ���������
virtual void ClearObjectsStorageByClass(const UId &classid);

/// ������������� ��������� ��� ��������� ���������� � ��������� �� ���������
virtual void DefaultObject(UEPtr<UContainer> object);
// --------------------------

// Методы управления описанием классов
// --------------------------
// Установка пути к папке с описаниями классов
// ��������� ���� � ����� � ���������� �������
void SetClDescPath(const std::string& value);

// ��������� ���� � ����� � ���������� �������
const std::string GetClDescPath() const;

// Также создаёт необходимые папки
// ����� ������ ����������� �����
const std::string GetCreateClDescPath(const std::string& class_name);

// ���������� XML �������� ������
const UEPtr<UContainerDescription> GetClassDescription(const std::string &classname, bool no_throw=false) const;

// Класс в хранилище должен существовать
// ����� � ��������� ������ ������������
void SetClassDescription(const std::string &classname, const UEPtr<UContainerDescription>& description);

// �������� �������� ������� �� xml-��������
virtual void LoadClassesDescription();

// ��������� �������� ������ � ���� �� ����
virtual void SaveClassDescriptionToFile(const std::string &classname);

// ��������� �������� ������ � xml
virtual void SaveClassDescription(const std::string &classname,
										USerStorageXML &xml);

// ��������� �������� ������ �� xml
virtual void LoadClassDescription(const std::string &classname,
										USerStorageXML &xml);

// ��������� �������� ���� ������� � xml
virtual void SaveClassesDescription(USerStorageXML &xml);

// ��������� �������� ���� ������� �� xml
virtual void LoadClassesDescription(USerStorageXML &xml);

// ��������� ����� �������� ���� ������� � xml
virtual bool SaveCommonClassesDescription(USerStorageXML &xml);

// ��������� ����� �������� ���� ������� �� xml
virtual bool LoadCommonClassesDescription(USerStorageXML &xml);
// --------------------------

// Методы управления библиотеками
// --------------------------
// Указатель на логгер
// ��������� �� ������
UEPtr<UExceptionLogger> const GetLogger(void) const;
virtual bool SetLogger(UEPtr<UExceptionLogger> logger);

// ���������� ���������� �� �������
UEPtr<ULibrary> GetCollection(int index);

// ���������� ����� ���������
int GetNumCollections(void) const;

// ���������� ���������� �� �����
UEPtr<ULibrary> GetCollection(const string &name);

// ���������� ��� ���������� �� �������
const string& GetCollectionName(int index);

// ���������� ������ ���������� �� �������
const string& GetCollectionVersion(int index);

// Нужно перед сборкой
// ����� ����� �������
void ClearAllLibsClassesNameArrays(void);

// Буфер 'buffer' будет очищен от предыдущих значений
// ����� 'buffer' ����� ������ �� ���������� ��������
void GetLibsNameListByType(std::string &buffer, int type) const;

//virtual bool AddClass(UContainer *newclass);
//virtual bool AddClass(UContainer *newclass);

// ��������� ���� � ������ ���������
void SetLibrariesPath(const std::string& value);

// ��������� ���� � ������ ���������
const std::string GetLibrariesPath() const;

/// Инициализация существующих динамических библиотек
/// Вызывается в Engine один раз. Добавляет библиотеки в CollectionList (сборки компонентов нет)
/// ���������� � Engine ���� ���. ��������� ���������� � CollectionList (������ ����������� ���)
void InitRTlibs(void);

/// ��������� runtime-���������� �� � ����� (��� �������� �����������)
virtual bool LoadRuntimeCollection(const std::string &lib_name);

/// Если класс с таким именем существует возможно перезапись при force_replace = true
/// ���� ����� � ����� ������ ���������� �������� ���������� ��� force_replace = true
virtual bool AddClassToCollection(const std::string &new_class_name, const std::string &new_comp_name, bool force_replace, UContainer *newclass, const std::string &lib_name);

/// ������� ������� ������ �� RT ���������
virtual bool DelClassFromCollection(const std::string &class_name, const std::string &lib_name);

/// ������� ����� ���������� � �������� ������
virtual bool CreateRuntimeCollection(const std::string &lib_name);

/// ������� runtime-���������� ������ � ������
bool DeleteRuntimeCollection(const std::string &lib_name);

// Если бибилиотека с таким именем уже существует то возвращает false.
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
// Если force_build == true то немедленно осущетсвляет развертывание бибилотеки
// в хранилище
// � ���������
virtual bool AddCollection(ULibrary *library, bool force_build=false);

// Ответственность за освобождение памяти лежит на вызывающей стороне.
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool DelCollection(int index);

// В правильном порядке (порядок опредлен в конкретном файле)
// � ���������� ������� (������� �������� � ���������� �����)
bool InitMockLibs(void);

// из статических библиотек
// �� ����������� ���������
bool CreateMockLibs(void);

// Также сохраняет порядок библиотек в виде отдельного файла
// ����� ��������� ������� ��������� � ���� ���������� �����
bool SaveMockLibs(void);

// Ответственность за освобождение памяти лежит на вызывающей стороне.
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool DelCollection(const string &name);

// Ответственность за освобождение памяти лежит на вызывающей стороне.
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool DelAllCollections(void);

// ��������� ������������ ������ ������
void SetBuildMode(int mode);

// ��������� �������� ������ ������
int GetBuildMode();

// ��������� ��������� ������� ���������
virtual bool BuildStorage(void);

// Тип библиотеки:
// 0 - Внутренняя библиотека (собрана вместе с ядром)
// 1 - Внешняя библиотека (загружена из внешней dll)
// 2 - Библиотека, созданная во время выполнения
// 3 - Библиотека-заглушка (все компоненты-заглушки)
// 3 - ����������-�������� (��� ����������-��������)
virtual bool BuildStorage(int lib_type);

/// а также все связанные образцы
/// � ����� ��� ��������� �������
virtual void DelAbandonedClasses(void);

/// ���������� ��������� �� ���������� ������ �� ����� ������
virtual UEPtr<ULibrary> FindCollection(const std::string &class_name);
virtual UEPtr<ULibrary> FindCollection(const UId &classid);

/// Метод не очищает переданный список библиотек, а только пополняет его
/// ����� �� ������� ���������� ������ ���������, � ������ ��������� ���
virtual void FindComponentDependencies(const std::string &class_name, std::vector<std::pair<std::string,std::string> > &dependencies);
// --------------------------

// Методы для работы с компонентами-заглушками (UMockUnet)
// --------------------------
// Добавление функции-создателя свойств для UMockUnet в массив в Storage
// ���������� �������-��������� ������� ��� UMockUnet � ������ � Storage
bool AddCrPropMockFunc(funcCrPropMock func_ptr);

// ��������� ������� �������-���������� ������� ��� UMockUnet
const std::list<funcCrPropMock> &GetFunctionsCrPropMock() const;

// Скрытые методы управления хранилищем объектов
// Выводит уже созданный объект из хранилища и возвращает
// его classid
// --------------------------
// --------------------------
protected:
// Если объект уже принадлежит иному хранилищу то возвращает false
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
virtual void PushObject(const UId &classid, UEPtr<UContainer> object);

public:
// его classid
// В случае ошибки возвращает ForbiddenId
// � ������ ������ ���������� ForbiddenId
virtual UId PopObject(UEPtr<UContainer> object);

protected:
// ���������� ������ � ������ ���������
virtual void MoveObject(UEPtr<UContainer> object, UEPtr<UStorage> newstorage);

public:
// Выбранный объект помечается как свободный в хранилище
// Флаг 'Activity' объекта выставляется в false
// ���� 'Activity' ������� ������������ � false
virtual void ReturnObject(UEPtr<UComponent> object);

protected:
// � ������ ������ ���������� ForbiddenId
virtual UId PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator);
// --------------------------


// Скрытые методы таблицы соответствий классов
// --------------------------
// --------------------------
protected:
// ��������� ����� � ������ 'name' � ������� ������������
virtual UId AddLookupClass(const std::string &name);

// ������� ����� � ������ 'name' �� ������� �����������
virtual void DelLookupClass(const std::string &name);
// --------------------------


// Исключения
// --------------------------
// --------------------------
public:
class IException: public UException {};

// ������� ������ � ������� �� �������������� classid ������������� � ���������
struct EClassIdNotExist: public EIdNotExist
{
explicit EClassIdNotExist(UId id) : EIdNotExist(id) {};
};

// ����� � �������� ������ ��� ����������
struct EClassIdAlreadyExist: public EIdAlreadyExist
{
explicit EClassIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// ������� ������ � ������� �� �����, �������������� � ���������
struct EClassNameNotExist: public ENameNotExist
{
explicit EClassNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ����� � �������� ������ ��� ����������
struct EClassNameAlreadyExist: public ENameAlreadyExist
{
explicit EClassNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// ������������ ��� ������
struct EInvalidClassName: public ENameError
{
explicit EInvalidClassName(const std::string &name) : ENameError(name) {};
};

// ������� ������ � ������� �� ��������������, �������������� � ���������
struct EObjectIdNotExist: public EIdNotExist
{
explicit EObjectIdNotExist(UId id) : EIdNotExist(id) {};
};

// ������� ���������� ����������� �������� � �������, ������� �������� ������������ � ���������
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

// ��������� ������ ���� �� ����������
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
// ������� ������ � ������� �� �������������� classid ������������� � ���������
class UStorage::EClassIdNotExist: public EError
{
public: // Ошибочный идентификатор
// ��������� �������������
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
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ������� ������ � ������� �� �����, �������������� � ���������
class UStorage::EClassNameNotExist: public EError
{
public: // Ошибочный идентификатор
// ��������� �������������
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
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ������������ ��� ������
class UStorage::EInvalidClassName: public EError
{
public: // Ошибочный идентификатор
// ��������� �������������
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
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ����� � �������� ������ ��� ����������
class UStorage::EClassNameAlreadyExist: public EError
{
public: // Ошибочное имя
// ��������� ���
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
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

                  */
}


#endif

