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

UVariable::UVariable(UIProperty* prop, unsigned int type)
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
std::shared_ptr<UComponent> UComponent::GetOwner(void) const
{
 return Owner.lock();
}

bool UComponent::SetOwner(std::shared_ptr<UComponent> owner)
{
 if(Owner.lock() == owner)
  return true;

 Owner=owner;
 return true;
}


// ���������� ��������� �� �������� ��������� ���� ��������
std::shared_ptr<UComponent> UComponent::GetMainOwner(void) const
{
 return MainOwner.lock();
}

void UComponent::SetMainOwner(std::shared_ptr<UComponent> mainowner)
{
 if(mainowner != MainOwner.lock())
 {
  MainOwner=mainowner;
  UpdateInternalData();
 }
}

// ���������� ��������� ��������� ����� �������
std::shared_ptr<UStorage> UComponent::GetStorage(void) const
{
 return Storage.lock();
}

bool UComponent::SetStorage(std::shared_ptr<UStorage> storage)
{
 if(Storage.lock() == storage)
  return true;

 Storage=storage;
 UpdateInternalData();
 return true;
}

// ���������� ����� ���������� ����� �������
std::shared_ptr<UEnvironment> UComponent::GetEnvironment(void) const
{
 return Environment.lock();
}

bool UComponent::SetEnvironment(std::shared_ptr<UEnvironment> environment)
{
 if(Environment.lock() == environment)
  return true;

 Environment=environment;
 UpdateInternalData();
 return true;
}

// ��������� �� ������
std::shared_ptr<ULoggerEnv> UComponent::GetLogger(void) const
{
 return Logger.lock();
}

bool UComponent::SetLogger(std::shared_ptr<ULoggerEnv> logger)
{
 if(Logger.lock() == logger)
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
 auto env = Environment.lock();
 if(!env)
  RDK_RAW_THROW(EEnvironmentNotExist());

 return env->GetTime();
}


/// ���������� ��������� �� ����� �� ���������
UAFont* UComponent::GetDefaultFont(void)
{
 auto env = Environment.lock();
 if(!env)
  RDK_RAW_THROW(EEnvironmentNotExist());

 return env->GetFonts().GetDefaultFont();
}

/// ���������� �������� �����
UAFont* UComponent::GetFont(const string &name, int size)
{
 auto env = Environment.lock();
 if(!env)
  RDK_RAW_THROW(EEnvironmentNotExist());

 return env->GetFonts().GetFont(name,size);
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
 auto storage = Storage.lock();
 if(!storage)
  return "";
 return storage->FindClassName(Class);
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
  std::shared_ptr<UIProperty> prop(I->second.Property);
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
std::shared_ptr<UIProperty> UComponent::FindProperty(const NameT &name) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.end();

 // TODO: ������� ��������� ������
 if(CheckAlias(name))
  I=PropertiesLookupTable.find(GetPropertyNameByAlias(name));
 else
  I=PropertiesLookupTable.find(name);

 if(I != PropertiesLookupTable.end())
  return std::shared_ptr<UIProperty>(I->second.Property, [](UIProperty*){}); // Non-owning deleter

 return std::shared_ptr<UIProperty>(0);
}

std::shared_ptr<UIProperty> UComponent::FindProperty(const NameT &name)
{
 VariableMapCIteratorT I=PropertiesLookupTable.end();

 // TODO: ������� ��������� ������
 if(CheckAlias(name))
  I=PropertiesLookupTable.find(GetPropertyNameByAlias(name));
 else
  I=PropertiesLookupTable.find(name);

 if(I != PropertiesLookupTable.end())
  return std::shared_ptr<UIProperty>(I->second.Property, [](UIProperty*){}); // Non-owning deleter

 return std::shared_ptr<UIProperty>(0);
}

std::shared_ptr<UVariableData> UComponent::GetProperty(const NameT &name, std::shared_ptr<UVariableData> values) const
{
 std::shared_ptr<UIProperty> property=FindProperty(name);

 if(property)
  property->Save(std::shared_ptr<USerStorage>(values.get()));

 return values;
}

std::string& UComponent::GetPropertyValue(const NameT &name, std::string &values) const
{
 USerStorageXML data;

 std::shared_ptr<UIProperty> property=FindProperty(name);

 if(property)
 {
  property->Save(std::shared_ptr<USerStorage>(&data),true);
  if(data.GetNumNodes() == 0)
   values=data.GetNodeText();
  else
   data.Save(values);
 }

 return values;
}

// ������������� �������� ��������� �� ����� 'name'
void UComponent::SetProperty(const NameT &name, std::shared_ptr<UVariableData> values)
{
 std::shared_ptr<UIProperty> property=FindProperty(name);

 if(property)
  property->Load(std::shared_ptr<USerStorage>(values.get()));
}

void UComponent::SetPropertyValue(const NameT &name, const std::string &values)
{
 USerStorageXML data;
 std::shared_ptr<UIProperty> property=FindProperty(name);

 if(property)
 {
  if(values.size()>0 && values[0]=='<')
  {
   data.Load(values,"");
   data.RenameNode(property->GetName());
   property->Load(std::shared_ptr<USerStorage>(&data),true);
  }
  else
  {
   property->Save(std::shared_ptr<USerStorage>(&data),true);
   data.SetNodeText(values);
   property->Load(std::shared_ptr<USerStorage>(&data),true);
  }
 }
}

const UComponent::VariableMapT& UComponent::GetPropertiesList(void) const
{
 return PropertiesLookupTable;
}

// �������� ��� ��������� ����� ������� � ������ 'comp', ���� ��������.
// ���������� ������ �������� ���� type
void UComponent::CopyProperties(std::shared_ptr<UComponent> comp, unsigned int type) const
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
  comp->SetProperty(I->first,GetProperty(I->first,std::shared_ptr<UVariableData>(&databuffer)));
 }
}

// ���� ��� �������� �� ��������� �� ����
const NameT& UComponent::FindPropertyName(const UIProperty* prop) const
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
UComponent::VariableMapCIteratorT UComponent::FindPropertyVariable(const UIProperty* prop) const
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
unsigned int UComponent::FindPropertyType(const UIProperty* prop) const
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
void UComponent::AddLookupProperty(const NameT &name, unsigned int type, UIProperty* property, bool delenable)
{
 if(PropertiesLookupTable.find(name) != PropertiesLookupTable.end())
  RDK_RAW_THROW(EPropertyNameAlreadyExist(name));

 UVariable P(property);
 P.DelEnable=delenable;
 P.Type=type;

 pair<VariableMapCIteratorT, bool> res=PropertiesLookupTable.insert(make_pair(name,P));
 P.Property->SetVariable(res.first);
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
UId UComponent::AddLookupShare(const NameT &name, std::shared_ptr<UIShare> property)
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
 std::map<std::string, std::string>::const_iterator I=Aliases.find(alias);
 if(I == Aliases.end())
  return false;
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


