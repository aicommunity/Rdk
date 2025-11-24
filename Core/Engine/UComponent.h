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
#include "UExceptionLogger.h"
#include "../Graphics/UFont.h"
#include <unordered_map>
#include <map>

#ifndef RDK_PROPERTY_TYPES
#define RDK_PROPERTY_TYPES
// 0x2 - Переменная состояния
// 0x4 - Временная переменная
// 0x8 - Вход
enum : unsigned int {ptNone=0, ptParameter=1, ptState=2, ptTemp=4, ptInput=8, ptOutput=16, ptAny=255};

// 0x100 - Общедоступный
// 0x200 - Системный
// 0x400 - Входные данные
// 0x800 - Выходные данные
// 0x1000 - Флаг смены режима работы компонента
enum : unsigned int {pgPublic=0x100, pgSystem=0x200, pgInput=0x400, pgOutput=0x800, pgMode=0x1000, pgAny=0xFFFFFF};

// Combined flags for public property types in component
constexpr unsigned int ptPubParameter = static_cast<unsigned int>(ptParameter) | static_cast<unsigned int>(pgPublic);
constexpr unsigned int ptPubState = static_cast<unsigned int>(ptState) | static_cast<unsigned int>(pgPublic);
constexpr unsigned int ptPubInput = static_cast<unsigned int>(ptInput) | static_cast<unsigned int>(pgPublic);
constexpr unsigned int ptPubOutput = static_cast<unsigned int>(ptOutput) | static_cast<unsigned int>(pgPublic);
// Combined flags for system property types
constexpr unsigned int ptSysParameter = static_cast<unsigned int>(ptParameter) | static_cast<unsigned int>(pgSystem);
constexpr unsigned int ptPubSysParameter = static_cast<unsigned int>(ptParameter) | static_cast<unsigned int>(pgPublic) | static_cast<unsigned int>(pgSystem);
constexpr unsigned int ptPubSysState = static_cast<unsigned int>(ptState) | static_cast<unsigned int>(pgPublic) | static_cast<unsigned int>(pgSystem);
// Combined flags for any type with public group
constexpr unsigned int ptAnyPub = static_cast<unsigned int>(ptAny) | static_cast<unsigned int>(pgPublic);

enum : unsigned int { ipData=1 }; // ipComp removed as legacy (was used for component pointers, now unused)
enum : unsigned int { ipSingle=16, ipRange=32, ipList=64 };
constexpr unsigned int ipDataSingle = static_cast<unsigned int>(ipData) | static_cast<unsigned int>(ipSingle);
constexpr unsigned int ipDataRange = static_cast<unsigned int>(ipData) | static_cast<unsigned int>(ipRange);
constexpr unsigned int ipDataList = static_cast<unsigned int>(ipData) | static_cast<unsigned int>(ipList);
#endif

