/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAItemCPP
#define UAItemCPP

#include <string.h>
#include "UItem.h"

namespace RDK {

/* *************************************************************************** */
// Контейнер - список указателей на подключенные элементы
// class UAConnectorVector
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAConnectorVector::UAConnectorVector(void)
{
 // Размер контейнера
 Size=0;

 // Массив элементов
 Data=0;

 // Реальный размер контейнера
 RealSize=0;
}

UAConnectorVector::UAConnectorVector(const UAConnectorVector &copy)
{
 *this=copy;
}


UAConnectorVector::~UAConnectorVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает контейнер
void UAConnectorVector::Clear(void)
{
 if(Data)
 {
  delete []Data;
  Data=0;
 }
 Size=RealSize=0;
}

// Меняет размер контейнера с сохранением элементов
void UAConnectorVector::Resize(int newsize)
{
 if(Size == newsize)
  return;

 if(RealSize<newsize || !Data)
 {
  PUAConnector* newbuffer=new PUAConnector[newsize];
  memcpy(newbuffer,Data,sizeof(PUAConnector)*Size);

  if(Data)
   delete []Data;
  Data=newbuffer;
  RealSize=newsize;
  Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Ищет в контейнере первый заданный элемент начиная с индекса index
// и возвращает его индекс или -1 если элемент не найден
int UAConnectorVector::Find(const PUAConnector &item, int index) const
{
 PUAConnector *pdata=Data;

 for(int i=index;i<Size;i++,pdata++)
  if(*pdata == item)
   return i;

 return -1;
}

// Добавляет в конец контейнера элемент
void UAConnectorVector::Add(const PUAConnector &item)
{
 Resize(Size+1);
 Data[Size-1]=item;
}

// Добавляет в конец контейнера элемент только если таких элементов еще нет
// Возвращает true в случае успешного добавления
bool UAConnectorVector::AddUnique(const PUAConnector &item)
{
 if(Find(item)>=0)
  return false;

 Add(item);
 return true;
}

// Удаляет элемент по индексу
void UAConnectorVector::Del(int index)
{
 if(index<0 || index >= Size)
  return;

 if(index < Size-1)
 {
  memcpy(Data+index,Data+index+1,(Size-index-1)*sizeof(PUAConnector));
 }

 Resize(Size-1);
}

// Удаляет элемент по указателю
void UAConnectorVector::Del(PUAConnector *item)
{
 int index=Find(*item);
 Del(index);
}
// --------------------------

// --------------------------
// Методы доступа
// --------------------------
// Оператор присваивания
UAConnectorVector& UAConnectorVector::operator = (const UAConnectorVector &copy)
{
 Resize(copy.Size);
 memcpy(Data,copy.Data,Size*sizeof(PUAConnector));

 return *this;
}

// Оператор доступа к элементу
PUAConnector& UAConnectorVector::operator [] (int index)
{
 return Data[index];
}

const PUAConnector& UAConnectorVector::operator [] (int index) const
{
 return Data[index];
}

// Оператор доступа к массиву
PUAConnector* UAConnectorVector::GetData(void)
{
 return Data;
}

// Оператор доступа к размеру
int UAConnectorVector::GetSize(void) const
{
 return Size;
}
// --------------------------
/* *************************************************************************** */

/* *************************************************************************** */
// Контейнер - список указателей на подключенные элементы
// class UAConnector2DVector
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAConnector2DVector::UAConnector2DVector(void)
{
 // Размер контейнера
 Size=0;

 // Массив элементов
 Data=0;

 // Реальный размер контейнера
 RealSize=0;
}

UAConnector2DVector::UAConnector2DVector(const UAConnector2DVector &copy)
{
 *this=copy;
}


UAConnector2DVector::~UAConnector2DVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает контейнер
void UAConnector2DVector::Clear(void)
{
 if(Data)
 {
  delete []Data;
  Data=0;
 }
 Size=RealSize=0;
}

// Меняет размер контейнера с сохранением элементов
void UAConnector2DVector::Resize(int newsize)
{
 if(Size == newsize)
  return;

 if(RealSize<newsize || !Data)
 {
  UAConnectorVector* newbuffer=new UAConnectorVector[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Data[i];
//  memcpy(newbuffer,Data,sizeof(UAConnectorVector)*Size);

  if(Data)
   delete []Data;
  Data=newbuffer;
  RealSize=newsize;
  Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Ищет в контейнере первый заданный элемент начиная с индекса index
// и возвращает его индекс или -1 если элемент не найден
/*int UAConnector2DVector::Find(const UAConnectorVector &item, int index) const
{
 UAConnectorVector *pdata=Data;

 for(int i=index;i<Size;i++,pdata++)
  if(*pdata == item)
   return i;

 return -1;
} */

// Добавляет в конец контейнера элемент
void UAConnector2DVector::Add(const UAConnectorVector &item)
{
 Resize(Size+1);
 Data[Size-1]=item;
}

// Добавляет в конец контейнера элемент только если таких элементов еще нет
// Возвращает true в случае успешного добавления
/*bool UAConnector2DVector::AddUnique(const UAConnectorVector &item)
{
 if(Find(item)>=0)
  return false;

 Add(item);
 return true;
} */

// Удаляет элемент по индексу
void UAConnector2DVector::Del(int index)
{
 if(index<0 || index >= Size)
  return;

 for(int i=index;i<Size-1;i++)
  Data[i]=Data[i+1];
// memcpy(Data+index,Data+index+1,(Size-1)*sizeof(UAConnectorVector));
 Resize(Size-1);
}
/*
// Удаляет элемент по указателю
void UAConnector2DVector::Del(UAConnectorVector *item)
{
 int index=Find(*item);
 Del(index);
}  */
// --------------------------

// --------------------------
// Методы доступа
// --------------------------
// Оператор присваивания
UAConnector2DVector& UAConnector2DVector::operator = (const UAConnector2DVector &copy)
{
 Resize(copy.Size);

 for(int i=0;i<Size;i++)
  Data[i]=copy.Data[i];
// memcpy(Data,copy.Data,Size*sizeof(UAConnectorVector));

 return *this;
}

// Оператор доступа к элементу
UAConnectorVector& UAConnector2DVector::operator [] (int index)
{
 return Data[index];
}

const UAConnectorVector& UAConnector2DVector::operator [] (int index) const
{
 return Data[index];
}

// Оператор доступа к массиву
UAConnectorVector* UAConnector2DVector::GetData(void)
{
 return Data;
}

// Оператор доступа к размеру
int UAConnector2DVector::GetSize(void) const
{
 return Size;
}
// --------------------------
/* *************************************************************************** */

/* *************************************************************************** */
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UItem::UItem(void)
{
 NumOutputs=0;
 AddLookupProperty("NumOutputs",ptParameter & pgSystem,new UVProperty<int,UItem>(this,&UItem::SetNumOutputs,&UItem::GetNumOutputs));
 AddLookupProperty("AutoNumOutputs",ptParameter & pgSystem,new UVProperty<bool,UItem>(this,&UItem::SetAutoNumOutputs,&UItem::GetAutoNumOutputs));
}

UItem::~UItem(void)
{
 DisconnectAll();
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Возвращает число подключенных элементов item
const int& UItem::GetNumOutputs(void) const
{
 return NumOutputs;//AssociatedConnectors.GetSize();
}

// Устанавливает число подключенных элементов item
bool UItem::SetNumOutputs(const int &value)
{
 if(NumOutputs == value)
  return true;
/*
 if(value > AssociatedConnectors.GetSize())
 {
  for(int i=value;i<AssociatedConnectors.GetSize();i++)
   for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
	Disconnect(AssociatedConnectors[i][j]);
 }

 AssociatedConnectors.Resize(value);*/
 NumOutputs=value;
 Ready = false;
 return true;
}

// Признак включения/выключения режима автоматического увеличения числа входов
// при подключении нового item.
const bool& UItem::GetAutoNumOutputs(void) const
{
 return AutoNumOutputs;
}

bool UItem::SetAutoNumOutputs(const bool &value)
{
 if(AutoNumOutputs == value)
  return true;

 AutoNumOutputs=value;
 return true;
}

/// Возвращает число выходов к которым кто-то подключен
int UItem::GetNumActiveOutputs(void) const
{
 return int(RelatedConnectors.size());
}

/// Возвращает число коннекторов к которым подключено заданное свойство
int UItem::GetNumActiveOutputs(const NameT &item_property_name) const
{
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(item_property_name);
 if(I == RelatedConnectors.end())
  return 0;

 return int(I->second.size());
}
// --------------------------

// --------------------------
// Методы доступа к описанию входов и выходов
// --------------------------
/// Ищет свойство-выход по заданному индексу
void UItem::FindOutputProperty(int index, UIProperty* &property) const
{
 // Ищем указатель на выходные данные
 property=0;
 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
 for(;I != J;++I)
 {
  if(I->second.Type & ptOutput)
  {
   property=I->second.Property.Get();
   if(!property || !property->CheckRange(index))
   {
    property=0;
	continue;
   }

   break;
  }
 }
}

void UItem::FindOutputProperty(const NameT &item_property_name, UIProperty* &property) const
{
 // Ищем указатель на выходные данные
 property=0;
 VariableMapCIteratorT I=PropertiesLookupTable.find(item_property_name);
 if(I == PropertiesLookupTable.end())
  return;

 if(I->second.Type & ptOutput)
 {
  property=I->second.Property.Get();
 }
}

/// Возвращает индекс входа с заданным именем
int UItem::FindOutputIndex(const NameT &output_name) const
{
 // Ищем указатель на выходные данные
 UIProperty* property=0;
 VariableMapCIteratorT I=PropertiesLookupTable.find(output_name);
 if(I == PropertiesLookupTable.end())
  return ForbiddenId;

 if(!(I->second.Type & ptOutput))
  return ForbiddenId;

 property=I->second.Property.Get();
 if(!property)
  return ForbiddenId;

 return property->GetMinRange();
}

// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
void UItem::Free(void)
{
 DisconnectAll();
 UConnector::Free();
}
// --------------------------

// ----------------------
// Защищенные коммуникационные методы
// ----------------------
// Устанавливает связь с элементом сети 'na' со входом по индексу index.
// Переназначает связь если na уже подключен.
bool UItem::ConnectToItem(UEPtr<UItem> na, int i_index, int &c_index)
{
 if(!UConnector::ConnectToItem(na, i_index, c_index))
  return false;

 // Ищем указатель на выходные данные
 UIProperty* output_property=0;
 na->FindOutputProperty(i_index, output_property);

// if(output_property)
//  CItemList[c_index].Name=output_property->GetName();

 // Ищем указатель на входные данные
 UIProperty* input_property=0;
 FindInputProperty(c_index, input_property);
 if(input_property)
 {
  if(input_property->GetIoType() & ipData)
  {
   if(input_property->GetIoType() & ipSingle)
   {
	if(output_property && output_property->CompareLanguageType(*input_property))
	 input_property->SetPointer(c_index,const_cast<void*>(output_property->GetPointer(i_index)));
   }
   else
   if(input_property->GetIoType() & ipRange)
   {
	if(output_property)
	 input_property->SetPointer(c_index,const_cast<void*>(output_property->GetPointer(i_index)));
   }
  }
  else
  if(input_property->GetIoType() & ipComp)
   input_property->SetPointer(c_index,na);
 }

 return true;
}

bool UItem::ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index)
{
 if(!UConnector::ConnectToItem(na, item_property_name, connector_property_name,c_index))
  return false;

 // Ищем указатель на выходные данные
 UIProperty* output_property=na->FindProperty(item_property_name);
// na->FindOutputProperty(i_index, output_property);

 if(output_property)
  ConnectedItemList[connector_property_name][c_index].Name=item_property_name;

 // Ищем указатель на входные данные
 UIProperty* input_property=FindProperty(connector_property_name);
// FindInputProperty(c_index, input_property);
 if(input_property)
 {
  if(input_property->GetIoType() & ipData)
  {
   if(input_property->GetIoType() & ipSingle)
   {
	if(output_property && output_property->CompareLanguageType(*input_property))
	 input_property->SetPointer(input_property->GetMinRange(),const_cast<void*>(output_property->GetPointer(output_property->GetMinRange())));
   }
   else
   if(input_property->GetIoType() & ipRange)
   {
	if(output_property)
	 input_property->SetPointer(input_property->GetMinRange(),const_cast<void*>(output_property->GetPointer(output_property->GetMinRange())));
   }
  }
  else
  if(input_property->GetIoType() & ipComp)
   input_property->SetPointer(input_property->GetMinRange(),na);
 }

 return true;
}

// Устанавливает связь с коннектором 'c'.
bool UItem::Connect(UEPtr<UConnector> c, int i_index, int c_index)
{
 if(c == 0)
  return false;

 if(i_index>=NumOutputs)
  SetNumOutputs(i_index+1);

 if(!Build())
  return false;

 UIProperty *i_item_property=0;
 UIProperty *i_conn_property=0;
 FindOutputProperty(i_index,i_item_property);
 c->FindInputProperty(c_index,i_conn_property);

 if(!i_item_property || !i_conn_property)
  return true;

 int cc_index;
 return Connect(c, i_item_property->GetName(), i_conn_property->GetName(), cc_index);
}

bool UItem::Connect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index)
{
 if(!c)
  return false;

 if(!Build())
  return false;

 if(!c->ConnectToItem(this,item_property_name, connector_property_name, c_index))
  return false;

 std::vector<PUAConnector> &vec=RelatedConnectors[item_property_name];
 for(size_t i=0;i<vec.size();i++)
  if(vec[i] == c)
   return true;

 vec.push_back(c);

 return true;
}

/// Разрывает все связи выхода этого объекта с коннектором 'c'.
void UItem::Disconnect(UEPtr<UConnector> c)
{
 Build();

 if(c)
  c->DisconnectFromItem(this);

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i] == c)
	I->second.erase(I->second.begin()+i);
   else
    ++i;
  }
 }
}

// Разрывает связь выхода этого объекта с коннектором 'c' по индексу
void UItem::Disconnect(UEPtr<UConnector> c, int i_index, int c_index)
{
 UIProperty *i_item_property=0;
 UIProperty *i_conn_property=0;
 FindOutputProperty(i_index,i_item_property);
 c->FindInputProperty(c_index,i_conn_property);

 if(!i_item_property || !i_conn_property)
  return;

 return Disconnect(c, i_item_property->GetName(), i_conn_property->GetName());
}

void UItem::Disconnect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name)
{
 if(!c)
  return;

 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.find(item_property_name);

 if(I == RelatedConnectors.end())
  return;

 int i=0;
 while(i<I->second.size())
 {
  if(I->second[i] == c)
  {
   UCItem citem=c->GetCItem(connector_property_name);
   if(citem.Name == item_property_name && citem.Item == this)
   {
	I->second.erase(I->second.begin()+i);
    c->DisconnectFromItem(this, item_property_name, connector_property_name);
   }
   else
    ++i;
  }
  else
   ++i;
 }
}
// ----------------------

