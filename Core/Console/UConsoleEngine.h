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
protected: // Основные параметры
// Информация о версии консоли
string VersionInfo;

// true если разрешено отображение текущей даты в строке запроса
bool QueryDateEnable;

// true если разрешено отображение текущего времени в строке запроса
bool QueryTimeEnable;

// true если разрешено отображение секунд текущего времени в строке запроса
bool QueryTimeSecondsEnable;

// true если команда запроса чувствительна к регистру
bool QueryCommandCaseSensitive;

// true если параметры запроса чувствительны к регистру
bool QueryParametersCaseSensitive;

// Префикс строки ответа консоли
string ReportPrefix;

// Префикс строки сообщения об ошибке
string ErrorPrefix;

protected: // Основные данные
// Строковой буфер, содержащий последнюю введенную команду
string QueryBuffer;

// Строка запроса
string QueryString;

// Буфер, содержащий разобранную команду запроса
string QueryCommand;

// Буфер, содержащий разобранную последнюю строку запроса
list<string> QueryResult;

// Список текстовых строк консоли
list<string> ConsoleList;

// Буфер вывода консоли
list<string> ResultBuffer;


// Потоки данных
istream *InputStream;
ostream *OutputStream;

protected: // Флаги состояния
// Флаг, определяющий запущена консоль или нет
// Если установлен в true, то работает главный цикл
// Если установлен в false, то главный цикл завершается на следующей итерации
bool RunningState;

protected: // Временные переменные
// Строковой буфер для расчета интерфейсных сообщений
string MSGBuffer;

// Строковой буфер
char temp_str[4096];

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UConsoleEngine(const string &versioninfo);
virtual ~UConsoleEngine(void);
// --------------------------

// --------------------------
// Методы доступа к основным параметрам
// --------------------------
public:
// Информация о версии консоли
const string& GetVersionInfo(void) { return VersionInfo; };

// true если разрешено отображение текущей даты в строке запроса
bool GetQueryDateEnable(void) { return QueryDateEnable; };

// true если разрешено отображение текущего времени в строке запроса
bool GetQueryTimeEnable(void) { return QueryTimeEnable; };

// true если разрешено отображение секунд текущего времени в строке запроса
bool GetQueryTimeSecondsEnable(void) { return QueryTimeSecondsEnable; };

// true если команда запроса чувствительна к регистру
bool GetQueryCommandCaseSensitive(void) { return QueryCommandCaseSensitive; };

// true если параметры запроса чувствительны к регистру
bool GetQueryParametersCaseSensitive(void) { return QueryParametersCaseSensitive; };

// Префикс строки ответа консоли
const string& GetReportPrefix(void) { return ReportPrefix; };

// Префикс строки сообщения об ошибке
const string& GetErrorPrefix(void) { return ErrorPrefix; };
// --------------------------

// --------------------------
// Методы доступа к основным данным
// --------------------------
// Строковой буфер, содержащий последнюю введенную команду
const string& GetQueryBuffer(void) { return QueryBuffer; };

// Список текстовых строк консоли
const list<string>& GetConsoleList(void) { return ConsoleList; };

// Потоки данных
istream* GetInputStream(void) { return InputStream; };
ostream* GetOutputStream(void) { return OutputStream; };
// --------------------------

// --------------------------
// Методы доступа к флагам состояния
// --------------------------
public:
bool GetRunningState(void) { return RunningState; };
// --------------------------

// --------------------------
// Методы управления параметрами по имени
// --------------------------
public:
// Возвращает параметер по имени name;
virtual string GetParameter(string &name);

// Устанавливает значение параметра по имени name
virtual void SetParameter(string &name, string &value);

// Возвращает список имен параметров
virtual void GetParametersList(list<string> &parameterslist);
// --------------------------

// --------------------------
// Интерфейсные сообщения
// --------------------------
public:
// Возвращает строку приветствия
virtual const string& GetWelcomeMSG(void);

// Возвращает информацию о версии консоли
virtual const string& GetVersionMSG(void);

// Возвращает строку выхода
virtual const string& GetExitMSG(void);

// Возвращает строку сообщения о восстановлении настроек по умолчанию
virtual const string& GetSetDefaultOptionsMSG(void);

// Возвращает строку сообщения о неизвестной команде
virtual const string& GetUnknownCommandMSG(void);

// Возвращает строку сообщения о неизвестном параметре
virtual const string& GetUnknownParameterMSG(void);

// Возвращает строку сообщения об отсутствия значения
virtual const string& GetParameterValueLostMSG(void);
// --------------------------

// --------------------------
// Методы выполнения команд
// Все методы записывают в buffer результаты работы
// --------------------------
protected:
// Запуск работы консоли
virtual void CRun(void);

// Возвращает строку приветствия консоли
virtual void CWelcome(void);

// Возвращает информацию о версии консоли
virtual void CVersion(void);

// Завершение работы консоли
virtual void CExit(void);

// Восстанавливает значения настроек по умолчанию
virtual void CSetDefaultOptions(void);

// Устанавливает или возвращает значение настройки
virtual void CSetOptions(list<string> &queryresult);

// Реакция на неизвестную команду
virtual void CUnknownCommand(void);
// --------------------------

// --------------------------
// Методы внутреннего управления
// --------------------------
protected:
// Метод осуществляет синтаксический разбор строки запроса
virtual void ParseQuery(string &query, string &command, list<string> &params);

// Метод анализирует запрос, вызывает необходимый
// метод выполнения команд и возвращает результат
virtual void Parser(const string &command, list<string> &params);

// Формирует и возвращает строку запроса
virtual const string& CalcQueryString(void);

// Форматирует и возвращает сообщение об ошибке error
virtual const string& CalcErrorMessage(string error);

// Запрос на ввод команды
virtual void Query(list<string> &buffer);

// Отображает результаты работы консоли из буфера buffer
// После выполнения буфер buffer будет пуст
virtual void Show(list<string> &buffer, bool isquery=false);

// Обработчик исключений
virtual void ProcessingException(void);
// --------------------------

// --------------------------
// Методы внешнего управления
// --------------------------
public:
// Метод осуществляющий запуск консоли
virtual void Run(void);
// --------------------------
};


}
#endif


