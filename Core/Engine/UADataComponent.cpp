#ifndef UADATA_COMPONENT_CPP
#define UADATA_COMPONENT_CPP
                            
#include "UADataComponent.h"                            

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
UVariable::UVariable(void)
{
 Id=ForbiddenId;
// Property=0;
 DelEnable=true;
}

UVariable::UVariable(UId id, UEPtr<UIProperty> prop)
 : Id(id), Property(prop), DelEnable(true)
{
}

UVariable::UVariable(const UVariable &copy)
{
 Id=copy.Id;
 Property=copy.Property;
 DelEnable=copy.DelEnable;
}


UVariable::~UVariable(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � �������� �����������
// --------------------------
// ���������� ��� ��������� �� ��� Id
const NameT& UADataComponent::GetPropertyName(const UId &id) const
{
 for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
						 J=PropertiesLookupTable.end(); I!=J;++I)
 {
   if(I->second.Id == id)
	return I->first;
 }
 throw new EPropertyIdNotExist(id);
}

// ���������� Id ��������� �� ��� �����
const UId& UADataComponent::GetPropertyId(const NameT &name) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);
 if(I == PropertiesLookupTable.end())
   throw new EPropertyNameNotExist(name);

 return I->second.Id;
}
 /*
// ���������� ������ ��� ��������� ��� �������� RDK, � �������� '*'
NameT UADataComponent::GetPropertyLongName(const NameT &name) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);
 if(I == PropertiesLookupTable.end())
  return NameT();
 else return GetPropertyLongName(*(I->second.Property));
}

NameT UADataComponent::GetPropertyLongName(const UId &id) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                                 J=PropertiesLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return GetPropertyLongName(*(I->second.Property));
   ++I;
  }
 return NameT();
}
		 */

// ���������� ��� ���������� ��������� �� ��� Id
const NameT& UADataComponent::GetStateName(const UId &id) const
{
 for(VariableMapCIteratorT I=StateLookupTable.begin(),
								 J=StateLookupTable.end(); I!=J;++I)
 {
  if(I->second.Id == id)
   return I->first;
 }
 throw new EStateIdNotExist(id);
}

// ���������� Id ���������� ��������� �� ��� �����
const UId& UADataComponent::GetStateId(const NameT &name) const
{
 VariableMapCIteratorT I=StateLookupTable.find(name);
 if(I == StateLookupTable.end())
  throw new EStateNameNotExist(name);

 return I->second.Id;
}
		 /*
// ���������� ������ ��� ���������� ��������� ��� �������� NMSDK, � �������� '*'
NameT UADataComponent::GetStateLongName(const NameT &name) const
{
 VariableMapCIteratorT I=StateLookupTable.find(name);
 if(I == StateLookupTable.end())
  return NameT();
 else return GetStateLongName(*(I->second.Property));
}

NameT UADataComponent::GetStateLongName(const UId &id) const
{
 VariableMapCIteratorT I=StateLookupTable.begin(),
                                 J=StateLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return GetStateLongName(*(I->second.Property));
   ++I;
  }
 return NameT();
}               */
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������� ��������� �������� ������
UContainerDescription* UADataComponent::NewDescription(void)
{
 UContainerDescription* result=new UContainerDescription;

 result->SetClassId(sntoa(Class));

 VariableMapIteratorT I=PropertiesLookupTable.begin();
 UParameterDescription dummydescr;
 while(I != PropertiesLookupTable.end())
 {
  result->SetParameter(I->first,dummydescr);
  ++I;
 }

 return result;
}
// --------------------------

// --------------------------
// ������ ������� � ����������
// --------------------------
// ���������� �������� ��������� �� Id 'id'
UEPtr<UVariableData> UADataComponent::GetProperty(const UId &id, UEPtr<UVariableData> values) const
{
 for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
							J=PropertiesLookupTable.end(); I!=J;++I)
 {
  if(I->second.Id == id)
  {
   I->second.Property->Save(values);
   return values;
  }
 }

 return values;
}

