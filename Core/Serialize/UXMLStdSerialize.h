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
#include <type_traits>
#include <cstdio>
#include <cstdlib>
#include "USerStorageXML.h"
#include "../Utilities/UTree.h"
#include "../Utilities/USupport.h"

namespace RDK {

// Вспомогательный trait для проверки, является ли тип вектором
template<typename T>
struct is_vector : std::false_type {};

template<typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};

template<typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;

// Вспомогательная функция для определения типа без создания XML объекта (оптимизация)
// Использует typeid и std::is_same вместо создания временных объектов
template<typename T>
inline std::string GetTypeName()
{
 return typeid(T).name();
}

// Специализации для часто используемых типов
template<> inline std::string GetTypeName<bool>() { return typeid(bool).name(); }
template<> inline std::string GetTypeName<char>() { return typeid(char).name(); }
template<> inline std::string GetTypeName<unsigned char>() { return typeid(unsigned char).name(); }
template<> inline std::string GetTypeName<short>() { return typeid(short).name(); }
template<> inline std::string GetTypeName<unsigned short>() { return typeid(unsigned short).name(); }
template<> inline std::string GetTypeName<int>() { return typeid(int).name(); }
template<> inline std::string GetTypeName<unsigned int>() { return typeid(unsigned int).name(); }
template<> inline std::string GetTypeName<long>() { return typeid(long).name(); }
template<> inline std::string GetTypeName<unsigned long>() { return typeid(unsigned long).name(); }
template<> inline std::string GetTypeName<long long>() { return typeid(long long).name(); }
template<> inline std::string GetTypeName<unsigned long long>() { return typeid(unsigned long long).name(); }
template<> inline std::string GetTypeName<float>() { return typeid(float).name(); }
template<> inline std::string GetTypeName<double>() { return typeid(double).name(); }
template<> inline std::string GetTypeName<long double>() { return typeid(long double).name(); }
template<> inline std::string GetTypeName<std::string>() { return "std::string"; }

// Оптимизированные функции сериализации простых типов
// Используют быстрые функции преобразования с сохранением полной точности для вещественных чисел

// Перегрузка для double - сохраняет полную мантиссу (17 значащих цифр)
inline USerStorageXML& USimpleToStorage(USerStorageXML& storage, const double data)
{
 storage.SetNodeAttribute("Type",typeid(double).name());
 std::string str;
 RDK::ntoa(data, str); // Использует оптимизированную функцию с %.17g
 storage.SetNodeText(str);
 return storage;
}

// Перегрузка для float - сохраняет полную мантиссу (9 значащих цифр)
inline USerStorageXML& USimpleToStorage(USerStorageXML& storage, const float data)
{
 storage.SetNodeAttribute("Type",typeid(float).name());
 std::string str;
 RDK::ntoa(data, str); // Использует оптимизированную функцию с %.9g
 storage.SetNodeText(str);
 return storage;
}

// Перегрузка для long double - сохраняет полную мантиссу (21 значащая цифра)
inline USerStorageXML& USimpleToStorage(USerStorageXML& storage, const long double data)
{
 storage.SetNodeAttribute("Type",typeid(long double).name());
 std::string str;
 RDK::ntoa(data, str); // Использует оптимизированную функцию с %.21Lg
 storage.SetNodeText(str);
 return storage;
}

// Общий шаблон для остальных типов (использует оптимизированные функции)
template<typename T>
USerStorageXML& USimpleToStorage (USerStorageXML& storage, const T data)
{
 storage.SetNodeAttribute("Type",typeid(T).name());
 std::string str;
 RDK::ntoa(data, str); // Использует оптимизированные перегрузки
 storage.SetNodeText(str);
 return storage;
}

// Оптимизированные функции десериализации
// Перегрузка для double - сохраняет полную точность
inline USerStorageXML& USimpleFromStorage(USerStorageXML& storage, double &data)
{
 std::string text = storage.GetNodeText();
 data = RDK::atof(text); // Использует оптимизированную функцию strtod
 return storage;
}

