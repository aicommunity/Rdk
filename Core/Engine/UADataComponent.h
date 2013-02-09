#ifndef UADATA_COMPONENT_H
#define UADATA_COMPONENT_H

#include "UAComponent.h"
#include "UAEnvSupport.h"
#include "UContainerDescription.h"
#include "../Serialize/Serialize.h"

namespace RDK {

typedef Serialize::USerStorage UVariableData;

class UADataComponent;
class UIProperty;
class UIShare;

// Варианты типа свойства (битовая маска) pt - Property Type
// 0x1 - Параметр
// 0x2 - Переменная состояния
// 0x4 - Временная переменная
// 0x8 - Вход
enum {ptParameter=1, ptState=2, ptTemp=4, ptInput=8, ptOutput=16, ptAny=255};

// Варианты групп свойства (битовая маска) pg - Property Group
// 0x100 - Общедоступный
// 0x200 - Системный
// 0x400 - Входные данные
// 0x800 - Выходные данные
// 0x1000 - Флаг смены режима работы компонента
enum {pgPublic=0x100, pgSystem=0x200, pgInput=0x400, pgOutput=0x800, pgMode=0x1000, pgAny=0xFFFFFF};

// Наиболее часто используемые сочетания типа и группы
enum {ptPubParameter=ptParameter|pgPublic, ptPubState=ptState|pgPublic, ptPubInput=ptInput|pgPublic, ptPubOutput=ptOutput|pgPublic};

// Хранилище свойств параметра
struct UVariable
{
// Id параметра
UId Id;

// Указатель на свойство
UEPtr<UIProperty> Property;

// Флаг разрешения удаления данных на которых указывает Property
bool DelEnable;

// Тип свойства (битовая маска)
// Младшие 8 бит на собственно тип:
// Старшие 24 на принадлежность группе
// (показан их отсчет от 0):
unsigned int Type;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UVariable(void);
UVariable(UId id, UEPtr<UIProperty> prop, unsigned int type=0);
UVariable(const UVariable &copy);
virtual ~UVariable(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает только маску типа свойства
unsigned int GetPropertyType(void) const;

// Возвращает только маску группы свойства
unsigned int GetPropertyGroup(void) const;

// Возвращает строковое имя типа свойства по заданному типу
static std::string GetPropertyTypeNameByType(unsigned int type);

// Возвращает тип свойства по строковому имени
static unsigned int GetPropertyTypeByTypeName(const std::string &name);

// Возвращает строковое имя типа свойства
std::string GetPropertyTypeName(void) const;

// Проверяет соответствие типа и группы свойства маске
bool CheckMask(unsigned int mask) const;
// --------------------------
};

