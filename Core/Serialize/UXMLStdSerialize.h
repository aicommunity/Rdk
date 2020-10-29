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

USerStorageXML& operator << (USerStorageXML& storage, const UnKnow &data);
USerStorageXML& operator >> (USerStorageXML& storage, UnKnow &data);

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

/* // Указать тип элемента контейнера (на случай отсутствия элементов внутри)
 T temp;
 USerStorageXML tempXML;
 tempXML.Destroy();
 tempXML.Create("temp");
 tempXML << temp;
 std::string type = tempXML.GetNodeAttribute("Type");
 storage.SetNodeAttribute("elemType",type);
*/
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

 int size=0;
// size=atoi(storage.GetNodeAttribute("Size"));
 size=storage.GetNumNodes();
 data.clear();

 if(size == 0)
  return storage;

 std::pair<T1,T2> p;
 for(int i=0;i<size;i++)
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
 size_t size=data.size();
 storage.SetNodeAttribute("Size",sntoa(size));

 // Указать тип элемента контейнера (на случай отсутствия элементов внутри)
 T temp;
 USerStorageXML tempXML;
 tempXML.Destroy();
 tempXML.Create("temp");
 tempXML << temp;
 std::string type = tempXML.GetNodeAttribute("Type");
 storage.SetNodeAttribute("elemType",type);

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

 int size=0;
// size=RDK::atoi(storage.GetNodeAttribute("Size"));
 size=storage.GetNumNodes();
 data.clear();

 if(size == 0)
  return storage;

 T p;
 for(int i=0;i<size;i++)
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

RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const std::vector<int> &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, std::vector<int> &data);

template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const std::vector<T> &data)
{
 storage.SetNodeAttribute("Type","std::vector");
 size_t size=data.size();
 storage.SetNodeAttribute("Size",sntoa(size));

 // Указать тип элемента контейнера (на случай отсутствия элементов внутри)

 T temp;
 USerStorageXML tempXML;
 tempXML.Destroy();
 tempXML.Create("temp");
 tempXML << temp;
 std::string type = tempXML.GetNodeAttribute("Type");

 if(type == "UBVSObject")
 {
     int k=0;
     k++;
 }
 // Если сериализация не сработало нормально
 if(type.empty())
 {
  type = typeid(T).name();
 }
 storage.SetNodeAttribute("elemType",type);

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

 int size=0;
// size=RDK::atoi(storage.GetNodeAttribute("Size"));
 size=storage.GetNumNodes();

 if(size == 0)
 {
  data.resize(0);
  return storage;
 }
 data.resize(size);

 T* pdata=&data[0];

 for(int i=0;i<size;i++)
 {
  if(!storage.SelectNode("elem",i))
   return storage;
  operator >>(storage,*(pdata+i));
  storage.SelectUp();
 }

 return storage;
}

// C-массивы
template<int Size>
USerStorageXML& operator << (USerStorageXML& storage, bool const (&data)[Size])
{
 storage.SetNodeAttribute("Type","C-simplearray");
 storage.SetNodeAttribute("Size",sntoa(Size));

 if(Size <= 0)
  return storage;

 std::stringstream stream;

 for(unsigned i=0;i<Size;i++)
 {
  stream<<int(data[i]);
  if(i<Size-1)
   stream<<" ";
 }

 storage.SetNodeText(stream.str());
 return storage;
}

template<int Size>
USerStorageXML& operator >> (USerStorageXML& storage, bool (&data)[Size])
{
 if(storage.GetNodeAttribute("Type") == "C-array")
 {
  int size=storage.GetNumNodes();

  if(size>Size)
   size=Size;

  if(size == 0)
   return storage;

  for(size_t i=0;i<size;i++)
  {
   if(!storage.SelectNode("elem",i))
	return storage;
   bool element;
   operator >>(storage,element);
   data[i]=element;
   storage.SelectUp();
  }
 }
 else
 {
  unsigned int size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить

  if(size>Size)
   size=Size;

  if(size>0)
  {
   std::string rvalue=storage.GetNodeText();
   std::stringstream stream(rvalue.c_str());

   for(unsigned i=0;i<size;i++)
   {
	int temp;
	stream>>temp;
	data[i]=temp;
   }
  }
 }

 return storage;
}

