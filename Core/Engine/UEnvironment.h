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
#include "ULoggerEnv.h"

namespace RDK {

class UEnvironment;

/// ������� ������ ���� ����������� � �������� �������
extern RDK_LIB_TYPE bool RDK_CALL RdkCreatePredefinedStructure(RDK::UEnvironment* env, int predefined_structure);

class RDK_LIB_TYPE UEnvironment: virtual public UModule
{
protected: // ���������
// ������ �������������� �������� ������ ���������
// 0 - ��������� ������������ �����
int PredefinedStructure;

// ��� �������� �������� �������� ������
std::string CurrentDataDir;

// ��� �������� �������� ������
std::string SystemDir;

/// ����������� �������� ������� ����� ���������� ����� (��)
ULongTime MinInterstepsInterval;

/// ������������ ����� ������� ������ (���)
/// ���� 0 - �� �� ����������
double MaxCalcTime;

protected: // ���������
// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool StoragePresent;

// ������� �������� �������������
//bool Initialized;

// ������� ������� �������������� ���������
bool Structured;

// ������������� ���������� ������, ������� ����� �������������
std::string ModelCalculationComponent;

protected: // ������
// ���������
UStorage* Storage;

// ����������� ������
UEPtr<UContainer> Model;

/// ����� �����
UTimeControl Time;

/// ������
RDK::UBitmapFontCollection Fonts;

// ������������ ��������� �������, ������� ������ �������� "�������" � ������
// ������� ��������� ������� (��)
double MaxModelDuration;

/// ������ �������� ������ � �������������� ����������
int ChannelIndex;

/// ��� ���������� ������ �������� ����� ������������ ������ � ���� � ������
/// ��������� ������
std::string SourceControllerName;

/// ��� ���������� ��������� ���������� ������ �������� ����� ������������
/// ������ � ���� � ������ ��������� ������
std::string SourceControllerProperty;

protected: // ���������� �������� �������
// ������� ��������� ������
UEPtr<UComponent> CurrentComponent;

protected: // ��������� ����������
unsigned long long StartupTime;

unsigned long long CurrentTime,LastDuration, ProcEndTime, LastStepStartTime;

/// �����, ����������� � ������ �� ���� ����� RTCalculate;
double RTModelCalcTime;

/// ��������� ������ ��� �����������
mutable UEPtr<ULoggerEnv> Logger;

/// ����, ������������ ���� ��������� ����� ������� (�� MaxCalcTime)
bool CalcFinishedFlag;

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
const std::string& GetModelCalculationComponent(void) const;
bool SetModelCalculationComponent(const std::string& value);

// ��� �������� �������� �������� ������
const std::string& GetCurrentDataDir(void);
void SetCurrentDataDir(const std::string& dir);

// ��� �������� �������� ������
const std::string& GetSystemDir(void) const;
void SetSystemDir(const std::string& dir);

/// ����������� �������� ������� ����� ���������� ����� (��)
long long GetMinInterstepsInterval(void) const;
bool SetMinInterstepsInterval(long long value);

/// ������������ ����� ������� ������ (���)
/// ���� 0 - �� �� ����������
double GetMaxCalcTime(void) const;
bool SetMaxCalcTime(double value);

/// ���� ��������� ������ �������
//bool GetDebugMode(void) const;
//bool SetDebugMode(bool value);

/// ����� ��������� ������� ��� �����������
//unsigned int GetDebugSysEventsMask(void) const;
//bool SetDebugSysEventsMask(unsigned int value);

/// ���������� ���� ��������� ������ ���� � ��������
//bool GetDebuggerMessageFlag(void) const;

/// ������������� ���� ��������� ������ ���� � ��������
//bool SetDebuggerMessageFlag(bool value);

/// ���� ��������� ���������� ����������� ������� � ���-����
/// true - ����������� ��������
//bool GetEventsLogMode(void) const;
//bool SetEventsLogMode(bool value);
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

/// ���������� �����, ����������� �� ��������� RT ������
double GetRTLastDuration(void) const;

/// �����, ����������� � ������ �� ���� ����� RTCalculate;
double GetRTModelCalcTime(void) const;

/// ������������������ RT ������� (��������� RTModelCalcTime/RTLastDuration)
double CalcRTPerformance(void) const;
// --------------------------

// --------------------------
// ������ ���������� ������� �����
// --------------------------
// ��������� �� ������
UEPtr<ULoggerEnv> const GetLogger(void) const;
virtual bool SetLogger(UEPtr<ULoggerEnv> logger);

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

/// ������
RDK::UBitmapFontCollection& GetFonts(void);
bool SetFonts(const RDK::UBitmapFontCollection& value);
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

// ������������� ��������� �� ������� ��������� ������ �� ���� ������
virtual void ResetCurrentComponent(void);

// ������������� ��������� �� ������� ��������� ������ �� ������������ ���������
// (������� �� ������� �����). ���� ��� ��������� �� ������, �� �� ������ ������
virtual void UpCurrentComponent(void);

// ������������� ��������� �� ������� ��������� ������ �� �������� ��������� ��
// ����� ������ (������� �� ������� ����).
virtual void DownCurrentComponent(const NameT &name);
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
// ��������� �������������� �������� ������ ���������
virtual bool CreateStructure(void);

// ���������� ������� ������ ���������
virtual bool DestroyStructure(void);

/// ����, ������������ ���� ��������� ����� ������� (�� MaxCalcTime)
bool IsCalcFinished(void) const;

// ������ ������ � �������� �������
virtual void RTCalculate(void);

/// ������ ������ �������� ������������� calc_intervsal ������ � ����������� ��������� ���������
virtual void FastCalculate(UTime calc_interval);

// --------------------------

// --------------------------
// ������ ���������� ������������
// --------------------------
/*
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
const char* GetLogLine(int i, int &error_level, int &number, time_t &time) const;

/// ���������� ����� ������������� ����� ����
int GetNumUnreadLogLines(void) const;

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
const char* GetUnreadLog(int &error_level, int &number, time_t &time);
bool GetUnreadLog(UException &ex);

// ���������� ��������-������������ ����������
PExceptionHandler GetExceptionHandler(void) const;
bool SetExceptionHandler(PExceptionHandler value);

/// ���������� ��������-���������������� ����������
PExceptionPreprocessor GetExceptionPreprocessor(void) const;
bool SetExceptionPreprocessor(PExceptionPreprocessor value);

/// ���������� ��������-������������� ����������
PExceptionPostprocessor GetExceptionPostprocessor(void) const;
bool SetExceptionPostprocessor(PExceptionPostprocessor value);

/// ������� ���
void ClearLog(void);

/// ������� ��� ����������� ���������
void ClearReadLog(void);

// ����� ����������� ���������� ����� ��� ������� ������ ������ � ���
void LogMessage(int msg_level, const std::string &line, int error_event_number=0);
void LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
void LogMessageEx(int msg_level, const std::string &object_name, const std::string &line, int error_event_number=0);
void LogMessageEx(int msg_level, const std::string &object_name, const std::string &method_name, const std::string &line, int error_event_number=0);
*/
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
