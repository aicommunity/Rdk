/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2004.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UCONSOLEENGINE_H
#define UCONSOLEENGINE_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <cctype>

using namespace std;

namespace RDK {

class UConsoleEngine
{
protected: // �������� ���������
// ���������� � ������ �������
string VersionInfo;

// true ���� ��������� ����������� ������� ���� � ������ �������
bool QueryDateEnable;

// true ���� ��������� ����������� �������� ������� � ������ �������
bool QueryTimeEnable;

// true ���� ��������� ����������� ������ �������� ������� � ������ �������
bool QueryTimeSecondsEnable;

// true ���� ������� ������� ������������� � ��������
bool QueryCommandCaseSensitive;

// true ���� ��������� ������� ������������� � ��������
bool QueryParametersCaseSensitive;

// ������� ������ ������ �������
string ReportPrefix;

// ������� ������ ��������� �� ������
string ErrorPrefix;

protected: // �������� ������
// ��������� �����, ���������� ��������� ��������� �������
string QueryBuffer;

// ������ �������
string QueryString;

// �����, ���������� ����������� ������� �������
string QueryCommand;

// �����, ���������� ����������� ��������� ������ �������
list<string> QueryResult;

// ������ ��������� ����� �������
list<string> ConsoleList;

// ����� ������ �������
list<string> ResultBuffer;


// ������ ������
istream *InputStream;
ostream *OutputStream;

protected: // ����� ���������
// ����, ������������ �������� ������� ��� ���
// ���� ���������� � true, �� �������� ������� ����
// ���� ���������� � false, �� ������� ���� ����������� �� ��������� ��������
bool RunningState;

protected: // ��������� ����������
// ��������� ����� ��� ������� ������������ ���������
string MSGBuffer;

// ��������� �����
char temp_str[4096];

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UConsoleEngine(const string &versioninfo);
virtual ~UConsoleEngine(void);
// --------------------------

// --------------------------
// ������ ������� � �������� ����������
// --------------------------
public:
// ���������� � ������ �������
const string& GetVersionInfo(void) { return VersionInfo; };

// true ���� ��������� ����������� ������� ���� � ������ �������
bool GetQueryDateEnable(void) { return QueryDateEnable; };

// true ���� ��������� ����������� �������� ������� � ������ �������
bool GetQueryTimeEnable(void) { return QueryTimeEnable; };

// true ���� ��������� ����������� ������ �������� ������� � ������ �������
bool GetQueryTimeSecondsEnable(void) { return QueryTimeSecondsEnable; };

// true ���� ������� ������� ������������� � ��������
bool GetQueryCommandCaseSensitive(void) { return QueryCommandCaseSensitive; };

// true ���� ��������� ������� ������������� � ��������
bool GetQueryParametersCaseSensitive(void) { return QueryParametersCaseSensitive; };

// ������� ������ ������ �������
const string& GetReportPrefix(void) { return ReportPrefix; };

// ������� ������ ��������� �� ������
const string& GetErrorPrefix(void) { return ErrorPrefix; };
// --------------------------

// --------------------------
// ������ ������� � �������� ������
// --------------------------
// ��������� �����, ���������� ��������� ��������� �������
const string& GetQueryBuffer(void) { return QueryBuffer; };

// ������ ��������� ����� �������
const list<string>& GetConsoleList(void) { return ConsoleList; };

// ������ ������
istream* GetInputStream(void) { return InputStream; };
ostream* GetOutputStream(void) { return OutputStream; };
// --------------------------

// --------------------------
// ������ ������� � ������ ���������
// --------------------------
public:
bool GetRunningState(void) { return RunningState; };
// --------------------------

// --------------------------
// ������ ���������� ����������� �� �����
// --------------------------
public:
// ���������� ��������� �� ����� name;
virtual string GetParameter(string &name);

// ������������� �������� ��������� �� ����� name
virtual void SetParameter(string &name, string &value);

// ���������� ������ ���� ����������
virtual void GetParametersList(list<string> &parameterslist);
// --------------------------

// --------------------------
// ������������ ���������
// --------------------------
public:
// ���������� ������ �����������
virtual const string& GetWelcomeMSG(void);

// ���������� ���������� � ������ �������
virtual const string& GetVersionMSG(void);

// ���������� ������ ������
virtual const string& GetExitMSG(void);

// ���������� ������ ��������� � �������������� �������� �� ���������
virtual const string& GetSetDefaultOptionsMSG(void);

// ���������� ������ ��������� � ����������� �������
virtual const string& GetUnknownCommandMSG(void);

// ���������� ������ ��������� � ����������� ���������
virtual const string& GetUnknownParameterMSG(void);

// ���������� ������ ��������� �� ���������� ��������
virtual const string& GetParameterValueLostMSG(void);
// --------------------------

// --------------------------
// ������ ���������� ������
// ��� ������ ���������� � buffer ���������� ������
// --------------------------
protected:
// ������ ������ �������
virtual void CRun(void);

// ���������� ������ ����������� �������
virtual void CWelcome(void);

// ���������� ���������� � ������ �������
virtual void CVersion(void);

// ���������� ������ �������
virtual void CExit(void);

// ��������������� �������� �������� �� ���������
virtual void CSetDefaultOptions(void);

// ������������� ��� ���������� �������� ���������
virtual void CSetOptions(list<string> &queryresult);

// ������� �� ����������� �������
virtual void CUnknownCommand(void);
// --------------------------

// --------------------------
// ������ ����������� ����������
// --------------------------
protected:
// ����� ������������ �������������� ������ ������ �������
virtual void ParseQuery(string &query, string &command, list<string> &params);

// ����� ����������� ������, �������� �����������
// ����� ���������� ������ � ���������� ���������
virtual void Parser(const string &command, list<string> &params);

// ��������� � ���������� ������ �������
virtual const string& CalcQueryString(void);

// ����������� � ���������� ��������� �� ������ error
virtual const string& CalcErrorMessage(string error);

// ������ �� ���� �������
virtual void Query(list<string> &buffer);

// ���������� ���������� ������ ������� �� ������ buffer
// ����� ���������� ����� buffer ����� ����
virtual void Show(list<string> &buffer, bool isquery=false);

// ���������� ����������
virtual void ProcessingException(void);
// --------------------------

// --------------------------
// ������ �������� ����������
// --------------------------
public:
// ����� �������������� ������ �������
virtual void Run(void);
// --------------------------
};


}
#endif


