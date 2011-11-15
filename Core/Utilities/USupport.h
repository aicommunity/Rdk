/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef USUPPORT_H
#define USUPPORT_H

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "UPtr.h"

#ifndef u_min
#define u_min(a,b) (a<b?a:b)
#endif

#ifndef u_max
#define u_max(a,b) (a>b?a:b)
#endif

#ifndef M_E
 #define M_E 2.71828182845904523536
#endif

#ifndef M_LOG2E
#define M_LOG2E 1.44269504088896340736
#endif

#ifndef M_LOG10E
#define M_LOG10E 0.434294481903251827651
#endif

#ifndef M_LN2
#define M_LN2 0.693147180559945309417
#endif

#ifndef M_LN10
#define M_LN10 2.30258509299404568402
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4 0.785398163397448309616
#endif

#ifndef M_1_PI
#define M_1_PI 0.318309886183790671538
#endif

#ifndef M_2_PI
#define M_2_PI 0.636619772367581343076
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.707106781186547524401
#endif

namespace RDK {

using namespace std;

// Cистемные буферы
extern string ustrbuf;
extern wstring uwstrbuf;

/*
 Функции возвращают указатель на внутренний буфер
 Значение буфера изменяется при следующем вызове функции библиотеки
*/

// Функция, преобразующая число в строку
template<typename CharT, typename NumT>
basic_string<CharT>& ntoa(NumT n, basic_string<CharT> &buf)
{
 basic_stringstream<CharT> stream;
 stream<<n;
 return buf=stream.str();
}

template<typename NumT>
string& sntoa(NumT n)
{
 return ntoa(n,ustrbuf);
}

template<typename NumT>
wstring& wntoa(NumT n)
{
 return ntoa(n,uwstrbuf);
}

template<typename CharT, typename NumT>
basic_string<CharT>& ntoa(NumT n, int digs, basic_string<CharT> &buf)
{
 basic_stringstream<CharT> stream;
 stream.width(digs);
 stream.fill('0');
 stream<<fixed<<setprecision(digs)<<n;
 return buf=stream.str();
}

template<typename NumT>
string& sntoa(NumT n,int digs)
{
 return ntoa(n,digs,ustrbuf);
}

template<typename NumT>
wstring& wntoa(NumT n,int digs)
{
 return ntoa(n,digs,uwstrbuf);
}

// Функция, преобразующая строку в вещественное число
template<typename CharT>
double atof(const std::basic_string<CharT> &str)
{
 basic_stringstream<CharT> stream(str);
 double res;
 stream>>res;
 return res;
}

// Функция, преобразующая строку в целое число
template<typename CharT>
int atoi(const std::basic_string<CharT> &str)
{
 basic_stringstream<CharT> stream(str);
 int res;
 stream>>res;
 return res;
}

// Выделяет дробную часть числа с точностью digs знаков после запятой
int fraction(double d, int digs);

// Разделяет строку на составлящие через сепаратор 'sep'
// Возвращает число полученных строк
template<typename CharT>
int separatestring(const basic_string<CharT> &str, vector<basic_string<CharT> > &output, CharT sep, int num=0, int *lastpos=0)
{
 typename basic_string<CharT>::size_type i=0,j=0;
 int size=0;
 int nnum=(num>0)?num-1:0;

 if(lastpos) *lastpos=0;
 output.resize(0);
 if(!str.size())
  return 0;

 while(i != string::npos && (nnum>=0) )
 {
  i=str.find_first_of(sep,j);
  if(i == j)
  {
   j++;
   continue;
  }

  ++size;
  output.resize(size);
  if(num)
   nnum--;
  if(i == string::npos)
   output[size-1]=str.substr(j);
  else
   output[size-1]=str.substr(j,i-j);
  j=i+1; 
  if(j >= str.size())
   break;
 }

 if(lastpos) *lastpos=i;

 return size;
}

/*
// Конвертация string<->wstring
// Копипаста с http://habrahabr.ru/blogs/cpp/112997/
//@brief Сужает широкую строку, используя локализацию loc
//   @return Возвращает суженную строку или пустую суженную строку, в
//   случае. если возникла ошибка
std::string narrow(const std::wstring& wstr, const std::locale& loc)
{
  const size_t sz = wstr.length();
  if(sz == 0)
    return std::string();
  mbstate_t state = 0;
  char *cnext;
  const wchar_t *wnext;
  const wchar_t *wcstr = wstr.c_str();
  char *buffer = new char[sz + 1];
  std::uninitialized_fill(buffer, buffer + sz + 1, 0);
  typedef std::codecvt<wchar_t, char, mbstate_t> cvt;
  cvt::result res;
  res = std::use_facet<cvt>(loc).out(state, wcstr, wcstr + sz, wnext,
      buffer, buffer + sz, cnext);
  std::string result(buffer);
  if(res == cvt::error)
    return std::string();
  return result;
}

//@brief Расширяет строку, используя локализацию loc
//   @return Возвращает расширенную строку или пустую расширенную строку, в
//   случае, если возникла ошибка.
std::wstring widen(const std::string& str, const std::locale& loc)
{
  const size_t sz = str.length();
  if(sz == 0)
    return std::wstring();
  mbstate_t state = 0;
  const char *cnext;
  wchar_t *wnext;
  const char *cstr = str.c_str();
  wchar_t *buffer = new wchar_t[sz + 1];
  std::uninitialized_fill(buffer, buffer + sz + 1, 0);
  typedef std::codecvt<wchar_t, char, mbstate_t> cvt;
  cvt::result res;
  res = std::use_facet<cvt>(loc).in(state, cstr, cstr + sz, cnext,
      buffer, buffer + sz, wnext);
  std::wstring result(buffer);
  delete [] buffer;
  if(res == cvt::error)
    return std::wstring();
  return result;
}
    */

}
#endif



