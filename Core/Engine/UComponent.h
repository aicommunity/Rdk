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
#include "../Serialize/Serialize.h"

#ifndef RDK_PROPERTY_TYPES
#define RDK_PROPERTY_TYPES
// �������� ���� �������� (������� �����) pt - Property Type
// 0x1 - ��������
// 0x2 - ���������� ���������
// 0x4 - ��������� ����������
// 0x8 - ����
enum {ptParameter=1, ptState=2, ptTemp=4, ptInput=8, ptOutput=16, ptAny=255};

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
struct UVariable
{
// ��������� �� ��������
UEPtr<UIProperty> Property;

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
UVariable(UEPtr<UIProperty> prop, unsigned int type=0);
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


class UComponent: public UModule
{
friend class UStorage;
public: // ���� ������
typedef std::map<NameT,UVariable> VariableMapT;
typedef std::map<NameT,UVariable>::iterator VariableMapIteratorT;
typedef std::map<NameT,UVariable>::const_iterator VariableMapCIteratorT;

typedef std::map<UId,UEPtr<UIShare> > ShareMapT;
typedef std::map<UId,UEPtr<UIShare> >::iterator ShareMapIteratorT;
typedef std::map<UId,UEPtr<UIShare> >::const_iterator ShareMapCIteratorT;

public: // ������ �������� ����������
class IException: public UException {};

protected: // �������� ��������
// ����, ������������ ��������� �������� �����������
// ��� ������������
bool StaticFlag;

// ��������� �� ��������� ���� ��������
UEPtr<UComponent> Owner;

// ��������� �� �������� ��������� ���� ��������
// ������������� ��������������� ��� ���� �������� ��������
UEPtr<UComponent> MainOwner;

// ��������� �� ��������� ��������� ����� �������
UEPtr<UStorage> Storage;

// ��������� �� ����� ���������� ����� �������
UEPtr<UEnvironment> Environment;

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


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UComponent(void);
virtual ~UComponent(void);
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� ����, ������������ ��������� �������� �����������
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

// ���������� ����� ���������� ����� �������
UEPtr<UEnvironment> const GetEnvironment(void) const;
virtual bool SetEnvironment(UEPtr<UEnvironment> environment);
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
UEPtr<UIProperty> FindProperty(const NameT &name);

// ���������� �������� ��������� �� ����� 'name'
UEPtr<UVariableData> GetProperty(const NameT &name, UEPtr<UVariableData> values) const;
std::string& GetPropertyValue(const NameT &name, std::string &values) const;

// ������������� �������� ��������� �� ����� 'name'
void SetProperty(const NameT &name, UEPtr<UVariableData> values);
void SetPropertyValue(const NameT &name, const std::string &values);

// ���������� ������ Id ����������, ������������ ���������������
// � ���� �������
const UComponent::VariableMapT& GetPropertiesList(void) const;

// ���� ��� �������� �� ��������� �� ����
const NameT& FindPropertyName(UEPtr<const UIProperty> prop) const;

// ���� ��� �������� �� ��������� �� ����
unsigned int FindPropertyType(UEPtr<const UIProperty> prop) const;

// ���� ���������� �������� � ������� �� ��������� �� ����
UComponent::VariableMapCIteratorT FindPropertyVariable(UEPtr<const UIProperty> prop) const;

// �������� ��� ��������� ����� ������� � ������ 'comp', ���� ��������.
// ���������� ������ �������� ���� type
virtual void CopyProperties(UEPtr<UComponent> comp, unsigned int type) const;
// --------------------------

// --------------------------
// ������� ������ ���������� �����������
// --------------------------
public:
// ��������� �������� � ������ 'name' � ������� �����������
// ���������� � ��������� ��� ���������� ������
// ������ ���������� � ������������� �������
void AddLookupProperty(const NameT &name, unsigned int type, UEPtr<UIProperty> property, bool delenable=true);

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
UId AddLookupShare(const NameT &name, UEPtr<UIShare> property);
// --------------------------

public: // ����������
// Id �������� �� ������
/*struct EPropertyIdNotExist: public EIdNotExist
{
EPropertyIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id �������� ��� ����������
struct EPropertyIdAlreadyExist: public EIdAlreadyExist
{
EPropertyIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
}; */

// ��� �������� �� �������
struct EPropertyNameNotExist: public ENameNotExist
{
EPropertyNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ��� �������� ��� ����������
struct EPropertyNameAlreadyExist: public ENameAlreadyExist
{
EPropertyNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};
/*
// Id ���������� ��������� �� ������
struct EStateIdNotExist: public EIdNotExist
{
EStateIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id ���������� ��������� ��� ����������
struct EStateIdAlreadyExist: public EIdAlreadyExist
{
EStateIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// ��� ���������� ��������� �� �������
struct EStateNameNotExist: public ENameNotExist
{
EStateNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ��� ���������� ��������� ��� ����������
struct EStateNameAlreadyExist: public ENameAlreadyExist
{
EStateNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};
    */
};

// ����� ������������ �������
class UIProperty
{
public:
// ����� ������������� �������� ��������� �� ��������-��������� ������ �� ����
// �������� � ������������ ����������
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)=0;

// ����� ���������� ��� ��������
virtual unsigned int GetType(void) const=0;

// ����� ���������� ��������� ��� ��������
virtual const std::string& GetName(void) const=0;

// ����� ���������� ��������� ��� ����������-��������� ��������
virtual std::string GetOwnerName(void) const=0;

// ����� ���������� ��������� ��� ������-��������� ��������
virtual std::string GetOwnerClassName(void) const=0;

// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<UVariableData> storage, bool simplemode=false)=0;

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<UVariableData> storage, bool simplemode=false)=0;

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

// --------------------------
// ������ ���������� �������
// --------------------------
/// ���������� ��� �������� �����-������
virtual int GetIoType(void) const=0;

/// ���������� �������� �������� �����/������
virtual bool CheckRange(int index)=0;

// �������� �������� ������
virtual int GetMinRange(void)=0;

virtual int GetMaxRange(void)=0;
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
/// ���������� ��������� �� ������
virtual void const* GetPointer(int index) const=0;

/// ������������� ��������� �� ������
virtual bool SetPointer(int index, void* value)=0;
// --------------------------

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
: EError(), OwnerName(owner_name), PropertyName(property_name) {};

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
: EPropertyError(owner_name, property_name) {};
};

// ����� Getter �������� ��������
struct EPropertyGetterFail: public EPropertyError
{
public:
EPropertyGetterFail(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

// ����� Setter �������� ��������
struct EPropertySetterFail: public EPropertyError
{
public:
EPropertySetterFail(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

};

// ����� ���������� ������ ����������
class UIShare
{
public:
 // ����� ���������� Id ������ ��������
// virtual int GetId(void) const=0;

 // ����� ���������� ��������� ��� ������-��������� ������ ��������
// virtual std::string GetOwnerName(void) const=0;

 // ����� ������������� ������ ��������
 virtual bool Init(UEPtr<UComponent> main_owner)=0;

 // ����� ��������������� ������ ��������
 virtual bool UnInit(void)=0;
};

}

#include "UProperty.h"
#include "ULocalProperty.h"
#include "UShare.h"

#endif

