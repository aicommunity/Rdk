#ifndef UADATA_COMPONENT_CPP
#define UADATA_COMPONENT_CPP

#include "UDataComponent.h"

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


// --------------------------
// Конструкторы и деструкторы
// --------------------------
UDataComponent::UDataComponent(void)
{

}

UDataComponent::~UDataComponent(void)
{
 ClearLookupPropertyTable();
 ShareLookupTable.clear();
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Создает экземпляр описания класса
UContainerDescription* UDataComponent::NewDescription(void)
{
 UContainerDescription* result=new UContainerDescription;

 return result;
}

UContainerDescription* UDataComponent::ANewDescription(UComponentDescription* description)
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
// --------------------------

// --------------------------
// Методы доступа к параметрам
// --------------------------
// Возвращает значение параметра по имени 'name'
UEPtr<UVariableData> UDataComponent::GetProperty(const NameT &name, UEPtr<UVariableData> values) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);

 if(I != PropertiesLookupTable.end())
  I->second.Property->Save(values);

 return values;
}

std::string& UDataComponent::GetPropertyValue(const NameT &name, std::string &values) const
{
 Serialize::USerStorageXML data;

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
void UDataComponent::SetProperty(const NameT &name, UEPtr<UVariableData> values)
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);

 if(I != PropertiesLookupTable.end())
  I->second.Property->Load(values);
}

void UDataComponent::SetPropertyValue(const NameT &name, const std::string &values)
{
 Serialize::USerStorageXML data;
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

const UDataComponent::VariableMapT& UDataComponent::GetPropertiesList(void) const
{
 return PropertiesLookupTable;
}

// Копирует все параметры этого объекта в объект 'comp', если возможно.
// копируются только свойства типа type
void UDataComponent::CopyProperties(UEPtr<UDataComponent> comp, unsigned int type) const
{
 Serialize::USerStorageBinary databuffer;
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
const NameT& UDataComponent::FindPropertyName(UEPtr<const UIProperty> prop) const
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
UDataComponent::VariableMapCIteratorT UDataComponent::FindPropertyVariable(UEPtr<const UIProperty> prop) const
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
unsigned int UDataComponent::FindPropertyType(UEPtr<const UIProperty> prop) const
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
void UDataComponent::AddLookupProperty(const NameT &name, unsigned int type, UEPtr<UIProperty> property, bool delenable)
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
bool UDataComponent::ChangeLookupPropertyType(const NameT &name, unsigned int type)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);

 if(I == PropertiesLookupTable.end())
  return false;

 I->second.Type=type;
 return true;
}

// Удаляет параметр с именем 'name' из таблицы соотвествий
// параметров
void UDataComponent::DelLookupProperty(const NameT &name)
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
void UDataComponent::ClearLookupPropertyTable(void)
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
UId UDataComponent::AddLookupShare(const NameT &name, UEPtr<UIShare> property)
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

