/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UACONNECTOR_CPP
#define UACONNECTOR_CPP

#include <string.h>
#include "UConnector.h"
#include "UStorage.h"
#include "UItem.h"

namespace RDK {

/* *************************************************************************** */
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
bool UCItem::operator == (const UCItem &value)
{
 return (Item == value.Item) & (Index == value.Index);
}

bool UCItem::operator != (const UCItem &value)
{
 return !(*this == value);
}
// --------------------------

/* *************************************************************************** */

/* *************************************************************************** */
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
/* *************************************************************************** */

/* *************************************************************************** */
// Контейнер - список указателей на подключенные элементы
// class UCItemList
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UCItemList::UCItemList(void)
{
 // Размер контейнера
 Size=0;

 // Массив элементов
 Data=0;

 // Реальный размер контейнера
 RealSize=0;
}

UCItemList::UCItemList(const UCItemList &copy)
{
 *this=copy;
}


UCItemList::~UCItemList(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает контейнер
void UCItemList::Clear(void)
{
 if(Data)
 {
  delete []Data;
  Data=0;
 }
 Size=RealSize=0;
}

// Меняет размер контейнера с сохранением элементов
void UCItemList::Resize(int newsize)
{
 if(Size == newsize)
  return;

 if(RealSize<newsize || !Data)
 {
  UCItem* newbuffer=0;
  if(newsize)
  {
   newbuffer=new UCItem[newsize];
   for(int i=0;i<Size;i++)
	newbuffer[i]=Data[i];
//   memcpy(newbuffer,Data,sizeof(UCItem)*Size);
  }

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
// и возвращает его описание
UCItem UCItemList::Find(const UEPtr<UItem> &item, int index) const
{
 return Find(item.Get(),index);
}

UCItem UCItemList::Find(const UItem *const item, int index) const
{
 UCItem *pdata=Data;

 for(int i=index;i<Size;i++,pdata++)
  if(pdata->Item == item)
   return *pdata;

 return UCItem();
}

// Ищет в контейнере первый заданный элемент начиная с индекса index
// и возвращает его индекс или -1 если элемент не найден
int UCItemList::Find(const UCItem &item, int index) const
{
 UCItem *pdata=Data;

 for(int i=index;i<Size;i++,pdata++)
  if(*pdata == item)
   return i;

 return -1;
}

// Добавляет в конец контейнера элемент
void UCItemList::Add(const UCItem &item)
{
 Resize(Size+1);
 Data[Size-1]=item;
}

// Добавляет в конец контейнера элемент только если таких элементов еще нет
// Возвращает true в случае успешного добавления
bool UCItemList::AddUnique(const UCItem &item)
{
 if(Find(item)>=0)
  return false;

 Add(item);
 return true;
}

// Удаляет элемент по индексу
void UCItemList::Del(int index)
{
 if(index<0 || index >= Size)
  return;

 memcpy(Data+index,Data+index+1,(Size-1)*sizeof(UCItem));
 Resize(Size-1);
}

// Удаляет элемент по указателю
void UCItemList::Del(UCItem *item)
{
 int index=Find(*item);
 Del(index);
}
// --------------------------

// --------------------------
// Методы доступа
// --------------------------
// Оператор присваивания
UCItemList& UCItemList::operator = (const UCItemList &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Data[i]=copy.Data[i];
 //memcpy(Data,copy.Data,Size*sizeof(UCItem));

 return *this;
}

// Оператор доступа к элементу
UCItem& UCItemList::operator [] (int index)
{
 return Data[index];
}

const UCItem& UCItemList::operator [] (int index) const
{
 return Data[index];
}

// Оператор доступа к массиву
UCItem* UCItemList::GetData(void)
{
 return Data;
}

// Оператор доступа к размеру
int UCItemList::GetSize(void) const
{
 return Size;
}
// --------------------------
/* *************************************************************************** */

/* *************************************************************************** */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UConnector::UConnector(void)
{
 DummyItem.Index=-1;
 DummyItem.Item=0;
}

UConnector::~UConnector(void)
{
 DisconnectAllItems();
}
// --------------------------


// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
/// Возвращает число реально подключенных элементов
int UConnector::GetNumActiveInputs(void) const
{
 return int(ConnectedItemList.size());
}

/// Возвращает число реально подключенных элементов к заданному свойству
int UConnector::GetNumActiveInputs(const NameT &connector_property_name) const
{
 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return 0;

 return int(I->second.size());
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
void UConnector::Free(void)
{
 DisconnectAllItems();
 UContainer::Free();
}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает подключенный к этому коннектору объект по индексу
const UCItem& UConnector::GetCItem(const NameT &connector_property_name, int index) const
{
 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return DummyItem;

 if(index>=int(I->second.size()))
  return DummyItem;
 return I->second[index];
}

const UCItem& UConnector::GetCItem(const NameT &connector_property_name, const UEPtr<UItem> &item, int &index) const
{
 index=-1;
 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return DummyItem;

 for(size_t i=0;i<I->second.size();i++)
  if(I->second[i].Item == item)
  {
   index = i;
   return I->second[i];
  }

 return DummyItem;
}

void UConnector::GetCItem(const NameT &connector_property_name, std::vector<UCItem> &buffer) const
{
 buffer.clear();
 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(connector_property_name);
 if(I != ConnectedItemList.end())
  buffer=I->second;
}


// Возвращает информацию об индексах связей с этим item или -1, -1
// если такая связь отсутствует
UCLink UConnector::GetCLink(const UEPtr<UItem> &item) const
{
/* UCLink indexes;

 if(!item)
  return indexes;

 UCItem citem=CItemList.Find(item);

 if(citem.Item == 0)
  return indexes;

 indexes.Input=CItemList.Find(citem);
 UIProperty* property=0;
 FindInputProperty(indexes.Input, property);
 if(property)
  indexes.InputName=property->GetName();
 indexes.Output=citem.Index;
 indexes.OutputName=citem.Name;

 return indexes;
 */

 UCLink indexes;

 if(!item)
  return indexes;

 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
 UCItem citem;
 bool flag=false;
 for(;I != ConnectedItemList.end();++I)
 {
  for(size_t i=0;i<I->second.size();i++)
   if(I->second[i].Item == item)
   {
	citem=I->second[i];
	flag=true;
	break;
   }
  if(flag)
   break;
 }

 if(I == ConnectedItemList.end())
  return indexes;

 if(citem.Item == 0)
  return indexes;

 UIProperty* property=0;
// if(!citem.Name.empty())
 FindInputProperty(I->first, property);
// else
//  FindInputProperty(citem.Index, property);
 if(property)
 {
  indexes.InputName=property->GetName();
//  indexes.Input=property->GetMinRange();
 }
 indexes.Output=citem.Index;
 indexes.OutputName=citem.Name;

 return indexes;
}

UCLink UConnector::GetCLink(const UItem* const item) const
{
/*
 UCLink indexes;

 if(!item)
  return indexes;

 UCItem citem=CItemList.Find(item);

 if(citem.Item == 0)
  return indexes;

 indexes.Input=CItemList.Find(citem);
 UIProperty* property=0;
 FindInputProperty(indexes.Input, property);
 if(property)
  indexes.InputName=property->GetName();
 indexes.Output=citem.Index;
 indexes.OutputName=citem.Name;

 return indexes;
 */
 const UEPtr<UItem> uitem=const_cast<UItem*>(item);
 return GetCLink(uitem);
}
// --------------------------

// --------------------------
// Методы доступа к описанию входов и выходов
// --------------------------
/// Ищет свойство-вход по заданному индексу
void UConnector::FindInputProperty(const NameT &connector_property_name, UIProperty* &property) const
{
 // Ищем указатель на входные данные
 property=0;

 VariableMapCIteratorT I=PropertiesLookupTable.find(connector_property_name);

 if(I == PropertiesLookupTable.end())
  return;

 if(I->second.Type & ptInput)
 {
  property=I->second.Property.Get();
 }
}
	 /*
/// Возвращает индекс входа с заданным именем
int UConnector::FindInputIndex(const NameT &input_name) const
{
 // Ищем указатель на входные данные
 UIProperty* property=0;

 VariableMapCIteratorT I=PropertiesLookupTable.find(input_name);
 if(I == PropertiesLookupTable.end())
  return ForbiddenId;

 if(!(I->second.Type & ptInput))
  return ForbiddenId;

 property=I->second.Property.Get();
 if(!property)
  return ForbiddenId;

 return property->GetMinRange();
}
			   */
// --------------------------

// ----------------------
// Коммуникационные методы
// ----------------------
// Устанавливает связь с элементом сети 'na'.
bool UConnector::ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index)
{
 if(!na)
  return false;

 if(!Build())
  return false;

 UIProperty* i_item_property=na->FindProperty(item_property_name);
 UIProperty* i_conn_property=FindProperty(connector_property_name);

 if(!i_item_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item_property_name);
  return false;
 }

 if(!i_conn_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector is empty and AutoNumInputs disabled"));
  return false;
 }

 if(!CheckItem(na, item_property_name, connector_property_name))
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Checking fail"));
  return false;
 }

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);
 if(I != ConnectedItemList.end())
 {
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i].Item == na)
   {
	if(I->second[i].Name == item_property_name)
	{
	 c_index=i;
	 return true;
	}
	else
	{
	 I->second[i].Name = item_property_name;
	 c_index=i;
	 return true;
	}
   }
  }
 }


 UCItem item;
 item.Item=na;
 item.Index=-1;
 item.Name=item_property_name;
 ConnectedItemList[connector_property_name].push_back(item);
 c_index=int(ConnectedItemList[connector_property_name].size()-1);

 return AConnectToItem(na, item_property_name, connector_property_name);
}

