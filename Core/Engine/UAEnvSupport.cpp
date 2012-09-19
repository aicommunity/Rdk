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
 if(start != string::npos)
  throw EDecodeFail(str,0);

 start=stop=0;
 while(start != string::npos)
 {
  start=str.find_first_of("0123456789",stop);
  if(start == string::npos)
   break;

  stop=str.find_first_of(".",start);
  try
  {
   if(stop == string::npos)
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
/*UIdVector::EDecodeFail::EDecodeFail(void)
{

}
  */
UIdVector::EDecodeFail::EDecodeFail(const std::string &stringid, int position)
 : StringId(stringid), Position(position)
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







}

#endif