// ���������� �������� ��������� �� ����� 'name'
UEPtr<UVariableData> UADataComponent::GetProperty(const NameT &name, UEPtr<UVariableData> values) const
{
 return GetProperty(GetPropertyId(name),values);
}

// ������������� �������� ��������� �� Id 'id'
void UADataComponent::SetProperty(const UId &id, UEPtr<UVariableData> values)
{
 for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
							 J=PropertiesLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Id == id)
  {
   I->second.Property->Load(values);
   return;
  }
 }
}

// ������������� �������� ��������� �� ����� 'name'
void UADataComponent::SetProperty(const NameT &name, UEPtr<UVariableData> values)
{
 SetProperty(GetPropertyId(name),values);
}

const UADataComponent::VariableMapT& UADataComponent::GetPropertiesList(void) const
{
 return PropertiesLookupTable;
}

// �������� ��� ��������� ����� ������� � ������ 'comp', ���� ��������.
void UADataComponent::CopyProperties(UEPtr<UADataComponent> comp) const
{
 Serialize::USerStorageBinary databuffer;
 for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
							J=PropertiesLookupTable.end(); I!=J; ++I)
 {
  databuffer.clear();
  comp->SetProperty(I->second.Id,GetProperty(I->second.Id,&databuffer));
 }
}

// ���� ��� �������� �� ��������� �� ����
const NameT& UADataComponent::FindPropertyName(UEPtr<const UIProperty> prop) const
{
  for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
						J=PropertiesLookupTable.end(); I!=J; ++I)
  {
   if(I->second.Property == prop)
    return I->first;
  }
 return ForbiddenName;
}
// --------------------------

// --------------------------
// ������ ������� � ���������� ���������
// --------------------------
// ���������� �������� ���������� ��������� �� Id 'id'
UEPtr<UVariableData> UADataComponent::GetState(const UId &id, UEPtr<UVariableData> values) const
{
 for(VariableMapCIteratorT I=StateLookupTable.begin(),
							 J=StateLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Id == id)
  {
   I->second.Property->Save(values);
   return values;
  }
 }

 return values;
}

// ���������� �������� ���������� ��������� �� ����� 'name'
UEPtr<UVariableData> UADataComponent::GetState(const NameT &name, UEPtr<UVariableData> values) const
{
 return GetState(GetStateId(name),values);
}

// ������������� �������� ���������� ��������� �� Id 'id'
void UADataComponent::SetState(const UId &id, UEPtr<UVariableData> values)
{
 for(VariableMapCIteratorT I=StateLookupTable.begin(),
							 J=StateLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Id == id)
  {
   I->second.Property->Load(values);
   return;
  }
 }
}

// ������������� �������� ���������� ��������� �� ����� 'name'
void UADataComponent::SetState(const NameT &name, UEPtr<UVariableData> values)
{
 SetState(GetStateId(name),values);
}

// ���������� ������ ���� � Id ���������� ���������, ������������ ���������������
// � ���� �������
const UADataComponent::VariableMapT& UADataComponent::GetStateList(void) const
{
 return StateLookupTable;
}

// ���� ��� �������� �� ��������� �� ����
const NameT& UADataComponent::FindStateName(UEPtr<const UIProperty> prop) const
{
 if(!prop)
  return ForbiddenName;

 for(VariableMapCIteratorT I=StateLookupTable.begin(),
						J=StateLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Property == prop)
   return I->first;
 }
 return ForbiddenName;
}

// �������� ��� ���������� ��������� ����� ������� � ������ 'comp', ���� ��������.
void UADataComponent::CopyState(UEPtr<UADataComponent> comp) const
{
 Serialize::USerStorageBinary serstorage;
 for(VariableMapCIteratorT I=StateLookupTable.begin(),
					   J=StateLookupTable.end(); I!=J; ++I)
 {
  comp->SetState(I->second.Id,GetState(I->second.Id,&serstorage));
  ++I;
 }
}
// --------------------------

