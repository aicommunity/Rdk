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
 memcpy(Data,copy.Data,Size*sizeof(UAConnectorVector));

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

// if(value == AssociatedConnectors.GetSize())
//  return true;

 if(value > AssociatedConnectors.GetSize())
 {
  for(int i=value;i<AssociatedConnectors.GetSize();i++)
   for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
    Disconnect(AssociatedConnectors[i][j]);
 }

 AssociatedConnectors.Resize(value);
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
// --------------------------

// --------------------------
// Методы доступа к описанию входов и выходов
// --------------------------
/// Ищет свойство-выход по заданному индексу
void UItem::FindOutputProperty(int index, UIProperty* &property)
{
 // Ищем указатель на выходные данные
 property=0;
 VariableMapIteratorT I=PropertiesLookupTable.begin(),
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
   /*
/// Ищет свойство-вход по заданному индексу
void UItem::FindInputProperty(int index, UIProperty* &property)
{
 // Ищем указатель на входные данные
 property=0;

 VariableMapIteratorT I=PropertiesLookupTable.begin(),
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
}     */
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
// Устанавливает связь с коннектором 'c'.
bool UItem::Connect(UEPtr<UConnector> c, int i_index, int c_index)
{
 if(c == 0)
  return false;

 if(i_index>=NumOutputs)
  SetNumOutputs(i_index+1);

 if(!Build())
  return false;

 if(!c->ConnectToItem(this,i_index,c_index))
  return false;

// if(AssociatedConnectors[i_index].Find(c) < 0)
 AssociatedConnectors[i_index].AddUnique(c);
// PAssociatedConnectors[i_index]=&AssociatedConnectors[i_index][0];
// NumAConnectors[i_index]=AssociatedConnectors[i_index].GetSize();

 return true;
}

// Разрывает связь выхода этого объекта с коннектором 'c'.
void UItem::Disconnect(UEPtr<UConnector> c)
{
 Build();

 if(c)
  c->DisconnectFromItem(this);

 for(int i=0;i<AssociatedConnectors.GetSize();i++)
 {
  int index=AssociatedConnectors[i].Find(c);
  while(index>=0)
  {
   AssociatedConnectors[i].Del(index);
//   NumAConnectors[i]=AssociatedConnectors[i].GetSize();
/*   if(NumAConnectors[i]>0)
	PAssociatedConnectors[i]=&AssociatedConnectors[i][0];
   else
	PAssociatedConnectors[i]=0;
  */
   index=AssociatedConnectors[i].Find(c,index);
  }
 }
}

// Разрывает связь выхода этого объекта с коннектором 'c' по индексу
void UItem::Disconnect(UEPtr<UConnector> c, int i_index, int c_index)
{
 Build();

 if(c)
  c->DisconnectFromItem(this);

// for(int i=0;i<AssociatedConnectors.GetSize();i++)
 int i=i_index;
 if(i<AssociatedConnectors.GetSize())
 {
  int index=AssociatedConnectors[i].Find(c);
  while(index>=0)
  {
   AssociatedConnectors[i].Del(index);
//   NumAConnectors[i]=AssociatedConnectors[i].GetSize();
/*   if(NumAConnectors[i]>0)
	PAssociatedConnectors[i]=&AssociatedConnectors[i][0];
   else
	PAssociatedConnectors[i]=0;
  */
   index=AssociatedConnectors[i].Find(c,index);
  }
 }

/*
 Build();

// for(int i=0;i<AssociatedConnectors.GetSize();i++)
// {
  if(i_index<AssociatedConnectors.GetSize() && c_index < AssociatedConnectors[i_index].GetSize())
  {
   if(c)
    c->DisconnectFromIndex(c_index);

   AssociatedConnectors[i_index].Del(c_index);
  }
// }

*/
}
// ----------------------

// ----------------------
// Коммуникационные методы
// ----------------------
// Возвращает текущее число соединений.
int UItem::GetNumAConnectors(int index) const
{
 return AssociatedConnectors[index].GetSize();//NumAConnectors[index];
}

// Разрывает связь выхода этого объекта с коннектором по Id 'id'.
bool UItem::Disconnect(const UId &id)
{
 Build();

 for(int j=0;j<AssociatedConnectors.GetSize();j++)
  for(int i=0;i<AssociatedConnectors[j].GetSize();i++)
  {
   if(AssociatedConnectors[j][i]->Id == id)
    {
     Disconnect(AssociatedConnectors[j][i]);
      break;
    }
  }
 return true;
}

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
void UItem::DisconnectAll(void)
{
 Build();
 for(int i=0;i<AssociatedConnectors.GetSize();i++)
 {
  while(AssociatedConnectors[i].GetSize() > 0)
  {
   Disconnect(AssociatedConnectors[i][AssociatedConnectors[i].GetSize()-1]);
  }
  AssociatedConnectors[i].Clear();
//  PAssociatedConnectors[i]=0;
//  NumAConnectors[i]=0;
 }
}

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
void UItem::DisconnectBy(UEPtr<UContainer> brklevel)
{
 Build();

 for(int i=0;i<AssociatedConnectors.GetSize();i++)
 {
  for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
  {
   for(int k=0;k<AssociatedConnectors[i].GetSize();k++)
    if(!AssociatedConnectors[i][k]->CheckOwner(brklevel))
    {
     Disconnect(AssociatedConnectors[i][k]);
     break;
    }
  }
 }
}

// Переустанавливает все связи этого item со всеми connectors которые получают
// данные от этого item
void UItem::BuildLinks(void)
{
// PAssociatedConnectors.resize(NumOutputs);
// NumAConnectors.resize(NumOutputs);

 for(int i=0;i<AssociatedConnectors.GetSize();i++)
 {
  // Заглушка!!! Тут почему то NumOutputs != AssociatedConnectors[i].Size
//  NumAConnectors[i]=AssociatedConnectors[i].GetSize();
//  if(NumAConnectors[i]>0)
//   PAssociatedConnectors[i]=&AssociatedConnectors[i][0];

  for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
  {
   if(!AssociatedConnectors[i][j])
    continue;
   UCLink indexes=AssociatedConnectors[i][j]->GetCLink(this);
   AssociatedConnectors[i][j]->ConnectToItem(this,indexes.Output,indexes.Input);
  }
 }
}

// Возвращает указатель на коннектор из списка подключений
// по имени 'name'.

UEPtr<UConnector> UItem::GetAConnector(const UId &id, int index) const
{
 for(int i=0;i<AssociatedConnectors[index].GetSize();i++)
  if(AssociatedConnectors[index][i]->Id == id)
   return AssociatedConnectors[index][i];
 return 0;
}

// Возвращает  коннектор из списка подключений.
UEPtr<UConnector> UItem::GetAConnectorByIndex(int output, int index) const
{
 return AssociatedConnectors[output][index];
}


// Проверяет, существует ли связь с заданным коннектором
bool UItem::CheckLink(const UEPtr<UConnector> &connector) const
{
 UCLink link=connector->GetCLink(this);
 if(link.Output>=0 && link.Input >=0)
  return true;
/*
 for(int i=0;i<AssociatedConnectors.GetSize();i++)
  for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
   if(AssociatedConnectors[i][j] == connector)
	return true;
  */
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

/*
 if(item_index>= AssociatedConnectors.GetSize())
  return false;

 if(item_index >=0)
 {
  for(int j=0;j<AssociatedConnectors[item_index].GetSize();j++)
   if(AssociatedConnectors[item_index][i] == connector)
	 return true;
 }
 else
 {
  return CheckLink(connector);
 }
  */
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
/*
 if(item_index>= AssociatedConnectors.GetSize())
  return false;

 if(item_index >=0)
 {
  for(int j=0;j<AssociatedConnectors[item_index].GetSize();j++)
   if(AssociatedConnectors[item_index][i] == connector)
   {
	if(conn_index <0)
	 return true;

	UCLink link=connector->GetCLink(this);
	if(conn_index == link.Input)
	 return true;
   }
 }
 else
 {
  for(int i=0;i<AssociatedConnectors.GetSize();i++)
   for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
	if(AssociatedConnectors[i][j] == connector)
	{
	 if(conn_index <0)
	  return true;

	 UCLink link=connector->GetCLink(this);
	 if(conn_index == link.Input)
	  return true;
	}

 }
	*/
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

 for(int j=0;j<AssociatedConnectors.GetSize();j++)
 {
  link.Connector.clear();
  for(int i=0;i<AssociatedConnectors[j].GetSize();i++)
  {
   if(exclude_internals)
   {
	if(AssociatedConnectors[j][i]->CheckOwner(internal_level))
	 continue;
   }
   AssociatedConnectors[j][i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	UCLink indexes=AssociatedConnectors[j][i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
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

 for(int j=0;j<AssociatedConnectors.GetSize();j++)
 {
  link.Connector.clear();
  for(int i=0;i<AssociatedConnectors[j].GetSize();i++)
  {
   if(AssociatedConnectors[j][i] != cont)
	continue;
   AssociatedConnectors[j][i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	UCLink indexes=AssociatedConnectors[j][i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
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


 for(int j=0;j<AssociatedConnectors.GetSize();j++)
  for(int i=0;i<AssociatedConnectors[j].GetSize();i++)
  {
   if(!AssociatedConnectors[j][i]->CheckOwner(static_pointer_cast<UContainer>(comp)) && AssociatedConnectors[j][i] != comp)
	continue;
   AssociatedConnectors[j][i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.GetSize() != 0)
   {
	UCLink indexes=AssociatedConnectors[j][i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
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

// Активирует все коннекторы, подключенные к выходу этого объекта.
/*void UItem::ActivateAll(void)
{
 Build();
 for(size_t j=0;j<AssociatedConnectors.size();j++)
  for(size_t i=0;i<AssociatedConnectors[j].size();i++)
   AssociatedConnectors[j][i]->Activate(this);
} */
/* *************************************************************************** */


}

#endif



