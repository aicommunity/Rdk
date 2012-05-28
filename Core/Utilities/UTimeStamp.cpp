/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2005.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru
Version:        3.0.0

This file - part of the project: RDK

File License:		BSD License
Project License:	BSD License
See file license.txt for more information
*********************************************************** */


#include "UTimeStamp.h"
#include "USupport.h"

namespace RDK {

// Методы UTimeStamp
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UTimeStamp::UTimeStamp(void)
 : Hours(0), Minutes(0), Seconds(0), Frames(0), FPS(25)
{
}

UTimeStamp::UTimeStamp(const UTimeStamp &copy)
{
 *this=copy;
}

UTimeStamp::UTimeStamp(double seconds, double fps)
 : Hours(0), Minutes(0), Seconds(0), Frames(0), FPS(25)
{
 FPS=fps;

 *this=seconds;
}

UTimeStamp::UTimeStamp(long frames, double fps)
 : Hours(0), Minutes(0), Seconds(0), Frames(0), FPS(25)
{
 FPS=fps;

 if(FPS)
  *this=double(frames)/FPS;
}

UTimeStamp::~UTimeStamp(void)
{
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UTimeStamp& UTimeStamp::operator = (const UTimeStamp &copy)
{
 Hours=copy.Hours;
 Minutes=copy.Minutes;
 Seconds=copy.Seconds;
 Frames=copy.Frames;
 FPS=copy.FPS;

 return *this;
}

UTimeStamp& UTimeStamp::operator = (double seconds)
{
 double sec=(seconds<0)?-seconds:seconds;
 Hours=int(sec/3600);
 Minutes=(unsigned char)((sec-Hours*3600)/60);
 Seconds=(unsigned char)(sec-Hours*3600-Minutes*60);
 Frames=(unsigned char)((double(sec)-double(Hours*3600)-double(Minutes*60)-double(Seconds))*FPS);
 if(seconds<0)
  Hours=-Hours;

 return *this;
}

// Арифметические операторы
UTimeStamp& UTimeStamp::operator -= (const UTimeStamp &copy)
{
 double sec=(*this)()-copy();
 *this=sec;

 return *this;
}

UTimeStamp& UTimeStamp::operator -= (double seconds)
{
 double sec=(*this)()-seconds;
 *this=sec;

 return *this;
}

UTimeStamp operator - (const UTimeStamp &copy1,const UTimeStamp &copy2)
{
 UTimeStamp res=copy1;
 res-=copy2;

 return res;
}

UTimeStamp operator - (const UTimeStamp &copy,double seconds)
{
 UTimeStamp res=copy;
 res-=seconds;

 return res;
}

UTimeStamp operator - (double seconds,const UTimeStamp &copy)
{
 double sec=seconds-copy();
 UTimeStamp res(sec,copy.FPS);

 return res;
}

UTimeStamp& UTimeStamp::operator += (const UTimeStamp &copy)
{
 double sec=(*this)()+copy();
 *this=sec;

 return *this;
}

UTimeStamp& UTimeStamp::operator += (double seconds)
{
 double sec=(*this)()+seconds;
 *this=sec;

 return *this;
}

UTimeStamp operator + (const UTimeStamp &copy1,const UTimeStamp &copy2)
{
 UTimeStamp res=copy1;
 res+=copy2;

 return res;
}

UTimeStamp operator + (const UTimeStamp &copy,double seconds)
{
 UTimeStamp res=copy;
 res+=seconds;
 return res;
}

UTimeStamp operator + (double seconds,const UTimeStamp &copy)
{
 UTimeStamp res(seconds+copy(),copy.FPS);
 return res;
}

// Оператор преобразования в секунды
double UTimeStamp::operator() (void) const
{
 return (double(abs(Hours))*3600.0+double(Minutes)*60.0+double(Seconds)
		+double(Frames)/double(FPS))*((Hours<0)?-1:1);
}


bool UTimeStamp::operator == (const UTimeStamp &copy)
{
 return ((Hours==copy.Hours) & (Minutes==copy.Minutes)
		& (Seconds==copy.Seconds) & (Frames==copy.Frames));
}

bool UTimeStamp::operator != (const UTimeStamp &copy)
{
 return !((*this)==copy);
}

bool UTimeStamp::operator < (const UTimeStamp &copy)
{
 if(Hours<copy.Hours) return true;
 if(Hours>copy.Hours) return false;

 if(Minutes<copy.Minutes) return true;
 if(Minutes>copy.Minutes) return false;

 if(Seconds<copy.Seconds) return true;
 if(Seconds>copy.Seconds) return false;

 if(Frames<copy.Frames) return true;
 if(Frames>copy.Frames) return false;

 return false;
}

bool UTimeStamp::operator > (const UTimeStamp &copy)
{
 if((*this) == copy)
  return false;

 return !((*this)<copy);
}


// Операторы ввода вывода в строку
// Разделитель ':'
string& UTimeStamp::operator >> (string &str) const
{
 str=sntoa(Hours,3);
 str+=':';
 str+=sntoa(int(Minutes),2);
 str+=':';
 str+=sntoa(int(Seconds),2);
 str+=':';
 str+=sntoa(int(Frames),2);

 return str;
};

UTimeStamp& UTimeStamp::operator << (const string &str)
{
 vector<string> seps;

 separatestring(str,seps,':');

 if(seps.size() != 4)
  return *this;

 Hours=RDK::atoi(seps[0]);
 Minutes=RDK::atoi(seps[1]);
 Seconds=RDK::atoi(seps[2]);
 Frames=RDK::atoi(seps[3]);

 return *this;
}
// --------------------------
//---------------------------------------------------------------------------
}

