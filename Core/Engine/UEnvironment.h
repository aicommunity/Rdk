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

/// �������� ����� ����� ������
struct UDataSource
{
/// ��� ����������
std::string ComponentName;

/// ��� ��������
std::string PropertyName;

/// ��������� ����� ���������� ������� ������ ������
double LastReadTime;

/// ��������� ����� ���������� ������� ���������� ������
double LastWriteTime;

/// ��������� �� �����
UEnvironment *Env;

UDataSource(UEnvironment *env);

/// ��������� �������� ������ �������� �����
virtual void UpdateData(void)=0;

/// ������� ��� �������� ������
virtual void ClearData(void)=0;

/// ����������� ����� ����� ������
virtual bool Configure(const std::string &component_name, const std::string &property_name)=0;

/// ���������� ��� ������
virtual const type_info& GetDataType(void) const=0;
};

template <class T>
struct UDataSourceT: public UDataSource
{
/// ������
std::vector<MDMatrix<T> > Data;

UDataSourceT(UEnvironment *env);

/// ��������� �������� ������ �������� �����
virtual void UpdateData(void);

/// ��������� ������ � ������� ������
virtual void ExtractData(std::vector<MDMatrix<T> > &buffer);

/// ������� ��� �������� ������
virtual void ClearData(void);

/// ����������� ����� ����� ������
virtual bool Configure(const std::string &component_name, const std::string &property_name);

/// ���������� ��� ������
virtual const type_info& GetDataType(void) const;
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

/// ����� ��������� predefined structure
std::map<int, UEnvPredefinedStructDescription> PredefinedStructures;

/// ����� ���������� ������
std::vector<UDataSource*> DataSources;

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
bool RegisterDataSource(const std::string &component_name, const std::string &property_name);

/// ������� ����������� ����� ����� ������ (���� MDMatrix)
void UnRegisterDataSource(const std::string &component_name, const std::string &property_name);

/// ������� ����������� ���� ����� ����� ������ (���� MDMatrix)
void UnRegisterAllDataSources(void);

/// ���������� ������ ����� �����
/// ���������� false ���� ��� ������ �� ������ � ���������
template<class T>
bool ReadDataSource(const std::string &component_name, const std::string &property_name, std::vector<MDMatrix<T> > &buffer);
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

template<class T>
UDataSourceT<T>::UDataSourceT(UEnvironment *env)
 : UDataSource(env)
{
}

/// ��������� �������� ������ �������� �����
template<class T>
void UDataSourceT<T>::UpdateData(void)
{
 if(!Env)
  return;

 if(!Env->GetModel())
 {
  Env->UnRegisterDataSource(ComponentName,PropertyName);
  return;
 }

 UContainer *cont=Env->GetModel()->GetComponentL(ComponentName);
 if(!cont)
 {
  Env->UnRegisterDataSource(ComponentName,PropertyName);
  return;
 }
 const MDMatrix<T> *data=cont->AccessPropertyData<MDMatrix<T> >(PropertyName);
 if(!data)
 {
  Env->UnRegisterDataSource(ComponentName,PropertyName);
  return;
 }

 if(LastReadTime>=LastWriteTime)
 {
  Data.clear();
  LastReadTime=0.0;
 }
 Data.push_back(*data);
 LastWriteTime=Env->GetTime().GetDoubleTime();
};

/// ��������� ������ � ������� ������
template<class T>
void UDataSourceT<T>::ExtractData(std::vector<MDMatrix<T> > &buffer)
{
 buffer=Data;
 LastReadTime=Env->GetTime().GetDoubleTime();
}

/// ������� ��� �������� ������
template<class T>
void UDataSourceT<T>::ClearData(void)
{
 Data.clear();
 LastReadTime=LastWriteTime=0.0;
};

/// ����������� ����� ����� ������
template<class T>
bool UDataSourceT<T>::Configure(const std::string &component_name, const std::string &property_name)
{
 if(ComponentName == component_name && PropertyName == property_name)
  return true;
 ClearData();
 ComponentName.clear();
 PropertyName.clear();
 if(!Env)
  return false;

 if(!Env->GetModel())
  return false;

 UContainer *cont=Env->GetModel()->GetComponentL(ComponentName);
 if(!cont)
  return false;

 UEPtr<UIProperty> prop=cont->FindProperty(PropertyName);
 if(!prop)
  return false;

 if(prop->GetLanguageType() != typeid(MDMatrix<T>))
  return false;

 ComponentName=component_name;
 PropertyName=property_name;
 return true;
};

/// ���������� ��� ������
template<class T>
const type_info& UDataSourceT<T>::GetDataType(void) const
{
 return typeid(MDMatrix<T>);
}

/// ���������� ������ ����� �����
/// ���������� false ���� ��� ������ �� ������ � ���������
template<class T>
bool UEnvironment::ReadDataSource(const std::string &component_name, const std::string &property_name, std::vector<MDMatrix<T> > &buffer)
{
 for(size_t i=0;i<DataSources.size();i++)
 {
  if(DataSources[i] && DataSources[i]->ComponentName == component_name && DataSources[i]->PropertyName == property_name)
  {
   if(DataSources[i]->GetDataType() != typeid(MDMatrix<T>)
	return false;

   buffer=DataSources[i]->Data;
   return true;
  }
 }
 return false;
}

}
#endif
