/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UIO_STREAM_H
#define UIO_STREAM_H

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <string>
//#include <memory.h>


namespace RDK {

//extern std::string LineBuffer;

// ��������������� ������� ���������� ����� ������ ��� ���������� � �����������

// ����
// ����������� � ������ ��������,��������
template<typename CharT, typename T1, typename T2>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const std::pair<T1,T2> &data)
{
 stream<<"{"<<data.first<<','<<data.second<<"}";
 return stream;
}

template<typename CharT, typename T1, typename T2>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, std::pair<T1,T2> &data)
{
 char ch;

 ch=stream.get();
 if(ch != '{')
  return stream;

 stream>>data.first;
 std::string LineBuffer;
 std::getline(stream, LineBuffer,',');
 stream>>data.second;
 ch=stream.get(); // ������� ����������� '}'

 return stream;
}

// �������
template<typename CharT, typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const std::vector<T> &data)
{
 size_t size=data.size();

 stream<<"{";
 if(size > 0)
 {
  for(size_t i=0;i<size;i++)
  {
   stream<<data[i];
   if(i != size-1)
    stream<<",";
  }
 }
 stream<<"}";

 return stream;
}

template<typename CharT, typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, std::vector<T> &data)
{
 size_t size=data.size();
 int i=0;
 CharT ch;

 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 do {
  if(i<int(size))
   stream>>data[i];
  else
  {
   T temp;
   stream>>temp;
   data.push_back(temp);
  }
  ++i;
  ch=stream.get();
 } while(ch != '}');

 data.resize(i);

 return stream;
}

// map
template<typename CharT, typename T1, typename T2>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const std::map<T1,T2> &data)
{
 size_t size=data.size();
 typename std::map<T1,T2>::const_iterator I,J;
 I=data.begin();
 J=data.end();

 stream<<"{";
 if(size > 0)
 {
  size_t i=0;
  while(I != J)
  {
   stream<<*I;
   if(i != size-1)
    stream<<",";
   ++I; ++i;
  }
 }
 stream<<"}";

 return stream;
}

template<typename CharT, typename T1, typename T2>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, std::map<T1,T2> &data)
{
 CharT ch;

 data.clear();

 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 do {
  std::pair<T1,T2> temp;
  stream>>temp;
  data.insert(temp);
  ch=stream.get();
 } while(ch != '}');

 return stream;
}

// list
template<typename CharT, typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const std::list<T> &data)
{
 size_t size=data.size();
 typename std::list<T>::const_iterator I,J;
 I=data.begin();
 J=data.end();

 stream<<"{";
 if(size > 0)
 {
  size_t i=0;
  while(I != J)
  {
   stream<<*I;
   if(i != size-1)
    stream<<",";
   ++I; ++i;
  }
 }
 stream<<"}";

 return stream;
}

template<typename CharT, typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, std::list<T> &data)
{
 CharT ch;

 data.clear();

 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 do {
  T temp;
  stream>>temp;
  data.push_back(temp);
  ch=stream.get();
 } while(ch != '}');

 return stream;
}

// ������
template<typename CharT>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const std::basic_string<CharT> &data)
{
 stream<<data.c_str();

 return stream;
}

template<typename CharT>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, std::basic_string<CharT> &data)
{
 std::string LineBuffer;
 LineBuffer.resize(4096);
 stream.getline(&LineBuffer[0],4095);
 data.resize(stream.gcount());
 memcpy(&data[0],&LineBuffer[0],data.size()*sizeof(CharT));

 return stream;
}
/*
// C-�������
template<typename CharT, typename T, int Size>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const T data[Size])
{
 if(Size<=0)
  return stream;

 for(int i=0;i<Size;i++)
  stream<<data[i]<<" ";

 return stream;
}

template<typename CharT, typename T, int Size>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, T data[Size])
{

 return stream;
}
   */


}
#endif

