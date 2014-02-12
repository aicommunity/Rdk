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

// ��������� �����
//string ustrbuf;
//wstring uwstrbuf;
// --------------------------------------------------



// �������� ������� ����� ����� � ��������� digs ������ ����� �������
int fraction(double d, int digs)
{
 d-=(int)d;
 d*=pow(10.0,digs);

 return int(d);
}

// ���������� ����� � ���� �������� ������ ���� YYYY.MM.DD HH:MM:SS
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
/*
// ���������� ����� � ���� �������� ������ ���� YYYY/MM/DD HH:MM:SS,MS
std::string get_text_current_time(char date_sep, char time_sep, char m_sec_sep, std::string additional_line)
{
 std::string result;
 unsigned short year, month, day;
 unsigned short hour, min, sec, msec;

 TDateTime  time_data=TDateTime::CurrentDateTime();
 time_data.DecodeDate(&year, &month, &day);
 time_data.DecodeTime(&hour, &min, &sec, &msec);

 result+=sntoa(year, 4);
 result+=date_sep;
 result+=sntoa(month,2);
 result+=date_sep;
 result+=sntoa(day,2);
 result+=" ";

 result+=sntoa(hour, 2);
 result+=time_sep;
 result+=sntoa(min,2);
 result+=time_sep;
 result+=sntoa(sec,2);
 result+=m_sec_sep;
 result+=sntoa(msec,2);
 result+=" ";
 result+=additional_line;

 return result;
}*/

// ����������� string<->wstring
// ��������� � http://habrahabr.ru/blogs/cpp/112997/
//@brief ������ ������� ������, ��������� ����������� loc
//   @return ���������� �������� ������ ��� ������ �������� ������, �
//   ������. ���� �������� ������
std::string& narrow(const std::wstring& wstr, const std::locale& loc, std::string &result)
{
  const size_t sz = wstr.length();
  if(sz == 0)
  {
   result.resize(0);
   return result;
  }
  mbstate_t state;
  memset(&state,0,sizeof(state));
  char *cnext;
  const wchar_t *wnext;
  const wchar_t *wcstr = wstr.c_str();
  result.resize(sz);
  std::uninitialized_fill(&result[0], &result[0] + sz + 1, 0);
  typedef std::codecvt<wchar_t, char, mbstate_t> cvt;
  cvt::result res;
  res = std::use_facet<cvt>(loc).out(state, wcstr, wcstr + sz, wnext,
	  &result[0], &result[0] + sz, cnext);
  if(res == cvt::error)
   result.resize(0);
  return result;
}

//std::locale my_locale("");
				 /*
std::string narrow2(const std::wstring& wstr)
{
// static std::locale my_locale("");
// std::locale cp866(std::locale(), new codecvt_cp866);
 return narrow(wstr,my_locale);
}       */

//@brief ��������� ������, ��������� ����������� loc
//   @return ���������� ����������� ������ ��� ������ ����������� ������, �
//   ������, ���� �������� ������.
std::wstring& widen(const std::string& str, const std::locale& loc, std::wstring &result)
{
  const size_t sz = str.length();
  if(sz == 0)
  {
   result.resize(0);
   return result;
  }
  mbstate_t state;
  memset(&state,0,sizeof(state));
  const char *cnext;
  wchar_t *wnext;
  const char *cstr = str.c_str();
  result.resize(sz);// = new wchar_t[sz + 1];
  std::uninitialized_fill(&result[0], &result[0] + sz + 1, 0);
  typedef std::codecvt<wchar_t, char, mbstate_t> cvt;
  cvt::result res;
  res = std::use_facet<cvt>(loc).in(state, cstr, cstr + sz, cnext,
      &result[0], &result[0] + sz, wnext);
  //std::wstring result(buffer);
//  delete [] buffer;
  if(res == cvt::error)
   result.resize(0); //return std::wstring();
  return result;
}
				/*
std::wstring widen2(const std::string& str)
{
// static std::locale my_locale("");
 return widen(str,my_locale);
}                 */


}
#endif



