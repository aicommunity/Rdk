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
#include "UContainerDescription.h"
#include "UTime.h"
#include "ULoggerEnv.h"
#include "../Serialize/Serialize.h"
#include "../Graphics/UFont.h"

#ifndef RDK_PROPERTY_TYPES
#define RDK_PROPERTY_TYPES
// Варианты типа свойства (битовая маска) pt - Property Type
// 0x1 - Параметр
// 0x2 - Переменная состояния
// 0x4 - Временная переменная
// 0x8 - Вход
enum {ptNone=0, ptParameter=1, ptState=2, ptTemp=4, ptInput=8, ptOutput=16, ptAny=255};

// Варианты групп свойства (битовая маска) pg - Property Group
// 0x100 - Общедоступный
// 0x200 - Системный
// 0x400 - Входные данные
// 0x800 - Выходные данные
// 0x1000 - Флаг смены режима работы компонента
enum {pgPublic=0x100, pgSystem=0x200, pgInput=0x400, pgOutput=0x800, pgMode=0x1000, pgAny=0xFFFFFF};

// Наиболее часто используемые сочетания типа и группы
enum {ptPubParameter=ptParameter|pgPublic, ptPubState=ptState|pgPublic, ptPubInput=ptInput|pgPublic, ptPubOutput=ptOutput|pgPublic};

enum { ipData=1, ipComp=2 };
enum { ipSingle=16, ipRange=32, ipList=64 };
enum { ipDataSingle=ipData|ipSingle, ipDataRange=ipData|ipRange,
       ipDataList=ipData|ipList, ipCompSingle=ipComp|ipSingle,
       ipCompRange=ipComp|ipRange, ipCompList=ipComp|ipList };
#endif

