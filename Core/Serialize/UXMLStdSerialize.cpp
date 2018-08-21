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
 return USimpleToStorageF(storage,data,20);
}

USerStorageXML& operator >> (USerStorageXML& storage, float &data)
{
 return USimpleFromStorageF(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, double data)
{
 return USimpleToStorageF(storage,data,40);
}

USerStorageXML& operator >> (USerStorageXML& storage, double &data)
{
 return USimpleFromStorageF(storage,data);
}

USerStorageXML& operator << (USerStorageXML& storage, long double data)
{
 return USimpleToStorageF(storage,data,40);
}

USerStorageXML& operator >> (USerStorageXML& storage, long double &data)
{
 return USimpleFromStorageF(storage,data);
}

// Вектора
USerStorageXML& operator << (USerStorageXML& storage, const std::vector<bool> &data)
{
 storage.SetNodeAttribute("Type","simplevector");
 size_t size=data.size();
 storage.SetNodeAttribute("Size",sntoa(size));

 if(size <= 0)
  return storage;

 std::stringstream stream;

 for(unsigned i=0;i<size;i++)
 {
  stream<<int(data[i]);
  if(i<size-1)
   stream<<" ";
 }

 storage.SetNodeText(stream.str());
 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, std::vector<bool> &data)
{
 if(storage.GetNodeAttribute("Type") == "std::vector")
 {
  int size=0;
//  size=RDK::atoi(storage.GetNodeAttribute("Size"));
  size=storage.GetNumNodes();

  if(size <= 0)
  {
   data.resize(0);
   return storage;
  }
  data.resize(size);

  for(int i=0;i<size;i++)
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
  int size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить
  data.resize(size);

  if(size>0)
  {
   std::string rvalue=storage.GetNodeText();
   std::stringstream stream(storage.GetNodeText().c_str());

   for(int i=0;i<size;i++)
   {
	int temp;
	stream>>temp;
	data[i]=temp;
   }
  }
 }

 return storage;
}

USerStorageXML& operator << (USerStorageXML& storage, const std::vector<double> &data)
{
 storage.SetNodeAttribute("Type","simplevector");
 size_t size=data.size();
 storage.SetNodeAttribute("Size",sntoa(size));

 if(size <= 0)
  return storage;

 std::stringstream stream;

 for(size_t i=0;i<size;i++)
 {
  stream<<data[i];
  if(i<size-1)
   stream<<" ";
 }

 storage.SetNodeText(stream.str());

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, std::vector<double> &data)
{
 if(storage.GetNodeAttribute("Type") == "std::vector")
 {
  int size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить

  if(size <= 0)
  {
   data.resize(0);
   return storage;
  }
  data.resize(size);

  for(int i=0;i<size;i++)
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
  int size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить
  data.resize(size);

  if(size>0)
  {
   std::string rvalue=storage.GetNodeText();
   std::stringstream stream(storage.GetNodeText().c_str());

   for(int i=0;i<size;i++)
	stream>>data[i];
  }
 }
 return storage;
}


USerStorageXML& operator << (USerStorageXML& storage, const std::vector<int> &data)
{
 storage.SetNodeAttribute("Type","simplevector");
 int size=int(data.size());
 storage.SetNodeAttribute("Size",sntoa(size));

 if(size <= 0)
  return storage;

 std::stringstream stream;

 for(int i=0;i<size;i++)
 {
  stream<<data[i];
  if(i<size-1)
   stream<<" ";
 }

 storage.SetNodeText(stream.str());

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, std::vector<int> &data)
{
 if(storage.GetNodeAttribute("Type") == "std::vector")
 {
  int size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить

  if(size <= 0)
  {
   data.resize(0);
   return storage;
  }
  data.resize(size);

  for(int i=0;i<size;i++)
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
  int size=RDK::atoi(storage.GetNodeAttribute("Size")); // TODO: заменить
  data.resize(size);

  if(size>0)
  {
   std::string rvalue=storage.GetNodeText();
   std::stringstream stream(storage.GetNodeText().c_str());

   for(int i=0;i<size;i++)
	stream>>data[i];
  }
 }
 return storage;
}


// Строки
//template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const std::string &data)
{
 storage.SetNodeAttribute("Type","std::string");

 storage.SetNodeText(data);
 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, std::string &data)
{
 data=storage.GetNodeText();

 return storage;
}

USerStorageXML& operator << (USerStorageXML& storage, const std::wstring &data)
{
// storage.SetNodeAttribute("Type","std::wstring");

// storage.SetNodeText(data);
 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, std::wstring &data)
{
// data=storage.GetNodeText();

 return storage;
}

}
#endif

