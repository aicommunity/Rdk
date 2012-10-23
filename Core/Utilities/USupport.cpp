/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef USUPPORT_CPP
#define USUPPORT_CPP


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <mem.h>

#include "USupport.h"
#include "UPtr.cpp"

namespace RDK {

// Системный буфер
string ustrbuf;
wstring uwstrbuf;
// --------------------------------------------------



// Выделяет дробную часть числа с точностью digs знаков после запятой
int fraction(double d, int digs)
{
 d-=(int)d;
 d*=pow(10.0,digs);

 return int(d);
}

// Возвращает время в виде понятной строки вида YYYY.MM.DD HH:MM:SS
std::string get_text_time(time_t time_data, char date_sep, char time_sep)
{
 std::string result;
 tm* time_stuct=localtime(&time_data);

 result+=sntoa(time_stuct->tm_year+1900,4);
 result+=date_sep;
 result+=sntoa(time_stuct->tm_mon+1,2);
 result+=date_sep;
 result+=sntoa(time_stuct->tm_mday,2);
 result+=" ";

 result+=sntoa(time_stuct->tm_hour,2);
 result+=time_sep;
 result+=sntoa(time_stuct->tm_min,2);
 result+=time_sep;
 result+=sntoa(time_stuct->tm_sec,2);
 return result;
}

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
  delete [] buffer;
  if(res == cvt::error)
	return std::string();
  return result;
}

std::string narrow2(const std::wstring& wstr)
{
 static std::locale my_locale("");
// std::locale cp866(std::locale(), new codecvt_cp866);
 return narrow(wstr,my_locale);
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

std::wstring widen2(const std::string& str)
{
 static std::locale my_locale("");
 return widen(str,my_locale);
}


}
#endif


 
