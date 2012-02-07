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
}

UVariable::UVariable(UId id, UIProperty *prop)
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
// Методы доступа к таблицам соотвествий
// --------------------------
// Возвращает имя параметра по его Id
const NameT& UADataComponent::GetPropertyName(const UId &id) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                         J=PropertiesLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// Возвращает Id параметра по его имени
const UId& UADataComponent::GetPropertyId(const NameT &name) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);
 if(I == PropertiesLookupTable.end())
  return ForbiddenId;
 else return I->second.Id;
}
 /*
// Возвращает полное имя параметра без префикса RDK, и суффикса '*'
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

// Возвращает имя переменной состояния по его Id
const NameT& UADataComponent::GetStateName(const UId &id) const
{
 VariableMapCIteratorT I=StateLookupTable.begin(),
                                 J=StateLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// Возвращает Id переменной состояния по его имени
const UId& UADataComponent::GetStateId(const NameT &name) const
{
 VariableMapCIteratorT I=StateLookupTable.find(name);
 if(I == StateLookupTable.end())
  return ForbiddenId;
 else return I->second.Id;
}
		 /*
// Возвращает полное имя переменной состояния без префикса NMSDK, и суффикса '*'
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
UVariableData* UADataComponent::GetProperty(const UId &id, UVariableData *values) const
{
 VariableMapCIteratorT I,J;

 I=PropertiesLookupTable.begin();
 J=PropertiesLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   I->second.Property->Save(values);
   return values;
  }

  ++I;
 }

 return values;
}

// Возвращает значение параметра по имени 'name'
UVariableData* UADataComponent::GetProperty(const NameT &name, UVariableData *values) const
{
 return GetProperty(GetPropertyId(name),values);
}

// Устанавливает значение параметра по Id 'id'
bool UADataComponent::SetProperty(const UId &id, UVariableData *values)
{
 VariableMapCIteratorT I,J;

 I=PropertiesLookupTable.begin();
 J=PropertiesLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   return I->second.Property->Load(values);
  }

  ++I;
 }

 return false;
}

// Устанавливает значение параметра по имени 'name'
bool UADataComponent::SetProperty(const NameT &name, UVariableData *values)
{
 return SetProperty(GetPropertyId(name),values);
}

const UADataComponent::VariableMapT& UADataComponent::GetPropertiesList(void) const
{
 return PropertiesLookupTable;
}

// Копирует все параметры этого объекта в объект 'comp', если возможно.
bool UADataComponent::CopyProperties(UEPtr<UADataComponent> comp) const
{
 bool key=true;

 if(!comp)
  return false;

// UEPtr<UADataComponent> ccomp=static_cast<UEPtr<UADataComponent>>(comp);

 Serialize::USerStorageBinary databuffer;
  VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                                        J=PropertiesLookupTable.end();
  while(I != J)
  {
   databuffer.clear();
   key &= comp->SetProperty(I->second.Id,GetProperty(I->second.Id,&databuffer));
   ++I;
  }
 return key;
}

// Ищет имя свойства по указателю на него
const NameT& UADataComponent::FindPropertyName(const UIProperty *prop) const
{
  VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                        J=PropertiesLookupTable.end();
  while(I != J)
  {
   if(I->second.Property == prop)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}
// --------------------------

// --------------------------
// Методы доступа к переменным состояния
// --------------------------
// Возвращает значение переменной состояния по Id 'id'
UVariableData* UADataComponent::GetState(const UId &id, UVariableData *values) const
{
 VariableMapCIteratorT I,J;

 I=StateLookupTable.begin();
 J=StateLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   I->second.Property->Save(values);
   return values;
  }

  ++I;
 }

 return values;
}

// Возвращает значение переменной состояния по имени 'name'
UVariableData* UADataComponent::GetState(const NameT &name, UVariableData *values) const
{
 return GetState(GetStateId(name),values);
}

// Устанавливает значение переменной состояния по Id 'id'
bool UADataComponent::SetState(const UId &id, UVariableData *values)
{
 VariableMapCIteratorT I,J;

 I=StateLookupTable.begin();
 J=StateLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   return I->second.Property->Load(values);
  }

  ++I;
 }

 return false;
}

// Устанавливает значение переменной состояния по имени 'name'
bool UADataComponent::SetState(const NameT &name, UVariableData *values)
{
 return SetState(GetStateId(name),values);
}

// Возвращает список имен и Id переменных состояния, содержащихся непосредственно
// в этом объекте
const UADataComponent::VariableMapT& UADataComponent::GetStateList(void) const
{
 return StateLookupTable;
}

// Ищет имя свойства по указателю на него
const NameT& UADataComponent::FindStateName(const UIProperty *prop) const
{
  VariableMapCIteratorT I=StateLookupTable.begin(),
                        J=StateLookupTable.end();
  while(I != J)
  {
   if(I->second.Property == prop)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// Копирует все переменные состояния этого объекта в объект 'comp', если возможно.
bool UADataComponent::CopyState(UEPtr<UADataComponent> comp) const
{
 VariableMapCIteratorT I=StateLookupTable.begin(),
                       J=StateLookupTable.end();
 bool key=true;

 if(!comp)
  return false;

 Serialize::USerStorageBinary serstorage;
 while(I != J)
  {
   if(!comp->SetState(I->second.Id,GetState(I->second.Id,&serstorage)))
    key=false;
   ++I;
  }
 return key;
}
// --------------------------

// --------------------------
// Скрытые методы управления параметрами
// --------------------------
// Добавляет параметр с именем 'name' в таблицу соотвествий
// параметров и назначает ему корректный индекс
// Должна вызываться в конструкторах классов
UId UADataComponent::AddLookupProperty(const NameT &name, UIProperty *property, bool delenable)
{
 if(!property)
  return ForbiddenId;

 VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
 UVariable P(1,property);
 P.DelEnable=delenable;

 if(PropertiesLookupTable.find(name) != J)
  return ForbiddenId;

 while(I != J)
  {
   if(P.Id <= I->second.Id)
	P.Id=I->second.Id+1;
   ++I;
  }

 PropertiesLookupTable.insert(make_pair(name,P));
// PropertiesLookupTable[name]=P;

 return P.Id;
}

// Удаляет параметр с именем 'name' из таблицы соотвествий
// параметров
bool UADataComponent::DelLookupProperty(const NameT &name)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);

 if(I == PropertiesLookupTable.end())
  return false;

 if(I->second.DelEnable)
  delete I->second.Property;
 PropertiesLookupTable.erase(I);
 return true;
}

// Удаляет всю таблицу соответствий
void UADataComponent::ClearLookupPropertyTable(void)
{
  VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
  while(I != J)
  {
   if(I->second.Property && I->second.DelEnable)
	delete I->second.Property;

   ++I;
  }
 PropertiesLookupTable.clear();
}
						 /*
// Возвращает полное имя параметра без префикса RDK, и суффикса '*'
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
// Скрытые методы управления состоянием
// --------------------------
// Добавляет переменную состояния с именем 'name' в таблицу соотвествий
// параметров и назначает ей корректный индекс
// Должна вызываться в конструкторах классов
UId UADataComponent::AddLookupState(const NameT &name,UIProperty *property, bool delenable)
{
 if(!property)
  return ForbiddenId;

 VariableMapIteratorT I=StateLookupTable.begin(),
					  J=StateLookupTable.end();
 UVariable P(1,property);
 P.DelEnable=delenable;

 if(StateLookupTable.find(name) != J)
  return ForbiddenId;

 while(I != J)
  {
   if(P.Id <= I->second.Id)
    P.Id=I->second.Id+1;
   ++I;
  }

 StateLookupTable.insert(make_pair(name,P));

 return P.Id;
}

// Удаляет переменную состояния с именем 'name' из таблицы соотвествий
bool UADataComponent::DelLookupState(const NameT &name)
{
 VariableMapIteratorT I=StateLookupTable.find(name);

 if(I == StateLookupTable.end())
  return false;

 if(I->second.DelEnable)
  delete I->second.Property;
 StateLookupTable.erase(I);
 return true;
}

// Удаляет всю таблицу соответствий
void UADataComponent::ClearLookupStateTable(void)
{
  VariableMapIteratorT I=StateLookupTable.begin(),
                      J=StateLookupTable.end();
  while(I != J)
  {
   if(I->second.Property && I->second.DelEnable)
    delete I->second.Property;

   ++I;
  }
 StateLookupTable.clear();
}

// Возвращает полное имя переменной состояния без префикса NMSDK, и суффикса '*'
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

