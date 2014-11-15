/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UAENVIRONMENT_H
#define UAENVIRONMENT_H

#include "UStorage.h"
#include "ULibrary.h"
#include "../System/rdk_system.h"

namespace RDK {

class RDK_LIB_TYPE UEnvironment: virtual public UModule
{
public:
// �������� ������� ��������� ������ ����������� ����������
typedef void (*PExceptionHandler)(int channel_index);

protected: // ���������
// ������ ������������� �������� ������ ���������
// 0 - ��������� ������������ �����
int PredefinedStructure;

// ��� �������� �������� �������� ������
std::string CurrentDataDir;

// ��� �������� �������� ������
std::string SystemDir;

/// ����������� �������� ������� ����� ���������� ����� (��)
ULongTime MinInterstepsInterval;

/// ���� ��������� ������ �������
bool DebugMode;

protected: // ���������
// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool StoragePresent;

// ������� �������� �������������
bool Initialized;

// ������� ������� �������������� ���������
bool Structured;

// ������������� ���������� ������, ������� ����� �������������
ULongId ModelCalculationComponent;

protected: // ������
// ���������
UStorage* Storage;

// ����������� ������
UEPtr<UContainer> Model;

/// ����� �����
UTimeControl Time;

// ������������ ��������� �������, ������� ������ �������� "�������" � ������
// ������� ��������� ������� (��)
double MaxModelDuration;

// ������� ���������� ����������
PExceptionHandler ExceptionHandler;

// ������� ����� ���������� �������
mutable int CurrentExceptionsLogSize;

// ������������ ����� �������� ����������
// ���� 0, �� �������������
int MaxExceptionsLogSize;

// ������ ���������� ���������� ������� ���� ����������
unsigned LastReadExceptionLogIndex;

// ������������ ������� ������������� ��������� � ����
mutable int LastErrorLevel;

/// ������ �������� ������ � �������������� ����������
int ChannelIndex;

/// ��� ���������� ������ �������� ����� ������������ ������ � ���� � ������
/// ��������� ������
std::string SourceControllerName;

/// ��� ���������� ��������� ���������� ������ �������� ����� ������������
/// ������ � ���� � ������ ��������� ������
std::string SourceControllerProperty;

protected: // �����������

protected: // ������ ������������ ��������� ������������


protected: // ���������� �������� �������
// ������� ��������� ������
UEPtr<UComponent> CurrentComponent;

protected: // ��������� ����������
unsigned long long StartupTime;

unsigned long long /*StartProcTime,*/CurrentTime,LastDuration/*,LastSentTime*/, ProcEndTime, LastStepStartTime;

// ��������� ��������� ������ ��� ����
//mutable string TempLogString;

// ��������� ��������� �����
mutable string TempString;

mutable std::map<unsigned, pair<std::string, int> > LogList;

/// ������ ��������� ������ ����
mutable unsigned LogIndex;

UGenericMutex* LogMutex;


public: // Public methods
// --------------------------
// Constructors & destructors
// --------------------------
UEnvironment(void);
virtual ~UEnvironment(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
// ������ ������������� �������� ������ ���������
int GetPredefinedStructure(void) const;
bool SetPredefinedStructure(int value);

// ������������� ���������� ������, ������� ����� �������������
const ULongId& GetModelCalculationComponent(void) const;
bool SetModelCalculationComponent(const ULongId& value);

// ��� �������� �������� �������� ������
const std::string& GetCurrentDataDir(void);
void SetCurrentDataDir(const std::string& dir);

// ��� �������� �������� ������
const std::string& GetSystemDir(void) const;
void SetSystemDir(const std::string& dir);

/// ����������� �������� ������� ����� ���������� ����� (��)
long long GetMinInterstepsInterval(void) const;
bool SetMinInterstepsInterval(long long value);

/// ���� ��������� ������ �������
bool GetDebugMode(void) const;
bool SetDebugMode(bool value);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool IsStoragePresent(void) const;

// ������� ������� �������������� ���������
virtual bool IsStructured(void) const;
// --------------------------

// --------------------------
// ������ ���������� ������� �����
// --------------------------
// ���������� ��������� �� ���������
virtual UStorage* GetStorage(void);

// ������������� ����� ���������
// ��������� �� ������ ��������� ����� �� ������������ ������
// ��������������� �� ������������ ������ ����� �� ���������� �������
// ������� ������ ������������.
virtual bool SetStorage(UStorage *storage);

// ���������� ��������� �� ������
UEPtr<UContainer> GetModel(void);

// ������� ����� ������ �� ��������� �� ����� ������
virtual bool CreateModel(const NameT& classname);

// ������� ����� ������ �� ��������� �� id ������
virtual bool CreateModel(const UId& classid);

// ���������� ������� ������
virtual bool DestroyModel(void);

/// ����� �����
const UTimeControl& GetTime(void) const;
UTimeControl& GetTime(void);

/// ������ �������� ������ � �������������� ����������
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// --------------------------

// --------------------------
// ������ ���������� �������������
// --------------------------
/// ������������� ���������� � ���������� ��������� ������ �������� �����
/// ������������ ������ � ���� � ������
/// ��������� ������
bool RegisterSourceController(const std::string &component_name, const std::string &property_name);

/// ��������� ��������� � ���� � ������ ��������� ������
bool CallSourceController(void);
// --------------------------

// --------------------------
// ��������� ������� � ������ �����
// --------------------------
// ���������� ��������� �� ������� ��������� ������
UEPtr<UContainer> GetCurrentComponent(void);

// ������������� ��������� �� ������� ��������� ������
// ���� ��� ��� id �� ������, ��� Forbidden, �� �������������
// ��������� �� ������
virtual void SelectCurrentComponent(const NameT &name);
virtual void SelectCurrentComponent(const ULongId &id);

// ������������� ��������� �� ������� ��������� ������ �� ���� ������
virtual void ResetCurrentComponent(void);

// ������������� ��������� �� ������� ��������� ������ �� ������������ ���������
// (������� �� ������� �����). ���� ��� ��������� �� ������, �� �� ������ ������
virtual void UpCurrentComponent(void);

// ������������� ��������� �� ������� ��������� ������ �� �������� ��������� ��
// ����� ������ (������� �� ������� ����).
virtual void DownCurrentComponent(const NameT &name);
virtual void DownCurrentComponent(const ULongId &id);
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
public:
// ���������� ���������� ������� ������ �� ��������� ��������
void IncreaseModelTimeByStep(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
public:
// ������������� �����
virtual void Init(void);

// ��������������� �����
virtual void UnInit(void);

// ��������� �������������� �������� ������ ���������
virtual bool CreateStructure(void);

// ���������� ������� ������ ���������
virtual bool DestroyStructure(void);

// ������ ������ � �������� �������
virtual void RTCalculate(void);
// --------------------------

// --------------------------
// ������ ���������� ������������
// --------------------------
public:
// ������������ ��������� ����������
virtual void ProcessException(UException &exception) const;

// ������������ ����� �������� ����������
// ���� 0, �� �������������
int GetMaxExceptionsLogSize(void) const;
void SetMaxExceptionsLogSize(int value);

// ���������� ������ ����� ����
const char* GetLog(int &error_level) const;

/// ���������� ����� ����� ����
int GetNumLogLines(void) const;

/// ���������� ������ ���� � �������� i
const char* GetLogLine(int i) const;

/// ���������� ����� ������������� ����� ����
int GetNumUnreadLogLines(void) const;

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
const char* GetUnreadLog(int &error_level);

// ���������� ��������-������������ ����������
PExceptionHandler GetExceptionHandler(void) const;
bool SetExceptionHandler(PExceptionHandler value);

/// ������� ���
void ClearLog(void);

/// ������� ��� ����������� ���������
void ClearReadLog(void);

// ����� ����������� ���������� ����� ��� ������� ������ ������ � ���
void LogMessage(int msg_level, const std::string &line);
void LogMessage(int msg_level, const std::string &method_name, const std::string &line);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// ������������� �����
virtual void AInit(void);

// ��������������� �����
virtual void AUnInit(void);

// ��������� �������������� �������� ������ ���������
virtual bool ACreateStructure(void);

// ���������� ������� ������ ���������
virtual bool ADestroyStructure(void);

protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool ABuild(void);

// ����� �������� �����.
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};

}
#endif
