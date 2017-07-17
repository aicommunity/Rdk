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

// Хранилище свойств параметра
struct RDK_LIB_TYPE UVariable
{
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
UVariable(UEPtr<UIProperty> prop, unsigned int type=0);
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


class RDK_LIB_TYPE UComponent: public UModule
{
friend class UStorage;
public: // Типы данных
typedef std::map<NameT,UVariable> VariableMapT;
typedef std::map<NameT,UVariable>::iterator VariableMapIteratorT;
typedef std::map<NameT,UVariable>::const_iterator VariableMapCIteratorT;

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
const UEPtr<T> FindPropertyByType(const NameT &name) const;

template<typename T>
UEPtr<T> FindPropertyByType(const NameT &name);

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
void FindPropertiesByType(std::vector<UEPtr<T> > &properties) const;

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
void AddLookupProperty(const NameT &name, unsigned int type, UEPtr<UIProperty> property, bool delenable=true);

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
EPropertyNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// Имя свойства уже существует
struct EPropertyNameAlreadyExist: public ENameAlreadyExist
{
EPropertyNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// Имя алиаса не найдено
struct EAliasNameNotExist: public ENameNotExist
{
EAliasNameNotExist(const std::string &name) : ENameNotExist(name) {};
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

class UNet;

// Класс сериализации свойств
class RDK_LIB_TYPE UIProperty
{
friend class UComponent;
public:
/// Возвращает указатель на владелька свойства
virtual const UNet* GetOwner(void) const=0;
virtual UNet* GetOwner(void)=0;

// Метод возвращает строковое имя свойства
virtual const std::string& GetName(void) const=0;

// Метод возвращает строковое имя компонента-владельца свойства
virtual std::string GetOwnerName(void) const=0;

// Метод возвращает строковое имя класса-владельца свойства
virtual std::string GetOwnerClassName(void) const=0;

// Метод записывает значение свойства в поток
virtual bool Save(UEPtr<UVariableData> storage, bool simplemode=false)=0;

// Метод читает значение свойства из потока
virtual bool Load(UEPtr<UVariableData> storage, bool simplemode=false)=0;

// Метод возвращает тип свойства
virtual unsigned int GetType(void) const=0;

// Возвращает языковой тип хранимого свойства
virtual const type_info& GetLanguageType(void) const=0;

// Метод сравнивает тип этого свойства с другим свойством
virtual bool CompareLanguageType(const UIProperty &dt) const=0;

// Метод возвращает указатель на область памяти, содержащую данные свойства
virtual const void* GetMemoryArea(void)=0;

// Метод копирует значение данных свойства из области памяти
// штатными средствами копирования реального типа данных
// входной указатель приводится к указателю на необходимый тип данных
virtual bool ReadFromMemory(const void *buffer)=0;

public: // Методы управления временем обновления данных свойства
/// Возвращает время обновления данных свойства (мс)
virtual ULongTime GetUpdateTime(void) const=0;

/// Устанавливает время обновления данных свойства
virtual void SetUpdateTime(ULongTime value)=0;

/// Сбрасывает время обновления до нуля
virtual void ResetUpdateTime(void)=0;

/// Обновляет время изменения данных свойства
virtual void RenewUpdateTime(void)=0;

/// Возвращает диапазон индексов входа/выхода
//virtual bool CheckRange(int index)=0;

// Диапазон индексов входов
//virtual int GetMinRange(void)=0;

//virtual int GetMaxRange(void)=0;


// --------------------------
// Методы управления входами
// --------------------------
/*
/// Возвращает имя подключенного компонента
virtual std::string GetItemName(void) const=0;

/// Возвращает полное имя подключенного компонента
virtual std::string GetItemFullName(void) const=0;

/// Возвращает имя подключенного выхода
virtual std::string GetItemOutputName(void) const=0;

/// Возвращает true, если на подключенном выходе новые данные
virtual bool IsNewData(void) const=0;

/// Возвращает true если вход имеет подключение
virtual bool IsConnected(void) const=0;

/// Инициализирует данные
virtual void Init(UItem* item, const std::string &output_name)=0;

/// Деинициализирует данные
virtual void UnInit(void)=0;
*/
// --------------------------

//public: // Методы управления выходами
///// Возвращает число подключенных входов
//virtual size_t GetNumConnectors(void) const=0;
//
///// Возвращает указатель на компонент-приемник
//virtual UComponent* GetConnector(int index)=0;
//
///// Возвращает имя подключенного входа компонента-приемника
//virtual std::string GetConnectorInputName(int index) const=0;

protected: // Вспомогательные методы
// Метод устанавливает значение указателя на итератор-хранилище данных об этом
// свойстве в родительском компоненте
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)=0;

/// Обновляет указатель PData
virtual void UpdatePData(void* data)=0;

public: // Исключения
// Обращение к неинициализированным данным свойства
struct EPropertyError: public EError
{
protected: // Данные исключения
/// Имя компонента владельца
std::string OwnerName;

/// Имя свойства
std::string PropertyName;

public:
EPropertyError(const std::string &owner_name, const std::string &property_name)
: EError(), OwnerName(owner_name), PropertyName(property_name) {};
virtual ~EPropertyError(void) throw() {};

// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" ")+OwnerName+std::string(":")+PropertyName;
}
};

// Обращение к неинициализированным данным свойства
struct EPropertyZeroPtr: public EPropertyError
{
public:
EPropertyZeroPtr(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

// Вызов Getter завершен неудачно
struct EPropertyGetterFail: public EPropertyError
{
public:
EPropertyGetterFail(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

// Вызов Setter завершен неудачно
struct EPropertySetterFail: public EPropertyError
{
public:
EPropertySetterFail(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

};

//class UItem;

// Описание подключаемого элемента "UConnectedITEM"
/*
struct RDK_LIB_TYPE UCItem
{
// Подключаемый элемент
UNet* Item;

// Индекс выхода
int Index;

// Имя выхода
std::string Name;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UCItem(void);
UCItem(const UCItem &copy);
// --------------------------

// --------------------------
// Операторы
// --------------------------
bool operator == (const UCItem &value) const;
bool operator != (const UCItem &value) const;
// --------------------------
};

// Описание входящей связи с заданным item "UConnectedLINK"
struct RDK_LIB_TYPE UCLink
{
// Индекс выхода
int Output;

// Индекс входа
int Input;

// Имя выхода
std::string OutputName;

// Имя входа
std::string InputName;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UCLink(void);
UCLink(const UCLink &copy);
// --------------------------
}; */

class UIPropertyOutput;

class RDK_LIB_TYPE UIPropertyInput: virtual public UIProperty
{
public:
/// Возвращает указатель на владелька свойства
//virtual UNet* GetOwner(void)=0;

/// Возвращает тип свойства входа
virtual int GetInputType(void) const=0;

public: // Методы доступа к источнику данных
/// Возвращает указатель на компонент-источник
//virtual const std::vector<UCItem>& GetItemsList(void) const=0;

/// Возвращает число подключений ко входу
virtual int GetNumConnections(void) const=0;

/// Возвращает указатели на свойства-приемники данных
virtual const std::list<UEPtr<UIPropertyOutput> > GetConnectedProperties(void) const=0;

// Возвращает указатель на компонент-источник
virtual const UNet* GetConnectedItem(int c_index=-1) const =0;
virtual UNet* GetConnectedItem(int c_index=-1)=0;

// Возвращает информацию о данных связей с item или пустой массив
// если такая связь отсутствует
//virtual void GetCLink(const UEPtr<UNet> &item, std::vector<UCLink> &buffer) const=0;
//virtual void GetCLink(const UNet* const item, std::vector<UCLink> &buffer) const=0;

/// Возвращает имя подключенного компонента
virtual std::string GetItemName(int c_index=-1) const=0;

/// Возвращает полное имя подключенного компонента
virtual std::string GetItemFullName(int c_index=-1) const=0;

/// Возвращает имя подключенного выхода
virtual std::string GetItemOutputName(int c_index=-1) const=0;

/// Возвращает true если вход имеет подключение
virtual bool IsConnected(void) const=0;

// Проверяет, существует ли связь с заданным коннектором
virtual bool IsConnectedTo(const UIPropertyOutput *output_property) const=0;

// Проверяет, существует ли связь с заданным коннектором
virtual bool IsConnectedTo(const UEPtr<UNet> &item) const=0;

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
virtual bool IsConnectedTo(const UEPtr<UNet> &item, const NameT &item_property_name, int c_index=-1) const=0;

/// Разрывает связь с элементом сети подключенным ко входу 'item_property_name'
/// Возвращает true, если действия по удалению выполнялись
//virtual bool Disconnect(const NameT &item_property_name, int c_index=-1)=0;

/// Разрывает связь со свойством output_property
virtual bool Disconnect(UIPropertyOutput *output_property)=0;

/// Разрывает связь с индексом c_index, или все связи если c_index == -1
/// Если c_index имеет не корректное значение, то не делает ничего
//virtual bool Disconnect(int c_index=-1)=0;

/// Разрывает все связи со свойством
virtual bool DisconnectAll(void)=0;

public: // Методы управления указателем на входные данные
/// Возвращает указатель на данные
virtual void const* GetPointer(int index) const=0;

/// Устанавливает указатель на данные
virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// Сбрасывает указатель на данные
virtual bool ResetPointer(int index, void* value)=0;
};

class RDK_LIB_TYPE UIPropertyOutput: virtual public UIProperty
{
public: // Методы доступа к подключенным входам
/// Возвращает число подключенных входов
virtual size_t GetNumConnectors(void) const=0;

/// Возвращает указатель на компонент-приемник
//virtual const UNet* GetConnector(int index) const=0;
//virtual UNet* GetConnector(int index)=0;

/// Возвращает указатели на свойства-приемники данных
virtual const std::list<UEPtr<UIPropertyInput> > GetConnectedProperties(void) const=0;

/// Возвращает имя подключенного входа компонента-приемника
//virtual std::string GetConnectorInputName(int index) const=0;

/// Устанавливает связь этого выхода со входом input_property
virtual bool Connect(UIPropertyInput *input_property)=0;

/// Разрывает связь этого выхода со входом input_property
virtual bool Disconnect(UIPropertyInput *input_property)=0;

// Разрывает связь выхода этого объекта с коннектором по Id 'id'.
//virtual bool Disconnect(const UId &id)=0;

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
virtual bool DisconnectAll(void)=0;

/// Возвращает true если выход подключен к выбранному входу
virtual bool IsConnectedTo(UIPropertyInput *input_property)=0;

/// Возвращает true если выход подключен к одному из входов выбранного компонента
virtual bool IsConnectedTo(UNet *component)=0;

public: // Методы управления указателем на выходные данные
/// Возвращает указатель на данные
virtual void const* GetPointer(int index) const=0;

/// Устанавливает указатель на данные
//virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// Сбрасывает указатель на данные
//virtual bool ResetPointer(int index, void* value)=0;

protected:
// Устанавливает связь с коннектором 'c'
//virtual bool Connect(UEPtr<UNet> c, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item=false)=0;

/// Разрывает все связи выхода этого объекта с коннектором 'c'.
//virtual bool Disconnect(UEPtr<UNet> c)=0;

// Разрывает связь выхода этого объекта с коннектором 'c' по индексу
//virtual bool Disconnect(UEPtr<UNet> c, const NameT &connector_property_name, int c_index=-1)=0;

// Проверяет, существует ли связь с заданным коннектором
//virtual bool CheckLink(const UEPtr<UNet> &connector, int c_index) const=0;

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
//virtual bool CheckLink(const UEPtr<UNet> &connector, const NameT &connector_property_name, int c_index=-1) const=0;

// Переустанавливает все связи этого выхода со всеми connectors
//virtual void BuildLinks(void)=0;

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


template<typename T>
void UComponent::FindPropertiesByType(std::vector<UEPtr<T> > &properties) const
{
 properties.clear();
 VariableMapCIteratorT I=PropertiesLookupTable.begin();
 for(;I != PropertiesLookupTable.end(); I++)
 {
  UEPtr<T> property=dynamic_pointer_cast<T>(I->second.Property);
  if(property)
   properties.push_back(property);
 }
}

// Возвращает указатель на данные свойства
template<typename T>
const UEPtr<T> UComponent::FindPropertyByType(const NameT &name) const
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);
 if(I != PropertiesLookupTable.end())
  return dynamic_pointer_cast<T>(I->second.Property);

 return 0;
}

template<typename T>
UEPtr<T> UComponent::FindPropertyByType(const NameT &name)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);
 if(I != PropertiesLookupTable.end())
  return dynamic_pointer_cast<T>(I->second.Property);

 return 0;
}





}

#endif

