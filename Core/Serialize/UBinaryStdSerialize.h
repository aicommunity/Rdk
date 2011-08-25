/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UBINARY_STD_SERIALIZE_H
#define UBINARY_STD_SERIALIZE_H

#include <vector>
#include <map>
//#include "../Utilities/UQueue.h"
#include "USerStorageBinary.h"
#include "../Utilities/UTree.h"

namespace RDK {

//typedef UQueue<unsigned char> USerStorage;

namespace Serialize {

// Простые типы
template<typename T>
USerStorageBinary& USimpleToStorage (USerStorageBinary& storage, const T data)
{
 for(size_t i=0;i<sizeof(data);i++)
  storage.push(reinterpret_cast<const unsigned char*>(&data)[i]);

 return storage;
}

template<typename T>
USerStorageBinary& USimpleFromStorage (USerStorageBinary& storage, T &data)
{
 for(size_t i=0;i<sizeof(data);i++)
 {
  if(storage.empty())
   return storage;

  reinterpret_cast<unsigned char*>(&data)[i]=storage.front();
  storage.pop();
 }
 return storage;
}

USerStorageBinary& operator << (USerStorageBinary& storage, bool data);

USerStorageBinary& operator >> (USerStorageBinary& storage, bool &data);

USerStorageBinary& operator << (USerStorageBinary& storage, char data);

USerStorageBinary& operator >> (USerStorageBinary& storage, char &data);

USerStorageBinary& operator << (USerStorageBinary& storage, unsigned char data);

USerStorageBinary& operator >> (USerStorageBinary& storage, unsigned char &data);

USerStorageBinary& operator << (USerStorageBinary& storage, short data);

USerStorageBinary& operator >> (USerStorageBinary& storage, short &data);

USerStorageBinary& operator << (USerStorageBinary& storage, unsigned short data);

USerStorageBinary& operator >> (USerStorageBinary& storage, unsigned short &data);

USerStorageBinary& operator << (USerStorageBinary& storage, int data);

USerStorageBinary& operator >> (USerStorageBinary& storage, int &data);

USerStorageBinary& operator << (USerStorageBinary& storage, unsigned int data);

USerStorageBinary& operator >> (USerStorageBinary& storage, unsigned int &data);

USerStorageBinary& operator << (USerStorageBinary& storage, long data);

USerStorageBinary& operator >> (USerStorageBinary& storage, long &data);

USerStorageBinary& operator << (USerStorageBinary& storage, unsigned long data);

USerStorageBinary& operator >> (USerStorageBinary& storage, unsigned long &data);

USerStorageBinary& operator << (USerStorageBinary& storage, long long data);

USerStorageBinary& operator >> (USerStorageBinary& storage, long long &data);

USerStorageBinary& operator << (USerStorageBinary& storage, unsigned long long data);

USerStorageBinary& operator >> (USerStorageBinary& storage, unsigned long long &data);

USerStorageBinary& operator << (USerStorageBinary& storage, float data);

USerStorageBinary& operator >> (USerStorageBinary& storage, float &data);

USerStorageBinary& operator << (USerStorageBinary& storage, double data);

USerStorageBinary& operator >> (USerStorageBinary& storage, double &data);

USerStorageBinary& operator << (USerStorageBinary& storage, long double data);

USerStorageBinary& operator >> (USerStorageBinary& storage, long double &data);

// Пары
template<typename T1, typename T2>
USerStorageBinary& operator << (USerStorageBinary& storage, const std::pair<T1,T2> &data)
{
 operator <<(storage,data.first);
 operator <<(storage,data.second);
 return storage;
}

template<typename T1, typename T2>
USerStorageBinary& operator >> (USerStorageBinary& storage, std::pair<T1,T2> &data)
{
 operator >>(storage,data.first);
 operator >>(storage,data.second);
 return storage;
}

// Map-ы
template<typename T1, typename T2>
USerStorageBinary& operator << (USerStorageBinary& storage, const std::map<T1,T2> &data)
{
 unsigned int size=data.size();
 operator <<(storage,size);

 if(size <= 0)
  return storage;

 std::map<T1,T2>::const_iterator I,J;
 I=data.begin(); J=data.end();

 while(I != J)
 {
  operator <<(storage,*I);
  ++I;
 }

 return storage;
}

template<typename T1, typename T2>
USerStorageBinary& operator >> (USerStorageBinary& storage, std::map<T1,T2> &data)
{
 unsigned int size=0;
 operator >>(storage,size);
 data.clear();

 if(size <= 0)
  return storage;

 std::pair<T1,T2> p;
 for(size_t i=0;i<size;i++)
 {
  operator >>(storage,p);
  data.insert(p);
 }

 return storage;
}

// Списки
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const std::list<T> &data)
{
 unsigned int size=data.size();
 operator <<(storage,size);

 if(size <= 0)
  return storage;

 std::list<T>::const_iterator I,J;
 I=data.begin(); J=data.end();

 while(I != J)
 {
  operator <<(storage,*I);
  ++I;
 }

 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, std::list<T> &data)
{
 unsigned int size=0;
 operator >>(storage,size);
 data.clear();

 if(size <= 0)
  return storage;

 T p;
 for(size_t i=0;i<size;i++)
 {
  operator >>(storage,p);
  data.push_back(p);
 }

 return storage;
}

// Вектора
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const std::vector<T> &data)
{
 unsigned int size=data.size();
 operator <<(storage,size);

 if(size <= 0)
  return storage;

 for(size_t i=0;i<size;i++)
  operator <<(storage,data[i]);

 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, std::vector<T> &data)
{
 unsigned int size=data.size();
 operator >>(storage,size);
 data.resize(size);

 if(size <= 0)
  return storage;

 T* pdata=&data[0];

 for(size_t i=0;i<size;i++)
  operator >>(storage,*(pdata+i));

 return storage;
}

// Строки
USerStorageBinary& operator << (USerStorageBinary& storage, const std::string &data);

USerStorageBinary& operator >> (USerStorageBinary& storage, std::string &data);


// Деревья
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const UTree<T> &data)
{
 size_t size;

 operator <<(storage,data.GetName());
 operator <<(storage,data.GetId());
 operator <<(storage,~data);

 size=data.GetSubTreeSize();
 operator <<(storage,size);
 for(size_t i=0;i<size;i++)
  operator <<(storage,data[i]);

 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, UTree<T> &data)
{
 size_t size;
 NameT name;
 UId id;

 operator >>(storage,name);
 operator >>(storage,id);
 data.SetId(id);
 data.SetName(name);
 operator >>(storage,~data);

 size=0;
 operator >>(storage,size);
 for(size_t i=0;i<size;i++)
 {
  UTree<T> *tree=new UTree<T>;
  operator >>(storage,*(tree));
  data.AddSorted(tree);
 }

 return storage;
}


}
}
#endif