// Перегрузка для float
inline USerStorageXML& USimpleFromStorage(USerStorageXML& storage, float &data)
{
 std::string text = storage.GetNodeText();
 data = (float)RDK::atof(text); // Использует оптимизированную функцию strtod
 return storage;
}

// Перегрузка для long double
inline USerStorageXML& USimpleFromStorage(USerStorageXML& storage, long double &data)
{
 std::string text = storage.GetNodeText();
 data = (long double)RDK::atof(text); // Использует оптимизированную функцию strtod
 return storage;
}

// Общий шаблон для остальных типов
template<typename T>
USerStorageXML& USimpleFromStorage (USerStorageXML& storage, T &data)
{
 std::string text = storage.GetNodeText();
 // Для целых чисел используем оптимизированную функцию
 if constexpr (std::is_integral_v<T>)
 {
  data = (T)RDK::atoi(text);
 }
 else
 {
  // Fallback на stringstream для других типов
  std::stringstream stream(text);
  stream>>data;
 }
 return storage;
}

// Оптимизированные функции для вещественных типов с заданной точностью
// Сохраняют полную мантиссу при prec >= необходимой точности
template<typename T>
USerStorageXML& USimpleToStorageF(USerStorageXML& storage, const T data, int prec)
{
 storage.SetNodeAttribute("Type",typeid(T).name());
 std::string str;
 RDK::ntoa(data, prec, str); // Использует оптимизированную функцию с учетом точности
 storage.SetNodeText(str);
 return storage;
}

