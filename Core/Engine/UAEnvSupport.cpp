/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAENV_SUPPORT_CPP
#define UAENV_SUPPORT_CPP

#include <string.h>
#include <stdlib.h>
#include "UAEnvSupport.h"

namespace RDK {

UId ForbiddenId=0;
ULongId ULongIdemp;

/* *********************************************************************** */
/* *********************************************************************** */
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UIdVector::UIdVector(void)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;
}

UIdVector::UIdVector(const UIdVector &copy)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;

 *this=copy;
}

UIdVector::UIdVector(UId id1, UId id2, ...)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;

 Add(id1);
 UId *p = &id2;
 while (*p != ForbiddenId)
 {
  Add(*p++);
 }
}

UIdVector::UIdVector(UId id1)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;

 Add(id1);
}

UIdVector::~UIdVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Возвращает размер массива
int UIdVector::GetSize(void) const
{
 return Size;
}

// Изменяет размер массива с сохранением прежних данных
void UIdVector::Resize(int newsize)
{
 if(RealSize<newsize || !Buffer)
 {
  UId *newbuffer=new UId[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Buffer[i];

  for(int i=Size;i<newsize;i++)
   newbuffer[i]=0;

  if(Buffer)
   delete []Buffer;
  Buffer=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Очищает массив
void UIdVector::Clear(void)
{
 if(Buffer)
 {
  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}

// Возвращает указатель на начало данных
UId* UIdVector::GetBuffer(void)
{
 return Buffer;
}

// Добавляет элемент в конец массива
// Возвращает индекс элемента
int UIdVector::Add(UId id)
{
 Resize(Size+1);
 Buffer[Size-1]=id;
 return Size-1;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UIdVector& UIdVector::operator = (const UIdVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Buffer[i]=copy.Buffer[i];

 return *this;
}

// Оператор доступа
UId& UIdVector::operator [] (int index)
{
 return Buffer[index];
}

const UId& UIdVector::operator [] (int index) const
{
 return Buffer[index];
}

// Операторы сравнения
bool UIdVector::operator == (const UIdVector &copy) const
{
 if(Size != copy.Size)
  return false;

 return !memcmp(Buffer,copy.Buffer,Size*sizeof(UId));
}

bool UIdVector::operator != (const UIdVector &copy) const
{
 return !(*this == copy);
}

bool UIdVector::operator < (const UIdVector &copy) const
{
 if(Size < copy.Size)
  return true;

 if(Size > copy.Size)
  return false;

 if(memcmp(Buffer,copy.Buffer,Size*sizeof(UId))<0)
  return true;

 return false;
}
// --------------------------

/* *********************************************************************** */
/* *********************************************************************** */


/* *********************************************************************** */
/* *********************************************************************** */
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULongIdVector::ULongIdVector(void)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;
}

ULongIdVector::ULongIdVector(const ULongIdVector &copy)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;

 *this=copy;
}

ULongIdVector::~ULongIdVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Возвращает размер массива
int ULongIdVector::GetSize(void) const
{
 return Size;
}

// Изменяет размер массива с сохранением прежних данных
void ULongIdVector::Resize(int newsize)
{
 if(RealSize<newsize || !Buffer)
 {
  ULongId *newbuffer=new ULongId[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Buffer[i];

  if(Buffer)
   delete []Buffer;
  Buffer=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Очищает массив
void ULongIdVector::Clear(void)
{
 if(Buffer)
 {
  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}

// Возвращает указатель на начало данных
ULongId* ULongIdVector::GetBuffer(void)
{
 return Buffer;
}

// Добавляет элемент в конец массива
// Возвращает индекс элемента
int ULongIdVector::Add(const ULongId &id)
{
 Resize(Size+1);
 Buffer[Size-1]=id;
 return Size-1;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
ULongIdVector& ULongIdVector::operator = (const ULongIdVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Buffer[i]=copy.Buffer[i];

 return *this;
}

// Оператор доступа
ULongId& ULongIdVector::operator [] (int index)
{
 return Buffer[index];
}

const ULongId& ULongIdVector::operator [] (int index) const
{
 return Buffer[index];
}
// --------------------------

/* *********************************************************************** */
/* *********************************************************************** */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULinkSide::ULinkSide(void)
{
 Index=0;
}

ULinkSide::ULinkSide(const ULongId &id, int index)
{
 Id=id;
 Index=index;
}

ULinkSide::ULinkSide(const ULinkSide &link)
{
 Id=link.Id;
 Index=link.Index;
}

ULinkSide::~ULinkSide(void)
{
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
bool operator < (const ULinkSide &linkside1, const ULinkSide &linkside2)
{
 return (linkside1.Id<linkside2.Id)
    || (linkside1.Id == linkside2.Id && linkside1.Index < linkside2.Index);
}

bool operator == (const ULinkSide &linkside1, const ULinkSide &linkside2)
{
 return (linkside1.Id == linkside2.Id) & (linkside1.Index == linkside2.Index);
}

bool operator != (const ULinkSide &linkside1, const ULinkSide &linkside2)
{
 return (!(linkside1 == linkside2));
}
// --------------------------


// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULink::ULink(void)
{
}

ULink::ULink(const ULinkSide &item, const ULinkSide &conn)
{
 Item=item;
// Connector=conn;
 Connector.push_back(conn);
}

ULink::ULink(const ULongId &item_id, int item_index,const ULongId &conn_id, int conn_index)
{
 Item.Id=item_id;
 Item.Index=item_index;
// Connector.Id=conn_id;
// Connector.Index=conn_index;
 Connector.resize(1);
 Connector[0].Id=conn_id;
 Connector[0].Index=conn_index;
}

ULink::ULink(const ULink &link)
{
 Item=link.Item;
 Connector=link.Connector;
}

ULink::~ULink(void)
{
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Ищет заданный элемент с приемником connector и возвращает индекс или отрицательное число, если не
// найдено
int ULink::FindConnector(const ULinkSide &connector)
{
  for(size_t j=0;j<Connector.size();j++)
   if(Connector[j] == connector)
	return j;

 return -1;
}
// --------------------------



// --------------------------
// Операторы
// --------------------------
bool operator < (const ULink &link1, const ULink &link2)
{
 return (link1.Item<link2.Item) & (link1.Connector<link2.Connector);
}

bool operator == (const ULink &link1, const ULink &link2)
{
 return (link1.Item == link2.Item) & (link1.Connector == link2.Connector);
}

bool operator != (const ULink &link1, const ULink &link2)
{
 return !(link1 == link2);
}
// --------------------------


// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULinksList::ULinksList(void)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Data=0;

 // Реальный размер массива
 RealSize=0;
}

ULinksList::ULinksList(const ULinksList &copy)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Data=0;

 // Реальный размер массива
 RealSize=0;

 *this=copy;
}

ULinksList::~ULinksList(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Очищает массив
void ULinksList::Clear(void)
{
 if(Data)
 {
  delete []Data;
  Data=0;
 }
 Size=RealSize=0;
}

// Изменяет размер массива с сохранением прежних данных
void ULinksList::Resize(int newsize)
{
 if(RealSize<newsize || !Data)
 {
  ULink *newbuffer=new ULink[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Data[i];

  if(Data)
   delete []Data;
  Data=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Добавляет элемент в конец массива
// Возвращает индекс элемента
int ULinksList::Add(const ULink &link)
{
 Resize(Size+1);
 Data[Size-1]=link;
 return Size-1;
}

// Объединяет элемент уже с существующим
int ULinksList::Merge(const ULink &link)
{
 int id=FindItem(link.Item);

 if(id < 0)
  return Add(link);

 for(size_t j=0;j<link.Connector.size();j++)
 {
  vector<ULinkSide>::iterator I=find(Data[id].Connector.begin(),Data[id].Connector.end(),link.Connector[j]);
  if(I == Data[id].Connector.end())
  {
   Data[id].Connector.push_back(link.Connector[j]);
  }
 }
 return id;
}

// Заменяет элемент (если элемент не существует - он создается)
int ULinksList::Set(const ULink &link)
{
 int id=FindItem(link.Item);

 if(id < 0)
  return Add(link);

 Data[id]=link;
 return id;
}


// Удаляет произвольный элемент по индексу
void ULinksList::Del(int index)
{
 if(index<0 || index>=Size)
  return;

 ULink *pdata=Data+index;
 ULink *pdata1=Data+index+1;
 for(int i=index;i<Size;i++,pdata++,pdata1++)
  *pdata=*pdata1;

 --Size;
}

// Ищет заданный элемент и возвращает индекс или отрицательное число, если не
// найдено
int ULinksList::Find(const ULink &link)
{
 ULink *pdata=Data;
 for(int i=0;i<Size;i++,pdata++)
  if(*pdata == link)
   return i;

 return -1;
}

// Ищет заданный элемент с источником item и возвращает индекс или отрицательное число, если не
// найдено
int ULinksList::FindItem(const ULinkSide &item)
{
 ULink *pdata=Data;
 for(int i=0;i<Size;i++,pdata++)
  if(pdata->Item == item)
   return i;

 return -1;
}

// Ищет заданный элемент с приемником connector и возвращает индекс или отрицательное число, если не
// найдено
int ULinksList::FindConnector(const ULinkSide &connector)
{
 ULink *pdata=Data;
 for(int i=0;i<Size;i++,pdata++)
 {
  for(size_t j=0;j<pdata->Connector.size();j++)
   if(pdata->Connector[j] == connector)
	return i;
 }

 return -1;
}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает размер массива
int ULinksList::GetSize(void) const
{
 return Size;
}

// Возвращает указатель на начало данных
ULink* ULinksList::GetData(void)
{
 return Data;
}
// --------------------------


// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
ULinksList& ULinksList::operator = (const ULinksList &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Data[i]=copy.Data[i];

 return *this;
}

// Оператор доступа
ULink& ULinksList::operator [] (int index)
{
 return Data[index];
}

const ULink& ULinksList::operator [] (int index) const
{
 return Data[index];
}
// --------------------------
/* *********************************************************************** */
/* *********************************************************************** */

}

#endif


