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
#include "UAItem.h"

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
  memcpy(newbuffer,Data,sizeof(UAConnectorVector)*Size);

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

 memcpy(Data+index,Data+index+1,(Size-1)*sizeof(UAConnectorVector));
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
UAItem::UAItem(void)
{
 NumOutputs=0;
 AddLookupProperty("NumOutputs",new UVProperty<int,UAItem>(this,&UAItem::SetNumOutputs,&UAItem::GetNumOutputs));
}

UAItem::~UAItem(void)
{
 DisconnectAll();
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Возвращает число подключенных элементов item
int UAItem::GetNumOutputs(void) const
{
 return NumOutputs;//AssociatedConnectors.GetSize();
}

// Устанавливает число подключенных элементов item
bool UAItem::SetNumOutputs(int value)
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
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
void UAItem::Free(void)
{
// DisconnectAll();
 UAConnector::Free();
}
// --------------------------

// ----------------------
// Защищенные коммуникационные методы
// ----------------------
// Устанавливает связь с коннектором 'c'.
bool UAItem::Connect(UAConnector *c, int i_index, int c_index)
{
 if(c == 0)
  return false;

 if(!Build())
  return false;

 if(!c->ConnectToItem(this,i_index,c_index))
  return false;

 AssociatedConnectors[i_index].Add(c);
// PAssociatedConnectors[i_index]=&AssociatedConnectors[i_index][0];
// NumAConnectors[i_index]=AssociatedConnectors[i_index].GetSize();

 return true;
}

// Разрывает связь выхода этого объекта с коннектором 'c'.
void UAItem::Disconnect(UAConnector *c)
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
// ----------------------

// ----------------------
// Коммуникационные методы
// ----------------------
// Возвращает текущее число соединений.
int UAItem::GetNumAConnectors(int index) const
{
 return AssociatedConnectors[index].GetSize();//NumAConnectors[index];
}

// Разрывает связь выхода этого объекта с коннектором по Id 'id'.
bool UAItem::Disconnect(const UId &id)
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
void UAItem::DisconnectAll(void)
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
void UAItem::DisconnectBy(const UAContainer *brklevel)
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
void UAItem::BuildLinks(void)
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

const UAConnector* UAItem::GetAConnector(const UId &id, int index) const
{
 for(int i=0;i<AssociatedConnectors[index].GetSize();i++)
  if(AssociatedConnectors[index][i]->Id == id)
   return AssociatedConnectors[index][i];
 return 0;
}

// Возвращает  коннектор из списка подключений.
const UAConnector* UAItem::GetAConnectorByIndex(int output, int index) const
{
 return AssociatedConnectors[output][index];
}

// Возвращает список подключений
ULinksList& UAItem::GetLinks(ULinksList &linkslist, const UAContainer *netlevel) const
{
 ULink link;

 GetLongId(netlevel,link.Item.Id);
 if(link.Item.Id.GetSize() == 0)
  return linkslist;


 for(int j=0;j<AssociatedConnectors.GetSize();j++)
  for(int i=0;i<AssociatedConnectors[j].GetSize();i++)
  {
   AssociatedConnectors[j][i]->GetLongId(netlevel,link.Connector.Id);
   if(link.Connector.Id.GetSize() != 0)
   {
    UCLink indexes=AssociatedConnectors[j][i]->GetCLink(this);
    link.Item.Index=indexes.Output;
    link.Connector.Index=indexes.Input;

    if(linkslist.Find(link) >= 0)
     continue;
    else
     linkslist.Add(link);
   }
  }

 return linkslist;
}

// Возвращает список подключений этого компонента и всех дочерних компонент
// к заданному компоненту comp и всем его дочерним компонентам
ULinksList& UAItem::GetFullItemLinks(ULinksList &linkslist, const UAItem *comp,
                                     const UAContainer *netlevel) const
{
 ULink link;

 if(!comp)
  return linkslist;

 GetLongId(netlevel,link.Item.Id);
 if(link.Item.Id.GetSize() == 0)
  return linkslist;


 for(int j=0;j<AssociatedConnectors.GetSize();j++)
  for(int i=0;i<AssociatedConnectors[j].GetSize();i++)
  {
   if(!AssociatedConnectors[j][i]->CheckOwner(comp) && AssociatedConnectors[j][i] != comp)
    continue;
   AssociatedConnectors[j][i]->GetLongId(netlevel,link.Connector.Id);
   if(link.Connector.Id.GetSize() != 0)
   {
    UCLink indexes=AssociatedConnectors[j][i]->GetCLink(this);
    link.Item.Index=indexes.Output;
    link.Connector.Index=indexes.Input;

    if(linkslist.Find(link) >= 0)
     continue;
    else
     linkslist.Add(link);
   }
  }

 for(int i=0;i<NumComponents;i++)
 {
  UAItem* item=dynamic_cast<UAItem*>(PComponents[i]);
  if(item)
   item->GetFullItemLinks(linkslist, comp, netlevel);
 }

 return linkslist;
}
// ----------------------


// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UAItem::Default(void)
{
 SetNumOutputs(1);
 return UAConnector::Default();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UAItem::Build(void)
{
 if(IsReady())
  return true;

 BuildLinks();

 return UAConnector::Build();
}

// Активирует все коннекторы, подключенные к выходу этого объекта.
/*void UAItem::ActivateAll(void)
{
 Build();
 for(size_t j=0;j<AssociatedConnectors.size();j++)
  for(size_t i=0;i<AssociatedConnectors[j].size();i++)
   AssociatedConnectors[j][i]->Activate(this);
} */
/* *************************************************************************** */


}

#endif



