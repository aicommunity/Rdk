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
#include "UPropertyEndpoints.h"
#include "UShare.h"
#include "UStorage.h"
#include "UEnvironment.h"
#include "UComponentDescription.h"
#include "UEnvException.h"
#include "UPropertyInterface.h"

namespace RDK {

/// Заглушка, возвращаемая в случае остутствия доступа к Environment::Time
//UELockVar<UTimeControl> UComponent::DummyTime;

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
 case ptInput:
  return std::string("Input");
 case ptOutput:
  return std::string("Output");

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
 if(name == "Input")
  return ptInput;
 if(name == "Output")
  return ptOutput;

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
// Методы доступа к свойствам
// --------------------------
// Возвращает флаг, определяющий компонент является статическим
// или динамическим
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
 {
  MainOwner=mainowner;
  UpdateInternalData();
 }
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
 UpdateInternalData();
 return true;
}

// Возвращает среду выполнения этого объекта
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

// Указатель на логгер
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


/// Возвращает ссылку на класс управления времени из Environment.
/// Если Environment отсутствует то возвращает указатель на заглушку
/// DummyTime
const UTimeControl& UComponent::GetTime(void) const
{
 if(!Environment)
  RDK_RAW_THROW(EEnvironmentNotExist());

 return Environment->GetTime();
}


/// Возвращает указатель на шрифт по умолчанию
UAFont* UComponent::GetDefaultFont(void)
{
 if(!Environment)
  RDK_RAW_THROW(EEnvironmentNotExist());

 return Environment->GetFonts().GetDefaultFont();
}

/// Возвращает заданный шрифт
UAFont* UComponent::GetFont(const string &name, int size)
{
 if(!Environment)
  RDK_RAW_THROW(EEnvironmentNotExist());

 return Environment->GetFonts().GetFont(name,size);
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

// Возвращает имя класса компоненты
const NameT UComponent::GetCompClassName(void) const
{
 return Storage->FindClassName(Class);
}
// --------------------------



// --------------------------
// Методы управления счетом
// --------------------------
// Создает экземпляр описания класса
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

// Уничтожение этого объекта
void UComponent::Free(void)
{
 if(!StaticFlag)
  delete this;
}

/// Осуществляет обновление внутренних данных компонента, обеспечивающих его целостность
void UComponent::UpdateInternalData(void)
{
 AUpdateInternalData();
}

void UComponent::AUpdateInternalData(void)
{

}
// --------------------------

// --------------------------
// Методы доступа к параметрам
// --------------------------
// Возвращает указатель на данные свойства
const UEPtr<UIProperty> UComponent::FindProperty(const NameT &name) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.end();

 // TODO: Сначала проверяем алиасы
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

 // TODO: Сначала проверяем алиасы
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

// Устанавливает значение параметра по имени 'name'
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

// Копирует все параметры этого объекта в объект 'comp', если возможно.
// копируются только свойства типа type
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
void UComponent::AddLookupProperty(UEPtr<UIProperty> property, bool delenable)
{
 std::string name=property->GetName();
 if(PropertiesLookupTable.find(name) != PropertiesLookupTable.end())
  RDK_RAW_THROW(EPropertyNameAlreadyExist(name));

 UVariable P(property);
 P.DelEnable=delenable;
 P.Type=property->GetType();

 pair<VariableMapCIteratorT, bool> res=PropertiesLookupTable.insert(make_pair(name,P));
 P.Property->SetVariable(res.first);
}

// Изменяет тип параметра
bool UComponent::ChangeLookupPropertyType(const NameT &name, unsigned int type)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);

 if(I == PropertiesLookupTable.end())
  return false;

 I->second.Property->ChangeType(type);
 I->second.Type=I->second.Property->GetType();;
 return true;
}

// Удаляет параметр с именем 'name' из таблицы соотвествий
// параметров
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
 Aliases.clear();
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


// --------------------------
// Методы управления алиасами
// --------------------------
/// Добавление алисаса
bool UComponent::AddAlias(const std::string &alias, const std::string &property_name)
{
 if(CheckAlias(alias))
  return false;

// if(PropertiesLookupTable.find(property_name) == PropertiesLookupTable.end())
//  return false;

 Aliases[alias]=property_name;
 return true;
}

/// Удаление алисаса
void UComponent::DelAlias(const std::string &alias)
{
 std::map<std::string, std::string>::iterator I=Aliases.find(alias);
 if(I == Aliases.end())
  return;
 Aliases.erase(I);
}

/// Проверка наличия алиаса
bool UComponent::CheckAlias(const std::string &alias) const
{
 std::map<std::string, std::string>::const_iterator I=Aliases.find(alias);
 if(I == Aliases.end())
  return false;
 return true;
}

/// Получение имени свойства по алиасу
const std::string& UComponent::GetPropertyNameByAlias(const std::string &alias) const
{
 std::map<std::string, std::string>::const_iterator I=Aliases.find(alias);
 if(I == Aliases.end())
  throw EAliasNameNotExist(alias);
 return I->second;
}
// --------------------------
/*
// Описание подключаемого элемента "UConnectedITEM"
//struct UCItem
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UCItem::UCItem(void)
{
 // Подключаемый элемент
 Item=0;

 // Индекс выхода
 Index=0;
}

UCItem::UCItem(const UCItem &copy)
{
 Item=copy.Item;
 Index=copy.Index;
 Name=copy.Name;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
bool UCItem::operator == (const UCItem &value) const
{
 return (Item == value.Item) & (Index == value.Index);
}

bool UCItem::operator != (const UCItem &value) const
{
 return !(*this == value);
}
// --------------------------


// Описание входящей связи с заданным item "UConnectedLINK"
//struct UCLink: public UCItem
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UCLink::UCLink(void)
{
 // Индекс входа
 Input=Output=-1;
}

UCLink::UCLink(const UCLink &copy)
{
 Input=copy.Input;
 Output=copy.Output;
 InputName=copy.InputName;
 OutputName=copy.OutputName;
}
// --------------------------
*/
/* *************************************************************************** */




}

#endif