// --------------------------
// ������� ������ ���������� �����������
// --------------------------
// ��������� �������� � ������ 'name' � ������� �����������
// ���������� � ��������� ��� ���������� ������
// ������ ���������� � ������������� �������
UId UADataComponent::AddLookupProperty(const NameT &name, UEPtr<UIProperty> property, bool delenable)
{
 if(PropertiesLookupTable.find(name) != PropertiesLookupTable.end())
  throw new EPropertyNameAlreadyExist(name);

 UVariable P(1,property);
 P.DelEnable=delenable;

 for(VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end(); I!=J; ++I)
 {
  if(P.Id <= I->second.Id)
   P.Id=I->second.Id+1;
 }

 PropertiesLookupTable.insert(make_pair(name,P));
 return P.Id;
}

// ������� �������� � ������ 'name' �� ������� �����������
// ����������
void UADataComponent::DelLookupProperty(const NameT &name)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);

 if(I == PropertiesLookupTable.end())
  throw new EPropertyNameNotExist(name);

 if(I->second.DelEnable)
  delete I->second.Property;
 PropertiesLookupTable.erase(I);
}

// ������� ��� ������� ������������
void UADataComponent::ClearLookupPropertyTable(void)
{
 for(VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Property && I->second.DelEnable)
   delete I->second.Property;
 }
 PropertiesLookupTable.clear();
}
						 /*
// ���������� ������ ��� ��������� ��� �������� RDK, � �������� '*'
NameT UADataComponent::GetPropertyLongName(const UIProperty &property) const
{
 NameT name=property.GetOwnerName();

 size_t i=name.find("RDK::");
 if(i == NameT::npos)
  i=0;
 else
  i=5;

 size_t j=name.find_last_of("*");
 if(j == NameT::npos)
  j=name.size()-i;
 else
  j-=i+1;

 return name.substr(i,j);
}                    */
// --------------------------

// --------------------------
// ������� ������ ���������� ����������
// --------------------------
// ��������� ���������� ��������� � ������ 'name' � ������� �����������
// ���������� � ��������� �� ���������� ������
// ������ ���������� � ������������� �������
UId UADataComponent::AddLookupState(const NameT &name, UEPtr<UIProperty> property, bool delenable)
{
 UVariable P(1,property);
 P.DelEnable=delenable;

 if(StateLookupTable.find(name) != StateLookupTable.end())
  throw new EStateNameAlreadyExist(name);

 for(VariableMapIteratorT I=StateLookupTable.begin(),
					  J=StateLookupTable.end(); I!=J; ++I)
 {
  if(P.Id <= I->second.Id)
   P.Id=I->second.Id+1;
 }

 StateLookupTable.insert(make_pair(name,P));
 return P.Id;
}

// ������� ���������� ��������� � ������ 'name' �� ������� �����������
void UADataComponent::DelLookupState(const NameT &name)
{
 VariableMapIteratorT I=StateLookupTable.find(name);

 if(I == StateLookupTable.end())
  throw new EStateNameNotExist(name);

 if(I->second.DelEnable)
  delete I->second.Property;
 StateLookupTable.erase(I);
}

// ������� ��� ������� ������������
void UADataComponent::ClearLookupStateTable(void)
{
 for(VariableMapIteratorT I=StateLookupTable.begin(),
					  J=StateLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Property && I->second.DelEnable)
   delete I->second.Property;
 }
 StateLookupTable.clear();
}

// ���������� ������ ��� ���������� ��������� ��� �������� NMSDK, � �������� '*'
/*NameT UADataComponent::GetStateLongName(const UIProperty &property) const
{
 NameT name=property.GetOwnerName();

 size_t i=name.find("RDK::");
 if(i == NameT::npos)
  i=0;
 else
  i=5;

 size_t j=name.find_last_of("*");
 if(j == NameT::npos)
  j=name.size()-i;
 else
  j-=i+1;

 return name.substr(i,j);
}       */
// --------------------------



}

#endif 

