/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UXML_STD_SERIALIZE_CPP
#define UXML_STD_SERIALIZE_CPP

#include "UXMLStdSerialize.h"

namespace RDK {
namespace Serialize {

USerStorageXML& operator << (USerStorageXML& storage, bool data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, bool &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, char data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, char &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, unsigned char data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, unsigned char &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, short data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, short &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, unsigned short data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, unsigned short &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, int data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, int &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, unsigned int data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, unsigned int &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, long data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, long &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, unsigned long data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, unsigned long &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, long long data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, long long &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, unsigned long long data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, unsigned long long &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, float data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, float &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, double data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, double &data)
{
 return USimpleFromStorage(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, long double data)
{
 return USimpleToStorage(storage,data);
}

USerStorageXML& operator >> (USerStorageXML& storage, long double &data)
{
 return USimpleFromStorage(storage,data);
}

// Вектора
USerStorageXML& operator << (USerStorageXML& storage, const std::vector<bool> &data)
{
 storage.SetNodeAttribute("Type","std::vector");
 unsigned int size=data.size();
 storage.SetNodeAttribute("Size",sntoa(size));

 if(size <= 0)
  return storage;

 for(size_t i=0;i<size;i++)
 {
  storage.AddNode("elem");
  storage<<data[i];
  storage.SelectUp();
 }

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, std::vector<bool> &data)
{
 unsigned int size=0;
 size=RDK::atoi(storage.GetNodeAttribute("Size"));

 if(size <= 0)
 {
  data.resize(0);
  return storage;
 }
 data.resize(size);

 for(size_t i=0;i<size;i++)
 {
  if(!storage.SelectNode("elem",i))
   return storage;
  bool element;
  operator >>(storage,element);
  data[i]=element;
  storage.SelectUp();
 }

 return storage;
}


// Строки
//template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const std::string &data)
{
 storage.SetNodeAttribute("Type","std::string");
// unsigned int size=data.size();
// storage.SetNodeAttribute("Size",sntoa(size));

// if(size <= 0)
//  return storage;

 storage.SetNodeText(data);
 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, std::string &data)
{
// if(storage.GetNodeAttribute("Type") != "std::string")
//  return storage;

// size_t size=0;
// size=atoi(storage.GetNodeAttribute("Size"));
// data.resize(size);

// if(size <= 0)
//  return storage;

 data=storage.GetNodeText();

 return storage;
}

}
}
#endif

