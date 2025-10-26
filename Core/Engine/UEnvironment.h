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
#include "UController.h"
#include <memory>
#include "ModernSmartPointers.h"
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace RDK {

class UEnvironment;

/// �������� Predefined structure
struct UEnvPredefinedStructDescription
{
/// Id ���������
int Id;

/// ������� ��������
std::string ShortDescription;

/// ��������� ��������
std::string DetailDescription;
};


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

/// ���� ��������� ������������� ��������������� ���������� TimeStep ���
/// ������� ����������
bool UseIndTimeStepFlag;

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
ULongId ModelCalculationComponent;

protected: // ������
// ���������
UStorage* Storage;

// Modern thread-safety
mutable std::shared_mutex environment_mutex_;
mutable std::mutex calculation_mutex_;

// ����������� ������
std::shared_ptr<UContainer> Model;

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

/// ����� ��������� predefined structure
std::map<int, UEnvPredefinedStructDescription> PredefinedStructures;

/// ����� ���������� ������
std::vector< std::pair<UControllerDataReader*, int> >DataReaders;

protected: // ���������� �������� �������
// ������� ��������� ������
std::shared_ptr<UComponent> CurrentComponent;

protected: // ��������� ����������
unsigned long long StartupTime;

unsigned long long CurrentTime,LastDuration, ProcEndTime, LastStepStartTime;

/// �����, ����������� � ������ �� ���� ����� RTCalculate;
double RTModelCalcTime;

/// ��������� ������ ��� �����������

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
const ULongId& GetModelCalculationComponent(void) const;
bool SetModelCalculationComponent(const ULongId& value);

// ��� �������� �������� �������� ������
const std::string& GetCurrentDataDir(void);
void SetCurrentDataDir(const std::string& dir);

// ��� �������� �������� ������
const std::string& GetSystemDir(void) const;
void SetSystemDir(const std::string& dir);

/// ����������� �������� ������� ����� ���������� ����� (��)
unsigned long long GetMinInterstepsInterval(void) const;
bool SetMinInterstepsInterval(unsigned long long value);

/// ������������ ����� ������� ������ (���)
/// ���� 0 - �� �� ����������
double GetMaxCalcTime(void) const;
bool SetMaxCalcTime(double value);

/// ���� ��������� ������������� ��������������� ���������� TimeStep ���
/// ������� ����������
bool GetUseIndTimeStepFlag(void) const;
bool SetUseIndTimeStepFlag(bool value);
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

// ���������� ��������� �� ���������
virtual UStorage* GetStorage(void);

// ������������� ����� ���������
// ��������� �� ������ ��������� ����� �� ������������ ������
// ��������������� �� ������������ ������ ����� �� ���������� �������
// ������� ������ ������������.
virtual bool SetStorage(UStorage *storage);

// ���������� ��������� �� ������
std::shared_ptr<UContainer> GetModel(void);

// ������� ����� ������ �� ��������� �� ����� ������
virtual bool CreateModel(const NameT& classname);

// ������� ����� ������ �� ��������� �� id ������
virtual bool CreateModel(const UId& classid);

// ���������� ������� ������
virtual bool DestroyModel(void);

/// �������������� ������
virtual void ModelInit(void);

/// ���������������� ������
virtual void ModelUnInit(void);

/// ����� �����
const UTimeControl& GetTime(void) const;
UTimeControl& GetTime(void);

/// ������ �������� ������ � �������������� ����������
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);

/// ������
RDK::UBitmapFontCollection& GetFonts(void);
bool SetFonts(const RDK::UBitmapFontCollection& value);

/// ���������� ����� ��������� predefined structures
const std::map<int, UEnvPredefinedStructDescription>& GetPredefinedStructures(void) const;

/// ���������� ��������� predefined structure �� id
UEnvPredefinedStructDescription GetPredefinedStructureDescription(int id) const;

/// ��������� ������� predefined structure
bool AddPredefinedStructure(const UEnvPredefinedStructDescription &descr);

/// ��������� ������� predefined structure �� id
void DelPredefinedStructure(int id);

/// ������� ��� predefined structures
void ClearPredefinedStructures(void);
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
// ������ ���������� ������������ ������
// --------------------------
/// ������������ ����� ����� ����� ������ (���� MDMatrix)
UControllerDataReader* RegisterDataReader(const std::string &component_name, const std::string &property_name, int row=0, int col=0);

/// ������� ����������� ����� ����� ������ (���� MDMatrix)
void UnRegisterDataReader(const std::string &component_name, const std::string &property_name, int row, int col);

/// ������� ����������� ���� ����� ����� ������ (���� MDMatrix)
void UnRegisterAllDataReaders(void);

/// ���������� ������ ����� �����
UControllerDataReader* GetDataReader(const std::string &component_name, const std::string &property_name, int row, int col);
// --------------------------

// --------------------------
// ��������� ������� � ������ �����
// --------------------------
// ���������� ��������� �� ������� ��������� ������
std::shared_ptr<UContainer> GetCurrentComponent(void);

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
// ��������� �������������� �������� ������ ���������
virtual bool CreateStructure(void);

// ���������� ������� ������ ���������
virtual bool DestroyStructure(void);

/// ����, ������������ ���� ��������� ����� ������� (�� MaxCalcTime)
bool IsCalcFinished(void) const;

// ������ ������ � �������� �������
virtual void RTCalculate(void);

/// ������ ������ �������� ������������� calc_intervsal ������ � ����������� ��������� ���������
virtual void FastCalculate(double calc_interval);

// Modern C++20 thread-safe methods
// Thread-safe model access
std::shared_ptr<UContainer> GetModelSafe(void) const;

// Thread-safe logger access

// Thread-safe storage access
UStorage* GetStorageSafe(void) const;

// Modern move semantics
UEnvironment(UEnvironment&& other) noexcept;
UEnvironment& operator=(UEnvironment&& other) noexcept;

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