namespace RDK {

class UStorage;
class UEnvironment;
class UComponentDescription;

typedef USerStorage UVariableData;

class UIProperty;
class UIShare;

// ��������� ������� ���������
struct RDK_LIB_TYPE UVariable
{
// ��������� �� ��������
UEPtr<UIProperty> Property;

// ���� ���������� �������� ������ �� ������� ��������� Property
bool DelEnable;

// Младшие 8 бит на собственно тип:
// Старшие 24 на принадлежность группе
// (показан их отсчет от 0):
// (������� �� ������ �� 0):
unsigned int Type;

// Конструкторы и деструкторы
// --------------------------
// --------------------------
UVariable(void);
UVariable(UEPtr<UIProperty> prop, unsigned int type=0);
UVariable(const UVariable &copy);
virtual ~UVariable(void);
// --------------------------

// Методы доступа к данным
// --------------------------
// Возвращает только маску типа свойства
// ���������� ������ ����� ���� ��������
unsigned int GetPropertyType(void) const;

// ���������� ������ ����� ������ ��������
unsigned int GetPropertyGroup(void) const;

// ���������� ��������� ��� ���� �������� �� ��������� ����
static std::string GetPropertyTypeNameByType(unsigned int type);

// ���������� ��� �������� �� ���������� �����
static unsigned int GetPropertyTypeByTypeName(const std::string &name);

// ���������� ��������� ��� ���� ��������
std::string GetPropertyTypeName(void) const;

// ��������� ������������ ���� � ������ �������� �����
bool CheckMask(unsigned int mask) const;
// --------------------------
};


class RDK_LIB_TYPE UComponent: public UModule
{
friend class UStorage;
public: // ���� ������
typedef std::unordered_map<NameT,UVariable> VariableMapT;
typedef std::unordered_map<NameT,UVariable>::iterator VariableMapIteratorT;
typedef std::unordered_map<NameT,UVariable>::const_iterator VariableMapCIteratorT;

typedef std::map<UId,UEPtr<UIShare> > ShareMapT;
typedef std::map<UId,UEPtr<UIShare> >::iterator ShareMapIteratorT;
typedef std::map<UId,UEPtr<UIShare> >::const_iterator ShareMapCIteratorT;

public: // ������ �������� ����������
class IException: public UException {};

protected: // Флаг, определяющий компонент является статическим
// или динамическим
// ��� ������������
bool StaticFlag;

// ��������� �� ��������� ���� ��������
UEPtr<UComponent> Owner;

// Автоматически устанавливается для всех дочерних объектов
// ������������� ��������������� ��� ���� �������� ��������
UEPtr<UComponent> MainOwner;

// ��������� �� ��������� ��������� ����� �������
UEPtr<UStorage> Storage;

// ��������� �� ����� ���������� ����� �������
UEPtr<UEnvironment> Environment;

// ��������� �� ������
UEPtr<UExceptionLogger> Logger;

protected: // Идентификатор класса
// ������������� ������
UId Class;

//UId Id;
//UId Id;

protected: //protected: // Системные свойства
// Таблица соответствий имен и Id параметров объекта
// ������� ������������ ���� � Id ���������� �������
VariableMapT PropertiesLookupTable;

// Таблица соответствий Id и общего свойства
mutable NameT CachedPropertyName;
mutable UEPtr<UIProperty> CachedProperty;

/// Карта алиасов вида <алиса, имя свойства>
mutable NameT CachedAliasName;
mutable NameT CachedAliasValue;

protected:
// ������� ������������ Id � ������ ��������
ShareMapT ShareLookupTable;

protected: // Конструкторы и деструкторы
// --------------------------
std::map<std::string, std::string> Aliases;


protected: // ��������� ����������

public: // Методы доступа к свойствам
// --------------------------
// Возвращает флаг, определяющий компонент является статическим
// или динамическим
UComponent(void);
virtual ~UComponent(void);
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// Возвращает указатель на главного владельца этим объектом
// ��� ������������
bool GetStaticFlag(void) const;
virtual bool SetStaticFlag(bool value);

// ���������� �������� ����� �������
UEPtr<UComponent> const GetOwner(void) const;
virtual bool SetOwner(UEPtr<UComponent> owner);

// ���������� ��������� �� �������� ��������� ���� ��������
UEPtr<UComponent> const GetMainOwner(void) const;
virtual void SetMainOwner(UEPtr<UComponent> mainowner);

// ���������� ��������� ��������� ����� �������
UEPtr<UStorage> const GetStorage(void) const;
virtual bool SetStorage(UEPtr<UStorage> storage);

/// Если Environment отсутствует то возвращает указатель на заглушку
UEPtr<UEnvironment> const GetEnvironment(void) const;
virtual bool SetEnvironment(UEPtr<UEnvironment> environment);

/// Возвращает указатель на шрифт по умолчанию
UEPtr<UExceptionLogger> const GetLogger(void) const;
virtual bool SetLogger(UEPtr<UExceptionLogger> logger);

/// ���������� ������ �� ����� ���������� ������� �� Environment.
// --------------------------
/// DummyTime
const UTimeControl& GetTime(void) const;

// --------------------------
UAFont* GetDefaultFont(void);

/// ���������� �������� �����
UAFont* GetFont(const string &name, int size);
// --------------------------

// --------------------------
// --------------------------
// --------------------------
// --------------------------
UId GetClass(void) const;
bool SetClass(UId value);


// ���������� ��� ������ ����������
const NameT GetCompClassName(void) const;
// --------------------------

// --------------------------
// Уничтожение этого объекта
// --------------------------
// ������� ��������� ����� ������
virtual UComponent* New(void)=0;

// ������� ��������� �������� ������
virtual UContainerDescription* NewDescription(void);
virtual UContainerDescription* ANewDescription(UComponentDescription* description);

// --------------------------
void Free(void);

protected:
// Возвращает указатель на данные свойства
virtual void UpdateInternalData(void);
virtual void AUpdateInternalData(void);
// --------------------------

// --------------------------
// ������ ������� � ����������
// --------------------------
public:
// ���������� ��������� �� ������ ��������
const UEPtr<UIProperty> FindProperty(const NameT &name) const;
UEPtr<UIProperty> FindProperty(const NameT &name);

// ���������� �������� ��������� �� ����� 'name'
UEPtr<UVariableData> GetProperty(const NameT &name, UEPtr<UVariableData> values) const;
std::string& GetPropertyValue(const NameT &name, std::string &values) const;

// ���������� �������� ��������� �� ����� 'name'
template<typename T>
const T* AccessPropertyData(const NameT &name) const;

template<typename T>
T* AccessPropertyData(const NameT &name);

// ������������� �������� ��������� �� ����� 'name'
void SetProperty(const NameT &name, UEPtr<UVariableData> values);
void SetPropertyValue(const NameT &name, const std::string &values);

// ���������� ������ Id ����������, ������������ ���������������
// Ищет переменную свойства в таблице по указателю на него
const UComponent::VariableMapT& GetPropertiesList(void) const;

// Копирует все параметры этого объекта в объект 'comp', если возможно.
const NameT& FindPropertyName(UEPtr<const UIProperty> prop) const;

// --------------------------
unsigned int FindPropertyType(UEPtr<const UIProperty> prop) const;

// Скрытые методы управления параметрами
UComponent::VariableMapCIteratorT FindPropertyVariable(UEPtr<const UIProperty> prop) const;

// Добавляет параметр с именем 'name' в таблицу соотвествий
// параметров и назначает ему корректный индекс
virtual void CopyProperties(UEPtr<UComponent> comp, unsigned int type) const;
// --------------------------

// Изменяет тип параметра
// ������� ������ ���������� �����������
// --------------------------
public:
// Удаляет параметр с именем 'name' из таблицы соотвествий
// параметров
// ������ ���������� � ������������� �������
void AddLookupProperty(const NameT &name, unsigned int type, UEPtr<UIProperty> property, bool delenable=true);

// �������� ��� ���������
bool ChangeLookupPropertyType(const NameT &name, unsigned int type);

protected:
// Скрытые методы управления общими свойствами
// --------------------------
void DelLookupProperty(const NameT &name);

// общих свойств и назначает ему корректный индекс
void ClearLookupPropertyTable(void);
// --------------------------

// --------------------------
// --------------------------
// Методы управления алиасами
public:
// ��������� ����� �������� �������� � ������ 'name' � ������� �����������
/// Добавление алисаса
// ������ ���������� � ������������� �������
UId AddLookupShare(const NameT &name, UEPtr<UIShare> property);
/// Удаление алисаса

// --------------------------
// ������ ���������� ��������
/// Проверка наличия алиаса
protected:
/// ���������� �������
bool AddAlias(const std::string &alias, const std::string &property_name);

// --------------------------
void DelAlias(const std::string &alias);

public:
/// �������� ������� ������
bool CheckAlias(const std::string &alias) const;

/// ��������� ����� �������� �� ������
const std::string& GetPropertyNameByAlias(const std::string &alias) const;
// --------------------------


public: // ����������
struct EEnvironmentNotExist: public EError
{
};

// ��� �������� �� �������
struct EPropertyNameNotExist: public ENameNotExist
{
explicit EPropertyNameNotExist(const std::string &name) : ENameNotExist(name) {}
};

// ��� �������� ��� ����������
struct EPropertyNameAlreadyExist: public ENameAlreadyExist
{
explicit EPropertyNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {}
};

// ��� ������ �� �������
struct EAliasNameNotExist: public ENameNotExist
{
explicit EAliasNameNotExist(const std::string &name) : ENameNotExist(name) {}
};

};

template<typename T>
static UComponent* NewStaticFunc()
{
 return new T;
}

class UItem;

// ����� ������������ �������
class RDK_LIB_TYPE UIProperty
{
public:
// ����� ������������� �������� ��������� �� ��������-��������� ������ �� ����
// Метод возвращает указатель компонента-владельца свойства
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)=0;

// Метод возвращает строковое имя компонента-владельца свойства
virtual unsigned int GetType(void) const=0;

// Метод возвращает строковое имя класса-владельца свойства
virtual const std::string& GetName(void) const=0;

// Метод записывает значение свойства в поток
virtual UContainer* GetOwner(void) const=0;

// Метод читает значение свойства из потока
virtual std::string GetOwnerName(void) const=0;

// Метод возвращает указатель на область памяти, содержащую данные свойства
virtual std::string GetOwnerClassName(void) const=0;

// Метод копирует значение данных свойства из области памяти
virtual bool Save(UEPtr<USerStorage> storage, bool simplemode=false)=0;

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<USerStorage> storage, bool simplemode=false)=0;

