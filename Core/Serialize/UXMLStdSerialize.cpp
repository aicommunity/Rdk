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
 storage.SetNodeAttribute("elemType",typeid(bool).name());

 if(size <= 0)
  return storage;

 // Оптимизированная сериализация - используем прямой вывод в буфер
 std::string result;
 result.reserve(size * 2); // Резервируем место для всех элементов

 for(unsigned i=0;i<size;i++)
 {
  if(i > 0)
   result += " ";
  result += (data[i] ? "1" : "0");
 }

 storage.SetNodeText(result);
 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, std::vector<bool> &data)
{
 if(storage.GetNodeAttribute("Type") == "std::vector")
 {
  int size=0;
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
  int size=RDK::atoi(storage.GetNodeAttribute("Size"));
  data.resize(size);

  if(size>0)
  {
   // Оптимизированная десериализация - прямой парсинг строки
   std::string text = storage.GetNodeText();
   const char* start = text.c_str();
   const char* end = start + text.length();
   
   for(int i=0;i<size && start < end;i++)
   {
    // Пропускаем пробелы
    while(start < end && (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r'))
     start++;
    
    if(start >= end)
     break;
    
    // Парсим как int, затем преобразуем в bool
    char* next;
    int temp = (int)std::strtol(start, &next, 10);
    data[i] = (temp != 0);
    if(next == start)
     break; // Ошибка парсинга
    start = next;
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
 storage.SetNodeAttribute("elemType",typeid(double).name());

 if(size <= 0)
  return storage;

 // Оптимизированная сериализация с сохранением полной мантиссы
 // Используем предварительно выделенный буфер вместо stringstream
 std::string result;
 result.reserve(size * 32); // Резервируем место для всех элементов

 char buffer[64];
 for(size_t i=0;i<size;i++)
 {
  // Используем формат с максимальной точностью для сохранения полной мантиссы
  int len = snprintf(buffer, sizeof(buffer), "%.17g", data[i]);
  if(len > 0 && len < (int)sizeof(buffer))
  {
   if(i > 0)
    result += " ";
   result.append(buffer, len);
  }
  else
  {
   // Fallback на stringstream в случае ошибки
   std::stringstream stream;
   stream << std::setprecision(17) << data[i];
   if(i > 0)
    result += " ";
   result += stream.str();
  }
 }

 storage.SetNodeText(result);

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, std::vector<double> &data)
{
 if(storage.GetNodeAttribute("Type") == "std::vector")
 {
  int size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size"));

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
  int size=RDK::atoi(storage.GetNodeAttribute("Size"));
  data.resize(size);

  if(size>0)
  {
   // Оптимизированная десериализация - прямой парсинг строки
   std::string text = storage.GetNodeText();
   const char* start = text.c_str();
   const char* end = start + text.length();
   
   for(int i=0;i<size && start < end;i++)
   {
    // Пропускаем пробелы
    while(start < end && (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r'))
     start++;
    
    if(start >= end)
     break;
    
    // Используем strtod для максимальной точности
    char* next;
    data[i] = std::strtod(start, &next);
    if(next == start)
     break; // Ошибка парсинга
    start = next;
   }
  }
 }
 return storage;
}


USerStorageXML& operator << (USerStorageXML& storage, const std::vector<int> &data)
{
 storage.SetNodeAttribute("Type","simplevector");
 int size=int(data.size());
 storage.SetNodeAttribute("Size",sntoa(size));
 storage.SetNodeAttribute("elemType",typeid(int).name());

 if(size <= 0)
  return storage;

 // Оптимизированная сериализация - используем прямой вывод в буфер
 std::string result;
 result.reserve(size * 16); // Резервируем место для всех элементов

 char buffer[32];
 for(int i=0;i<size;i++)
 {
  int len = snprintf(buffer, sizeof(buffer), "%d", data[i]);
  if(len > 0 && len < (int)sizeof(buffer))
  {
   if(i > 0)
    result += " ";
   result.append(buffer, len);
  }
  else
  {
   // Fallback на stringstream в случае ошибки
   std::stringstream stream;
   stream << data[i];
   if(i > 0)
    result += " ";
   result += stream.str();
  }
 }

 storage.SetNodeText(result);

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, std::vector<int> &data)
{
 if(storage.GetNodeAttribute("Type") == "std::vector")
 {
  int size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size"));

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
  int size=RDK::atoi(storage.GetNodeAttribute("Size"));
  data.resize(size);

  if(size>0)
  {
   // Оптимизированная десериализация - прямой парсинг строки
   std::string text = storage.GetNodeText();
   const char* start = text.c_str();
   const char* end = start + text.length();
   
   for(int i=0;i<size && start < end;i++)
   {
    // Пропускаем пробелы
    while(start < end && (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r'))
     start++;
    
    if(start >= end)
     break;
    
    // Используем strtol для быстрого парсинга
    char* next;
    data[i] = (int)std::strtol(start, &next, 10);
    if(next == start)
     break; // Ошибка парсинга
    start = next;
   }
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

