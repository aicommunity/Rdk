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
void UConnector::GetCLink(const UEPtr<UItem> &item, std::vector<UCLink> &buffer) const
{
 UCLink indexes;
 buffer.clear();

 if(!item)
  return;

 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
 UCItem citem;
 for(;I != ConnectedItemList.end();++I)
 {
  for(size_t i=0;i<I->second.size();i++)
   if(I->second[i].Item == item)
   {
	citem=I->second[i];

	if(citem.Item == 0)
	 continue;

	UIProperty* property=0;
	FindInputProperty(I->first, property);
	if(property)
	{
	 indexes.InputName=property->GetName();
	}
	indexes.Output=citem.Index;
	indexes.OutputName=citem.Name;
	buffer.push_back(indexes);
   }
 }
}

void UConnector::GetCLink(const UItem* const item, std::vector<UCLink> &buffer) const
{
 const UEPtr<UItem> uitem=const_cast<UItem*>(item);
 GetCLink(uitem,buffer);
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

 if(!i_item_property && !(i_conn_property && (i_conn_property->GetIoType() & ipComp)))
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item_property_name);
  return false;
 }

 if(!i_conn_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector is empty and AutoNumInputs disabled"));
  return false;
 }

 if(!(i_conn_property->GetIoType() & ipComp) && !CheckItem(na, item_property_name, connector_property_name))
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Checking fail"));
  return false;
 }

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);
 if(I != ConnectedItemList.end())
 {
  if(c_index == -1)
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
//	  I->second[i].Name = item_property_name;
//	  c_index=i;
//	  return true;
	 }
	}
   }
  }
  else
  {
   if(int(I->second.size())>c_index && I->second[c_index].Item == na)
   {
	return true;
   }
  }
 }

 // TODO: Этот код не будет работать в случае, если c_index будет подаваться на
 // вход не по возрастанию
 UCItem item;
 item.Item=na;
 item.Index=-1;
 item.Name=item_property_name;
 ConnectedItemList[connector_property_name].push_back(item);
 c_index=int(ConnectedItemList[connector_property_name].size())-1;

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
void UConnector::DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index)
{
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);

 if(I == ConnectedItemList.end())
  return;

/*
 int i=0;
 while(i<int(I->second.size()))
 {
  if(I->second[i].Item == na && I->second[i].Name == item_property_name)
   DisconnectFromIndex(connector_property_name,I->second[i].Name,i); // TODO: индекс не определен
  else
   ++i;
 }*/
 DisconnectFromIndex(connector_property_name,I->second[connected_c_index].Name,connected_c_index); // TODO индекс не определен
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
//	if(i_conn_property->CheckRange(index)) // TODO: тут возможно что-то другое
	UIProperty* output_property=I->second[index].Item->FindProperty(item_property_name);
	if(output_property)
	{
	 if(i_conn_property->GetIoType() & ipData)
	 {
	  if(!i_conn_property->ResetPointer(index,const_cast<void*>(output_property->GetPointer(0))))
	  {
	   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Data ResetPointer fail"));
	  }
	 }
	 else
	 if(i_conn_property->GetIoType() & ipComp)
	 {
	  if(!i_conn_property->ResetPointer(index,reinterpret_cast<void*>(I->second[index].Item)))
	  {
	   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Comp ResetPointer fail"));
	  }

	 }
	 else
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("ResetPointer fail"));
	}
	else
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found"));

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
//	if(i_conn_property->CheckRange(i)) // TODO: воозможно тут что то другое

	UIProperty* output_property=I->second[i].Item->FindProperty(I->second[i].Name);
	if(output_property)
	{
	 if(!i_conn_property->ResetPointer(i,const_cast<void*>(output_property->GetPointer(0))))
	 {
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("ResetPointer fail"));
	 }
	}
	else
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found"));

	ADisconnectFromItem(I->second[i].Item,I->second[i].Name,connector_property_name);
	I->second.erase(I->second.begin()+i);
  }
}


// Выполняет действия после физически установленой связи
bool UConnector::AConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
 return true;
}

// Выполняет действия после физически разорваной связи
void UConnector::ADisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
}

// Разрывает все текущие связи
void UConnector::DisconnectAllItems(void)
{
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  while(!I->second.empty())
  {
   int index=int(I->second.size())-1;
   I->second[index].Item->Disconnect(this);
  }
 }
}

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
void UConnector::DisconnectByObject(UEPtr<UContainer> brklevel)
{
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
 std::vector<UCLink> buffer;
 GetCLink(item,buffer);
 for(size_t i=0;i<buffer.size();i++)
 {
  UCLink &link=buffer[i];
  if((link.Output>=0 && link.Input >=0) || (!link.InputName.empty() && !link.OutputName.empty()))
   return true;
 }

 return false;
}

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UConnector::CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name) const
{
 std::vector<UCLink> buffer;
 GetCLink(item,buffer);
 for(size_t i=0;i<buffer.size();i++)
 {
  UCLink &link=buffer[i];
  if(!link.InputName.empty())
  {
   if(link.OutputName == item_property_name)
	return true;
  }
 }
 return false;
}

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UConnector::CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name, const NameT &connector_property_name) const
{
 std::vector<UCLink> buffer;
 GetCLink(item,buffer);
 for(size_t i=0;i<buffer.size();i++)
 {
  UCLink &link=buffer[i];
  if(!link.OutputName.empty())
  {
   if(link.InputName == connector_property_name)
	return true;
  }
 }

 return false;
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
UItem* UIPropertyInput::GetItem(void)
{
 return Item;
}

// Возвращает имя подключенного выхода
const std::string& UIPropertyInput::GetItemOutputName(void)
{
 return ItemOutputName;
}


/// Возвращает true, если на подключенном выходе новые данные
bool UIPropertyInput::IsNewData(void) const
{
 return true;
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

