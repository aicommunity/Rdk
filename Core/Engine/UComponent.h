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

class RDK_LIB_TYPE UComponent: public UModule
{
friend class UStorage;
public: // ���� ������
typedef std::map<NameT,UEPtr<UIProperty> > VariableMapT;
typedef std::map<NameT,UEPtr<UIProperty> >::iterator VariableMapIteratorT;
typedef std::map<NameT,UEPtr<UIProperty> >::const_iterator VariableMapCIteratorT;

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

// ��������� �� ������
UEPtr<ULoggerEnv> Logger;

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

// ��������� �� ������
UEPtr<ULoggerEnv> const GetLogger(void) const;
virtual bool SetLogger(UEPtr<ULoggerEnv> logger);

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

// ���������� ��������� ��� ���� �������� �� ��������� ����
static std::string GetPropertyTypeNameByType(unsigned int type);

// ���������� ��� �������� �� ���������� �����
static unsigned int GetPropertyTypeByTypeName(const std::string &name);
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
const UEPtr<UIProperty> FindProperty(const NameT &name) const;
UEPtr<UIProperty> FindProperty(const NameT &name);

// ���������� ��������� �� ������ ��������
template<typename T>
const UEPtr<T> FindPropertyByType(const NameT &name, unsigned int mask) const;

template<typename T>
UEPtr<T> FindPropertyByType(const NameT &name, unsigned int mask);

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

template<typename T>
void FindPropertiesByType(std::vector<UEPtr<T> > &properties, unsigned int mask) const;

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
void AddLookupProperty(UEPtr<UIProperty> property);

// �������� ��� ���������
bool ChangeLookupPropertyType(const NameT &name, unsigned int type);

// ������� �������� � ������ 'name' �� ������� �����������
// ����������
void DelLookupProperty(const NameT &name, bool force_no_delete);

protected:
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
explicit EPropertyNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ��� �������� ��� ����������
struct EPropertyNameAlreadyExist: public ENameAlreadyExist
{
explicit EPropertyNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// ��� ������ �� �������
struct EAliasNameNotExist: public ENameNotExist
{
explicit EAliasNameNotExist(const std::string &name) : ENameNotExist(name) {};
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




// ����� ���������� ������ ����������
class RDK_LIB_TYPE UIShare
{
public:
 // ����� ������������� ������ ��������
 virtual bool Init(UEPtr<UComponent> main_owner)=0;

 // ����� ��������������� ������ ��������
 virtual bool UnInit(void)=0;
};

}

#endif