// ----------------------
// Коммуникационные методы
// ----------------------
// Возвращает текущее число соединений.
int UItem::GetNumAConnectors(int index) const
{
 UIProperty *i_item_property=0;
 FindOutputProperty(index,i_item_property);

 if(!i_item_property)
  return 0;

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(i_item_property->GetName());

 if(I==RelatedConnectors.end())
  return 0;

 return int(I->second.size());
// return AssociatedConnectors[index].GetSize();//NumAConnectors[index];
}

int UItem::GetNumAConnectors(const NameT &item_property_name) const
{
 return GetNumActiveOutputs(item_property_name);
}

// Разрывает связь выхода этого объекта с коннектором по Id 'id'.
bool UItem::Disconnect(const UId &id)
{
 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i]->Id == id)
	Disconnect(I->second[i]);
   else
    ++i;
  }
 }

 return true;
}

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
void UItem::DisconnectAll(void)
{
 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=int(I->second.size())-1;
  while(i>0)
  {
   Disconnect(I->second[i]);
   i=int(I->second.size())-1;
  }
 }
}

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
void UItem::DisconnectBy(UEPtr<UContainer> brklevel)
{
 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(!I->second[i]->CheckOwner(brklevel))
	Disconnect(I->second[i]);
   else
    ++i;
  }
 }


}

