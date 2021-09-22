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
#include <string.h>
//#include <mem.h>
#include <fstream>

#include "USupport.h"
//#ifdef __BORLANDC__
//extern std::wstring s2ws(const std::string& s);
//extern std::string ws2s(const std::wstring& s);
//#endif

//#include "UPtr.cpp"

namespace RDK {

// Системный буфер
//string ustrbuf;
//wstring uwstrbuf;
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
 tm time_result;
 tm* time_struct(0);
#if defined(_MSC_VER)
 localtime_s(&time_result,&time_data);
 time_struct=&time_result;
#elif defined(__STDC_LIB_EXT1__)
 time_struct=localtime_s(&time_data,&time_result);
#else
 time_struct=localtime(&time_data); // TODO: Possible unsafe!!
 memcpy(&time_result,time_struct,sizeof(time_result));
#endif


 result+=sntoa(time_struct->tm_year+1900,4);
 result+=date_sep;
 result+=sntoa(time_struct->tm_mon+1,2);
 result+=date_sep;
 result+=sntoa(time_struct->tm_mday,2);
 result+=" ";

 result+=sntoa(time_struct->tm_hour,2);
 result+=time_sep;
 result+=sntoa(time_struct->tm_min,2);
 result+=time_sep;
 result+=sntoa(time_struct->tm_sec,2);
 return result;
}

/// Возвращает время в виде понятной строки вида YYYYy MMm DDd HHh MMm SS:MSMSs из времени в секундах
/// отображает только те элементы времени, которые необходимы
std::string get_text_time_from_seconds(double time_data, char date_sep, char time_sep, bool is_full_time)
{
 std::string result;

 int secs(0),mins(0),hours(0),days(0),mons(0),years(0),msecs(0);
 if(time_data<1e-6)
 {
  if(is_full_time)
      return "00:00:00.000";
  else
   return "00.000";
 }
 years=int(time_data/(365*86400.0));
 time_data-=years*(365*86400.0);
 mons=int(time_data/(30*86400.0));
 time_data-=mons*(30*86400.0);
 days=int(time_data/86400.0);
 time_data-=days*86400.0;
 hours=int(time_data/3600.0);
 time_data-=hours*3600.0;
 mins=int(time_data/60.0);
 time_data-=mins*60.0;
 secs=int(time_data);
 time_data-=secs;
 msecs=int(time_data*1000);

 if(years)
  result+=sntoa(years)+date_sep;

 if(mons || years)
  result+=sntoa(mons,2)+date_sep;

 if(days || mons || years)
  result+=sntoa(days,2);

 if(!result.empty())
  result+=" ";

 if(is_full_time || (days || mons || years || hours))
  result+=sntoa(hours,2)+time_sep;

 if(is_full_time || (days || mons || years || mins || hours))
  result+=sntoa(mins,2)+time_sep;

 result+=sntoa(secs,2)+"."+sntoa(msecs,3);
 return result;
}


/*
// Возвращает время в виде понятной строки вида YYYY/MM/DD HH:MM:SS,MS
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
	  /*
#ifndef __BORLANDC__
// Конвертация string<->wstring
// Копипаста с http://habrahabr.ru/blogs/cpp/112997/
//@brief Сужает широкую строку, используя локализацию loc
//   @return Возвращает суженную строку или пустую суженную строку, в
//   случае. если возникла ошибка
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

//@brief Расширяет строку, используя локализацию loc
//   @return Возвращает расширенную строку или пустую расширенную строку, в
//   случае, если возникла ошибка.
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

#else

// Конвертация string<->wstring
// Игнорирует loc!
std::string& narrow(const std::wstring& wstr, const std::locale& loc, std::string &result)
{
  result=ws2s(wstr);
  return result;
}

//@brief Расширяет строку
// Игнорирует loc!
std::wstring& widen(const std::string& str, const std::locale& loc, std::wstring &result)
{
  result=s2ws(str);
  return result;
}

#endif
       */

