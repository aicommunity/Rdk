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
 virtual bool Save(Serialize::USerStorage *storage)=0;

 // ����� ������ �������� �������� �� ������
 virtual bool Load(Serialize::USerStorage *storage)=0;
};



// ��������� ������� ���������
struct UVariable
{
 // Id ���������
 UId Id;

 // ��������� �� ��������
 UIProperty* Property;

 // ���� ���������� �������� ������ �� ������� ��������� Property
 bool DelEnable;

// --------------------------
// ������������ � �����������
// --------------------------
UVariable(void);
UVariable(UId id, UIProperty *prop);
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


// --------------------------
// ������ ������� � �������� �����������
// --------------------------
public:
// ���������� ��� ��������� �� ��� Id
const NameT& GetPropertyName(const UId &id) const;

// ���������� Id ��������� �� ��� �����
const UId& GetPropertyId(const NameT &name) const;

// ���������� ������ ��� ��������� ��� �������� NMSDK, � �������� '*'
//NameT GetPropertyLongName(const NameT &name) const;
//NameT GetPropertyLongName(const UId &id) const;

// ���������� ��� ���������� ��������� �� ��� Id
const NameT& GetStateName(const UId &id) const;

// ���������� Id ���������� ��������� �� ��� �����
const UId& GetStateId(const NameT &name) const;

// ���������� ������ ��� ���������� ��������� ��� �������� NMSDK, � �������� '*'
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
UVariableData* GetProperty(const UId &id, UVariableData *values) const;

// ���������� �������� ��������� �� ����� 'name'
UVariableData* GetProperty(const NameT &name, UVariableData *values) const;

// ������������� �������� ��������� �� Id 'id'
bool SetProperty(const UId &id, UVariableData *values);

// ������������� �������� ��������� �� ����� 'name'
bool SetProperty(const NameT &name, UVariableData *values);

// ���������� ������ Id ����������, ������������ ���������������
// � ���� �������
const UADataComponent::VariableMapT& GetPropertiesList(void) const;

// ���� ��� �������� �� ��������� �� ����
const NameT& FindPropertyName(const UIProperty *prop) const;

// �������� ��� ��������� ����� ������� � ������ 'comp', ���� ��������.
virtual bool CopyProperties(UEPtr<UADataComponent> comp) const;
// --------------------------

// --------------------------
// ������ ������� � ���������� ���������
// --------------------------
// ���������� �������� ���������� ��������� �� Id 'id'
virtual UVariableData* GetState(const UId &id, UVariableData *values) const;

// ���������� �������� ���������� ��������� �� ����� 'name'
UVariableData* GetState(const NameT &name, UVariableData *values) const;

// ������������� �������� ���������� ��������� �� Id 'id'
virtual bool SetState(const UId &id, UVariableData *values);

// ������������� �������� ���������� ��������� �� ����� 'name'
bool SetState(const NameT &name, UVariableData *values);

// ���������� ������ ���� � Id ���������� ���������, ������������ ���������������
// � ���� �������
const UADataComponent::VariableMapT& GetStateList(void) const;

// ���� ��� �������� �� ��������� �� ����
const NameT& FindStateName(const UIProperty *prop) const;

// �������� ��� ���������� ��������� ����� ������� � ������ 'comp', ���� ��������.
virtual bool CopyState(UEPtr<UADataComponent> comp) const;
// --------------------------

// --------------------------
// ������� ������ ���������� �����������
// --------------------------
public:
// ��������� �������� � ������ 'name' � ������� �����������
// ���������� � ��������� ��� ���������� ������
// ������ ���������� � ������������� �������
UId AddLookupProperty(const NameT &name, UIProperty *property, bool delenable=true);

protected:
// ������� �������� � ������ 'name' �� ������� �����������
// ����������
bool DelLookupProperty(const NameT &name);

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
UId AddLookupState(const NameT &name,UIProperty *property, bool delenable=true);

protected:
// ������� ���������� ��������� � ������ 'name' �� ������� �����������
bool DelLookupState(const NameT &name);

// ������� ��� ������� ������������
void ClearLookupStateTable(void);

// ���������� ������ ��� ���������� ��������� ��� �������� NMSDK, � �������� '*'
//NameT GetStateLongName(const UIProperty &property) const;
// --------------------------
};

}

#include "UProperty.h"
#include "ULocalProperty.h"

#endif 

