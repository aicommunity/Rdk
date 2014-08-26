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
 NumInputs=0;
 DummyItem.Index=-1;
 DummyItem.Item=0;
 AddLookupProperty("NumInputs",ptParameter & pgSystem,new UVProperty<int,UConnector>(this,&UConnector::SetNumInputs,&UConnector::GetNumInputs));
 AddLookupProperty("AutoNumInputs",ptParameter & pgSystem,new UVProperty<bool,UConnector>(this,&UConnector::SetAutoNumInputs,&UConnector::GetAutoNumInputs));
}

UConnector::~UConnector(void)
{
 DisconnectAllItems();
}
// --------------------------


// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Возвращает число подключенных элементов item
const int& UConnector::GetNumInputs(void) const
{
 return NumInputs;
}

// Устанавливает число подключенных элементов item
bool UConnector::SetNumInputs(const int &value)
{
 if(NumInputs == value)
  return true;
/*
 if(NumInputs > value)
 {
  for(int i=value;i<NumInputs;++i)
   if(CItemList[i].Item)
	CItemList[i].Item->Disconnect(this);
 }

 int index=NumInputs;
 CItemList.Resize(value);
 for(int i=index;i<value;i++)
 {
  CItemList[i].Item=0;
  CItemList[i].Index=-1;
 }

 Ready=false;
 NumInputs=CItemList.GetSize();*/

 Ready=false;
 NumInputs=value;

 return true;
}

// Признак включения/выключения режима автоматического увеличения числа входов
// при подключении нового item.
const bool& UConnector::GetAutoNumInputs(void) const
{
 return AutoNumInputs;
}

bool UConnector::SetAutoNumInputs(const bool &value)
{
 if(AutoNumInputs == value)
  return true;

 AutoNumInputs=value;
 return true;
}

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
const UCItem& UConnector::GetCItem(int c_index) const
{
// if(c_index>=CItemList.GetSize())
//  throw EInputIndexNotExist(c_index);
// return CItemList[c_index];

 UIProperty* i_conn_property=0;

 FindInputProperty(c_index, i_conn_property);

 if(!i_conn_property)
  return DummyItem;

 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(i_conn_property->GetName());
 if(I == ConnectedItemList.end())
  return DummyItem;

 if(I->second.empty())
  return DummyItem;

 return I->second[0];
}

const UCItem& UConnector::GetCItem(const NameT &connector_property_name, int index) const
{
 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return DummyItem;

 if(index>=int(I->second.size()))
  return DummyItem;
 return I->second[index];
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

 if(citem.Item == 0)
  return indexes;

 UIProperty* property=0;
 FindInputProperty(indexes.Input, property);
 if(property)
 {
  indexes.InputName=property->GetName();
  indexes.Input=property->GetMinRange();
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
void UConnector::FindInputProperty(int index, UIProperty* &property) const
{
 // Ищем указатель на входные данные
 property=0;

 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
 J=PropertiesLookupTable.end();
 for(;I != J;++I)
 {
  if(I->second.Type & ptInput)
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

// --------------------------

// ----------------------
// Коммуникационные методы
// ----------------------
// Устанавливает связь с элементом сети 'na'.
bool UConnector::ConnectToItem(UEPtr<UItem> na, int i_index, int &c_index)
{
 UIProperty* i_item_property=0;
 UIProperty* i_conn_property=0;

 na->FindOutputProperty(i_index,i_item_property);
 FindInputProperty(c_index,i_conn_property);

 if(!i_item_property || !i_conn_property)
  return false;

 return ConnectToItem(na, i_item_property->GetName(), i_conn_property->GetName(), c_index);

/*
 if(!na)
  return false;

 if((c_index < 0 || c_index >= int(NumInputs)) && AutoNumInputs == false)
  return false;

 if(i_index < 0 || i_index >= na->GetNumOutputs())
  return false;

 // Если включен режим автоматического добавления нового входа
 // и c_index<0 то добавляем новый вход либо подключаемся к наименьшему свободному
 if(c_index<0)
 {
  int i=0;
  for(i=0;i<CItemList.GetSize();i++)
   if(!CItemList[i].Item)
    break;

  if(i == NumInputs)
  {
   SetNumInputs(NumInputs+1);
   CItemList.Resize(NumInputs+1);
   c_index=NumInputs-1;
  }
  else
   c_index=i;
 }
 else // Если c_index >= NumInputs то расширяем число входов до требуемого
 if(c_index >= NumInputs)
 {
  SetNumInputs(c_index+1);
 }

 if(!Build())
  return false;

 if(!CheckItem(na, i_index,c_index))
  return false;

 if(CItemList[c_index].Item == na)
 {
  if(CItemList[c_index].Index == i_index)
   return true;

  CItemList[c_index].Index = i_index;
  return true;
 }

 if(CItemList[c_index].Item)
  return false;

 CItemList[c_index].Item=na;
 CItemList[c_index].Index=i_index;

 return AConnectToItem(na, i_index, c_index);    */
}

bool UConnector::ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index)
{
 if(!na)
  return false;

 UIProperty* i_item_property=na->FindProperty(item_property_name);
 UIProperty* i_conn_property=FindProperty(connector_property_name);

 if(!i_item_property || !i_conn_property)
  return false;

 if(!Build())
  return false;

 if(!CheckItem(na, item_property_name, connector_property_name))
  return false;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);
 if(I != ConnectedItemList.end())
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
//CItemList[i_conn_property->GetMinRange()].Index = i_item_property->GetMinRange();
	 return true;
	}
   }
  }

 UCItem item;
 item.Item=na;
 item.Index=-1;
 item.Name=item_property_name;
 ConnectedItemList[connector_property_name].push_back(item);
 c_index=int(ConnectedItemList[connector_property_name].size()-1);

