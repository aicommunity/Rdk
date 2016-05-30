/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UXML_STD_SERIALIZE_H
#define UXML_STD_SERIALIZE_H

#include <vector>
#include <map>
//#include <strstream>
#include <sstream>
#include <typeinfo>
#include "USerStorageXML.h"
#include "../Utilities/UTree.h"
#include "../Utilities/USupport.h"

namespace RDK {

// Простые типы
template<typename T>
USerStorageXML& USimpleToStorage (USerStorageXML& storage, const T data)
{
 storage.SetNodeAttribute("Type",typeid(T).name());
 std::stringstream stream;

 stream<<std::setprecision(40)<<data;

 std::string str;
 str=stream.str();

 storage.SetNodeText(str);

 return storage;
}

template<typename T>
USerStorageXML& USimpleFromStorage (USerStorageXML& storage, T &data)
{
// std::string type=typeid(T).name();
// std::string rtype=storage.GetNodeAttribute("Type");
// if(storage.GetNodeAttribute("Type") != typeid(T).name())
//  return storage;

// std::string rvalue=storage.GetNodeText();

 std::stringstream stream(storage.GetNodeText().c_str());

 stream>>data;

 return storage;
}

// Простые вещественные типы
template<typename T>
USerStorageXML& USimpleToStorageF(USerStorageXML& storage, const T data, int prec)
{
 storage.SetNodeAttribute("Type",typeid(T).name());
 std::stringstream stream;

 stream<<std::setprecision(prec)<<data;

 std::string str;
 str=stream.str();

 storage.SetNodeText(str);

 return storage;
}

template<typename T>
USerStorageXML& USimpleFromStorageF(USerStorageXML& storage, T &data)
{
 std::stringstream stream(storage.GetNodeText().c_str());

 stream>>data;

 return storage;
}

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, bool data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, bool &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, char data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, char &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, unsigned char data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, unsigned char &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, short data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, short &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, unsigned short data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, unsigned short &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, int data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, int &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, unsigned int data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, unsigned int &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, long data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, long &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, unsigned long data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, unsigned long &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, long long data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, long long &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, unsigned long long data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, unsigned long long &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, float data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, float &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, double data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, double &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, long double data);

RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, long double &data);

// Указатели
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const T *data)
{
 storage.SetNodeAttribute("Type","pointer");
 std::stringstream stream;
 stream<<data;

 std::string str;
 str=stream.str();

 storage.SetNodeText(str);

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, T* &data)
{
// std::string rvalue=storage.GetNodeText();

// std::stringstream stream(storage.GetNodeText().c_str());

// stream>>data; // Заглушка!

 return storage;
}

// Пары
template<typename T1, typename T2>
USerStorageXML& operator << (USerStorageXML& storage, const std::pair<T1,T2> &data)
{
 storage.SetNodeAttribute("Type","std::pair");
 storage.AddNode("first");
 storage<<data.first;
 storage.SelectUp();
 storage.AddNode("second");
 storage<<data.second;
 storage.SelectUp();

 return storage;
}

template<typename T1, typename T2>
USerStorageXML& operator >> (USerStorageXML& storage, std::pair<T1,T2> &data)
{
// if(storage.GetNodeAttribute("Type") != "std::pair")
//  return storage;

 if(!storage.SelectNode("first"))
  return storage;
 storage>>data.first;
 storage.SelectUp();
 if(!storage.SelectNode("second"))
  return storage;
 storage>>data.second;
 storage.SelectUp();

 return storage;
}

// Map-ы
template<typename T1, typename T2>
USerStorageXML& operator << (USerStorageXML& storage, const std::map<T1,T2> &data)
{
 storage.SetNodeAttribute("Type","std::map");
 storage.SetNodeAttribute("Size",sntoa(data.size()));

 if(data.empty())
  return storage;

 typename std::map<T1,T2>::const_iterator I,J;
 I=data.begin(); J=data.end();

 while(I != J)
 {
  storage.AddNode("elem");
  storage<<*I;
  storage.SelectUp();
  ++I;
 }

 return storage;
}

