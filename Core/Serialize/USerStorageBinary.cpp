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
  // Используем геометрическое увеличение для резервирования
  int newMaxSize = (MaxSize > 0) ? (MaxSize + MaxSize / 2) : INIT_SIZE;
  while(newMaxSize < size_)
   newMaxSize = newMaxSize + newMaxSize / 2;
  
  InternalResize(newMaxSize);
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

// Оптимизированные методы для блоковой записи/чтения
// Записывает блок данных в очередь (для оптимизации производительности)
void USerStorageBinary::WriteBlock(const unsigned char* data, int blockSize)
{
 if(blockSize <= 0 || !data)
  return;

 // Резервируем место, если нужно
 int newSize = Size + blockSize;
 if(newSize > MaxSize)
 {
  // Вычисляем новый размер с запасом (геометрическое увеличение x1.5)
  int newMaxSize = (MaxSize > 0) ? (MaxSize + MaxSize / 2) : INIT_SIZE;
  while(newMaxSize < newSize)
   newMaxSize = newMaxSize + newMaxSize / 2;
  
  // Если данные разорваны, нужно их уплотнить
  if(Front != 0 && Size > 0)
  {
   unsigned char* temp = new unsigned char[newMaxSize];
   // Копируем данные непрерывно
   if(Front + Size <= MaxSize)
   {
    memcpy(temp, m_pData + Front, Size);
   }
   else
   {
    int firstPart = MaxSize - Front;
    memcpy(temp, m_pData + Front, firstPart);
    memcpy(temp + firstPart, m_pData, Size - firstPart);
   }
   delete []m_pData;
   m_pData = temp;
   Front = 0;
   Back = Size - 1;
   MaxSize = newMaxSize;
  }
  else
  {
   InternalResize(newMaxSize);
  }
 }

 // Записываем блок данных
 if(Size == 0)
 {
  // Очередь пуста
  if(!MaxSize)
  {
   InternalResize(INIT_SIZE);
   MaxSize = INIT_SIZE;
  }
  Front = 0;
  memcpy(m_pData, data, blockSize);
  Back = blockSize - 1;
  Size = blockSize;
 }
 else
 {
  // Определяем, где писать данные
  int writePos = (Back + 1) % MaxSize;
  
  // Проверяем, нужно ли "обернуться" через границу
  if(writePos + blockSize <= MaxSize)
  {
   // Данные помещаются без обертывания
   memcpy(m_pData + writePos, data, blockSize);
   Back = writePos + blockSize - 1;
  }
  else
  {
   // Данные нужно разбить на две части
   int firstPart = MaxSize - writePos;
   int secondPart = blockSize - firstPart;
   memcpy(m_pData + writePos, data, firstPart);
   memcpy(m_pData, data + firstPart, secondPart);
   Back = secondPart - 1;
  }
  Size += blockSize;
 }
}

// Читает блок данных из очереди (для оптимизации производительности)
// Возвращает количество прочитанных байтов
int USerStorageBinary::ReadBlock(unsigned char* data, int blockSize)
{
 if(blockSize <= 0 || !data || Size == 0)
  return 0;

 // Определяем, сколько данных можем прочитать
 int readSize = (blockSize < Size) ? blockSize : Size;
 
 // Читаем данные
 if(Front + readSize <= MaxSize)
 {
  // Данные непрерывны
  memcpy(data, m_pData + Front, readSize);
  Front = (Front + readSize) % MaxSize;
 }
 else
 {
  // Данные разорваны, нужно читать в два приема
  int firstPart = MaxSize - Front;
  memcpy(data, m_pData + Front, firstPart);
  int secondPart = readSize - firstPart;
  memcpy(data + firstPart, m_pData, secondPart);
  Front = secondPart;
 }
 
 Size -= readSize;
 return readSize;
}
// --------------------------


// --------------------------
// Скрытые методы управления данными
// --------------------------
// Метод масштабирования массива
// Оптимизирован: использует геометрическое увеличение (x1.5) и уплотняет данные
void USerStorageBinary::InternalResize(int size)
{
 if(size <= MaxSize && MaxSize > 0)
 {
  // Если новый размер меньше текущего, просто уплотняем данные
  if(Size > 0 && Front != 0)
  {
   // Данные разорваны, уплотняем их
   unsigned char *temp = new unsigned char[size];
   if(Front + Size <= MaxSize)
   {
    memcpy(temp, m_pData + Front, Size);
   }
   else
   {
    int firstPart = MaxSize - Front;
    memcpy(temp, m_pData + Front, firstPart);
    memcpy(temp + firstPart, m_pData, Size - firstPart);
   }
   delete []m_pData;
   m_pData = temp;
   Front = 0;
   Back = Size - 1;
   MaxSize = size;
  }
  return;
 }

 // Вычисляем оптимальный размер с геометрическим увеличением (x1.5)
 int optimalSize = size;
 if(MaxSize > 0)
 {
  optimalSize = MaxSize + MaxSize / 2; // Геометрическое увеличение x1.5
  if(optimalSize < size)
   optimalSize = size;
 }

 unsigned char *temp = new unsigned char[optimalSize];

 int minsize = (optimalSize < MaxSize) ? optimalSize : MaxSize;
 if(MaxSize > 0 && Size > 0)
 {
  // Копируем данные с учетом возможного разрыва
  if(Front + Size <= MaxSize)
  {
   memcpy(temp, m_pData + Front, Size);
  }
  else
  {
   int firstPart = MaxSize - Front;
   memcpy(temp, m_pData + Front, firstPart);
   memcpy(temp + firstPart, m_pData, Size - firstPart);
  }
  Front = 0;
  Back = Size - 1;
 }
 
 delete []m_pData;
 m_pData = temp;
 MaxSize = optimalSize;
}

// Освобождает неиспользуемую память (оптимизация)
void USerStorageBinary::shrink_to_fit(void)
{
 if(Size == 0)
 {
  // Очередь пуста, освобождаем всю память
  if(m_pData)
  {
   delete []m_pData;
   m_pData = 0;
   MaxSize = 0;
   Front = 0;
   Back = 0;
  }
  return;
 }

 if(Size < MaxSize)
 {
  // Есть неиспользуемая память, уплотняем
  unsigned char *temp = new unsigned char[Size];
  
  if(Front + Size <= MaxSize)
  {
   memcpy(temp, m_pData + Front, Size);
  }
  else
  {
   int firstPart = MaxSize - Front;
   memcpy(temp, m_pData + Front, firstPart);
   memcpy(temp + firstPart, m_pData, Size - firstPart);
  }
  
  delete []m_pData;
  m_pData = temp;
  MaxSize = Size;
  Front = 0;
  Back = Size - 1;
 }
}
// --------------------------

}
#endif

