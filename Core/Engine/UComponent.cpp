/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UACOMPONENT_CPP
#define UACOMPONENT_CPP

#include "UComponent.h"
#include "UProperty.h"
#include "UStorage.h"
#include "UEnvironment.h"
#include "UComponentDescription.h"
#include "UEnvException.h"

namespace RDK {

UIProperty::~UIProperty()
{

}


/// ��������, ������������ � ������ ���������� ������� � Environment::Time
//UELockVar<UTimeControl> UComponent::DummyTime;

// --------------------------
// ������������ � �����������
// --------------------------
UVariable::UVariable(void)
{
 Property=0;
 DelEnable=true;
 Type=ptParameter & pgPublic;
}

UVariable::UVariable(UEPtr<UIProperty> prop, unsigned int type)
 : Property(prop), DelEnable(true), Type(type)
{
}

UVariable::UVariable(const UVariable &copy)
{
 Property=copy.Property;
 DelEnable=copy.DelEnable;
 Type=copy.Type;
}


UVariable::~UVariable(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������ ����� ���� ��������
unsigned int UVariable::GetPropertyType(void) const
{
 return Type & 0x000000FF;
}

// ���������� ������ ����� ������ ��������
unsigned int UVariable::GetPropertyGroup(void) const
{
 return Type & 0xFFFFFF00;
}

// ���������� ��������� ��� ���� �������� �� ��������� ����
std::string UVariable::GetPropertyTypeNameByType(unsigned int type)
{
 switch (type & 0x000000FF)
 {
 case ptAny:
  return std::string("Properties");
 case ptParameter:
  return std::string("Parameters");
 case ptState:
  return std::string("State");
 case ptTemp:
  return std::string("Temp");
 case ptInput:
  return std::string("Input");
 case ptOutput:
  return std::string("Output");

 default:
  return std::string("ptUnknown");
 }
}

// ���������� ��� �������� �� ���������� �����
unsigned int UVariable::GetPropertyTypeByTypeName(const std::string &name)
{
 if(name == "Properties")
  return ptAny;
 if(name == "Parameters")
  return ptParameter;
 if(name == "State")
  return ptState;
 if(name == "Temp")
  return ptTemp;
 if(name == "Input")
  return ptInput;
 if(name == "Output")
  return ptOutput;

 return 0;
}

// ���������� ��������� ��� ���� ��������
std::string UVariable::GetPropertyTypeName(void) const
{
 return GetPropertyTypeNameByType(GetPropertyType());
}

// ��������� ������������ ���� � ������ �������� �����
bool UVariable::CheckMask(unsigned int mask) const
{
 return (GetPropertyType() & mask) && (GetPropertyGroup() & mask);
}
// --------------------------


//class UComponent
// --------------------------
// ������������ � �����������
// --------------------------
UComponent::UComponent(void)
: StaticFlag(false)
{
 Class=ForbiddenId;
}

UComponent::~UComponent(void)
{
 ClearLookupPropertyTable();
 ShareLookupTable.clear();
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� ����, ������������ ��������� �������� �����������
// ��� ������������
bool UComponent::GetStaticFlag(void) const
{
 return StaticFlag;
}

bool UComponent::SetStaticFlag(bool value)
{
 if(StaticFlag == value)
  return true;

 StaticFlag=value;
 return true;
}


// ���������� �������� ����� �������.
UEPtr<UComponent> const UComponent::GetOwner(void) const
{
 return Owner;
}

bool UComponent::SetOwner(UEPtr<UComponent> owner)
{
 if(Owner == owner)
  return true;

 Owner=owner;
 return true;
}


// ���������� ��������� �� �������� ��������� ���� ��������
UEPtr<UComponent> const UComponent::GetMainOwner(void) const
{
 return MainOwner;
}

void UComponent::SetMainOwner(UEPtr<UComponent> mainowner)
{
 if(mainowner != MainOwner)
 {
  MainOwner=mainowner;
  UpdateInternalData();
 }
}

// ���������� ��������� ��������� ����� �������
UEPtr<UStorage> const UComponent::GetStorage(void) const
{
 return Storage;
}

bool UComponent::SetStorage(UEPtr<UStorage> storage)
{
 if(Storage == storage)
  return true;

 Storage=storage;
 UpdateInternalData();
 return true;
}

// ���������� ����� ���������� ����� �������
UEPtr<UEnvironment> const UComponent::GetEnvironment(void) const
{
 return Environment;
}

bool UComponent::SetEnvironment(UEPtr<UEnvironment> environment)
{
 if(Environment == environment)
  return true;

 Environment=environment;
 UpdateInternalData();
 return true;
}

// ��������� �� ������
UEPtr<ULoggerEnv> const UComponent::GetLogger(void) const
{
 return Logger;
}

bool UComponent::SetLogger(UEPtr<ULoggerEnv> logger)
{
 if(Logger == logger)
  return true;

 Logger=logger;
 UpdateInternalData();
 return true;
}


/// ���������� ������ �� ����� ���������� ������� �� Environment.
/// ���� Environment ����������� �� ���������� ��������� �� ��������
/// DummyTime
const UTimeControl& UComponent::GetTime(void) const
{
 if(!Environment)
  RDK_RAW_THROW(EEnvironmentNotExist());

 return Environment->GetTime();
}


/// ���������� ��������� �� ����� �� ���������
UAFont* UComponent::GetDefaultFont(void)
{
 if(!Environment)
  RDK_RAW_THROW(EEnvironmentNotExist());

 return Environment->GetFonts().GetDefaultFont();
}

/// ���������� �������� �����
UAFont* UComponent::GetFont(const string &name, int size)
{
 if(!Environment)
  RDK_RAW_THROW(EEnvironmentNotExist());

 return Environment->GetFonts().GetFont(name,size);
}
// --------------------------


// --------------------------
// ������ ���������� �������
// --------------------------
// ������������� ������
UId UComponent::GetClass(void) const
{
 return Class;
}

bool UComponent::SetClass(UId value)
{
 if(Class == value)
  return true;

 Class=value;
 return true;
}

// ���������� ��� ������ ����������
const NameT UComponent::GetCompClassName(void) const
{
 return Storage->FindClassName(Class);
}
// --------------------------



// --------------------------
// ������ ���������� ������
// --------------------------
// ������� ��������� �������� ������
UContainerDescription* UComponent::NewDescription(void)
{
 UContainerDescription* result=new UContainerDescription;

 return ANewDescription(result);
}

UContainerDescription* UComponent::ANewDescription(UComponentDescription* description)
{
 UContainerDescription* result=dynamic_cast<UContainerDescription*>(description);
 if(!result)
  return result;

 VariableMapIteratorT I=PropertiesLookupTable.begin();
 UPropertyDescription dummydescr;
 while(I != PropertiesLookupTable.end())
 {
  UEPtr<UIProperty> prop(I->second.Property);
  dummydescr.Type=prop->GetLanguageType().name();
  result->SetPropertyDescription(I->first,dummydescr);
  ++I;
 }

 return result;
}

// ����������� ����� �������
void UComponent::Free(void)
{
 if(!StaticFlag)
  delete this;
}

/// ������������ ���������� ���������� ������ ����������, �������������� ��� �����������
void UComponent::UpdateInternalData(void)
{
 AUpdateInternalData();
}

void UComponent::AUpdateInternalData(void)
{

}
// --------------------------

// --------------------------
// ������ ������� � ����������
// --------------------------
// ���������� ��������� �� ������ ��������
const UEPtr<UIProperty> UComponent::FindProperty(const NameT &name) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.end();

 // TODO: ������� ��������� ������
 if(CheckAlias(name))
  I=PropertiesLookupTable.find(GetPropertyNameByAlias(name));
 else
  I=PropertiesLookupTable.find(name);

 if(I != PropertiesLookupTable.end())
  return I->second.Property;

 return UEPtr<UIProperty>(0);
}

UEPtr<UIProperty> UComponent::FindProperty(const NameT &name)
{
 VariableMapCIteratorT I=PropertiesLookupTable.end();

 // TODO: ������� ��������� ������
 if(CheckAlias(name))
  I=PropertiesLookupTable.find(GetPropertyNameByAlias(name));
 else
  I=PropertiesLookupTable.find(name);

 if(I != PropertiesLookupTable.end())
  return I->second.Property;

 return UEPtr<UIProperty>(0);
}

UEPtr<UVariableData> UComponent::GetProperty(const NameT &name, UEPtr<UVariableData> values) const
{
 UEPtr<UIProperty> property=FindProperty(name);

 if(property)
  property->Save(values);

 return values;
}

std::string& UComponent::GetPropertyValue(const NameT &name, std::string &values) const
{
 USerStorageXML data;

 UEPtr<UIProperty> property=FindProperty(name);

 if(property)
 {
  property->Save(&data,true);
  if(data.GetNumNodes() == 0)
   values=data.GetNodeText();
  else
   data.Save(values);
 }

 return values;
}

// ������������� �������� ��������� �� ����� 'name'
void UComponent::SetProperty(const NameT &name, UEPtr<UVariableData> values)
{
 UEPtr<UIProperty> property=FindProperty(name);

 if(property)
  property->Load(values);
}

void UComponent::SetPropertyValue(const NameT &name, const std::string &values)
{
 USerStorageXML data;
 UEPtr<UIProperty> property=FindProperty(name);

 if(property)
 {
  if(values.size()>0 && values[0]=='<')
  {
   data.Load(values,"");
   data.RenameNode(property->GetName());
   property->Load(&data,true);
  }
  else
  {
   property->Save(&data,true);
   data.SetNodeText(values);
   property->Load(&data,true);
  }
 }
}

const UComponent::VariableMapT& UComponent::GetPropertiesList(void) const
{
 return PropertiesLookupTable;
}

// �������� ��� ��������� ����� ������� � ������ 'comp', ���� ��������.
// ���������� ������ �������� ���� type
void UComponent::CopyProperties(UEPtr<UComponent> comp, unsigned int type) const
{
 USerStorageXML databuffer;
 for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                            J=PropertiesLookupTable.end(); I!=J; ++I)
 {
  if(!(I->second.Type & type))
   continue;
//  databuffer.clear();
  databuffer.Destroy();
  databuffer.Create(I->first);
  comp->SetProperty(I->first,GetProperty(I->first,&databuffer));
 }
}

// ���� ��� �������� �� ��������� �� ����
const NameT& UComponent::FindPropertyName(UEPtr<const UIProperty> prop) const
{
  for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
						J=PropertiesLookupTable.end(); I!=J; ++I)
  {
   if(I->second.Property == prop)
	return I->first;
  }
 return ForbiddenName;
}