// Переустанавливает все связи этого item со всеми connectors которые получают
// данные от этого item
void UItem::BuildLinks(void)
{

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(!I->second[i])
	continue;
   UCLink indexes=I->second[i]->GetCLink(this);
   int c_index;
   I->second[i]->ConnectToItem(this,indexes.OutputName,indexes.InputName,c_index);
  }

}

// Возвращает указатель на коннектор из списка подключений
// по имени 'name'.

UEPtr<UConnector> UItem::GetAConnector(const UId &id, int index) const
{
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(!I->second[i])
	continue;

   if(I->second[i]->Id == id)
    return I->second[i];
  }

 return 0;
}

// Возвращает  коннектор из списка подключений.
UEPtr<UConnector> UItem::GetAConnectorByIndex(int output, int index) const
{
 UIProperty *i_item_property=0;
 FindOutputProperty(output,i_item_property);

 if(!i_item_property)
  return 0;

 return GetAConnectorByIndex(i_item_property->GetName(), index);
}

UEPtr<UConnector> UItem::GetAConnectorByIndex(const NameT &item_property_name, int index) const
{
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(item_property_name);
 if(I == RelatedConnectors.end())
  return 0;

 if(index<0 || index>= int(I->second.size()))
  return 0;
 return I->second[index];
}