/// Разрывает все связи с элементом сети 'na'
void UConnector::DisconnectFromItem(UEPtr<UItem> na)
{
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i].Item == na)
	DisconnectFromIndex(I->first, I->second[i].Name,i);
   else
    ++i;
  }
 }
}

/// Разрывает связь с элементом сети 'na', подключенную от i_index
void UConnector::DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name)
{
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i].Item == na && I->second[i].Name == item_property_name)
	DisconnectFromIndex(I->first,I->second[i].Name,i); // TODO индекс не определен
   else
    ++i;
  }
 }
}

/// Разрывает связь с элементом сети 'na', подключенную от i_index к c_index
void UConnector::DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);

 if(I == ConnectedItemList.end())
  return;

 int i=0;
 while(i<int(I->second.size()))
 {
// for(size_t i=0;i<I->second.size();i++)
  if(I->second[i].Item == na && I->second[i].Name == item_property_name)
   DisconnectFromIndex(connector_property_name,I->second[i].Name,i); // TODO индекс не определен
  else
   ++i;
 }
}

// Разрывает связь с элементом сети подключенным ко входу 'index'
void UConnector::DisconnectFromIndex(const NameT &connector_property_name, const NameT &item_property_name, int index)
{
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return;

 if(item_property_name.empty())
 {
  DisconnectFromIndex(connector_property_name);
 }
 else
 {
  if(index<0 || index >=int(I->second.size()))
   return;

   if(I->second[index].Name == item_property_name)
   {
	UIProperty* i_conn_property=0;
	FindInputProperty(connector_property_name,i_conn_property);
//	if(i_conn_property->CheckRange(index)) // TODO тут возможно что-то другое
	 i_conn_property->SetPointer(index,0);

	ADisconnectFromItem(I->second[index].Item,I->second[index].Name,connector_property_name);
	I->second.erase(I->second.begin()+index);
   }
 }
}

