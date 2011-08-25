/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2004.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UCONSOLEENGINE_CPP
#define UCONSOLEENGINE_CPP

#include <locale>
#include "../Utilities/USupport.h"

#include "UConsoleEngine.h"

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
UConsoleEngine::UConsoleEngine(const string &versioninfo)
{
 VersionInfo=versioninfo;
 QueryCommandCaseSensitive=true;
 QueryParametersCaseSensitive=true;
 InputStream=&cin;
 OutputStream=&cout;
}

UConsoleEngine::~UConsoleEngine(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� ����������� �� �����
// --------------------------
// ���������� ��������� �� ����� name;
string UConsoleEngine::GetParameter(string &name)
{
 if(name == "QueryDateEnable")
  return string(sntoa((int)QueryDateEnable));

 if(name == "QueryTimeEnable")
  return string(sntoa((int)QueryTimeEnable));

 if(name == "QueryTimeSecondsEnable")
  return string(sntoa((int)QueryTimeSecondsEnable));

 if(name == "ReportPrefix")
  return ReportPrefix;

 if(name == "ErrorPrefix")
  return ErrorPrefix;

 throw "unknownparam";
}

// ������������� �������� ��������� �� ����� name
void UConsoleEngine::SetParameter(string &name, string &value)
{
 if(name == "QueryDateEnable")
  { QueryDateEnable=atoi(value); return; }

 if(name == "QueryTimeEnable")
  { QueryTimeEnable=atoi(value); return; };

 if(name == "QueryTimeSecondsEnable")
  { QueryTimeSecondsEnable=atoi(value); return; }

 if(name == "ReportPrefix")
  { ReportPrefix=value; return; }

 if(name == "ErrorPrefix")
  { ErrorPrefix=value; return; }

 throw "unknownparam";
}

// ���������� ������ ���� ����������
void UConsoleEngine::GetParametersList(list<string> &parameterslist)
{
 parameterslist.push_back("QueryDateEnable");
 parameterslist.push_back("QueryTimeEnable");
 parameterslist.push_back("QueryTimeSecondsEnable");
 parameterslist.push_back("ReportPrefix");
 parameterslist.push_back("ErrorPrefix");
}
// --------------------------

// --------------------------
// ������������ ���������
// --------------------------
// ���������� ������ �����������
const string& UConsoleEngine::GetWelcomeMSG(void)
{
 return MSGBuffer="Welcome to console engine";
}

// ���������� ���������� � ������ �������
const string& UConsoleEngine::GetVersionMSG(void)
{
 return MSGBuffer=VersionInfo;
}

// ���������� ������ ������
const string& UConsoleEngine::GetExitMSG(void)
{
 return MSGBuffer="...console closed.";
}

// ���������� ������ ��������� � �������������� �������� �� ���������
const string& UConsoleEngine::GetSetDefaultOptionsMSG(void)
{
 return MSGBuffer="Default options restored";
}

// ���������� ������ ��������� � ����������� �������
const string& UConsoleEngine::GetUnknownCommandMSG(void)
{
 return CalcErrorMessage(string("unknown command"));
}

// ���������� ������ ��������� � ����������� ���������
const string& UConsoleEngine::GetUnknownParameterMSG(void)
{
 return CalcErrorMessage(string("unknown parameter"));
}

// ���������� ������ ��������� �� ���������� ��������
const string& UConsoleEngine::GetParameterValueLostMSG(void)
{
 return CalcErrorMessage(string("parameter value lost"));
}
// --------------------------

// --------------------------
// ������ ���������� ������
// ��� ������ ���������� � buffer ���������� ������
// --------------------------
// ������ ������ �������
void UConsoleEngine::CRun(void)
{
 if(RunningState)
  return;
 CWelcome();
 CVersion();
 CSetDefaultOptions();
 RunningState=true;
}

// ���������� ������ ����������� �������
void UConsoleEngine::CWelcome(void)
{
 ResultBuffer.push_back(GetWelcomeMSG());
}

// ���������� ���������� � ������ �������
void UConsoleEngine::CVersion(void)
{
 ResultBuffer.push_back(GetVersionMSG());
}

// ���������� ������ �������
void UConsoleEngine::CExit(void)
{
 RunningState=false;
 ResultBuffer.push_back(GetExitMSG());
}

// ��������������� �������� �������� �� ���������
void UConsoleEngine::CSetDefaultOptions(void)
{
 QueryDateEnable=false;
 QueryTimeEnable=true;
 QueryTimeSecondsEnable=false;
 ReportPrefix=" ";
 ErrorPrefix= "Error: ";

 ResultBuffer.push_back(GetSetDefaultOptionsMSG());
}

// ������������� ��� ���������� �������� ���������
void UConsoleEngine::CSetOptions(list<string> &queryresult)
{
 list<string>::iterator I=queryresult.begin();
 string name, value;
 ++I;
 try
  {
   if(queryresult.size() == 1)
    {
      ResultBuffer.push_back(GetUnknownParameterMSG());
      return;
    }

   if(queryresult.size() == 2)
    {
     value=GetParameter(*I);
     ResultBuffer.push_back(*I+string(" = ")+value);
     return;
    }

   if(queryresult.size() == 3)
    {
     name=*I;
     ++I;
     SetParameter(name,*I);
     ResultBuffer.push_back(string("OK"));
     return;
    }

   ResultBuffer.push_back(GetParameterValueLostMSG());
  }
 catch(const char *s)
  {
   if(!strcmp(s,"unknownparam"))
    {
      ResultBuffer.push_back(GetUnknownParameterMSG());
    }
   else
    throw;
  } 
}

// ������� �� ����������� �������
void UConsoleEngine::CUnknownCommand(void)
{
 ResultBuffer.push_back(GetUnknownCommandMSG());
}
// --------------------------

// --------------------------
// ������ ����������� ����������
// --------------------------
// ����� ������������ �������������� ������ ������ �������
void UConsoleEngine::ParseQuery(string &query, string &command, list<string> &params)
{
 int left, right;
 int i=0,j;
 const char *p=query.c_str();
 locale loc;
 unsigned int size;
 string *s;

 params.clear();

 left=0;

 right=query.size()-1;

 while(right >= 0 && isspace(*(p+right))) --right;
 if(right < 0)
  return;

 while(i<=right)
  {
   while(left <= right && isspace(*(p+left))) ++left;
   i=left;
   while(i <= right && !isspace(*(p+i))) ++i;
   params.push_back(string(p+left,i-left));
   if(!QueryCommandCaseSensitive && params.size() == 1)
    {
     s=&params.back();
     size=s->size();
     for(i=0;i<=(int)size;++i)
      (*s)[i]=tolower((*s)[i]);
    }
   if(!QueryParametersCaseSensitive && params.size() != 1)
    {
     s=&params.back();
     size=s->size();
     for(i=0;i<=(int)size;++i)
      (*s)[i]=tolower((*s)[i]);
    }

   if(i==right)
    return;
   left=i+1;
  }
  command=params.front();
  params.pop_front();
}

// ����� ����������� ������, �������� �����������
// ����� ���������� ������ � ���������� ���������
void UConsoleEngine::Parser(const string &command, list<string> &params)
{
 if(command == "run")
  {
   CRun();
   return;
  }

 if(command == "welcome")
  {
   CWelcome();
   return;
  }

 if(command == "ver")
  {
   CVersion();
   return;
  }

 if(command == "exit")
  {
   CExit();
   return;
  }

 if(command == "setd")
  {
   CSetDefaultOptions();
   return;
  }

 if(command == "set")
  {
   CSetOptions(params);
   return;
  }

 CUnknownCommand();
}

// ��������� � ���������� ������ �������
const string& UConsoleEngine::CalcQueryString(void)
{
 time_t timer;
 tm *currtime;
 char buf[25];

 time(&timer);
 currtime=localtime(&timer);

 QueryString="";
 if(QueryDateEnable)
  {
   *buf='\0';
   sprintf(buf,"%.4i-%.2i-%.2i",1900+currtime->tm_year,currtime->tm_mon,currtime->tm_mday);
   QueryString.append(buf);
  }
 if(QueryTimeEnable)
  {
   if(QueryString.size() > 0)
    QueryString.append(" ",1);
   *buf='\0';
   sprintf(buf,"%.2i:%.2i",currtime->tm_hour,currtime->tm_min);
   QueryString.append(buf);
   *buf='\0';
   if(QueryTimeSecondsEnable)
    sprintf(buf,":%.2i",currtime->tm_sec);
   QueryString.append(buf);
  }
 QueryString.append(">",1);

 return QueryString;
}

// ����������� � ���������� ��������� �� ������ error
const string& UConsoleEngine::CalcErrorMessage(string error)
{
 return MSGBuffer=ErrorPrefix+error;
}

// ������ �� ���� �������
void UConsoleEngine::Query(list<string> &buffer)
{
 *OutputStream<<CalcQueryString();
 getline(*InputStream,QueryBuffer,'\n');
 buffer.push_back(QueryString+QueryBuffer);
}

// ���������� ���������� ������ ������� �� ������ buffer
// ����� ���������� ����� buffer ����� ����
void UConsoleEngine::Show(list<string> &buffer, bool isquery)
{
 list<string>::iterator I=buffer.begin();

 if(!isquery)
  while(I != buffer.end())
   {
    *OutputStream<<ReportPrefix<<*I<<"\n";
    ++I;
   }
 ConsoleList.splice(ConsoleList.end(), buffer, buffer.begin(), buffer.end());
}

// ���������� ����������
void UConsoleEngine::ProcessingException(void)
{
 throw;
}
// --------------------------

// --------------------------
// ������ �������� ����������
// --------------------------
// �����, �������������� ������ �������
void UConsoleEngine::Run(void)
{
 bool startenable=true;
 list<string> buffer;

 QueryBuffer="run";
 while(!RunningState && startenable)
  {
   try
    {
     ParseQuery(QueryBuffer, QueryCommand, QueryResult);
     Parser(QueryCommand, QueryResult);
     Show(ResultBuffer);
    }
   catch(...)
    {
     startenable=false;
     ProcessingException();
    }
  }  

 while(RunningState)
  {
   try
    {
     Query(buffer);
     Show(buffer,true);
     ParseQuery(QueryBuffer, QueryCommand, QueryResult);
     Parser(QueryCommand, QueryResult);
     Show(ResultBuffer);
    }
   catch(...)
    {
     ProcessingException();
    }
  }
}
// --------------------------

}
#endif


