#ifndef UADATA_COMPONENT_H
#define UADATA_COMPONENT_H

#include "UAComponent.h"
#include "UAEnvSupport.h"
#include "UContainerDescription.h"
#include "../Serialize/Serialize.h"

namespace RDK {

typedef Serialize::USerStorage UVariableData;

class UADataComponent;

// ����� ������������ �������
class UIProperty
{
public:
 // ����� ���������� ��������� ��� ��������
 virtual const std::string& GetName(void) const=0;

 // ����� ���������� ��������� ��� ������-��������� ��������
 virtual std::string GetOwnerName(void) const=0;

 // ����� ���������� �������� �������� � �����
 virtual bool Save(UEPtr<UVariableData> storage, bool simplemode=false)=0;

 // ����� ������ �������� �������� �� ������
 virtual bool Load(UEPtr<UVariableData> storage, bool simplemode=false)=0;
};



// ��������� ������� ���������
struct UVariable
{
 // Id ���������
 UId Id;

 // ��������� �� ��������
 UEPtr<UIProperty> Property;

 // ���� ���������� �������� ������ �� ������� ��������� Property
 bool DelEnable;

// --------------------------
// ������������ � �����������
// --------------------------
UVariable(void);
UVariable(UId id, UEPtr<UIProperty> prop);
UVariable(const UVariable &copy);
virtual ~UVariable(void);
// --------------------------
};



class UADataComponent: public UAComponent
{
public: // ���� ������
typedef std::map<NameT,UVariable> VariableMapT;
typedef std::map<NameT,UVariable>::iterator VariableMapIteratorT;
typedef std::map<NameT,UVariable>::const_iterator VariableMapCIteratorT;

private: // ��������� ��������
//protected: // ��������� ��������
// ������� ������������ ���� � Id ���������� �������
VariableMapT PropertiesLookupTable;

// ������� ������������ ���� � Id ������ ��������� �������
VariableMapT StateLookupTable;

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

// ���������� ������ ��� ��������� ��� �������� RDK, � �������� '*'
//NameT GetPropertyLongName(const NameT &name) const;
//NameT GetPropertyLongName(const UId &id) const;

// ���������� ��� ���������� ��������� �� ��� Id
const NameT& GetStateName(const UId &id) const;

// ���������� Id ���������� ��������� �� ��� �����
const UId& GetStateId(const NameT &name) const;

// ���������� ������ ��� ���������� ��������� ��� �������� RDK, � �������� '*'
//NameT GetStateLongName(const NameT &name) const;
//NameT GetStateLongName(const UId &id) const;
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������� ��������� �������� ������
virtual UContainerDescription* NewDescription(void);
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

// �������� ��� ��������� ����� ������� � ������ 'comp', ���� ��������.
virtual void CopyProperties(UEPtr<UADataComponent> comp) const;
// --------------------------

// --------------------------
// ������ ������� � ���������� ���������
// --------------------------
// ���������� �������� ���������� ��������� �� Id 'id'
virtual UEPtr<UVariableData> GetState(const UId &id, UEPtr<UVariableData> values) const;
virtual std::string& GetStateValue(const UId &id, std::string &values) const;

// ���������� �������� ���������� ��������� �� ����� 'name'
UEPtr<UVariableData> GetState(const NameT &name, UEPtr<UVariableData> values) const;
std::string& GetStateValue(const NameT &name, std::string &values) const;

// ������������� �������� ���������� ��������� �� Id 'id'
virtual void SetState(const UId &id, UEPtr<UVariableData> values);
virtual void SetStateValue(const UId &id, const std::string &values);

// ������������� �������� ���������� ��������� �� ����� 'name'
void SetState(const NameT &name, UEPtr<UVariableData> values);
void SetStateValue(const NameT &name, const std::string &values);

// ���������� ������ ���� � Id ���������� ���������, ������������ ���������������
// � ���� �������
const UADataComponent::VariableMapT& GetStateList(void) const;

// ���� ��� �������� �� ��������� �� ����
const NameT& FindStateName(UEPtr<const UIProperty> prop) const;

// �������� ��� ���������� ��������� ����� ������� � ������ 'comp', ���� ��������.
virtual void CopyState(UEPtr<UADataComponent> comp) const;
// --------------------------

// --------------------------
// ������� ������ ���������� �����������
// --------------------------
public:
// ��������� �������� � ������ 'name' � ������� �����������
// ���������� � ��������� ��� ���������� ������
// ������ ���������� � ������������� �������
UId AddLookupProperty(const NameT &name, UEPtr<UIProperty> property, bool delenable=true);

protected:
// ������� �������� � ������ 'name' �� ������� �����������
// ����������
void DelLookupProperty(const NameT &name);

// ������� ��� ������� ������������
void ClearLookupPropertyTable(void);

// ���������� ������ ��� ��������� ��� �������� RDK, � �������� '*'
//NameT GetPropertyLongName(const UIProperty &property) const;
// --------------------------

protected:
// --------------------------
// ������� ������ ���������� ����������
// --------------------------
public:
// ��������� ���������� ��������� � ������ 'name' � ������� �����������
// ���������� � ��������� �� ���������� ������
// ������ ���������� � ������������� �������
UId AddLookupState(const NameT &name, UEPtr<UIProperty> property, bool delenable=true);

protected:
// ������� ���������� ��������� � ������ 'name' �� ������� �����������
void DelLookupState(const NameT &name);

// ������� ��� ������� ������������
void ClearLookupStateTable(void);

// ���������� ������ ��� ���������� ��������� ��� �������� RDK, � �������� '*'
//NameT GetStateLongName(const UIProperty &property) const;
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

};

}

#include "UProperty.h"
#include "ULocalProperty.h"

#endif 

