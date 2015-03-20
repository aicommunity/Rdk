/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2008.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UITEMDATA_CPP
#define UITEMDATA_CPP

#include <string.h>
#include "UItemData.h"

namespace RDK {

// Размер единичного элемента данных по умолчанию
int UItemData::DefaultDataSize=sizeof(void*);

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UItemData::UItemData(void)
{
 Void=0;

 // Размер данных в байтах
 Size=ByteSize=0;
 DataSize=DefaultDataSize;
}

UItemData::UItemData(const UItemData &copy)
{
 Void=0;

 // Размер данных в байтах
 Size=ByteSize=0;
 DataSize=DefaultDataSize;

 DataSize=copy.DataSize;
 Resize(copy.Size);
 memcpy(Void,copy.Void,copy.ByteSize);
}

UItemData::~UItemData(void)
{
 if(Char)
 {
  delete [] Char;
  Char=0;
 }
 Size=ByteSize=0;
}
// --------------------------

// --------------------------
// Методы доступа к параметрам
// --------------------------
// Размер вектора данных
int UItemData::GetSize(void) const
{
 return Size;
}

// Размер данных в байтах
int UItemData::GetByteSize(void) const
{
 return ByteSize;
}

// Размер единичного данного в байтах
int UItemData::GetDataSize(void) const
{
 return DataSize;
}
// --------------------------

// --------------------------
// Методы
// --------------------------
// Выделение памяти
void UItemData::Resize(int size)
{
 if(size == Size)
  return;

 if(!size && Char)
 {
  delete []Char;
  Void=0;
  Size=0;
  ByteSize=0;
  return;
 }

 void *tmp=new char[size*DataSize];
 memcpy(tmp,Void,((size<Size)?size:Size)*DataSize);
 if(size>Size)
  memset(static_cast<char*>(tmp)+Size*DataSize,0,DataSize*(size-Size));

 delete []Char;
 Size=size;
 ByteSize=Size*DataSize;
 Void=tmp;
}

// Выделение памяти и заполнение заданным байтом
void UItemData::Assign(int size, char ch)
{
 Resize(size);

 if(ByteSize)
  memset(Void,ch,ByteSize);
}


// Выделение памяти и заполнение заданной последовательностью
// длиной ByteSize
void UItemData::Assign(int size, const void *data)
{
 Resize(size);

 if(ByteSize)
 {
  char *p=Char;
  for(int i=0;i<Size;i++,p+=DataSize)
   memcpy(p,data,DataSize);
 }
}

// Выделение памяти и заполнение заданной последовательностью
// длиной size с элементами длиной datasize
// Массив записывается начиная с индекса shift
// Выделяется память разером datasize*size+shift*datasize*shift
void UItemData::Assign(int size, int datasize, const void *data, int shift)
{
 if(datasize < 1)
  return;

 if(DataSize != datasize || Size != size)
 {
  DataSize=datasize;
  Resize(0);
  Resize(size+shift);
 }

 if(Size)
  memcpy(Char+shift*DataSize,data,size*DataSize);
}

// Заполнение заданной последовательностью
// в предположении что память уже выделена
// Массив записывается начиная с индекса shift
void UItemData::Assign(int size, const void *data, int shift) const
{
 if(Size)
  memcpy(Char+shift*DataSize,data,size*DataSize);
}


// Устанавливает размер единичного данного в байтах
bool UItemData::SetDataSize(int size)
{
 if(size < 1)
  return false;

 if(DataSize == size)
  return true;

 DataSize=size;
 int current_size=Size;
 Resize(0);
 Resize(current_size);
 return true;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UItemData& UItemData::operator = (const UItemData &data)
{
 Resize(0);
 DataSize=data.DataSize;
 Resize(data.Size);
 memcpy(Void,data.Void,data.ByteSize);
 return *this;
}

// Оператор расширения массива
// Предполагает, что добавляемый массив имеет тот же формат
// Добавление происходит прямым копированием области памяти
UItemData& UItemData::operator += (const UItemData &data)
{
 if(data.ByteSize == 0)
  return *this;
 int shift=ByteSize;
 Resize(Size+data.ByteSize/DataSize);
 memcpy(Char+shift,data.Void,data.ByteSize);

 return *this;
}

/*
// Оператор доступа
inline char UItemData::operator [] (int i)
{
 return Char[i];
}      */
// --------------------------


USerStorageXML& operator << (USerStorageXML& storage, const UItemData &data)
{
 storage.SetNodeAttribute("Type","simplevector");
 unsigned int size=data.GetSize();
 storage.SetNodeAttribute("Size",sntoa(size));
 storage.SetNodeAttribute("DataSize",sntoa(data.GetDataSize()));
/*
 if(size <= 0)
  return storage;

 std::stringstream stream;

 for(unsigned i=0;i<size;i++)
 {
  stream<<int(data[i]);
  if(i<size-1)
   stream<<" ";
 }

 storage.SetNodeText(stream.str());*/
 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UItemData &data)
{
  unsigned int size=RDK::atoi(storage.GetNodeAttribute("Size"));
  data.Resize(size);
  unsigned int data_size=RDK::atoi(storage.GetNodeAttribute("DataSize"));
  data.SetDataSize(data_size);
/*
  if(size>0)
  {
   std::string rvalue=storage.GetNodeText();
   std::stringstream stream(storage.GetNodeText().c_str());

   for(unsigned i=0;i<size;i++)
   {
	int temp;
	stream>>temp;
	data[i]=temp;
   }
  }
              */
 return storage;
}
	  /*
USerStorageBinary& operator << (USerStorageBinary& storage, const UItemData &data)
{
 return storage;
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UItemData &data)
{
 return storage;
}
        */
}

#endif

