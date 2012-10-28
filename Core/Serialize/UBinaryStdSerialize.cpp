/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UBINARY_STD_SERIALIZE_CPP
#define UBINARY_STD_SERIALIZE_CPP

#include "UBinaryStdSerialize.h"

namespace RDK {
namespace Serialize {

USerStorageBinary& operator << (USerStorageBinary& storage, bool data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, bool &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, char data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, char &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, unsigned char data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, unsigned char &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, short data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, short &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, unsigned short data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, unsigned short &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, int data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, int &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, unsigned int data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, unsigned int &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, long data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, long &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, unsigned long data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, unsigned long &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, long long data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, long long &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, unsigned long long data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, unsigned long long &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, float data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, float &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, double data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, double &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageBinary& operator << (USerStorageBinary& storage, long double data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, long double &data)
{
 return USimpleFromStorage(storage,data);
}

// Вектора
USerStorageBinary& operator << (USerStorageBinary& storage, const std::vector<bool> &data)
{
 unsigned int size=data.size();
 operator <<(storage,size);

 if(size <= 0)
  return storage;

 for(size_t i=0;i<size;i++)
  operator <<(storage,data[i]);

 return storage;
}

USerStorageBinary& operator >> (USerStorageBinary& storage, std::vector<bool> &data)
{
 unsigned int size=data.size();
 operator >>(storage,size);
 data.resize(size);

 if(size <= 0)
  return storage;

 for(size_t i=0;i<size;i++)
 {
  bool element;
  operator >>(storage,element);
  data[i]=element;
 }

 return storage;
}

// Строки
//template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const std::string &data)
{
 size_t size=data.size();
 operator <<(storage,size);

 if(size <= 0)
  return storage;

// const char* pdata=&data[0];
 for(size_t i=0;i<size;i++)
//  operator <<(storage,*(pdata+i));
  operator <<(storage,data[i]);

 return storage;
}

USerStorageBinary& operator >> (USerStorageBinary& storage, std::string &data)
{
 size_t size=0;
 operator >>(storage,size);
 data.resize(size);

 if(size <= 0)
  return storage;

 char* pdata=&data[0];

 for(size_t i=0;i<size;i++)
  operator >>(storage,*(pdata+i));

 return storage;
}

}
}
#endif