   /*
// Хранилище свойств параметра
struct USharedVariable
{
 // Id параметра
 UId Id;

 // Указатель на свойство
 UEPtr<UIShare> Property;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
USharedVariable(void);
USharedVariable(UId id, UEPtr<UIShare> prop);
USharedVariable(const USharedVariable &copy);
virtual ~USharedVariable(void);
// --------------------------
};    */

class UADataComponent: public UAComponent
{
public: // Типы данных
typedef std::map<NameT,UVariable> VariableMapT;
typedef std::map<NameT,UVariable>::iterator VariableMapIteratorT;
typedef std::map<NameT,UVariable>::const_iterator VariableMapCIteratorT;

typedef std::map<UId,UEPtr<UIShare> > ShareMapT;
typedef std::map<UId,UEPtr<UIShare> >::iterator ShareMapIteratorT;
typedef std::map<UId,UEPtr<UIShare> >::const_iterator ShareMapCIteratorT;

protected: // Системные свойства
//protected: // Системные свойства
// Таблица соответствий имен и Id параметров объекта
VariableMapT PropertiesLookupTable;

protected:
// Таблица соответствий Id и общего свойства
ShareMapT ShareLookupTable;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UADataComponent(void);
virtual ~UADataComponent(void);
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
public:
// Возвращает имя параметра по его Id
const NameT& GetPropertyName(const UId &id) const;

// Возвращает Id параметра по его имени
const UId& GetPropertyId(const NameT &name) const;
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Создает экземпляр описания класса
virtual UContainerDescription* NewDescription(void);
virtual UContainerDescription* ANewDescription(UComponentDescription* description);
// --------------------------

// --------------------------
// Методы доступа к параметрам
// --------------------------
// Возвращает значение параметра по Id 'id'
UEPtr<UVariableData> GetProperty(const UId &id, UEPtr<UVariableData> values) const;
std::string& GetPropertyValue(const UId &id, std::string &values) const;

// Возвращает значение параметра по имени 'name'
UEPtr<UVariableData> GetProperty(const NameT &name, UEPtr<UVariableData> values) const;
std::string& GetPropertyValue(const NameT &name, std::string &values) const;

// Устанавливает значение параметра по Id 'id'
void SetProperty(const UId &id, UEPtr<UVariableData> values);
void SetPropertyValue(const UId &id, const std::string &values);

// Устанавливает значение параметра по имени 'name'
void SetProperty(const NameT &name, UEPtr<UVariableData> values);
void SetPropertyValue(const NameT &name, const std::string &values);

// Возвращает список Id параметров, содержащихся непосредственно
// в этом объекте
const UADataComponent::VariableMapT& GetPropertiesList(void) const;

// Ищет имя свойства по указателю на него
const NameT& FindPropertyName(UEPtr<const UIProperty> prop) const;

// Ищет тип свойства по указателю на него
unsigned int FindPropertyType(UEPtr<const UIProperty> prop) const;

// Ищет переменную свойства в таблице по указателю на него
UADataComponent::VariableMapCIteratorT FindPropertyVariable(UEPtr<const UIProperty> prop) const;

// Копирует все параметры этого объекта в объект 'comp', если возможно.
// копируются только свойства типа type
virtual void CopyProperties(UEPtr<UADataComponent> comp, unsigned int type) const;
// --------------------------

// --------------------------
// Скрытые методы управления параметрами
// --------------------------
public:
// Добавляет параметр с именем 'name' в таблицу соотвествий
// параметров и назначает ему корректный индекс
// Должна вызываться в конструкторах классов
UId AddLookupProperty(const NameT &name, unsigned int type, UEPtr<UIProperty> property, bool delenable=true);

// Изменяет тип параметра
bool ChangeLookupPropertyType(const NameT &name, unsigned int type);

protected:
// Удаляет параметр с именем 'name' из таблицы соотвествий
// параметров
void DelLookupProperty(const NameT &name);

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

public: // Исключения
// Id свойства не найден
struct EPropertyIdNotExist: public EIdNotExist
{
EPropertyIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id свойства уже существует
struct EPropertyIdAlreadyExist: public EIdAlreadyExist
{
EPropertyIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// Имя свойства не найдено
struct EPropertyNameNotExist: public ENameNotExist
{
EPropertyNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// Имя свойства уже существует
struct EPropertyNameAlreadyExist: public ENameAlreadyExist
{
EPropertyNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
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

// Класс сериализации свойств
class UIProperty
{
public:
// Метод устанавливает значение указателя на итератор-хранилище данных об этом
// свойстве в родительском компоненте
virtual void SetVariable(UADataComponent::VariableMapCIteratorT &var)=0;

// Метод возвращает тип свойства
virtual unsigned int GetType(void) const=0;

// Метод возвращает строковое имя свойства
virtual const std::string& GetName(void) const=0;

// Метод возвращает строковое имя класса-владельца свойства
virtual std::string GetOwnerName(void) const=0;

// Метод записывает значение свойства в поток
virtual bool Save(UEPtr<UVariableData> storage, bool simplemode=false)=0;

// Метод читает значение свойства из потока
virtual bool Load(UEPtr<UVariableData> storage, bool simplemode=false)=0;
};

// Класс управления общими свойствами
class UIShare
{
public:
 // Метод возвращает Id общего свойства
// virtual int GetId(void) const=0;

 // Метод возвращает строковое имя класса-владельца общего свойства
// virtual std::string GetOwnerName(void) const=0;

 // Метод инициализации общего свойства
 virtual bool Init(UEPtr<UADataComponent> main_owner)=0;

 // Метод деинициализации общего свойства
 virtual bool UnInit(void)=0;
};

}

#include "UProperty.h"
#include "ULocalProperty.h"
#include "UShare.h"

#endif