// Проверяет, существует ли связь с заданным коннектором
bool UItem::CheckLink(const UEPtr<UConnector> &connector) const
{
 UCLink link=connector->GetCLink(this);
 if(link.Output>=0 && link.Input >=0)
  return true;

 return false;
}

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UItem::CheckLink(const UEPtr<UConnector> &connector, int item_index) const
{
 UCLink link=connector->GetCLink(this);
 if(link.Input >=0)
 {
  if(link.Output == item_index || item_index <0)
   return true;
 }
 return false;
}

bool UItem::CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name) const
{
 UCLink link=connector->GetCLink(this);
 if(!link.InputName.empty())
 {
  if(link.OutputName == item_property_name)
   return true;
 }

 return false;
}


// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UItem::CheckLink(const UEPtr<UConnector> &connector, int item_index, int conn_index) const
{
 UCLink link=connector->GetCLink(this);
 if(link.Output>=0)
 {
  if(link.Input == conn_index || conn_index<0)
   return true;
 }

 return false;
}

bool UItem::CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name) const
{
 UCLink link=connector->GetCLink(this);
 if(!link.OutputName.empty())
 {
  if(link.InputName == connector_property_name)
   return true;
 }
 return false;
}

// Возвращает список подключений
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
template<typename T>
ULinksListT<T>& UItem::GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

 GetLongId(netlevel,item.Id);
 if(item.Id.size() == 0)
  return linkslist;
 link.Item=item;

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
 {
  link.Connector.clear();
  for(size_t i=0;i<I->second.size();i++)
  {
   if(exclude_internals)
   {
	if(I->second[i]->CheckOwner(internal_level))
	 continue;
   }
   I->second[i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	UCLink indexes=I->second[i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
	link.Item.Index=indexes.Output;
	link.Item.Name=indexes.OutputName;
	connector.Index=indexes.Input;
	connector.Name=indexes.InputName;

	link.Connector.push_back(connector);
	linkslist.Set(link);
   }
  }
 }

 return linkslist;

}