template<int Size>
USerStorageXML& operator << (USerStorageXML& storage, double const (&data)[Size])
{
 storage.SetNodeAttribute("Type","C-simplearray");
 storage.SetNodeAttribute("Size",sntoa(Size));

 if(Size <= 0)
  return storage;

 std::stringstream stream;

 for(unsigned i=0;i<Size;i++)
 {
  stream<<data[i];
  if(i<Size-1)
   stream<<" ";
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<int Size>
USerStorageXML& operator >> (USerStorageXML& storage, double const (&data)[Size])
{
 if(storage.GetNodeAttribute("Type") == "C-array")
 {
  unsigned int size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить

  if(size>Size)
   size=Size;

  if(size == 0)
   return storage;

  for(size_t i=0;i<size;i++)
  {
   if(!storage.SelectNode("elem",i))
	return storage;
   double element;
   operator >>(storage,element);
   data[i]=element;
   storage.SelectUp();
  }
 }
 else
 {
  unsigned int size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить
  if(size>Size)
   size=Size;

  if(size>0)
  {
   std::string rvalue=storage.GetNodeText();
   std::stringstream stream(rvalue.c_str());

   for(unsigned i=0;i<size;i++)
	stream>>data[i];
  }
 }
 return storage;
}

template<int Size>
USerStorageXML& operator << (USerStorageXML& storage, const int (&data)[Size])
{
 storage.SetNodeAttribute("Type","simplevector");
 storage.SetNodeAttribute("Size",sntoa(Size));

 if(Size <= 0)
  return storage;

 std::stringstream stream;

 for(unsigned i=0;i<Size;i++)
 {
  stream<<data[i];
  if(i<Size-1)
   stream<<" ";
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<int Size>
USerStorageXML& operator >> (USerStorageXML& storage, int (&data)[Size])
{
 if(storage.GetNodeAttribute("Type") == "C-array")
 {
  unsigned int size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить

  if(size>Size)
   size=Size;

  if(size == 0)
   return storage;

  for(size_t i=0;i<size;i++)
  {
   if(!storage.SelectNode("elem",i))
	return storage;
   int element;
   operator >>(storage,element);
   data[i]=element;
   storage.SelectUp();
  }
 }
 else
 {
  unsigned int size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить
  if(size>Size)
   size=Size;

  if(size>0)
  {
   std::string rvalue=storage.GetNodeText();
   std::stringstream stream(rvalue.c_str());

   for(unsigned i=0;i<size;i++)
	stream>>data[i];
  }
 }
 return storage;
}

template<int Size>
USerStorageXML& operator << (USerStorageXML& storage, const uint32_t (&data)[Size])
{
 storage.SetNodeAttribute("Type","simplevector");
 storage.SetNodeAttribute("Size",sntoa(Size));

 if(Size <= 0)
  return storage;

 std::stringstream stream;

 for(unsigned i=0;i<Size;i++)
 {
  stream<<data[i];
  if(i<Size-1)
   stream<<" ";
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<int Size>
USerStorageXML& operator >> (USerStorageXML& storage, uint32_t (&data)[Size])
{
 if(storage.GetNodeAttribute("Type") == "C-array")
 {
  size_t size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить

  if(size>Size)
   size=Size;

  if(size == 0)
   return storage;

  for(size_t i=0;i<size;i++)
  {
   if(!storage.SelectNode("elem",i))
	return storage;
   int element;
   operator >>(storage,element);
   data[i]=element;
   storage.SelectUp();
  }
 }
 else
 {
  unsigned int size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить
  if(size>Size)
   size=Size;

  if(size>0)
  {
   std::string rvalue=storage.GetNodeText();
   std::stringstream stream(rvalue.c_str());

   for(unsigned i=0;i<size;i++)
	stream>>data[i];
  }
 }
 return storage;
}

template<int Size>
USerStorageXML& operator << (USerStorageXML& storage, const uint8_t (&data)[Size])
{
 storage.SetNodeAttribute("Type","simplevector");
 storage.SetNodeAttribute("Size",sntoa(Size));

 if(Size <= 0)
  return storage;

 std::stringstream stream;

 for(unsigned i=0;i<Size;i++)
 {
  stream<<data[i];
  if(i<Size-1)
   stream<<" ";
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<int Size>
USerStorageXML& operator >> (USerStorageXML& storage, uint8_t (&data)[Size])
{
 if(storage.GetNodeAttribute("Type") == "C-array")
 {
  size_t size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить

  if(size>Size)
   size=Size;

  if(size == 0)
   return storage;

  for(size_t i=0;i<size;i++)
  {
   if(!storage.SelectNode("elem",i))
	return storage;
   int element;
   operator >>(storage,element);
   data[i]=element;
   storage.SelectUp();
  }
 }
 else
 {
  unsigned int size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить
  if(size>Size)
   size=Size;

  if(size>0)
  {
   std::string rvalue=storage.GetNodeText();
   std::stringstream stream(rvalue.c_str());

   for(unsigned i=0;i<size;i++)
	stream>>data[i];
  }
 }
 return storage;
}



template<int Size,typename T>
USerStorageXML& operator << (USerStorageXML& storage, const T (&data)[Size])
{
 storage.SetNodeAttribute("Type","C-array");

 if(Size< 0)
  return storage;

 storage.SetNodeAttribute("Size",sntoa(Size));

 for(size_t i=0;i<Size;i++)
 {
  storage.AddNode("elem");
  storage<<data[i];
  storage.SelectUp();
 }

 return storage;
}

template<int Size, typename T>
USerStorageXML& operator >> (USerStorageXML& storage, T (&data)[Size])
{
 if(storage.GetNodeAttribute("Type") == "C-array")
 {
  int size=storage.GetNumNodes();

  if(size>Size)
   size=Size;

  if(size <= 0)
   return storage;

  T* pdata=&data[0];

  for(int i=0;i<size;i++)
  {
   if(!storage.SelectNode("elem",i))
	return storage;
   operator >>(storage,*(pdata+i));
   storage.SelectUp();
  }
 }
 else
 if(storage.GetNodeAttribute("Type") == "C-simplearray")
 {
  unsigned int size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить

  if(size>Size)
   size=Size;

  if(size>0)
  {
   std::string rvalue=storage.GetNodeText();
   std::stringstream stream(rvalue.c_str());

   for(unsigned i=0;i<size;i++)
   {
	T temp;
	stream>>temp;
	data[i]=temp;
   }
  }
 }

 return storage;
}

// C-массивы 2D
template<int Size1, int Size2>
USerStorageXML& operator << (USerStorageXML& storage, bool const (&data)[Size1][Size2])
{
 storage.SetNodeAttribute("Type","C-simplearray");
 storage.SetNodeAttribute("Size1",sntoa(Size1));
 storage.SetNodeAttribute("Size2",sntoa(Size2));

 if(Size1 <= 0 || Size2 <= 0)
  return storage;

 std::stringstream stream;

 stream<<endl;
 for(unsigned i=0;i<Size1;i++)
 {
  for(unsigned j=0;j<Size2;j++)
  {
   stream<<int(data[i][j]);
//   if(j<Size2-1)
	stream<<"\t";
  }
  if(i<Size1-1)
   stream<<endl;
 }

 storage.SetNodeText(stream.str());
 return storage;
}

template<int Size1, int Size2>
USerStorageXML& operator >> (USerStorageXML& storage, bool (&data)[Size1][Size2])
{
 unsigned int size1=RDK::atoi(storage.GetNodeAttribute("Size1")); // TODO: заменить
 unsigned int size2=RDK::atoi(storage.GetNodeAttribute("Size2")); // TODO: заменить

 if(size1>Size1)
  size1=Size1;

 if(size2>Size2)
  size2=Size2;

 if(size1>0 && size2>0)
 {
  std::string rvalue=storage.GetNodeText();
  std::stringstream stream(rvalue.c_str());

  for(int i=0;i<size1;i++)
  {
   for(int j=0;j<size2;j++)
   {
	int temp;
	stream>>temp;
    data[i][j]=temp;
   }
  }
 }

 return storage;
}

template<int Size1, int Size2>
USerStorageXML& operator << (USerStorageXML& storage, double const (&data)[Size1][Size2])
{
 storage.SetNodeAttribute("Type","C-simplearray");
 storage.SetNodeAttribute("Size1",sntoa(Size1));
 storage.SetNodeAttribute("Size2",sntoa(Size2));

 if(Size1 <= 0 || Size2 <= 0)
  return storage;

 std::stringstream stream;

 stream<<endl;
 for(unsigned i=0;i<Size1;i++)
 {
  for(unsigned j=0;j<Size2;j++)
  {
   stream<<data[i][j];
//   if(j<Size2-1)
	stream<<"\t";
  }
  if(i<Size1-1)
   stream<<endl;
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<int Size1, int Size2>
USerStorageXML& operator >> (USerStorageXML& storage, double (&data)[Size1][Size2])
{
 unsigned int size1=RDK::atoi(storage.GetNodeAttribute("Size1")); // TODO: заменить
 unsigned int size2=RDK::atoi(storage.GetNodeAttribute("Size2")); // TODO: заменить

 if(size1>Size1)
  size1=Size1;

 if(size2>Size2)
  size2=Size2;

 if(size1>0 && size2>0)
 {
  std::string rvalue=storage.GetNodeText();
  std::stringstream stream(rvalue.c_str());

  for(int i=0;i<size1;i++)
  {
   for(int j=0;j<size2;j++)
   {
	double temp;
	stream>>temp;
    data[i][j]=temp;
   }
  }
 }

 return storage;
}

template<int Size1, int Size2>
USerStorageXML& operator << (USerStorageXML& storage, int const (&data)[Size1][Size2])
{
 storage.SetNodeAttribute("Type","simplevector");
 storage.SetNodeAttribute("Size1",sntoa(Size1));
 storage.SetNodeAttribute("Size2",sntoa(Size2));

 if(Size1 <= 0 || Size2 <= 0)
  return storage;

 std::stringstream stream;

 stream<<endl;
 for(unsigned i=0;i<Size1;i++)
 {
  for(unsigned j=0;j<Size2;j++)
  {
   stream<<data[i][j];
//   if(j<Size2-1)
	stream<<"\t";
  }
  if(i<Size1-1)
   stream<<endl;
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<int Size1, int Size2>
USerStorageXML& operator >> (USerStorageXML& storage, int (&data)[Size1][Size2])
{
 unsigned int size1=RDK::atoi(storage.GetNodeAttribute("Size1")); // TODO: заменить
 unsigned int size2=RDK::atoi(storage.GetNodeAttribute("Size2")); // TODO: заменить

 if(size1>Size1)
  size1=Size1;

 if(size2>Size2)
  size2=Size2;

 if(size1>0 && size2>0)
 {
  std::string rvalue=storage.GetNodeText();
  std::stringstream stream(rvalue.c_str());

  for(int i=0;i<size1;i++)
  {
   for(int j=0;j<size2;j++)
   {
	int temp;
	stream>>temp;
    data[i][j]=temp;
   }
  }
 }

 return storage;
}

template<int Size1, int Size2>
USerStorageXML& operator << (USerStorageXML& storage, uint32_t const (&data)[Size1][Size2])
{
 storage.SetNodeAttribute("Type","simplevector");
 storage.SetNodeAttribute("Size1",sntoa(Size1));
 storage.SetNodeAttribute("Size2",sntoa(Size2));

 if(Size1 <= 0 || Size2 <= 0)
  return storage;

 std::stringstream stream;

 stream<<endl;
 for(unsigned i=0;i<Size1;i++)
 {
  for(unsigned j=0;j<Size2;j++)
  {
   stream<<data[i][j];
//   if(j<Size2-1)
	stream<<"\t";
  }
  if(i<Size1-1)
   stream<<endl;
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<int Size1, int Size2>
USerStorageXML& operator >> (USerStorageXML& storage, uint32_t (&data)[Size1][Size2])
{
 unsigned int size1=RDK::atoi(storage.GetNodeAttribute("Size1")); // TODO: заменить
 unsigned int size2=RDK::atoi(storage.GetNodeAttribute("Size2")); // TODO: заменить

 if(size1>Size1)
  size1=Size1;

 if(size2>Size2)
  size2=Size2;

 if(size1>0 && size2>0)
 {
  std::string rvalue=storage.GetNodeText();
  std::stringstream stream(rvalue.c_str());

  for(int i=0;i<size1;i++)
  {
   for(int j=0;j<size2;j++)
   {
	uint32_t temp;
	stream>>temp;
    data[i][j]=temp;
   }
  }
 }

 return storage;
}

template<int Size1, int Size2>
USerStorageXML& operator << (USerStorageXML& storage, uint8_t const (&data)[Size1][Size2])
{
 storage.SetNodeAttribute("Type","simplevector");
 storage.SetNodeAttribute("Size1",sntoa(Size1));
 storage.SetNodeAttribute("Size2",sntoa(Size2));

 if(Size1 <= 0 || Size2 <= 0)
  return storage;

 std::stringstream stream;

 stream<<endl;
 for(unsigned i=0;i<Size1;i++)
 {
  for(unsigned j=0;j<Size2;j++)
  {
   stream<<uint32_t(data[i][j]);
//   if(j<Size2-1)
	stream<<"\t";
  }
  if(i<Size1-1)
   stream<<endl;
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<int Size1, int Size2>
USerStorageXML& operator >> (USerStorageXML& storage, uint8_t (&data)[Size1][Size2])
{
 unsigned int size1=RDK::atoi(storage.GetNodeAttribute("Size1")); // TODO: заменить
 unsigned int size2=RDK::atoi(storage.GetNodeAttribute("Size2")); // TODO: заменить

 if(size1>Size1)
  size1=Size1;

 if(size2>Size2)
  size2=Size2;

 if(size1>0 && size2>0)
 {
  std::string rvalue=storage.GetNodeText();
  std::stringstream stream(rvalue.c_str());

  for(int i=0;i<size1;i++)
  {
   for(int j=0;j<size2;j++)
   {
	uint8_t temp;
	stream>>temp;
    data[i][j]=temp;
   }
  }
 }

 return storage;
}


template<int Size1, int Size2, typename T>
USerStorageXML& operator << (USerStorageXML& storage, T const (&data)[Size1][Size2])
{
 storage.SetNodeAttribute("Type","C-array");

 storage.SetNodeAttribute("Size1",sntoa(Size1));
 storage.SetNodeAttribute("Size2",sntoa(Size2));

 if(Size1 <= 0 || Size2 <= 0)
  return storage;

 for(unsigned i=0;i<Size1;i++)
 {
  storage.AddNode("elem");
  for(unsigned j=0;j<Size2;j++)
  {
   storage.AddNode("elem");
   storage<<data[i][j];
   storage.SelectUp();
  }
  storage.SelectUp();
 }

 return storage;
}

template<int Size1, int Size2, typename T>
USerStorageXML& operator >> (USerStorageXML& storage, T (&data)[Size1][Size2])
{
 if(storage.GetNodeAttribute("Type") != "C-array")
  return storage;

 int size1=storage.GetNumNodes();

 if(size1>Size1)
  size1=Size1;

 if(size1 <= 0)
  return storage;

 for(int i=0;i<size1;i++)
 {
  if(!storage.SelectNode("elem",i))
   return storage;
  int size2=storage.GetNumNodes();
  if(size2>Size2)
   size2=Size2;
  for(int j=0;j<size2;j++)
  {
   if(!storage.SelectNode("elem",j))
	break;
   operator >>(storage,data[i][j]);
   storage.SelectUp();
  }

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

template<int Size, typename T>
bool USerStorageXML::ReadData(const std::string &name, T (&data)[Size])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size,T>(*this,data);
 SelectUp();
 return true;
}

template<int Size>
bool USerStorageXML::ReadData(const std::string &name, bool (&data)[Size])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size>(*this,data);
 SelectUp();
 return true;
}

template<int Size>
bool USerStorageXML::ReadData(const std::string &name, double (&data)[Size])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size>(*this,data);
 SelectUp();
 return true;
}

template<int Size>
bool USerStorageXML::ReadData(const std::string &name, int (&data)[Size])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size>(*this,data);
 SelectUp();
 return true;
}

template<int Size>
bool USerStorageXML::ReadData(const std::string &name, uint8_t (&data)[Size])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size>(*this,data);
 SelectUp();
 return true;
}

template<int Size>
bool USerStorageXML::ReadData(const std::string &name, uint32_t (&data)[Size])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size>(*this,data);
 SelectUp();
 return true;
}

template<int Size>
bool USerStorageXML::WriteData(const std::string &name, bool const (&data)[Size])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size>(*this,data);
 SelectUp();
 return true;
}

template<int Size>
bool USerStorageXML::WriteData(const std::string &name, double const (&data)[Size])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size>(*this,data);
 SelectUp();
 return true;
}

template<int Size>
bool USerStorageXML::WriteData(const std::string &name, int const (&data)[Size])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size>(*this,data);
 SelectUp();
 return true;
}

template<int Size>
bool USerStorageXML::WriteData(const std::string &name, uint8_t const (&data)[Size])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size>(*this,data);
 SelectUp();
 return true;
}

template<int Size>
bool USerStorageXML::WriteData(const std::string &name, uint32_t const (&data)[Size])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size>(*this,data);
 SelectUp();
 return true;
}

template<int Size, typename T>
bool USerStorageXML::WriteData(const std::string &name, T const (&data)[Size])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size,T>(*this,data);
 SelectUp();
 return true;
}

template<int Size, typename T>
bool USerStorageXML::WriteDataOnce(const std::string &name, T const (&data)[Size])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size,T>(*this,data);

 SelectUp();
 return true;
}

template<int Size1, int Size2, typename T>
bool USerStorageXML::ReadData(const std::string &name, T (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size1,Size2,T>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2>
bool USerStorageXML::ReadData(const std::string &name, bool (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size1,Size2>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2>
bool USerStorageXML::ReadData(const std::string &name, double (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size1,Size2>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2>
bool USerStorageXML::ReadData(const std::string &name, int (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size1,Size2>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2>
bool USerStorageXML::ReadData(const std::string &name, uint32_t (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size1,Size2>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2>
bool USerStorageXML::ReadData(const std::string &name, uint8_t (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  return false;
 operator >> <Size1,Size2>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2, typename T>
bool USerStorageXML::WriteData(const std::string &name, T const (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size1,Size2,T>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2>
bool USerStorageXML::WriteData(const std::string &name, bool const (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size1,Size2>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2>
bool USerStorageXML::WriteData(const std::string &name, double const (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size1,Size2>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2>
bool USerStorageXML::WriteData(const std::string &name, int const (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size1,Size2>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2>
bool USerStorageXML::WriteData(const std::string &name, uint32_t const (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size1,Size2>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2>
bool USerStorageXML::WriteData(const std::string &name, uint8_t const (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size1,Size2>(*this,data);
 SelectUp();
 return true;
}

template<int Size1, int Size2, typename T>
bool USerStorageXML::WriteDataOnce(const std::string &name, T const (&data)[Size1][Size2])
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << <Size1,Size2,T>(*this,data);

 SelectUp();
 return true;
}

}
#endif

