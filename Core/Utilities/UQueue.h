/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, Sergey Vishnevskiy, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UQUEUE_H
#define UQUEUE_H

#include <vector>
#include <list>
#include <string>

namespace RDK {

using namespace std;

template<typename Ty>
class UQueue
{
private:
// Начальный максимальный размер очереди
enum { INIT_SIZE = 1024 };

protected: // Данные
// Данные очереди
vector<Ty> m_pData;

// Число данных в очерди
unsigned int Size;

// Максимально доступный размер очереди
unsigned int MaxSize;

// Индекс первого элемента
unsigned int Front;

// Индекс последнего элемента
unsigned int Back;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Создает пустую очередь
UQueue(void)
{
 Size = 0;
 MaxSize = 0;
};

// Создает очередь с резервированым максимальным размером
UQueue(size_t size)
{
 MaxSize = size;
 Size = 0;
 m_pData.resize(size);
};

// Создает очередь заполненную данными из вектора vec
UQueue(const std::vector<Ty>& vec)
{
 Size = MaxSize = vec.size();
 m_pData=vec;
 Front = 0;
 Back = Size-1;
};

~UQueue(void)
{
};
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает первый элемент
Ty&    front(void)
{
 return    m_pData[Front];
};

// Возвращает последний элемент
Ty&    back(void)
{
 return    m_pData[Back];
};

// Оператор доступа к элементам по индексу
// Индекс считается от начала очереди
Ty& operator [] (int i)
{
 if(Front+i<MaxSize)
  return m_pData[Front+i];
 else
  return m_pData[Front+i-MaxSize];
}

// Возвращает true если очередь пуста
bool empty(void)
{
 return (size() == 0);
};

// Возвращает число элементов данных в очереди
size_t size(void)
{
 return    Size;
};
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Добавляет элемент в конец
void push(const Ty& obj)
{
 if(empty())
 {
  // если не создано хранилище
  if(!MaxSize)
  {
   m_pData.resize(INIT_SIZE);
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
    // элементы хранятся не разрывно
    m_pData.resize(m_pData.size()+MaxSize);
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
   vector<Ty> temp;
   temp.resize(temp.size()+MaxSize);
   for(size_t i=Front,j=0;i<MaxSize;i++,j++)
    temp[j]=m_pData[i];
   for(size_t i=0,j=Front+1;i<Front;i++,j++)
    temp[j]=m_pData[i];

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
void pop(void)
{
 if(++Front == MaxSize)
  Front = 0;
 --Size;
};

// Очищает очередь
void clear(void)
{
 Size = 0;
};


// Резервирует место для size_ элементов
void reserve(size_t size_)
{
 if(size() < size_)
 {
  MaxSize = size_;
  m_pData.resize(MaxSize);
 }
};

// Заполняет очередь данными из вектора vec.
// 0-й элемент вектора становится началом очереди
void FromVec(const std::vector<Ty>& vec)
{
 Size = MaxSize = vec.size();
 m_pData=vec;
 Front = 0;
 Back = Size-1;
};
// --------------------------
};

}
#endif
