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
#include "UAConnector.h"
#include "UAStorage.h"
#include "UAItem.h"

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
  UCItem* newbuffer=new UCItem[newsize];
  memcpy(newbuffer,Data,sizeof(UCItem)*Size);

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
UCItem UCItemList::Find(UEPtr<UAItem> item, int index) const
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
 memcpy(Data,copy.Data,Size*sizeof(UCItem));

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
UAConnector::UAConnector(void)
{
 NumInputs=0;
 AddLookupProperty("NumInputs",new UVProperty<int,UAConnector>(this,&UAConnector::SetNumInputs,&UAConnector::GetNumInputs));
 AddLookupProperty("AutoNumInputs",new UVProperty<bool,UAConnector>(this,&UAConnector::SetAutoNumInputs,&UAConnector::GetAutoNumInputs));
}

UAConnector::~UAConnector(void)
{
 DisconnectAllItems();
}
// --------------------------


// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Возвращает число подключенных элементов item
int UAConnector::GetNumInputs(void) const
{
 return NumInputs;
}

// Устанавливает число подключенных элементов item
bool UAConnector::SetNumInputs(int value)
{
 if(NumInputs == value)
  return true;

// NumInputs=CItemList.size();
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
 NumInputs=CItemList.GetSize();
 return true;
}

// Признак включения/выключения режима автоматического увеличения числа входов
// при подключении нового item.
bool UAConnector::GetAutoNumInputs(void) const
{
 return AutoNumInputs;
}

bool UAConnector::SetAutoNumInputs(bool value)
{
 if(AutoNumInputs == value)
  return true;

 AutoNumInputs=value;
 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
void UAConnector::Free(void)
{
 DisconnectAllItems();
 UAContainer::Free();
}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает подключенный к этому коннектору объект по индексу
const UCItem& UAConnector::GetCItem(int c_index) const
{
 return CItemList[c_index];
}

// Возвращает информацию об индексах связей с этим item или -1, -1
// если такая связь отсутствует
UCLink UAConnector::GetCLink(UEPtr<UAItem> item) const
{
 UCLink indexes;

 if(!item)
  return indexes;

 UCItem citem=CItemList.Find(item);

 if(citem.Item == 0)
  return indexes;

 indexes.Input=CItemList.Find(citem);
 indexes.Output=citem.Index;

 return indexes;
}
// --------------------------

// ----------------------
// Коммуникационные методы
// ----------------------
// Устанавливает связь с элементом сети 'na'.
bool UAConnector::ConnectToItem(UEPtr<UAItem> na, int i_index, int &c_index)
{
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
 return AConnectToItem(na, i_index, c_index);
}

// Разрывает связь с элементом сети 'na'
void UAConnector::DisconnectFromItem(UEPtr<UAItem> na)
{
 if(!na)
  return;

 for(int i=0;i<CItemList.GetSize();i++)
 {
  if(CItemList[i].Item == na)
  {
   DisconnectFromIndex(i);
  }
 }
}

// Разрывает связь с элементом сети подключенным ко входу 'index'
void UAConnector::DisconnectFromIndex(int c_index)
{
 if(c_index<0 || c_index >=NumInputs)
  return;

 ADisconnectFromItem(CItemList[c_index].Item,CItemList[c_index].Index,c_index);
 CItemList[c_index].Item=0;
 CItemList[c_index].Index=-1;

 // Подчищаем лишние входы, если разрешено
/* if(AutoNumInputs)
 {
  int newsize=NumInputs;
  for(int i=NumInputs-1;i>=0;i--)
  {
   if(CItemList[i].Item != 0)
	break;
   --newsize;
  }
  SetNumInputs(newsize);
 }*/
}

// Выполняет действия после физически установленой связи
bool UAConnector::AConnectToItem(UEPtr<UAItem> na, int i_index, int c_index)
{
 return true;
}

// Выполняет действия после физически разорваной связи
void UAConnector::ADisconnectFromItem(UEPtr<UAItem> na, int i_index, int c_index)
{
}

// Разрывает все текущие связи
void UAConnector::DisconnectAllItems(void)
{
// Build();
 for(int i=0;i<CItemList.GetSize();i++)
  if(CItemList[i].Item)
   CItemList[i].Item->Disconnect(this);
}

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
void UAConnector::DisconnectByObject(UEPtr<UAContainer> brklevel)
{
// Build();
 for(int i=0;i<CItemList.GetSize();i++)
  if(CItemList[i].Item)
   if(!CItemList[i].Item->CheckOwner(brklevel))
    CItemList[i].Item->Disconnect(this);
}
                      /*
// Возвращает интерфейс входа
UEInterface* UAConnector::GetInputInterface(int c_index)
{
 return 0;
}

// Возвращает интерфейс входа
NameT UAConnector::GetInputInterfaceTypeName(int c_index)
{
 UEInterface* ueinterface=GetInputInterface(c_index);
 if(ueinterface)
  return ueinterface->GetTypeName();

 return NameT("");
}                       */

// Проверяет, допустимо ли подключение заданного item к этому коннектору
bool UAConnector::CheckItem(UEPtr<UAItem> item, int item_index, int conn_index)
{
/* if(item->GetOutputType(item_index) == GetInputType(conn_index))
  return true;
 return false;*/
 return true;
}

// Возвращает список подключений
ULinksList& UAConnector::GetLinks(ULinksList &linkslist, UEPtr<UAContainer> netlevel) const
{
 ULink link;
 GetLongId(netlevel,link.Connector.Id);
 if(link.Connector.Id.GetSize()==0)
  return linkslist;

 for(int i=0;i<CItemList.GetSize();i++)
 {
  if(CItemList[i].Item)
  {
   CItemList[i].Item->GetLongId(netlevel,link.Item.Id);
   link.Connector.Index=i;
   link.Item.Index=CItemList[i].Index;
   if(link.Connector.Id.GetSize() != 0)
   {
    if(linkslist.Find(link) >= 0)
     continue;
    else
     linkslist.Add(link);
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
bool UAConnector::Default(void)
{
 SetNumInputs(1);
 SetAutoNumInputs(true);
 return UAContainer::Default();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UAConnector::Build(void)
{
 return UAContainer::Build();
}
// --------------------------

/* *************************************************************************** */


}
#endif