// ���� ���������� �������� � ������� �� ��������� �� ����
UComponent::VariableMapCIteratorT UComponent::FindPropertyVariable(UEPtr<const UIProperty> prop) const
{
  for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
						J=PropertiesLookupTable.end(); I!=J; ++I)
  {
   if(I->second.Property == prop)
	return I;
  }
 return PropertiesLookupTable.end();
}

// ���� ��� �������� �� ��������� �� ����
unsigned int UComponent::FindPropertyType(UEPtr<const UIProperty> prop) const
{
  for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
						J=PropertiesLookupTable.end(); I!=J; ++I)
  {
   if(I->second.Property == prop)
	return I->second.Type;
  }
 return 0;
}
// --------------------------

// --------------------------
// ������� ������ ���������� �����������
// --------------------------
// ��������� �������� � ������ 'name' � ������� �����������
// ���������� � ��������� ��� ���������� ������
// ������ ���������� � ������������� �������
void UComponent::AddLookupProperty(const NameT &name, unsigned int type, UEPtr<UIProperty> property, bool delenable)
{
 if(PropertiesLookupTable.find(name) != PropertiesLookupTable.end())
  RDK_RAW_THROW(EPropertyNameAlreadyExist(name));

 // Clear cache when adding new property
 CachedPropertyName.clear();
 CachedProperty = UEPtr<UIProperty>(0);

 UVariable P(property);
 P.DelEnable=delenable;
 P.Type=type;

 auto res=PropertiesLookupTable.insert(make_pair(name,P));
 // Find iterator after insert (SetVariable needs const_iterator reference)
 VariableMapCIteratorT it = PropertiesLookupTable.find(name);
 P.Property->SetVariable(it);
}