void UConnector::DisconnectFromIndex(const NameT &connector_property_name)
{
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return;

  int i=0;
  while(i<int(I->second.size()))
  {
	UIProperty* i_conn_property=0;
	FindInputProperty(connector_property_name,i_conn_property);
//	if(i_conn_property->CheckRange(i)) // TODO воозможно тут что то другое
	 i_conn_property->SetPointer(i,0);

	ADisconnectFromItem(I->second[i].Item,I->second[i].Name,connector_property_name);
	I->second.erase(I->second.begin()+i);
  }
}


// Выполняет действия после физически установленой связи
bool UConnector::AConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
/* UIProperty* i_item_property=na->FindProperty(item_property_name);
 UIProperty* i_conn_property=FindProperty(connector_property_name);

 if(!i_item_property || !i_conn_property)
  return false;

 return AConnectToItem(na,i_item_property->GetMinRange(), i_conn_property->GetMinRange());
 */
 return true;
}

// Выполняет действия после физически разорваной связи
void UConnector::ADisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
/* UIProperty* i_item_property=na->FindProperty(item_property_name);
 UIProperty* i_conn_property=FindProperty(connector_property_name);

 if(!i_item_property || !i_conn_property)
  return;

 AConnectToItem(na,i_item_property->GetMinRange(), i_conn_property->GetMinRange());
 */
}

