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
#include "UStorage.h"
#include "UComponentDescription.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
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
// Методы доступа к данным
// --------------------------
// Возвращает только маску типа свойства
unsigned int UVariable::GetPropertyType(void) const
{
 return Type & 0x000000FF;
}

// Возвращает только маску группы свойства
unsigned int UVariable::GetPropertyGroup(void) const
{
 return Type & 0xFFFFFF00;
}

// Возвращает строковое имя типа свойства по заданному типу
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

 default:
  return std::string("ptUnknown");
 }
}

// Возвращает тип свойства по строковому имени
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

 return 0;
}

// Возвращает строковое имя типа свойства
std::string UVariable::GetPropertyTypeName(void) const
{
 return GetPropertyTypeNameByType(GetPropertyType());
}

// Проверяет соответствие типа и группы свойства маске
bool UVariable::CheckMask(unsigned int mask) const
{
 return (GetPropertyType() & mask) && (GetPropertyGroup() & mask);
}
// --------------------------


//class UComponent
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UComponent::UComponent(void)
{
// Owner=0;

// MainOwner=0;

// Storage=0;

 Class=ForbiddenId;
}

UComponent::~UComponent(void)
{
 ClearLookupPropertyTable();
 ShareLookupTable.clear();
}
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возвращает владелца этого объекта.
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


// Возвращает указатель на главного владельца этим объектом
UEPtr<UComponent> const UComponent::GetMainOwner(void) const
{
 return MainOwner;
}

void UComponent::SetMainOwner(UEPtr<UComponent> mainowner)
{
 if(mainowner != MainOwner)
  MainOwner=mainowner;
}

// Возвращает хранилище компонент этого объекта
UEPtr<UStorage> const UComponent::GetStorage(void) const
{
 return Storage;
}

bool UComponent::SetStorage(UEPtr<UStorage> storage)
{
 if(Storage == storage)
  return true;

 Storage=storage;
 return true;
}
// --------------------------


// --------------------------
// Методы управления данными
// --------------------------
// Идентификатор класса
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
// --------------------------



// --------------------------
// Методы управления счетом
// --------------------------
// Создает экземпляр описания класса
UContainerDescription* UComponent::NewDescription(void)
{
 UContainerDescription* result=new UContainerDescription;

 return result;
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
  result->SetProperty(I->first,dummydescr);
  ++I;
 }

 return result;
}

// Уничтожение этого объекта
void UComponent::Free(void)
{
 delete this;
}
// --------------------------

// --------------------------
// Методы доступа к параметрам
// --------------------------
// Возвращает значение параметра по имени 'name'
UEPtr<UVariableData> UComponent::GetProperty(const NameT &name, UEPtr<UVariableData> values) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);

 if(I != PropertiesLookupTable.end())
  I->second.Property->Save(values);

 return values;
}

std::string& UComponent::GetPropertyValue(const NameT &name, std::string &values) const
{
 USerStorageXML data;

 VariableMapCIteratorT I=PropertiesLookupTable.find(name);

 if(I != PropertiesLookupTable.end())
 {
  I->second.Property->Save(&data,true);
  if(data.GetNumNodes() == 0)
   values=data.GetNodeText();
  else
   data.Save(values);
 }

 return values;
}

// Устанавливает значение параметра по имени 'name'
void UComponent::SetProperty(const NameT &name, UEPtr<UVariableData> values)
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);

 if(I != PropertiesLookupTable.end())
  I->second.Property->Load(values);
}

void UComponent::SetPropertyValue(const NameT &name, const std::string &values)
{
 USerStorageXML data;
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);

 if(I != PropertiesLookupTable.end())
 {
  if(values.size()>0 && values[0]=='<')
  {
   data.Load(values,"");
   data.RenameNode(I->second.Property->GetName());
   I->second.Property->Load(&data,true);
  }
  else
  {
   I->second.Property->Save(&data,true);
   data.SetNodeText(values);
   I->second.Property->Load(&data,true);
  }
 }
}

const UComponent::VariableMapT& UComponent::GetPropertiesList(void) const
{
 return PropertiesLookupTable;
}

// Копирует все параметры этого объекта в объект 'comp', если возможно.
// копируются только свойства типа type
void UComponent::CopyProperties(UEPtr<UComponent> comp, unsigned int type) const
{
 USerStorageBinary databuffer;
 for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                            J=PropertiesLookupTable.end(); I!=J; ++I)
 {
  if(!(I->second.Type & type))
   continue;
  databuffer.clear();
  comp->SetProperty(I->first,GetProperty(I->first,&databuffer));
 }
}

// Ищет имя свойства по указателю на него
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

// Ищет переменную свойства в таблице по указателю на него
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

// Ищет тип свойства по указателю на него
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
// Скрытые методы управления параметрами
// --------------------------
// Добавляет параметр с именем 'name' в таблицу соотвествий
// параметров и назначает ему корректный индекс
// Должна вызываться в конструкторах классов
void UComponent::AddLookupProperty(const NameT &name, unsigned int type, UEPtr<UIProperty> property, bool delenable)
{
 if(PropertiesLookupTable.find(name) != PropertiesLookupTable.end())
  throw new EPropertyNameAlreadyExist(name);

 UVariable P(property);
 P.DelEnable=delenable;
 P.Type=type;

 pair<VariableMapCIteratorT, bool> res=PropertiesLookupTable.insert(make_pair(name,P));
 P.Property->SetVariable(res.first);
}

// Изменяет тип параметра
bool UComponent::ChangeLookupPropertyType(const NameT &name, unsigned int type)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);

 if(I == PropertiesLookupTable.end())
  return false;

 I->second.Type=type;
 return true;
}

// Удаляет параметр с именем 'name' из таблицы соотвествий
// параметров
void UComponent::DelLookupProperty(const NameT &name)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);

 if(I == PropertiesLookupTable.end())
  throw new EPropertyNameNotExist(name);

 UIProperty *prop=I->second.Property;
 bool del_enable=I->second.DelEnable;
 PropertiesLookupTable.erase(I);
 if(prop && del_enable)
  delete prop;
}

// Удаляет всю таблицу соответствий
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
}
// --------------------------

// --------------------------
// Скрытые методы управления общими свойствами
// --------------------------
// Добавляет общее свойство параметр с именем 'name' в таблицу соотвествий
// общих свойств и назначает ему корректный индекс
// Должна вызываться в конструкторах классов
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



}

#endif


