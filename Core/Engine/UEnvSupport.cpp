/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UENV_SUPPORT_CPP
#define UENV_SUPPORT_CPP

#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include "UEnvSupport.h"
#include "../Utilities/USupport.h"

namespace RDK {

const UId ForbiddenId=0;
const NameT ForbiddenName="";

//ULongId ULongIdemp;
/*
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

int UIdVector::size(void) const
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
// Методы ввода вывода идентификаторов
// --------------------------
UIdVector& UIdVector::DecodeFromString(const std::string &str)
{
 Resize(0);

 if(!str.size())
  return *this;

 std::size_t start=str.find_first_not_of("0123456789. ");
 std::size_t stop;
 if(start != std::string::npos)
  throw EDecodeFail(str,0);

 start=stop=0;
 while(start != std::string::npos)
 {
  start=str.find_first_of("0123456789",stop);
  if(start == std::string::npos)
   break;

  stop=str.find_first_of(".",start);
  try
  {
   if(stop == std::string::npos)
	Add(atoi(str.substr(start)));
   else
	Add(atoi(str.substr(start,stop-start)));
  }
  catch (EStrToNumber *strtonumber)
  {
   delete strtonumber;
   throw EDecodeFail(str,stop);
  }
 }

 return *this;
}

std::string& UIdVector::EncodeToString(std::string &str) const
{
 str.clear();
 for(int i=0;i<Size;i++)
 {
  str+=sntoa(Buffer[i]);
  if(i < Size-1)
   str+=".";
 }

 return str;
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

UIdVector& UIdVector::operator = (const UId &copy)
{
 Clear();
 Add(copy);
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

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UIdVector::EDecodeFail::EDecodeFail(const std::string &stringid, int position)
 : StringId(stringid), Position(position)
{

}

UIdVector::EDecodeFail::~EDecodeFail(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UIdVector::EDecodeFail::CreateLogMessage(void) const
{
 std::string result=UException::CreateLogMessage();
 result+=" StringId=";
 result+=StringId;
 result+=" Position=";
 result+=sntoa(Position);
 return result;
}
// --------------------------

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
*/
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULinkSide::ULinkSide(void)
 : Index(0)
{
}

ULinkSide::ULinkSide(const std::string &comp_name, const std::string &property_name)
 :ComponentName(comp_name), Index(-1), PropertyName(property_name)
{
}

ULinkSide::ULinkSide(const std::string &comp_name, const std::string &property_name, int index)
 :ComponentName(comp_name), Index(index), PropertyName(property_name)
{
}

ULinkSide::ULinkSide(const ULinkSide &link)
{
 ComponentName=link.ComponentName;
 Index=link.Index;
 PropertyName=link.PropertyName;
}

ULinkSide::~ULinkSide(void)
{
}
// --------------------------

bool ULinkSide::operator < (const ULinkSide &linkside2) const
{
 return (ComponentName<linkside2.ComponentName)
	|| (ComponentName == linkside2.ComponentName && PropertyName < linkside2.PropertyName) ||
	   (ComponentName == linkside2.ComponentName && PropertyName == linkside2.PropertyName && Index<linkside2.Index);
}

bool ULinkSide::operator == (const ULinkSide &linkside2) const
{
 return (ComponentName == linkside2.ComponentName) & (PropertyName == linkside2.PropertyName) & (Index == linkside2.Index);
}

bool ULinkSide::operator != (const ULinkSide &linkside2) const
{
 return (!(*this == linkside2));
}
// --------------------------
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULink::ULink(void)
{
}

ULink::ULink(const ULinkSide &item, const ULinkSide &conn)
{
 Item=item;
 Connector.push_back(conn);
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


/******************************************************************************/
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULinksList::ULinksList(void)
{
 Size=0;
}

ULinksList::ULinksList(const ULinksList &copy)
{
 Size=0;

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
 Data.clear();
 Size=0;
}

// Изменяет размер массива с сохранением прежних данных
void ULinksList::Resize(int newsize)
{
 Data.resize(newsize);
 Size=Data.size();
}

// Добавляет элемент в конец массива
// Возвращает индекс элемента
int ULinksList::Add(const ULink &link)
{
 Data.push_back(link);
 ++Size;
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
  std::vector<ULinkSide >::iterator I=find(Data[id].Connector.begin(),Data[id].Connector.end(),link.Connector[j]);
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

 Data.erase(Data.begin()+index);
 --Size;
}

// Ищет заданный элемент и возвращает индекс или отрицательное число, если не
// найдено
int ULinksList::Find(const ULink &link)
{
 if(!Size)
  return -1;
 ULink *pdata=&Data[0];
 for(int i=0;i<Size;i++,pdata++)
  if(*pdata == link)
   return i;

 return -1;
}

// Ищет заданный элемент с источником item и возвращает индекс или отрицательное число, если не
// найдено
int ULinksList::FindItem(const ULinkSide &item)
{
 if(!Size)
  return -1;

 ULink *pdata=&Data[0];
 for(int i=0;i<Size;i++,pdata++)
  if(pdata->Item == item)
   return i;

 return -1;
}

// Ищет заданный элемент с приемником connector и возвращает индекс или отрицательное число, если не
// найдено
int ULinksList::FindConnector(const ULinkSide &connector)
{
 if(!Size)
  return -1;

 ULink *pdata=&Data[0];
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

int ULinksList::size(void) const
{
 return Size;
}


// Возвращает указатель на начало данных
ULink* ULinksList::GetData(void)
{
 if(!Size)
  return 0;

 return &Data[0];
}
// --------------------------


// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
ULinksList& ULinksList::operator = (const ULinksList &copy)
{
 Data=copy.Data;
 Size=copy.Size;

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