// Разрывает все текущие связи
void UConnector::DisconnectAllItems(void)
{
/*
 for(int i=0;i<CItemList.GetSize();i++)
  if(CItemList[i].Item)
   CItemList[i].Item->Disconnect(this);
   */
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  while(!I->second.empty())
  {
   int index=int(I->second.size())-1;
   I->second[index].Item->Disconnect(this);
  }
//  for(size_t i=0;i<I->second.size();i++)
//   I->second[i].Item->Disconnect(this);
 }

}

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
void UConnector::DisconnectByObject(UEPtr<UContainer> brklevel)
{
/* for(int i=0;i<CItemList.GetSize();i++)
  if(CItemList[i].Item)
   if(!CItemList[i].Item->CheckOwner(brklevel))
	CItemList[i].Item->Disconnect(this);
	*/
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  size_t i=0;
  while(i<I->second.size())
  {
   if(!I->second[i].Item->CheckOwner(brklevel))
	I->second[i].Item->Disconnect(this);
   else
    ++i;
  }
//  for(size_t i=0;i<I->second.size();i++)
//   if(!I->second[i].Item->CheckOwner(brklevel))
//	I->second[i].Item->Disconnect(this);
 }

}

// Проверяет, допустимо ли подключение заданного item к этому коннектору
bool UConnector::CheckItem(UEPtr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name)
{
 return true;
}


// Проверяет, существует ли связь с заданным коннектором
bool UConnector::CheckLink(const UEPtr<UItem> &item) const
{
 UCLink link=GetCLink(item);
 if((link.Output>=0 && link.Input >=0) || (!link.InputName.empty() && !link.OutputName.empty()))
  return true;

 return false;
}

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UConnector::CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name) const
{
 UCLink link=GetCLink(item);
 if(!link.InputName.empty())
 {
  if(link.OutputName == item_property_name)
   return true;
 }
 return false;
}

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UConnector::CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name, const NameT &connector_property_name) const
{
 UCLink link=GetCLink(item);
 if(!link.OutputName.empty())
 {
  if(link.InputName == connector_property_name)
   return true;
 }

 return false;
}

// Возвращает список подключений
template<typename T>
ULinksListT<T>& UConnector::GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
/*
 ULinkT<T> link;
 ULinkSideT<T> connector;
 ULinkSideT<T> item;
 GetLongId(netlevel,connector.Id);
 if(connector.Id.size()==0)
  return linkslist;

 for(int i=0;i<CItemList.GetSize();i++)
 {
  if(CItemList[i].Item)
  {
   if(exclude_internals)
   {
	if(CItemList[i].Item->CheckOwner(internal_level))
	 continue;
   }
   CItemList[i].Item->GetLongId(netlevel,item.Id);
   connector.Index=i;
   UIProperty* property=0;
   FindInputProperty(i, property);
   if(property)
	connector.Name=property->GetName();

   item.Index=CItemList[i].Index;
   item.Name=CItemList[i].Name;
   if(connector.Id.size() != 0)
   {
	int item_id=linkslist.FindItem(item);
	if(item_id >= 0)
	{
	 if(linkslist[item_id].FindConnector(connector) >= 0)
	  continue;
	 linkslist[item_id].Connector.push_back(connector);
	}
	else
	{
	 link.Item=item;
	 link.Connector.clear();
	 link.Connector.push_back(connector);
	 linkslist.Add(link);
	}
   }
  }
 }

 return linkslist;
 */

 ULinkT<T> link;
 ULinkSideT<T> connector;
 ULinkSideT<T> item;
 GetLongId(netlevel,connector.Id);
 if(connector.Id.size()==0)
  return linkslist;


 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i].Item)
   {
	if(exclude_internals)
	{
	 if(I->second[i].Item->CheckOwner(internal_level))
	  continue;
	}
   I->second[i].Item->GetLongId(netlevel,item.Id);
   UIProperty* property=0;
   FindInputProperty(I->first, property);
   if(property)
	connector.Index=-1;//property->GetMinRange();
   else
    connector.Index=-1;//i;
   connector.Name=I->first;

   item.Index=-1;//CItemList[i].Index;
   item.Name=I->second[i].Name;//CItemList[i].Name;
   if(connector.Id.size() != 0)
   {
	int item_id=linkslist.FindItem(item);
	if(item_id >= 0)
	{
	 if(linkslist[item_id].FindConnector(connector) >= 0)
	  continue;
	 linkslist[item_id].Connector.push_back(connector);
	}
	else
	{
	 link.Item=item;
	 link.Connector.clear();
	 link.Connector.push_back(connector);
	 linkslist.Add(link);
	}
   }
  }
 }

 return linkslist;
}