template<typename T>
USerStorageXML& USimpleFromStorageF(USerStorageXML& storage, T &data)
{
 std::string text = storage.GetNodeText();
 data = (T)RDK::atof(text); // Использует оптимизированную функцию strtod для максимальной точности
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
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning (disable: 4700)
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
template<typename T1, typename T2>
USerStorageXML& operator << (USerStorageXML& storage, const std::map<T1,T2> &data)
{
 storage.SetNodeAttribute("Type","std::map");
 storage.SetNodeAttribute("Size",sntoa(data.size()));

 // Оптимизация: используем typeid вместо создания временных XML объектов
 std::string first_type = GetTypeName<T1>();
 storage.SetNodeAttribute("firstType",first_type);

 std::string second_type = GetTypeName<T2>();
 storage.SetNodeAttribute("secondType",second_type);


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
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning ( pop )
#endif

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

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning (disable: 4700)
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
// Списки
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const std::list<T> &data)
{
 storage.SetNodeAttribute("Type","std::list");
 size_t size=data.size();
 storage.SetNodeAttribute("Size",sntoa(size));

 // Оптимизация: используем typeid вместо создания временных XML объектов
 std::string type = GetTypeName<T>();
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
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning ( pop )
#endif

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

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning (disable: 4700)
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const std::vector<T> &data)
{ 
 storage.SetNodeAttribute("Type","std::vector");

 // Исключительный случай для конкретного свойства сложного типа
 if(storage.GetNodeName() == "PPlanePoints")
 {
  storage.SetNodeAttribute("Type","PPlanePoints");
 }

 // Исключительный случай для конкретного свойства сложного типа
 if(storage.GetNodeName() == "NetStructure")
 {
  storage.SetNodeAttribute("Type","NetStructure");
 }


 size_t size=data.size();
 storage.SetNodeAttribute("Size",sntoa(size));

 // Оптимизация: используем typeid вместо создания временных XML объектов
 std::string elem_type = GetTypeName<T>();
 storage.SetNodeAttribute("elemType",elem_type);

 // Для определения вложенных типов используем проверку во время компиляции
 // Если это массив, сохраняем размер
 if constexpr (std::is_array_v<T>)
 {
  storage.SetNodeAttribute("elemSize", sntoa(std::extent_v<T>));
  // Для массивов определяем тип элемента
  using ElemType = std::remove_extent_t<T>;
  std::string elem_SV_type = GetTypeName<ElemType>();
  storage.SetNodeAttribute("elemSVType", elem_SV_type);
 }
 else if constexpr (is_vector_v<T>)
 {
  // Для векторов проверяем тип элемента
  using ElemType = typename T::value_type;
  std::string elem_SV_type = GetTypeName<ElemType>();
  
  // Проверяем, является ли элемент вектором (вложенный вектор)
  if constexpr (is_vector_v<ElemType>)
  {
   storage.SetNodeAttribute("elemVecType", elem_SV_type);
  }
  else if constexpr (std::is_array_v<ElemType>)
  {
   storage.SetNodeAttribute("elemSVType", elem_SV_type);
  }
 }

 /*
 if(elem_type == "pointer")
 {
     T elem_temp;
     USerStorageXML elem_tempXML;
     elem_tempXML.Destroy();
     elem_tempXML.Create("elem_temp");
     elem_tempXML << elem_temp;
     std::string elem_point_type = elem_tempXML.GetNodeAttribute("pointType");

     storage.SetNodeAttribute("elemPointType",elem_point_type);
 }
 */

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
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning ( pop )
#endif

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
// Оптимизированная сериализация C-массивов с сохранением полной точности для вещественных чисел
template<int Size>
USerStorageXML& operator << (USerStorageXML& storage, bool const (&data)[Size])
{
 storage.SetNodeAttribute("Type","C-simplearray");
 storage.SetNodeAttribute("Size",sntoa(Size));

 if(Size <= 0)
  return storage;

 // Оптимизированная сериализация - используем прямой вывод в буфер
 std::string result;
 result.reserve(Size * 2);

 for(unsigned i=0;i<Size;i++)
 {
  if(i > 0)
   result += " ";
  result += (data[i] ? "1" : "0");
 }

 storage.SetNodeText(result);
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
  unsigned int size=RDK::atoi(storage.GetNodeAttribute("Size"));

  if(size>Size)
   size=Size;

  if(size>0)
  {
   // Оптимизированная десериализация - прямой парсинг строки
   std::string text = storage.GetNodeText();
   const char* start = text.c_str();
   const char* end = start + text.length();
   
   for(unsigned i=0;i<size && start < end;i++)
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

template<int Size>
USerStorageXML& operator << (USerStorageXML& storage, double const (&data)[Size])
{
 storage.SetNodeAttribute("Type","C-simplearray");
 storage.SetNodeAttribute("Size",sntoa(Size));

 if(Size <= 0)
  return storage;

 // Оптимизированная сериализация с сохранением полной мантиссы
 std::string result;
 result.reserve(Size * 32);

 char buffer[64];
 for(unsigned i=0;i<Size;i++)
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

template<int Size>
USerStorageXML& operator >> (USerStorageXML& storage, double (&data)[Size])
{
 if(storage.GetNodeAttribute("Type") == "C-array")
 {
  unsigned int size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size"));

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
  unsigned int size=RDK::atoi(storage.GetNodeAttribute("Size"));
  if(size>Size)
   size=Size;

  if(size>0)
  {
   // Оптимизированная десериализация с сохранением полной точности
   std::string text = storage.GetNodeText();
   for(char &ch : text)
   {
    if(ch == ',')
     ch = '.';
   }
   const char* start = text.c_str();
   const char* end = start + text.length();
   
   for(unsigned i=0;i<size && start < end;i++)
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

template<int Size>
USerStorageXML& operator << (USerStorageXML& storage, const int (&data)[Size])
{
 storage.SetNodeAttribute("Type","simplevector");
 storage.SetNodeAttribute("Size",sntoa(Size));

 if(Size <= 0)
  return storage;

 // Оптимизированная сериализация - используем прямой вывод в буфер
 std::string result;
 result.reserve(Size * 16);

 char buffer[32];
 for(unsigned i=0;i<Size;i++)
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

template<int Size>
USerStorageXML& operator >> (USerStorageXML& storage, int (&data)[Size])
{
 if(storage.GetNodeAttribute("Type") == "C-array")
 {
  unsigned int size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size"));

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
  unsigned int size=RDK::atoi(storage.GetNodeAttribute("Size"));
  if(size>Size)
   size=Size;

  if(size>0)
  {
   // Оптимизированная десериализация - прямой парсинг строки
   std::string text = storage.GetNodeText();
   const char* start = text.c_str();
   const char* end = start + text.length();
   
   for(unsigned i=0;i<size && start < end;i++)
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