//CItemList[i_conn_property->GetMinRange()].Item = na;
//CItemList[i_conn_property->GetMinRange()].Index = i_item_property->GetMinRange();

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
  for(size_t i=0;i<I->second.size();i++)
   if(I->second[i].Item == na)
	DisconnectFromIndex(I->first, i);
 }
}
/// Разрывает связь с элементом сети 'na', подключенную от i_index
void UConnector::DisconnectFromItem(UEPtr<UItem> na, int i_index)
{
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  for(size_t i=0;i<I->second.size();i++)
   if(I->second[i].Item == na && I->second[i].Index == i_index)
	DisconnectFromIndex(I->first, i);
 }
}

/// Разрывает связь с элементом сети 'na', подключенную от i_index к c_index
void UConnector::DisconnectFromItem(UEPtr<UItem> na, int i_index, int c_index)
{
/*
 if(!na)
  return;
 if(i_index<0 || i_index>=na->GetNumOutputs())
  return;

 if(c_index<0 || c_index>=CItemList.GetSize())
  return;

  if(CItemList[c_index].Item == na && CItemList[c_index].Index == i_index)
  {
   DisconnectFromIndex(c_index);
 }

 */

 // TODO тут не применен c_index
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  for(size_t i=0;i<I->second.size();i++)
   if(I->second[i].Item == na && I->second[i].Index == i_index)
	DisconnectFromIndex(I->first, i);
 }

}

// Разрывает связь с элементом сети подключенным ко входу 'index'
void UConnector::DisconnectFromIndex(int c_index)
{
/*
 if(c_index<0 || c_index >=NumInputs)
  return;

 ADisconnectFromItem(CItemList[c_index].Item,CItemList[c_index].Index,c_index);

 CItemList[c_index].Item=0;
 CItemList[c_index].Index=-1;

 */
 UIProperty* i_conn_property=0;
 FindInputProperty(c_index,i_conn_property);

 if(!i_conn_property)
  return;

 DisconnectFromIndex(i_conn_property->GetName(), 0);
}

void UConnector::DisconnectFromIndex(const NameT &connector_property_name, int index)
{
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return;

 if(index<0 || index >=int(I->second.size()))
  return;


 ADisconnectFromItem(I->second[index].Item,I->second[index].Name,connector_property_name);
 I->second.erase(I->second.begin()+index);
// if(I->second.empty())
//  ConnectedItemList.erase(I);
}

// Выполняет действия после физически установленой связи
bool UConnector::AConnectToItem(UEPtr<UItem> na, int i_index, int c_index)
{
 return true;
}

bool UConnector::AConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
 UIProperty* i_item_property=na->FindProperty(item_property_name);
 UIProperty* i_conn_property=FindProperty(connector_property_name);

 if(!i_item_property || !i_conn_property)
  return false;

 return AConnectToItem(na,i_item_property->GetMinRange(), i_conn_property->GetMinRange());
}

// Выполняет действия после физически разорваной связи
void UConnector::ADisconnectFromItem(UEPtr<UItem> na, int i_index, int c_index)
{
}

void UConnector::ADisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
 UIProperty* i_item_property=na->FindProperty(item_property_name);
 UIProperty* i_conn_property=FindProperty(connector_property_name);

 if(!i_item_property || !i_conn_property)
  return;

 AConnectToItem(na,i_item_property->GetMinRange(), i_conn_property->GetMinRange());
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
  for(size_t i=0;i<I->second.size();i++)
   I->second[i].Item->Disconnect(this);
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
  for(size_t i=0;i<I->second.size();i++)
   if(!I->second[i].Item->CheckOwner(brklevel))
	I->second[i].Item->Disconnect(this);
 }

}

// Проверяет, допустимо ли подключение заданного item к этому коннектору
bool UConnector::CheckItem(UEPtr<UItem> item, int item_index, int conn_index)
{
/* if(item->GetOutputType(item_index) == GetInputType(conn_index))
  return true;
 return false;*/
 return true;
}

bool UConnector::CheckItem(UEPtr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name)
{
 return true;
}


// Проверяет, существует ли связь с заданным коннектором
bool UConnector::CheckLink(const UEPtr<UItem> &item) const
{
 UCLink link=GetCLink(item);
 if(link.Output>=0 && link.Input >=0)
  return true;

 return false;
}

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UConnector::CheckLink(const UEPtr<UItem> &item, int item_index) const
{
 UCLink link=GetCLink(item);
 if(link.Input >=0)
 {
  if(link.Output == item_index || item_index <0)
   return true;
 }
 return false;
}

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
bool UConnector::CheckLink(const UEPtr<UItem> &item, int item_index, int conn_index) const
{
 UCLink link=GetCLink(item);
 if(link.Output>=0)
 {
  if(link.Input == conn_index || conn_index<0)
   return true;
 }

 return false;
}

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
   connector.Index=-1;//i;
//   UIProperty* property=0;
//   FindInputProperty(i, property);
//   if(property)
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
	connector.Index=i;
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
 SetNumInputs(1);
 SetAutoNumInputs(true);
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