// ����� ���������� ��������� �� ������� ������, ���������� ������ ��������
virtual const void* GetMemoryArea(void)=0;

// ����� �������� �������� ������ �������� �� ������� ������
// �������� ���������� ����������� ��������� ���� ������
// Возвращает языковой тип хранимого свойства для одного элемента
virtual bool ReadFromMemory(const void *buffer)=0;

// Метод сравнивает тип этого свойства с другим свойством (по одному элементу)
virtual const type_info& GetLanguageType(void) const=0;

// --------------------------
virtual bool CompareLanguageType(const UIProperty &dt) const=0;

/// Возвращает тип свойства ввода-вывода
virtual const type_info& GetElemLanguageType(void) const=0;

/// Возвращает время обновления данных свойства (мс)
virtual bool CompareElemLanguageType(const UIProperty &dt) const=0;

/// Устанавливает время обновления данных свойства
// ������ ���������� �������
// --------------------------
/// Сбрасывает время обновления до нуля
virtual int GetIoType(void) const=0;

/// ���������� ����� ���������� ������ �������� (��)
virtual ULongTime GetUpdateTime(void) const=0;

// Исключения
virtual void SetUpdateTime(ULongTime value)=0;

/// ���������� ����� ���������� �� ����
virtual void ResetUpdateTime(void)=0;
/// Имя компонента владельца