// Возвращает список подключений непосредственно коннектора cont
template<typename T>
ULinksListT<T>& UConnector::GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const
{
/*
 ULinkT<T> link;
 ULinkSideT<T> connector;
 ULinkSideT<T> item;
 GetLongId(netlevel,connector.Id);
 if(connector.Id.size()==0)
  return linkslist;

 for(int i=0;i<CItemList.GetSize();i++)
 {
  if(CItemList[i].Item == cont)
  {
   CItemList[i].Item->GetLongId(netlevel,item.Id);
   connector.Index=i;
   UIProperty* property=0;
   FindInputProperty(i, property);
   if(property)
	connector.Name=property->GetName();
   item.Index=CItemList[i].Index;
   item.Name=CItemList[i].Name;
   if(connector.Id.size() != 0)
   {
	int item_id=linkslist.FindItem(item);
	if(item_id >= 0)
	{
	 if(linkslist[item_id].FindConnector(connector) >= 0)
	  continue;
	 linkslist[item_id].Connector.push_back(connector);
	}
	else
	{
	 link.Item=item;
	 link.Connector.clear();
	 link.Connector.push_back(connector);
	 linkslist.Add(link);
	}
   }
  }
 }

 return linkslist;*/

 ULinkT<T> link;
 ULinkSideT<T> connector;
 ULinkSideT<T> item;
 GetLongId(netlevel,connector.Id);
 if(connector.Id.size()==0)
  return linkslist;

 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i].Item == cont)
   {
	I->second[i].Item->GetLongId(netlevel,item.Id);
	UIProperty* property=0;
	FindInputProperty(I->first, property);
	if(property)
	 connector.Index=property->GetMinRange();
	else
	 connector.Index=-1;//i; // TODO тут неопределенность
	connector.Name=I->first;
	item.Index=-1;//CItemList[i].Index;
	item.Name=I->second[i].Name;
	if(connector.Id.size() != 0)
	{
	 int item_id=linkslist.FindItem(item);
	 if(item_id >= 0)
	 {
	  if(linkslist[item_id].FindConnector(connector) >= 0)
	   continue;
	 linkslist[item_id].Connector.push_back(connector);
	}
	else
	{
	 link.Item=item;
	 link.Connector.clear();
	 link.Connector.push_back(connector);
	 linkslist.Add(link);
	}
   }
  }
 }

 return linkslist;
}
// ----------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UConnector::Default(void)
{
 return UContainer::Default();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UConnector::Build(void)
{
 return UContainer::Build();
}
// --------------------------

/* *************************************************************************** */

//class UIPropertyInput: public UIPropertyIO
// Возвращает указатель на компонент-источник
UEPtr<UItem> UIPropertyInput::GetItem(void)
{
 return Item;
}

// Возвращает имя подключенного выхода
const std::string& UIPropertyInput::GetItemOutputName(void)
{
 return ItemOutputName;
}

//class UIPropertyOutput: public UIPropertyIO
// Возвращает число подключенных входов
size_t UIPropertyOutput::GetNumConnectors(void)
{
 return Connectors.size();
}

// Возвращает указатель на компонент-приемник
UEPtr<UConnector> UIPropertyOutput::GetConnector(int index)
{
 return Connectors[index];
}

// Возвращает имя подключенного входа компонента-приемника
const std::string& UIPropertyOutput::GetConnectorInputName(int index)
{
 return ConnectorInputNames[index];
}

}
#endif