namespace RDK {

class UStorage;
class UEnvironment;
class UComponentDescription;

typedef USerStorage UVariableData;

class UIProperty;
class UIShare;

class RDK_LIB_TYPE UComponent: public UModule
{
friend class UStorage;
public: // Типы данных
typedef std::map<NameT,UEPtr<UIProperty> > VariableMapT;
typedef std::map<NameT,UEPtr<UIProperty> >::iterator VariableMapIteratorT;
typedef std::map<NameT,UEPtr<UIProperty> >::const_iterator VariableMapCIteratorT;

typedef std::map<UId,UEPtr<UIShare> > ShareMapT;
typedef std::map<UId,UEPtr<UIShare> >::iterator ShareMapIteratorT;
typedef std::map<UId,UEPtr<UIShare> >::const_iterator ShareMapCIteratorT;

public: // Классы описания исключений
class IException: public UException {};

protected: // Основные свойства
// Флаг, определяющий компонент является статическим
// или динамическим
bool StaticFlag;

// Указатель на владельца этим объектом
UEPtr<UComponent> Owner;

// Указатель на главного владельца этим объектом
// Автоматически устанавливается для всех дочерних объектов
UEPtr<UComponent> MainOwner;

// Указатель на хранилище компонент этого объекта
UEPtr<UStorage> Storage;

// Указатель на среду выполнения этого объекта
UEPtr<UEnvironment> Environment;

// Указатель на логгер
UEPtr<ULoggerEnv> Logger;

protected: // Данные
// Идентификатор класса
UId Class;

// Идентификатор экземпляра объекта
//UId Id;

protected: // Системные свойства
//protected: // Системные свойства
// Таблица соответствий имен и Id параметров объекта
VariableMapT PropertiesLookupTable;

protected:
// Таблица соответствий Id и общего свойства
ShareMapT ShareLookupTable;

protected: // Временные алиасы для свойств
/// Карта алиасов вида <алиса, имя свойства>
std::map<std::string, std::string> Aliases;


protected: // Временные переменные

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
// Возвращает флаг, определяющий компонент является статическим
// или динамическим
bool GetStaticFlag(void) const;
virtual bool SetStaticFlag(bool value);

// Возвращает владелца этого объекта
UEPtr<UComponent> const GetOwner(void) const;
virtual bool SetOwner(UEPtr<UComponent> owner);

// Возвращает указатель на главного владельца этим объектом
UEPtr<UComponent> const GetMainOwner(void) const;
virtual void SetMainOwner(UEPtr<UComponent> mainowner);

// Возвращает хранилище компонент этого объекта
UEPtr<UStorage> const GetStorage(void) const;
virtual bool SetStorage(UEPtr<UStorage> storage);

// Возвращает среду выполнения этого объекта
UEPtr<UEnvironment> const GetEnvironment(void) const;
virtual bool SetEnvironment(UEPtr<UEnvironment> environment);

// Указатель на логгер
UEPtr<ULoggerEnv> const GetLogger(void) const;
virtual bool SetLogger(UEPtr<ULoggerEnv> logger);

/// Возвращает ссылку на класс управления времени из Environment.
/// Если Environment отсутствует то возвращает указатель на заглушку
/// DummyTime
const UTimeControl& GetTime(void) const;

/// Возвращает указатель на шрифт по умолчанию
UAFont* GetDefaultFont(void);

/// Возвращает заданный шрифт
UAFont* GetFont(const string &name, int size);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Идентификатор класса
UId GetClass(void) const;
bool SetClass(UId value);


// Возвращает имя класса компоненты
const NameT GetCompClassName(void) const;

// Возвращает строковое имя типа свойства по заданному типу
static std::string GetPropertyTypeNameByType(unsigned int type);

// Возвращает тип свойства по строковому имени
static unsigned int GetPropertyTypeByTypeName(const std::string &name);
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Создает экземпляр этого класса
virtual UComponent* New(void)=0;

// Создает экземпляр описания класса
virtual UContainerDescription* NewDescription(void);
virtual UContainerDescription* ANewDescription(UComponentDescription* description);

// Уничтожение этого объекта
void Free(void);

protected:
/// Осуществляет обновление внутренних данных компонента, обеспечивающих его целостность
virtual void UpdateInternalData(void);
virtual void AUpdateInternalData(void);
// --------------------------

// --------------------------
// Методы доступа к параметрам
// --------------------------
public:
// Возвращает указатель на данные свойства
const UEPtr<UIProperty> FindProperty(const NameT &name) const;
UEPtr<UIProperty> FindProperty(const NameT &name);

// Возвращает указатель на данные свойства
template<typename T>
const UEPtr<T> FindPropertyByType(const NameT &name, unsigned int mask) const;

template<typename T>
UEPtr<T> FindPropertyByType(const NameT &name, unsigned int mask);

// Возвращает значение параметра по имени 'name'
UEPtr<UVariableData> GetProperty(const NameT &name, UEPtr<UVariableData> values) const;
std::string& GetPropertyValue(const NameT &name, std::string &values) const;

// Устанавливает значение параметра по имени 'name'
void SetProperty(const NameT &name, UEPtr<UVariableData> values);
void SetPropertyValue(const NameT &name, const std::string &values);

// Возвращает список Id параметров, содержащихся непосредственно
// в этом объекте
const UComponent::VariableMapT& GetPropertiesList(void) const;

// Ищет имя свойства по указателю на него
const NameT& FindPropertyName(UEPtr<const UIProperty> prop) const;

// Ищет тип свойства по указателю на него
unsigned int FindPropertyType(UEPtr<const UIProperty> prop) const;

// Ищет переменную свойства в таблице по указателю на него
UComponent::VariableMapCIteratorT FindPropertyVariable(UEPtr<const UIProperty> prop) const;

template<typename T>
void FindPropertiesByType(std::vector<UEPtr<T> > &properties, unsigned int mask) const;

// Копирует все параметры этого объекта в объект 'comp', если возможно.
// копируются только свойства типа type
virtual void CopyProperties(UEPtr<UComponent> comp, unsigned int type) const;
// --------------------------

// --------------------------
// Скрытые методы управления параметрами
// --------------------------
public:
// Добавляет параметр с именем 'name' в таблицу соотвествий
// параметров и назначает ему корректный индекс
// Должна вызываться в конструкторах классов
void AddLookupProperty(UEPtr<UIProperty> property);

// Изменяет тип параметра
bool ChangeLookupPropertyType(const NameT &name, unsigned int type);

// Удаляет параметр с именем 'name' из таблицы соотвествий
// параметров
void DelLookupProperty(const NameT &name, bool force_no_delete);

protected:
// Удаляет всю таблицу соответствий
void ClearLookupPropertyTable(void);
// --------------------------

// --------------------------
// Скрытые методы управления общими свойствами
// --------------------------
public:
// Добавляет общее свойство параметр с именем 'name' в таблицу соотвествий
// общих свойств и назначает ему корректный индекс
// Должна вызываться в конструкторах классов
UId AddLookupShare(const NameT &name, UEPtr<UIShare> property);
// --------------------------

// --------------------------
// Методы управления алиасами
// --------------------------
protected:
/// Добавление алисаса
bool AddAlias(const std::string &alias, const std::string &property_name);

/// Удаление алисаса
void DelAlias(const std::string &alias);

public:
/// Проверка наличия алиаса
bool CheckAlias(const std::string &alias) const;

/// Получение имени свойства по алиасу
const std::string& GetPropertyNameByAlias(const std::string &alias) const;
// --------------------------


public: // Исключения
struct EEnvironmentNotExist: public EError
{
};

// Id свойства не найден
/*struct EPropertyIdNotExist: public EIdNotExist
{
EPropertyIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id свойства уже существует
struct EPropertyIdAlreadyExist: public EIdAlreadyExist
{
EPropertyIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
}; */

// Имя свойства не найдено
struct EPropertyNameNotExist: public ENameNotExist
{
explicit EPropertyNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// Имя свойства уже существует
struct EPropertyNameAlreadyExist: public ENameAlreadyExist
{
explicit EPropertyNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// Имя алиаса не найдено
struct EAliasNameNotExist: public ENameNotExist
{
explicit EAliasNameNotExist(const std::string &name) : ENameNotExist(name) {};
};
/*
// Id переменной состояния не найден
struct EStateIdNotExist: public EIdNotExist
{
EStateIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id переменной состояния уже существует
struct EStateIdAlreadyExist: public EIdAlreadyExist
{
EStateIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// Имя переменной состояния не найдено
struct EStateNameNotExist: public ENameNotExist
{
EStateNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// Имя переменной состояния уже существует
struct EStateNameAlreadyExist: public ENameAlreadyExist
{
EStateNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};
    */
};




// Класс управления общими свойствами
class RDK_LIB_TYPE UIShare
{
public:
 // Метод инициализации общего свойства
 virtual bool Init(UEPtr<UComponent> main_owner)=0;

 // Метод деинициализации общего свойства
 virtual bool UnInit(void)=0;
};

}

#endif

