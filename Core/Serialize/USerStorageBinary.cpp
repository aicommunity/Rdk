/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef USER_STORAGE_BINARY_CPP
#define USER_STORAGE_BINARY_CPP

#include <string.h>
#include "USerStorageBinary.h"

namespace RDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Создает пустую очередь
USerStorageBinary::USerStorageBinary(void)
{
 Size = 0;
 MaxSize = 0;
 m_pData=0;
 Front=0;
 Back=0;
};

// Создает очередь с резервированым максимальным размером
USerStorageBinary::USerStorageBinary(int size)
{
 MaxSize = size;
 Size = 0;
 m_pData=0;
 InternalResize(size);
};

// Создает очередь заполненную данными из вектора vec
USerStorageBinary::USerStorageBinary(const unsigned char* vec, int nsize)
{
 Size = MaxSize = 0;
 m_pData=0;
 InternalResize(nsize);
 memcpy(m_pData,vec,nsize*sizeof(unsigned char));
 Front = 0;
 Back = Size-1;
};

USerStorageBinary::~USerStorageBinary(void)
{
 if(m_pData)
 {
  delete []m_pData;
  m_pData=0;
 }
};
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает первый элемент
unsigned char&    USerStorageBinary::front(void)
{
 return    m_pData[Front];
};

// Возвращает последний элемент
unsigned char&    USerStorageBinary::back(void)
{
 return    m_pData[Back];
};

// Оператор доступа к элементам по индексу
// Индекс считается от начала очереди
unsigned char& USerStorageBinary::operator [] (int i)
{
 if(Front+i<MaxSize)
  return m_pData[Front+i];
 else
  return m_pData[Front+i-MaxSize];
}

// Возвращает true если очередь пуста
bool USerStorageBinary::empty(void)
{
 return (size() == 0);
};

// Возвращает число элементов данных в очереди
int USerStorageBinary::size(void)
{
 return    Size;
};
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Добавляет элемент в конец
void USerStorageBinary::push(const unsigned char& obj)
{
 if(empty())
 {
  // если не создано хранилище
  if(!MaxSize)
  {
   InternalResize(INIT_SIZE);
   MaxSize = INIT_SIZE;
  }
  Front = Back = 0;
 }
 else
 {
  // если дощли до конца блока памяти
  if(++Back == MaxSize)
  {
   // если всю память израсходовли
   if(Size == MaxSize)
   {
    // выделяем новый блок, причём все
    // элементы хранятся неразрывно
    InternalResize(Size+MaxSize);
    Front = 0;
    Back = MaxSize;
    MaxSize *= 2;
   }
   else
    // память ещё есть в начале
    Back = 0;
  }
  else
  if(Back == Front)
  {
   // если здесь то вся память потрачена
   // и разбита на две части
   unsigned char* temp=new unsigned char[MaxSize];
   for(int i=Front,j=0;i<MaxSize;i++,j++)
    temp[j]=m_pData[i];
   for(int i=0,j=Front+1;i<Front;i++,j++)
    temp[j]=m_pData[i];

   delete []m_pData;
   m_pData=temp;

   Front = 0;
   Back = MaxSize;
   MaxSize *= 2;
  }
 }

 ++Size;
 m_pData[Back] = obj;
};

// Извлекает элемент из начала
void USerStorageBinary::pop(void)
{
 if(++Front == MaxSize)
  Front = 0;
 --Size;
};

// Очищает очередь
void USerStorageBinary::clear(void)
{
 Size = 0;
};


// Резервирует место для size_ элементов
void USerStorageBinary::reserve(int size_)
{
 if(size() < size_)
 {
  MaxSize = size_;
  InternalResize(MaxSize);
 }
};

// Заполняет очередь данными из вектора vec.
// 0-й элемент вектора становится началом очереди
void USerStorageBinary::FromVec(const unsigned char* vec, int nsize)
{
 InternalResize(nsize);
 memcpy(m_pData,vec,nsize*sizeof(unsigned char));
 Front = 0;
 Back = Size-1;
};
// --------------------------


// --------------------------
// Скрытые методы управления данными
// --------------------------
// Метод масштабирования массива
void USerStorageBinary::InternalResize(int size)
{
 unsigned char *temp=new unsigned char[size];

 int minsize=(size<MaxSize)?size:MaxSize;
 if(MaxSize>0)
  memcpy(temp,m_pData,sizeof(unsigned char)*minsize);
 delete []m_pData;
 m_pData=temp;
 MaxSize=size;
}
// --------------------------

}
#endif