template<typename T1, typename T2>
USerStorageXML& operator >> (USerStorageXML& storage, std::map<T1,T2> &data)
{
// if(storage.GetNodeAttribute("Type") != "std::map")
//  return storage;

 size_t size=0;
// size=atoi(storage.GetNodeAttribute("Size"));
 size=storage.GetNumNodes();
 data.clear();

 if(size == 0)
  return storage;

 std::pair<T1,T2> p;
 for(size_t i=0;i<size;i++)
 {
  if(!storage.SelectNode("elem",i))
  {
   std::pair<T1,T2> dummy_p;
   data.insert(dummy_p);
  }
  else
  {
   operator >>(storage,p);
   data.insert(p);
   storage.SelectUp();
  }
 }

 return storage;
}

// Списки
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const std::list<T> &data)
{
 storage.SetNodeAttribute("Type","std::list");
 unsigned int size=data.size();
 storage.SetNodeAttribute("Size",sntoa(size));

 if(size == 0)
  return storage;

 typename std::list<T>::const_iterator I,J;
 I=data.begin(); J=data.end();

 while(I != J)
 {
  storage.AddNode("elem");
  storage<<*I;
  storage.SelectUp();
  ++I;
 }

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, std::list<T> &data)
{
// if(storage.GetNodeAttribute("Type") != "std::list")
//  return storage;

 unsigned int size=0;
// size=RDK::atoi(storage.GetNodeAttribute("Size"));
 size=storage.GetNumNodes();
 data.clear();

 if(size == 0)
  return storage;

 T p;
 for(size_t i=0;i<size;i++)
 {
  if(!storage.SelectNode("elem",i))
  {
   T dummy_p;
   data.push_back(dummy_p);
  }
  else
  {
   operator >>(storage,p);
   data.push_back(p);
   storage.SelectUp();
  }
 }

 return storage;
}

// Вектора
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const std::vector<bool> &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, std::vector<bool> &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const std::vector<double> &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, std::vector<double> &data);

template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const std::vector<T> &data)
{
 storage.SetNodeAttribute("Type","std::vector");
 unsigned int size=data.size();
 storage.SetNodeAttribute("Size",sntoa(size));

 if(size == 0)
  return storage;

 for(size_t i=0;i<size;i++)
 {
  storage.AddNode("elem");
  storage<<data[i];
  storage.SelectUp();
 }

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, std::vector<T> &data)
{
// if(storage.GetNodeAttribute("Type") != "std::vector")
//  return storage;

 unsigned int size=0;
// size=RDK::atoi(storage.GetNodeAttribute("Size"));
 size=storage.GetNumNodes();

 if(size == 0)
 {
  data.resize(0);
  return storage;
 }
 data.resize(size);

 T* pdata=&data[0];

 for(size_t i=0;i<size;i++)
 {
  if(!storage.SelectNode("elem",i))
   return storage;
  operator >>(storage,*(pdata+i));
  storage.SelectUp();
 }

 return storage;
}

// Строки
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const std::string &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, std::string &data);

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const std::wstring &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, std::wstring &data);

// Деревья
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const UTree<T> &data)
{
 storage.SetNodeAttribute("Type","UTree");
 storage.SetNodeAttribute("NodeName",data.GetName());
 storage.SetNodeAttribute("NodeId",sntoa(data.GetId()));
 size_t size=data.GetSubTreeSize();
 storage.SetNodeAttribute("Size",sntoa(size));

 storage.AddNode("Data");
 operator << (storage,~data);
 storage.SelectUp();

 for(size_t i=0;i<size;i++)
 {
  storage.AddNode("Node");
  operator <<(storage,data[i]);
  storage.SelectUp();
 }

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, UTree<T> &data)
{
 size_t size;
 typename UTree<T>::NameT name;
 typename UTree<T>::IdT id;

 if(storage.GetNodeAttribute("Type") != "UTree")
  return storage;

 size=atoi(storage.GetNodeAttribute("Size"));
 id=atoi(storage.GetNodeAttribute("NodeId"));
 name=storage.GetNodeAttribute("NodeName");

 data.SetId(id);
 data.SetName(name);

 if(storage.SelectNode("Data"))
  return storage;
 operator >>(storage,~data);
 storage.SelectUp();

 size=0;
 operator >>(storage,size);
 for(size_t i=0;i<size;i++)
 {
  if(!storage.SelectNode("Node",i))
   return storage;
  UTree<T> *tree=new UTree<T>;
  operator >>(storage,*(tree));
  data.AddSorted(tree);
  storage.SelectUp();
 }

 return storage;
}

}
#endif