// Возвращает список подключений непосредственно коннектора cont
template<typename T>
ULinksListT<T>& UItem::GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

 GetLongId(netlevel,item.Id);
 if(item.Id.size() == 0)
  return linkslist;
 link.Item=item;

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
 {
  link.Connector.clear();
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i] != cont)
	continue;
   I->second[i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	UCLink indexes=I->second[i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
	link.Item.Index=indexes.Output;
	link.Item.Name=indexes.OutputName;
	connector.Index=indexes.Input;
	connector.Name=indexes.InputName;

	link.Connector.push_back(connector);
	linkslist.Set(link);
   }
  }
 }

 return linkslist;
}

// Возвращает список подключений этого компонента и всех дочерних компонент
// к заданному компоненту comp и всем его дочерним компонентам
template<typename T>
ULinksListT<T>& UItem::GetFullItemLinks(ULinksListT<T> &linkslist, UEPtr<UItem> comp,
                                     UEPtr<UContainer> netlevel) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

 if(!comp)
  return linkslist;

 GetLongId(netlevel,item.Id);
 if(link.Item.Id.GetSize() == 0)
  return linkslist;
 link.Item=item;


 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(!I->second[i]->CheckOwner(static_pointer_cast<UContainer>(comp)) && I->second[i] != comp)
	continue;
   I->second[i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.GetSize() != 0)
   {
	UCLink indexes=I->second[i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
	link.Item.Index=indexes.Output;
	link.Item.Name=indexes.OutputName;
	connector.Index=indexes.Input;
	connector.Name=indexes.InputName;

	link.Connector.push_back(connector);
	linkslist.Set(link);
   }
  }

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UItem> item=dynamic_cast<UItem*>(PComponents[i].operator->());
  item->GetFullItemLinks(linkslist, comp, netlevel);
 }

 return linkslist;

}
// ----------------------


// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UItem::Default(void)
{
 SetNumOutputs(1);
 SetAutoNumOutputs(true);
 return UConnector::Default();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UItem::Build(void)
{
 if(IsReady())
  return true;

 BuildLinks();

 return UConnector::Build();
}
/* *************************************************************************** */


}

#endif