/// Обрезает лидирующие и завершающие пробелы в строке
RDK_LIB_TYPE std::string trim_spaces(const std::string &str)
{
 std::string::size_type first_sym=str.find_first_not_of(" \r\n\t");
 std::string::size_type last_sym=str.find_last_not_of(" \r\n\t");
 if(first_sym == std::string::npos || last_sym == std::string::npos)
  return "";
 return str.substr(first_sym,last_sym-first_sym+1);
}

/// Извлекает путь из полного имени файла
std::string extract_file_path(const std::string& full_name)
{
	 size_t pos = full_name.find_last_of("\\/");
	 return (std::string::npos == pos)
		 ? std::string("")
		 : full_name.substr(0, pos+1);
}


/// Извлекает имя файла из полного имени файла
std::string extract_file_name(const std::string& full_name)
{
	 size_t pos = full_name.find_last_of("\\/");
	 return (std::string::npos == pos)
           ? full_name
		 : full_name.substr(pos+1);
}

/// Извлекает имя файла из полного имени файла исключая расширение
std::string extract_file_name_wo_ext(const std::string& full_name)
{
     std::string file_name=extract_file_name(full_name);

     size_t pos = file_name.find_last_of(".");
     return (std::string::npos == pos)
         ? file_name
         : file_name.substr(0,pos);
}

/// Извлекает расширение файла из имени файла
RDK_LIB_TYPE std::string extract_file_ext(const std::string& full_name)
{
	 size_t pos = full_name.find_last_of(".");
	 return (std::string::npos == pos)
		 ? std::string("")
		 : full_name.substr(pos+1);
}

/// Модифицирует имя файла так, чтобы оно стало относительным относительно заданного пути
RDK_LIB_TYPE string relative_file_path(const string &path, const string &relative_path_base)
{
	return replace_substring(path, relative_path_base, ".\\");
}


/// Заменяет все вхождения подстроки find_str на подстроку replace_str
RDK_LIB_TYPE string replace_substring(const string &src, const string &find_str, const string &replace_str )
{
 std::string result(src);
 string::size_type i = 0;

 while ( ( i = result.find( find_str, i ) ) != result.npos )
 {
  result.replace( i, find_str.length(), replace_str);
  i+=replace_str.length();
 }

 return result;
}


/// Загружает файл в строку
RDK_LIB_TYPE bool LoadFile(const std::string &file_name, std::string &buffer)
{
 std::ifstream t(file_name.c_str(), ios::in);

 if(!t || t.fail() || t.bad())
 {
  buffer.clear();
  return false;
 }

 t.seekg(0, std::ios::end);
 if(t.fail() || t.bad())
  return false;
 buffer.reserve(t.tellg());
 t.seekg(0, std::ios::beg);
 if(t.fail() || t.bad())
  return false;

 buffer.assign((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
 if(t.fail() || t.bad())
  return false;
 return true;
}

bool LoadFileBin(const std::string &file_name, std::vector<uint8_t> &buffer)
{
 std::ifstream t(file_name.c_str(), ios::in | ios::binary);

 if(!t || t.fail() || t.bad())
 {
  buffer.clear();
  return false;
 }

 t.seekg(0, std::ios::end);
 if(t.fail() || t.bad())
  return false;
 buffer.reserve(t.tellg());
 t.seekg(0, std::ios::beg);
 if(t.fail() || t.bad())
  return false;

 buffer.assign((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
 if(t.fail() || t.bad())
  return false;
 return true;
}


/// Сохраняет файл из строки
RDK_LIB_TYPE bool SaveFile(const std::string &file_name, const std::string &buffer)
{
 std::ofstream t(file_name.c_str(), ios::trunc);

 if(!t || t.fail() || t.bad())
 {
  return false;
 }

 t<<buffer;
 if(t.fail() || t.bad())
  return false;
 return true;
}

bool SaveFileBin(const std::string &file_name, const std::vector<uint8_t> &buffer)
{
 std::ofstream t(file_name.c_str(), ios::trunc | ios::binary);

 if(!t || t.fail() || t.bad())
 {
  return false;
 }

 if(!buffer.empty())
  t.write(reinterpret_cast<const char *>(&buffer[0]),buffer.size());
 if(t.fail() || t.bad())
  return false;
 return true;
}


}
#endif



