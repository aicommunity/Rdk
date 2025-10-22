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
#include <memory>
#include "ModernSmartPointers.h"
#include "UContainerDescription.h"
#include "UTime.h"
#include "ULoggerEnv.h"
#include "../Graphics/UFont.h"

#ifndef RDK_PROPERTY_TYPES
#define RDK_PROPERTY_TYPES
// �������� ���� �������� (������� �����) pt - Property Type
// 0x1 - ��������
// 0x2 - ���������� ���������
// 0x4 - ��������� ����������
// 0x8 - ����
enum {ptNone=0, ptParameter=1, ptState=2, ptTemp=4, ptInput=8, ptOutput=16, ptAny=255};

// �������� ����� �������� (������� �����) pg - Property Group
// 0x100 - �������������
// 0x200 - ���������
// 0x400 - ������� ������
// 0x800 - �������� ������
// 0x1000 - ���� ����� ������ ������ ����������
enum {pgPublic=0x100, pgSystem=0x200, pgInput=0x400, pgOutput=0x800, pgMode=0x1000, pgAny=0xFFFFFF};

// �������� ����� ������������ ��������� ���� � ������
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

// ��������� ������� ���������
struct RDK_LIB_TYPE UVariable
{
// ��������� �� ��������
UIProperty* Property;

// ���� ���������� �������� ������ �� ������� ��������� Property
bool DelEnable;

// ��� �������� (������� �����)
// ������� 8 ��� �� ���������� ���:
// ������� 24 �� �������������� ������
// (������� �� ������ �� 0):
unsigned int Type;

// --------------------------
// ������������ � �����������
// --------------------------
UVariable(void);
UVariable(UIProperty* prop, unsigned int type=0);
UVariable(const UVariable &copy);
virtual ~UVariable(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
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
typedef std::map<NameT,UVariable> VariableMapT;
typedef std::map<NameT,UVariable>::iterator VariableMapIteratorT;
typedef std::map<NameT,UVariable>::const_iterator VariableMapCIteratorT;

typedef std::map<UId,std::shared_ptr<UIShare> > ShareMapT;
typedef std::map<UId,std::shared_ptr<UIShare> >::iterator ShareMapIteratorT;
typedef std::map<UId,std::shared_ptr<UIShare> >::const_iterator ShareMapCIteratorT;

public: // ������ �������� ����������
class IException: public UException {};

protected: // �������� ��������
// ����, ������������ ��������� �������� �����������
// ��� ������������
bool StaticFlag;

// ��������� �� ��������� ���� ��������
std::weak_ptr<UComponent> Owner;

// ��������� �� �������� ��������� ���� ��������
// ������������� ��������������� ��� ���� �������� ��������
std::weak_ptr<UComponent> MainOwner;

// ��������� �� ��������� ��������� ����� �������
std::weak_ptr<UStorage> Storage;

// ��������� �� ����� ���������� ����� �������
std::weak_ptr<UEnvironment> Environment;

// ��������� �� ������
std::weak_ptr<ULoggerEnv> Logger;

protected: // ������
// ������������� ������
UId Class;

// ������������� ���������� �������
//UId Id;

protected: // ��������� ��������
//protected: // ��������� ��������
// ������� ������������ ���� � Id ���������� �������
VariableMapT PropertiesLookupTable;

protected:
// ������� ������������ Id � ������ ��������
ShareMapT ShareLookupTable;

protected: // ��������� ������ ��� �������
/// ����� ������� ���� <�����, ��� ��������>
std::map<std::string, std::string> Aliases;


protected: // ��������� ����������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UComponent(void);
UComponent(const UComponent&) = default;
UComponent(UComponent&&) noexcept = default;
virtual ~UComponent(void);

UComponent& operator=(const UComponent&) = default;
UComponent& operator=(UComponent&&) noexcept = default;
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� ����, ������������ ��������� �������� �����������
// ��� ������������
bool GetStaticFlag(void) const;
virtual bool SetStaticFlag(bool value);

// ���������� �������� ����� �������
std::shared_ptr<UComponent> GetOwner(void) const;
virtual bool SetOwner(std::shared_ptr<UComponent> owner);

// ���������� ��������� �� �������� ��������� ���� ��������
std::shared_ptr<UComponent> GetMainOwner(void) const;
virtual void SetMainOwner(std::shared_ptr<UComponent> mainowner);

// ���������� ��������� ��������� ����� �������
std::shared_ptr<UStorage> GetStorage(void) const;
virtual bool SetStorage(std::shared_ptr<UStorage> storage);

// ���������� ����� ���������� ����� �������
std::shared_ptr<UEnvironment> GetEnvironment(void) const;
virtual bool SetEnvironment(std::shared_ptr<UEnvironment> environment);

// ��������� �� ������
std::shared_ptr<ULoggerEnv> GetLogger(void) const;
virtual bool SetLogger(std::shared_ptr<ULoggerEnv> logger);

/// ���������� ������ �� ����� ���������� ������� �� Environment.
/// ���� Environment ����������� �� ���������� ��������� �� ��������
/// DummyTime
const UTimeControl& GetTime(void) const;

/// ���������� ��������� �� ����� �� ���������
UAFont* GetDefaultFont(void);

/// ���������� �������� �����
UAFont* GetFont(const string &name, int size);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ������������� ������
UId GetClass(void) const;
bool SetClass(UId value);


// ���������� ��� ������ ����������
const NameT GetCompClassName(void) const;
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// ������� ��������� ����� ������
virtual UComponent* New(void)=0;

// ������� ��������� �������� ������
virtual UContainerDescription* NewDescription(void);
virtual UContainerDescription* ANewDescription(UComponentDescription* description);

// ����������� ����� �������
void Free(void);

protected:
/// ������������ ���������� ���������� ������ ����������, �������������� ��� �����������
virtual void UpdateInternalData(void);
virtual void AUpdateInternalData(void);
// --------------------------

// --------------------------
// ������ ������� � ����������
// --------------------------
public:
// ���������� ��������� �� ������ ��������
std::shared_ptr<UIProperty> FindProperty(const NameT &name) const;
std::shared_ptr<UIProperty> FindProperty(const NameT &name);

// ���������� �������� ��������� �� ����� 'name'
std::shared_ptr<UVariableData> GetProperty(const NameT &name, std::shared_ptr<UVariableData> values) const;
std::string& GetPropertyValue(const NameT &name, std::string &values) const;

// ���������� �������� ��������� �� ����� 'name'
template<typename T>
const T* AccessPropertyData(const NameT &name) const;

template<typename T>
T* AccessPropertyData(const NameT &name);

// ������������� �������� ��������� �� ����� 'name'
void SetProperty(const NameT &name, std::shared_ptr<UVariableData> values);
void SetPropertyValue(const NameT &name, const std::string &values);

// ���������� ������ Id ����������, ������������ ���������������
// � ���� �������
const UComponent::VariableMapT& GetPropertiesList(void) const;

// ���� ��� �������� �� ��������� �� ����
const NameT& FindPropertyName(const UIProperty* prop) const;

// ���� ��� �������� �� ��������� �� ����
unsigned int FindPropertyType(const UIProperty* prop) const;

// ���� ���������� �������� � ������� �� ��������� �� ����
UComponent::VariableMapCIteratorT FindPropertyVariable(const UIProperty* prop) const;

// �������� ��� ��������� ����� ������� � ������ 'comp', ���� ��������.
// ���������� ������ �������� ���� type
virtual void CopyProperties(std::shared_ptr<UComponent> comp, unsigned int type) const;
// --------------------------

// --------------------------
// ������� ������ ���������� �����������
// --------------------------
public:
// ��������� �������� � ������ 'name' � ������� �����������
// ���������� � ��������� ��� ���������� ������
// ������ ���������� � ������������� �������
void AddLookupProperty(const NameT &name, unsigned int type, UIProperty* property, bool delenable=true);

// �������� ��� ���������
bool ChangeLookupPropertyType(const NameT &name, unsigned int type);

protected:
// ������� �������� � ������ 'name' �� ������� �����������
// ����������
void DelLookupProperty(const NameT &name);

// ������� ��� ������� ������������
void ClearLookupPropertyTable(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������ ����������
// --------------------------
public:
// ��������� ����� �������� �������� � ������ 'name' � ������� �����������
// ����� ������� � ��������� ��� ���������� ������
// ������ ���������� � ������������� �������
UId AddLookupShare(const NameT &name, std::shared_ptr<UIShare> property);
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
protected:
/// ���������� �������
bool AddAlias(const std::string &alias, const std::string &property_name);

/// �������� �������
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
// �������� � ������������ ����������
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)=0;

// ����� ���������� ��� ��������
virtual unsigned int GetType(void) const=0;

// ����� ���������� ��������� ��� ��������
virtual const std::string& GetName(void) const=0;

// ����� ���������� ��������� ����������-��������� ��������
virtual UContainer* GetOwner(void) const=0;

// ����� ���������� ��������� ��� ����������-��������� ��������
virtual std::string GetOwnerName(void) const=0;

// ����� ���������� ��������� ��� ������-��������� ��������
virtual std::string GetOwnerClassName(void) const=0;

// ����� ���������� �������� �������� � �����
virtual bool Save(std::shared_ptr<USerStorage> storage, bool simplemode=false)=0;

// ����� ������ �������� �������� �� ������
virtual bool Load(std::shared_ptr<USerStorage> storage, bool simplemode=false)=0;

// ����� ���������� ��������� �� ������� ������, ���������� ������ ��������
virtual const void* GetMemoryArea(void)=0;

// ����� �������� �������� ������ �������� �� ������� ������
// �������� ���������� ����������� ��������� ���� ������
// ������� ��������� ���������� � ��������� �� ����������� ��� ������
virtual bool ReadFromMemory(const void *buffer)=0;

// ���������� �������� ��� ��������� ��������
virtual const type_info& GetLanguageType(void) const=0;

// ����� ���������� ��� ����� �������� � ������ ���������
virtual bool CompareLanguageType(const UIProperty &dt) const=0;

// ���������� �������� ��� ��������� �������� ��� ������ ��������
virtual const type_info& GetElemLanguageType(void) const=0;

// ����� ���������� ��� ����� �������� � ������ ��������� (�� ������ ��������)
virtual bool CompareElemLanguageType(const UIProperty &dt) const=0;

// --------------------------
// ������ ���������� �������
// --------------------------
/// ���������� ��� �������� �����-������
virtual int GetIoType(void) const=0;

/// ���������� ����� ���������� ������ �������� (��)
virtual ULongTime GetUpdateTime(void) const=0;

/// ������������� ����� ���������� ������ ��������
virtual void SetUpdateTime(ULongTime value)=0;

/// ���������� ����� ���������� �� ����
virtual void ResetUpdateTime(void)=0;
// --------------------------

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

 /// ���������� ��������� �� ���������-��������
 virtual UItem* GetItem(int index=0)=0;

 /// ���������� ��� ������������� ����������
 virtual std::string GetItemName(int index=0) const=0;

 /// ���������� ������ ��� ������������� ����������
 virtual std::string GetItemFullName(int index=0) const=0;

 /// ���������� ��� ������������� ������
 virtual std::string GetItemOutputName(int index=0) const=0;

 // ������������� ��������� �� ������ �����
 virtual bool SetPointer(int index, UIPropertyOutput* property)=0;

 /// ���������� ��������� �� ������
 virtual bool ResetPointer(int index, UIPropertyOutput* property)=0;

 /// ���������� true, ���� �� ������������ ������ ����� ������
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

 /// ���������� ����� ������������ ������
 virtual size_t GetNumConnectors(void) const;

 /// ���������� ��������� �� ���������-��������
 virtual UConnector* GetConnector(int index);

 /// ���������� ��� ������������� ����� ����������-���������
 virtual std::string GetConnectorInputName(int index) const;

 /// ���������� ��������� �� �������� ������������� ����� ����������-���������
 virtual UIPropertyInput* GetConnectorProperty(int index);

 /// �������� ��������� �������-������
 virtual void UpdateConnectedPointers(void);
};


// ����� ���������� ������ ����������
class RDK_LIB_TYPE UIShare
{
public:
 // ����� ���������� Id ������ ��������
// virtual int GetId(void) const=0;

 // ����� ���������� ��������� ��� ������-��������� ������ ��������
// virtual std::string GetOwnerName(void) const=0;

 // ����� ������������� ������ ��������
 virtual bool Init(std::shared_ptr<UComponent> main_owner)=0;

 // ����� ��������������� ������ ��������
 virtual bool UnInit(void)=0;
};

// ���������� �������� ��������� �� ����� 'name'
template<typename T>
const T* UComponent::AccessPropertyData(const NameT &name) const
{
 std::shared_ptr<UIProperty> property=FindProperty(name);
 if(!property)
  return 0;

 if(property->GetLanguageType() != typeid(T))
  return 0;

 return reinterpret_cast<T*>(property->GetMemoryArea());
}

template<typename T>
T* UComponent::AccessPropertyData(const NameT &name)
{
 std::shared_ptr<UIProperty> property=FindProperty(name);
 if(!property)
  return 0;

 if(property->GetLanguageType() != typeid(T))
  return 0;

 return (T*)(property->GetMemoryArea());
}

}

#endif

