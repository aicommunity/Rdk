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


}
#endif


 
