#ifndef UADATA_COMPONENT_CPP
#define UADATA_COMPONENT_CPP
                            
#include "UADataComponent.h"                            

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UVariable::UVariable(void)
{
 Id=ForbiddenId;
 Property=0;
 DelEnable=true;
 Type=ptParameter & pgPublic;
}

UVariable::UVariable(UId id, UEPtr<UIProperty> prop, unsigned int type)
 : Id(id), Property(prop), DelEnable(true), Type(type)
{
}

UVariable::UVariable(const UVariable &copy)
{
 Id=copy.Id;
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
/*
USharedVariable::USharedVariable(void)
{
 Id=ForbiddenId;
// Property=0;
 DelEnable=true;
}

USharedVariable::USharedVariable(UId id, UEPtr<UIProperty> prop)
 : Id(id), Property(prop), DelEnable(true)
{
}

USharedVariable::USharedVariable(const USharedVariable &copy)
{
 Id=copy.Id;
 Property=copy.Property;
 DelEnable=copy.DelEnable;
}


USharedVariable::~USharedVariable(void)
{
}     */
// --------------------------



// --------------------------
// Конструкторы и деструкторы
// --------------------------
UADataComponent::UADataComponent(void)
{

}

UADataComponent::~UADataComponent(void)
{
 ClearLookupPropertyTable();
 ShareLookupTable.clear();
}
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
// Возвращает имя параметра по его Id
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

// Возвращает Id параметра по его имени
const UId& UADataComponent::GetPropertyId(const NameT &name) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);
 if(I == PropertiesLookupTable.end())
   throw new EPropertyNameNotExist(name);

 return I->second.Id;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Создает экземпляр описания класса
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
// Методы доступа к параметрам
// --------------------------
// Возвращает значение параметра по Id 'id'
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

std::string& UADataComponent::GetPropertyValue(const UId &id, std::string &values) const
{
 Serialize::USerStorageXML data;

 for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                            J=PropertiesLookupTable.end(); I!=J;++I)
 {
  if(I->second.Id == id)
  {
   I->second.Property->Save(&data,true);
   if(data.GetNumNodes() == 0)
    values=data.GetNodeText();
   else
    data.Save(values);
   return values;
  }
 }

 return values;
}

// Возвращает значение параметра по имени 'name'
UEPtr<UVariableData> UADataComponent::GetProperty(const NameT &name, UEPtr<UVariableData> values) const
{
 return GetProperty(GetPropertyId(name),values);
}

std::string& UADataComponent::GetPropertyValue(const NameT &name, std::string &values) const
{
 return GetPropertyValue(GetPropertyId(name),values);
}

// Устанавливает значение параметра по Id 'id'
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

void UADataComponent::SetPropertyValue(const UId &id, const std::string &values)
{
 Serialize::USerStorageXML data;
 for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                             J=PropertiesLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Id == id)
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
   return;
  }
 }
}


// Устанавливает значение параметра по имени 'name'
void UADataComponent::SetProperty(const NameT &name, UEPtr<UVariableData> values)
{
 SetProperty(GetPropertyId(name),values);
}

void UADataComponent::SetPropertyValue(const NameT &name, const std::string &values)
{
 SetPropertyValue(GetPropertyId(name),values);
}

const UADataComponent::VariableMapT& UADataComponent::GetPropertiesList(void) const
{
 return PropertiesLookupTable;
}

// Копирует все параметры этого объекта в объект 'comp', если возможно.
// копируются только свойства типа type
void UADataComponent::CopyProperties(UEPtr<UADataComponent> comp, unsigned int type) const
{
 Serialize::USerStorageBinary databuffer;
 for(VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                            J=PropertiesLookupTable.end(); I!=J; ++I)
 {
  if(!(I->second.Type & type))
   continue;
  databuffer.clear();
  comp->SetProperty(I->second.Id,GetProperty(I->second.Id,&databuffer));
 }
}

// Ищет имя свойства по указателю на него
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

// Ищет переменную свойства в таблице по указателю на него
UADataComponent::VariableMapCIteratorT UADataComponent::FindPropertyVariable(UEPtr<const UIProperty> prop) const
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
unsigned int UADataComponent::FindPropertyType(UEPtr<const UIProperty> prop) const
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
UId UADataComponent::AddLookupProperty(const NameT &name, unsigned int type, UEPtr<UIProperty> property, bool delenable)
{
 if(PropertiesLookupTable.find(name) != PropertiesLookupTable.end())
  throw new EPropertyNameAlreadyExist(name);

 UVariable P(1,property);
 P.DelEnable=delenable;
 P.Type=type;

 for(VariableMapIteratorT I=PropertiesLookupTable.begin(),
                      J=PropertiesLookupTable.end(); I!=J; ++I)
 {
  if(P.Id <= I->second.Id)
   P.Id=I->second.Id+1;
 }

 pair<VariableMapIteratorT, bool> res=PropertiesLookupTable.insert(make_pair(name,P));
 P.Property->SetVariable(res.first);

 return P.Id;
}

// Удаляет параметр с именем 'name' из таблицы соотвествий
// параметров
void UADataComponent::DelLookupProperty(const NameT &name)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);

 if(I == PropertiesLookupTable.end())
  throw new EPropertyNameNotExist(name);

 if(I->second.Property && I->second.DelEnable)
  delete I->second.Property;
 PropertiesLookupTable.erase(I);
}

// Удаляет всю таблицу соответствий
void UADataComponent::ClearLookupPropertyTable(void)
{
 for(VariableMapIteratorT I=PropertiesLookupTable.begin(),
                      J=PropertiesLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Property && I->second.DelEnable)
  {
   delete I->second.Property;
   I->second.Property=0;
  }
 }
 PropertiesLookupTable.clear();
}
// --------------------------

// --------------------------
// Скрытые методы управления общими свойствами
// --------------------------
// Добавляет общее свойство параметр с именем 'name' в таблицу соотвествий
// общих свойств и назначает ему корректный индекс
// Должна вызываться в конструкторах классов
UId UADataComponent::AddLookupShare(const NameT &name, UEPtr<UIShare> property)
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