// �������� ��� ���������
bool UComponent::ChangeLookupPropertyType(const NameT &name, unsigned int type)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);

 if(I == PropertiesLookupTable.end())
  return false;

 I->second.Type=type;
 return true;
}

// ������� �������� � ������ 'name' �� ������� �����������
// ����������
void UComponent::DelLookupProperty(const NameT &name)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);

 if(I == PropertiesLookupTable.end())
  RDK_RAW_THROW(EPropertyNameNotExist(name));

 // Clear cache if deleting cached property
 if(CachedPropertyName == name)
 {
  CachedPropertyName.clear();
  CachedProperty = UEPtr<UIProperty>(0);
 }

 UIProperty *prop=I->second.Property;
 bool del_enable=I->second.DelEnable;
 PropertiesLookupTable.erase(I);
 if(prop && del_enable)
  delete prop;

 std::map<std::string, std::string>::iterator J=Aliases.begin(),K;

 for(;J != Aliases.end();)
 {
  if(J->second == name)
  {
   K=J; ++K;
   Aliases.erase(J);
   J=K;
  }
  else
   ++J;
 }
}

// ������� ��� ������� ������������
void UComponent::ClearLookupPropertyTable(void)
{
 while(PropertiesLookupTable.begin() != PropertiesLookupTable.end())
 {
  VariableMapIteratorT I=PropertiesLookupTable.begin();
  UIProperty* prop=I->second.Property;
  bool del_enable=I->second.DelEnable;
  PropertiesLookupTable.erase(I);
  if(prop && del_enable)
   delete prop;
 }
 Aliases.clear();
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������ ����������
// --------------------------
// ��������� ����� �������� �������� � ������ 'name' � ������� �����������
// ����� ������� � ��������� ��� ���������� ������
// ������ ���������� � ������������� �������
UId UComponent::AddLookupShare(const NameT &name, UEPtr<UIShare> property)
{
 ShareMapIteratorT I=ShareLookupTable.end();
 UId id=1;
 if(I != ShareLookupTable.begin())
 {
  --I;
  id=I->first+1;
 }
 ShareLookupTable[id]=property;
 return id;
}
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
/// ���������� �������
bool UComponent::AddAlias(const std::string &alias, const std::string &property_name)
{
 if(CheckAlias(alias))
  return false;

// if(PropertiesLookupTable.find(property_name) == PropertiesLookupTable.end())
//  return false;

 Aliases[alias]=property_name;
 return true;
}

/// �������� �������
void UComponent::DelAlias(const std::string &alias)
{
 std::map<std::string, std::string>::iterator I=Aliases.find(alias);
 if(I == Aliases.end())
  return;
 Aliases.erase(I);
}

/// �������� ������� ������
bool UComponent::CheckAlias(const std::string &alias) const
{
 // Use cache for repeated lookups
 if(CachedAliasName == alias)
  return true;
 
 auto I=Aliases.find(alias);
 if(I == Aliases.end())
 {
  CachedAliasName.clear();
  return false;
 }
 
 // Cache the result
 CachedAliasName = alias;
 CachedAliasValue = I->second;
 return true;
}

/// ��������� ����� �������� �� ������
const std::string& UComponent::GetPropertyNameByAlias(const std::string &alias) const
{
 std::map<std::string, std::string>::const_iterator I=Aliases.find(alias);
 if(I == Aliases.end())
  throw EAliasNameNotExist(alias);
 return I->second;
}
// --------------------------


}

#endif