virtual ~UIProperty();

public: // ����������
// ��������� � �������������������� ������ ��������
struct EPropertyError: public EError
{
protected: // ������ ����������
/// ��� ���������� ���������
std::string OwnerName;

/// ��� ��������
std::string PropertyName;

public:
EPropertyError(const std::string &owner_name, const std::string &property_name)
: EError(), OwnerName(owner_name), PropertyName(property_name) {}
virtual ~EPropertyError(void) throw() {}

// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" ")+OwnerName+std::string(":")+PropertyName;
}
};

// ��������� � �������������������� ������ ��������
struct EPropertyZeroPtr: public EPropertyError
{
public:
EPropertyZeroPtr(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {}
};

// ����� Getter �������� ��������
struct EPropertyGetterFail: public EPropertyError
{
public:
EPropertyGetterFail(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {}
};

// ����� Setter �������� ��������
struct EPropertySetterFail: public EPropertyError
{
public:
EPropertySetterFail(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {}
};

};


class RDK_LIB_TYPE UIPropertyOutput;
class UConnector;
class UItem;

class RDK_LIB_TYPE UIPropertyInput: public UIProperty
{
public:
 /// ������������ � �����������
 UIPropertyInput(void);
 virtual ~UIPropertyInput(void);

 /// Возвращает имя подключенного выхода
 virtual UItem* GetItem(int index=0)=0;

 // Устанавливает указатель на данные входа
 virtual std::string GetItemName(int index=0) const=0;

 /// Сбрасывает указатель на данные
 virtual std::string GetItemFullName(int index=0) const=0;

 /// Возвращает true, если на подключенном выходе новые данные
 virtual std::string GetItemOutputName(int index=0) const=0;

 /// Возвращает true если вход имеет подключение
 virtual bool SetPointer(int index, UIPropertyOutput* property)=0;

 /// ���������� ��������� �� ������
 virtual bool ResetPointer(int index, UIPropertyOutput* property)=0;

 // Данные
 virtual bool IsNewData(void) const=0;

 /// ���������� true ���� ���� ����� �����������
 virtual bool IsConnected(void) const=0;
};

class RDK_LIB_TYPE UIPropertyOutput: public UIPropertyInput
{
protected: // ������
 /// ��������� �� ����������-��������� ������
 std::vector<UItem*> Connectors;

 /// ����� ������ ����������-���������� ������
 std::vector<std::string> ConnectorInputNames;

public:
 /// ������������ � �����������
 UIPropertyOutput(void);
 virtual ~UIPropertyOutput(void);

 /// Возвращает указатель на свойство подключенного входа компонента-приемника
 virtual size_t GetNumConnectors(void) const;

 /// Обновить указатели свойств-входов
 virtual UConnector* GetConnector(int index);

 /// ���������� ��� ������������� ����� ����������-���������
 virtual std::string GetConnectorInputName(int index) const;

 /// ���������� ��������� �� �������� ������������� ����� ����������-���������
 virtual UIPropertyInput* GetConnectorProperty(int index);

 // Метод возвращает Id общего свойства
 virtual void UpdateConnectedPointers(void);
};


// ����� ���������� ������ ����������
class RDK_LIB_TYPE UIShare
{
public:
 // Метод деинициализации общего свойства
// virtual int GetId(void) const=0;

 // ����� ���������� ��������� ��� ������-��������� ������ ��������
// Возвращает значение параметра по имени 'name'

 // ����� ������������� ������ ��������
 virtual bool Init(UEPtr<UComponent> main_owner)=0;

 // ����� ��������������� ������ ��������
 virtual bool UnInit(void)=0;
};

// ���������� �������� ��������� �� ����� 'name'
template<typename T>
const T* UComponent::AccessPropertyData(const NameT &name) const
{
 UEPtr<UIProperty> property=FindProperty(name);
 if(!property)
  return 0;

 if(property->GetLanguageType() != typeid(T))
  return 0;

 return reinterpret_cast<T*>(property->GetMemoryArea());
}

template<typename T>
T* UComponent::AccessPropertyData(const NameT &name)
{
 UEPtr<UIProperty> property=FindProperty(name);
 if(!property)
  return 0;

 if(property->GetLanguageType() != typeid(T))
  return 0;

 return (T*)(property->GetMemoryArea());
}

}

#endif

