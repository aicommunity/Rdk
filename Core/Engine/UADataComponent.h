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

// ��������� ������� ���������
struct UVariable
{
// Id ���������
UId Id;

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
UVariable(UId id, UEPtr<UIProperty> prop, unsigned int type=0);
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

   /*
// ��������� ������� ���������
struct USharedVariable
{
 // Id ���������
 UId Id;

 // ��������� �� ��������
 UEPtr<UIShare> Property;

// --------------------------
// ������������ � �����������
// --------------------------
USharedVariable(void);
USharedVariable(UId id, UEPtr<UIShare> prop);
USharedVariable(const USharedVariable &copy);
virtual ~USharedVariable(void);
// --------------------------
};    */

class UADataComponent: public UAComponent
{
public: // ���� ������
typedef std::map<NameT,UVariable> VariableMapT;
typedef std::map<NameT,UVariable>::iterator VariableMapIteratorT;
typedef std::map<NameT,UVariable>::const_iterator VariableMapCIteratorT;

typedef std::map<UId,UEPtr<UIShare> > ShareMapT;
typedef std::map<UId,UEPtr<UIShare> >::iterator ShareMapIteratorT;
typedef std::map<UId,UEPtr<UIShare> >::const_iterator ShareMapCIteratorT;

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
UADataComponent(void);
virtual ~UADataComponent(void);
// --------------------------

// --------------------------
// ������ ������� � �������� �����������
// --------------------------
public:
// ���������� ��� ��������� �� ��� Id
const NameT& GetPropertyName(const UId &id) const;

// ���������� Id ��������� �� ��� �����
const UId& GetPropertyId(const NameT &name) const;
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������� ��������� �������� ������
virtual UContainerDescription* NewDescription(void);
virtual UContainerDescription* ANewDescription(UComponentDescription* description);
// --------------------------

// --------------------------
// ������ ������� � ����������
// --------------------------
// ���������� �������� ��������� �� Id 'id'
UEPtr<UVariableData> GetProperty(const UId &id, UEPtr<UVariableData> values) const;
std::string& GetPropertyValue(const UId &id, std::string &values) const;

// ���������� �������� ��������� �� ����� 'name'
UEPtr<UVariableData> GetProperty(const NameT &name, UEPtr<UVariableData> values) const;
std::string& GetPropertyValue(const NameT &name, std::string &values) const;

// ������������� �������� ��������� �� Id 'id'
void SetProperty(const UId &id, UEPtr<UVariableData> values);
void SetPropertyValue(const UId &id, const std::string &values);

// ������������� �������� ��������� �� ����� 'name'
void SetProperty(const NameT &name, UEPtr<UVariableData> values);
void SetPropertyValue(const NameT &name, const std::string &values);

// ���������� ������ Id ����������, ������������ ���������������
// � ���� �������
const UADataComponent::VariableMapT& GetPropertiesList(void) const;

// ���� ��� �������� �� ��������� �� ����
const NameT& FindPropertyName(UEPtr<const UIProperty> prop) const;

// ���� ��� �������� �� ��������� �� ����
unsigned int FindPropertyType(UEPtr<const UIProperty> prop) const;

// ���� ���������� �������� � ������� �� ��������� �� ����
UADataComponent::VariableMapCIteratorT FindPropertyVariable(UEPtr<const UIProperty> prop) const;

// �������� ��� ��������� ����� ������� � ������ 'comp', ���� ��������.
// ���������� ������ �������� ���� type
virtual void CopyProperties(UEPtr<UADataComponent> comp, unsigned int type) const;
// --------------------------

// --------------------------
// ������� ������ ���������� �����������
// --------------------------
public:
// ��������� �������� � ������ 'name' � ������� �����������
// ���������� � ��������� ��� ���������� ������
// ������ ���������� � ������������� �������
UId AddLookupProperty(const NameT &name, unsigned int type, UEPtr<UIProperty> property, bool delenable=true);

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
struct EPropertyIdNotExist: public EIdNotExist
{
EPropertyIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id �������� ��� ����������
struct EPropertyIdAlreadyExist: public EIdAlreadyExist
{
EPropertyIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

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
virtual void SetVariable(UADataComponent::VariableMapCIteratorT &var)=0;

// ����� ���������� ��� ��������
virtual unsigned int GetType(void) const=0;

// ����� ���������� ��������� ��� ��������
virtual const std::string& GetName(void) const=0;

// ����� ���������� ��������� ��� ������-��������� ��������
virtual std::string GetOwnerName(void) const=0;

// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<UVariableData> storage, bool simplemode=false)=0;

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<UVariableData> storage, bool simplemode=false)=0;
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
 virtual bool Init(UEPtr<UADataComponent> main_owner)=0;

 // ����� ��������������� ������ ��������
 virtual bool UnInit(void)=0;
};

}

#include "UProperty.h"
#include "ULocalProperty.h"
#include "UShare.h"

#endif

