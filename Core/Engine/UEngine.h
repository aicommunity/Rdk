/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2009.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UENGINE_H
#define UENGINE_H

#include "UModule.h"
#include "UAContainerEnvironment.h"
#include "UAContainerStorage.h"
#include "../Utilities/UIniFile.h"
#include "UANet.h"

namespace RDK{

//using namespace RDK;


class UEngine: public UModule
{
public:
// ������� ����� ���������
//typedef RDK::UAContainerStorage* (*PCreateNewStorage)(void);

// ������� ����� �����
//typedef RDK::UAContainerEnvironment* (*PCreateNewEnvironment)(void);

// �������� ������� ��������� ������ ����������� ����������
typedef void (*PExceptionHandler)(void);

protected: // ��������� �������������
// ��������� �� ������� �������� ����������� ��������� � �����
//PCreateNewStorage FuncCreateNewStorage;
//PCreateNewEnvironment FuncCreateNewEnvironment;

// ��� ����� �������������
string OptionsFileName;

// ��� ����� �������� ���������� �������
string ClassesDescriptionFileName;

// ��� ������ ������ �������� ����������
//string MainLibrarySectionName;

// ��� ���������� �������� ����������
//string MainLibraryName;

// ��� ������ ������ ��������� ���������
string ComponentLibrariesSectionName;

// ��� ������ ������ ������� ���������
string ComponentClassesSectionName;

// ������ ������������� ��������� � ����������
// ���� < 0, �� ����� ��������� ����� �������
//int StorageIndex;

// ������ ������������ ����� � ����������
// ���� < 0, �� ����� ����� ����� �������
//int EnvironmentIndex;


protected: // ���������� ���������
// ���� ������ �������
// 1 - ������� ��������
// 0 - ������� ��������������
// -1 - ������� ���������
int Runned;

protected: // ������
// ������ �������������
UIniFile<char> Options;

// ���������
RDK::UEPtr<UAContainerStorage> Storage;

// �����
RDK::UEPtr<UAContainerEnvironment> Environment;

protected: // ��������� ����������
// ��� ���������� �������
mutable vector<USharedPtr<Exception> > ExceptionsLog;

// ������������ ����� �������� ����������
// ���� 0, �� �������������
int MaxExceptionsLogSize;

// ������� ���������� ����������
PExceptionHandler ExceptionHandler;

protected: // ��������� ����������
// ������ ����������� ���������
std::list<UALibrary*> LibrariesList;

// ������ ����������� �������� �������
std::list<UAContainer*> ClassesList;

RDK::Serialize::USerStorageXML XmlStorage;

// ��������� ��������� �����
mutable string TempString;

// ��������� ��������� ������ ��� ����
mutable string TempLogString;

// ��������� ��������� ����� ����������
mutable string CompName;

// ������ ���������� ���������� ������� ���� ����������
size_t LastReadExceptionLogIndex;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UEngine(void);
virtual ~UEngine(void);
// --------------------------

// --------------------------
// ������ ���������� ����������� �������������
// --------------------------
// ��������� �� ������� �������� ����������� ��������� � �����
/*UEngine::PCreateNewStorage GetFuncCreateNewStorage(void) const;
bool SetFuncCreateNewStorage(UEngine::PCreateNewStorage value);

UEngine::PCreateNewEnvironment GetFuncCreateNewEnvironment(void) const;
bool SetFuncCreateNewEnvironment(UEngine::PCreateNewEnvironment value);
  */
// ��� ����� �������������
const string& GetOptionsFileName(void) const;
bool SetOptionsFileName(const string& value);
// --------------------------


// --------------------------
// ������ ������� � ���������� ���������
// --------------------------
// ���� ������ �������
int IsRunned(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ��������� �� �����
virtual UAContainerEnvironment* GetEnvironment(void);

// ���������� ��������� �� ������
virtual UAContainer* GetModel(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// ������ ����� � ��� �� �������������
// --------------------------
// �������������� ������ ������
virtual void Init(void);
virtual bool Init(UEPtr<UAContainerStorage> storage, UEPtr<UAContainerEnvironment> env);

// ���������������� ������ ������
// � ��������� ������� ���������
virtual void UnInit(void);

// ��������� �������
virtual bool Start(void);

// ���������������� �������
virtual bool Pause(void);

// ������������� �������
virtual bool Stop(void);
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
virtual int Storage_GetNumClasses(void);

// ���������� id ������� � ���������. ������ ������ ���� ��������
virtual void Storage_GetClassesList(int *buffer) const;

// ���������� ��� ������ �� ��� id.
virtual const char * Storage_GetClassName(int id) const;

// ���������� Id ������ �� ��� �����
virtual int Storage_GetClassId(const char *name) const;

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
virtual bool Storage_DelClass(int classid);

// �������� ��� ��������� ������� �� ���������
virtual void Storage_FreeObjectsStorage(void);

// ������� ��� ������� �� ���������
virtual void Storage_ClearObjectsStorage(void);

// ��������� ��������� ����� �������� � ���������
virtual int Storage_CalcNumObjects(void) const;
virtual int Storage_CalcNumObjectsById(int classid) const;
virtual int Storage_CalcNumObjectsByName(const char* classname) const;

// ���������� �������� ������ �� ��� id � ������� xml
virtual const char* Storage_GetClassDescription(int classid);

// ������������� �������� ������ �� ��� id, �������� ��� �� ������� xml
virtual bool Storage_SetClassDescription(int classid, const char* description);

// ��������� �������� ���� ������� � xml
virtual const char* Storage_SaveClassesDescription(void);

// ��������� �������� ���� ������� �� xml
virtual bool Storage_LoadClassesDescription(const char* xmltext);

// ��������� ����� �������� ���� ������� � xml
virtual const char* Storage_SaveCommonClassesDescription(void);

// ��������� ����� �������� ���� ������� �� xml
virtual bool Storage_LoadCommonClassesDescription(const char* xmltext);


// ������ ���������� ������
// ----------------------------
// ������ ������������� �������� ������ ���������
virtual int Env_GetPredefinedStructure(void) const;
virtual bool Env_SetPredefinedStructure(int value);

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
virtual bool Env_IsStoragePresent(void) const;

// ���������� ��������� �������������
virtual bool Env_IsInit(void) const;

// ������� ������� �������������� ���������
virtual bool Env_IsStructured(void) const;

// ������������� �����
virtual bool Env_Init(void);

// ��������������� �����
virtual bool Env_UnInit(void);

// ��������� �������������� �������� ������ ���������
virtual bool Env_CreateStructure(void);

// ���������� ������� ������ ���������
virtual bool Env_DestroyStructure(void);

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
virtual void Env_Destroy(void);

// ��������� ���������� �� ����� dll-�����
virtual int Env_LoadStorageLibrary(const char *filename);

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool Env_DelClassLibraryByIndex(int index);

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool Env_DelClassLibraryByName(const char *name);

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool Env_DelAllClassLibraries(void);

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
virtual bool Env_BuildStorage(void);

// ���������� ����� ���������
virtual int Env_GetNumClassLibraries(void) const;

// ���������� ��� ���������� �� �������
virtual const char * Env_GetClassLibraryName(int index);

// ���������� ������ ���������� �� �������
virtual const char * Env_GetClassLibraryVersion(int index);

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
// ���������� id ������ ������ � ���������
virtual int Env_CreateClass(const char* stringid);

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
virtual int Env_Calculate(const char* stringid=0);

// ������ ���� ������ � �������� �������
virtual void Env_RTCalculate(void);

// ����� ������ �����
// ���� stringid == 0 �� ���������� ��� ������ �������,
// ����� - ������ ��������� ��������� ������
virtual int Env_Reset(const char* stringid=0);

// ���������� ���������� ������� ������ �� ��������� ��������
virtual void Env_IncreaseModelTimeByStep(void);

// !!! ��������� ������ ���������� ������� ����������� ������ �� ���
// ������, ������������ � ����������� �� ���������� id !!!
// ������������� ������� ��������� (��������� ������������ ����� - ������)
virtual int Env_SelectCurrentComponent(const char *stringid);

// ���������� ������� ��������� � ��������� �� ��������� (������)
virtual int Env_ResetCurrentComponent(const char *stringid);

// ������ ������� ��������� �� ��� �������� (������ �� ������� �����)
// ���� ��� �� ������� ������, �� �� ������ ������
virtual int Env_UpCurrentComponent(void);

// ������ ������� ��������� �� ��� �������� �� ������������ ������ �����������
// (����� �� N ������� ���� ������������ �������� ����������)
virtual int Env_DownCurrentComponent(const char *stringid);

// ���������� ������� ��� �������� ����������
virtual const char* Env_GetCurrentComponentName(void) const;

// ���������� ������� ��������� id �������� ����������
virtual const char* Env_GetCurrentComponentId(void) const;

// ������ ���������� �������
// ----------------------------
// ������� ������
virtual int Model_Destroy(void);

// ������� ����� ������ �� id ������ � ���������
// �������������� ������� ������������ ������
virtual int Model_Create(int classid);

// ������� ������
virtual int Model_Clear(void);

// ���������, ���������� �� ������
virtual bool Model_Check(void);

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'classid'
// ���� stringid - ������ ������, �� ��������� � ���� ������
virtual int Model_AddComponent(const char* stringid, int classid);

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'id'
// ���� stringid - ������ ������, �� ������� �� ����� ������
virtual int Model_DelComponent(const char* stringid, int id);

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
virtual int Model_GetNumComponents(const char* stringid);

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
virtual int Model_GetComponentsList(const char* stringid, int *buffer);

// ���������� xml-������ ������� ��������������� ���� ����������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ���������� ����� ��������� � ������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ����������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ����������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
virtual const char* Model_GetConnectorsList(const char* stringid,
						  int sublevel=-1, const char* owner_level_stringid=0);

// ���������� xml-������ ������� ��������������� ���� ��������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// �������� ����� ��������� � ������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ��������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
virtual const char* Model_GetItemsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// ���������� xml-������ ������� ��������������� ���� �������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ������� ����� ��������� � ������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� �������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
virtual const char* Model_GetNetsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
virtual const char* Model_GetComponentName(const char* stringid);

// ���������� ������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
virtual const char* Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid=0);

// ���������� ������� id ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
virtual const char* Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid=0);

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual const char* Model_GetComponentParameters(const char *stringid);

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual const char* Model_GetComponentSelectedParameters(const char *stringid);

// ���������� ��������� ���������� �� �������������� � ����������
// ������ ��� buffer ������ ���� ��������!
virtual const char* Model_GetComponentParametersEx(const char *stringid);

// ���������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
virtual const char * Model_GetComponentParameterValue(const char *stringid, const char *paramname);

// ������������� ��������� ���������� �� ��������������
virtual bool Model_SetComponentParameters(const char *stringid, const char* buffer);

// ������������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
virtual void Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer);

// ��������� ��������� ���������� ���� � ������
virtual int Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number);

// ��������� ��� ���������� ���������� ���������� �� ����������� id � �������: 0 ����� � 0 �����
virtual int Model_ChainLinking(const char* stringid);

// ��������� ��� ���������� ���������� ���������� �����������, ��������� �� � ������������ ����� ������� ������
// ������������ ��� ������������ ������������������
virtual int Model_ParallelLinking(const char* stringid);

// ��������� ��������� �����
virtual int Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number);

// ��������� ��� �����
virtual int Model_BreakAllLinks(void);

// ��������� ��� ������� � �������� ����� ���������� ����������
virtual int Model_BreakAllComponentLinks(const char* stringid);

// ��������� ��� ������� ����� ���������� ����������
virtual int Model_BreakAllComponentInputLinks(const char* stringid);

// ��������� ��� �������� ����� ���������� ����������
virtual int Model_BreakAllComponentOutputLinks(const char* stringid);

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
virtual const char* Model_GetComponentInternalLinks(const char* stringid);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
virtual int Model_SetComponentInternalLinks(const char* stringid,const char* buffer);

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
virtual const char* Model_GetComponentInputLinks(const char* stringid);

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
virtual const char* Model_GetComponentOutputLinks(const char* stringid);

// ���������� ��������� ���������� �� ��������������
virtual const char* Model_GetComponentState(const char *stringid);

// ���������� ���������� ������ ��������� ���������� �� ��������������
virtual const char* Model_GetComponentSelectedState(const char *stringid);

// ���������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
virtual const char * Model_GetComponentStateValue(const char *stringid, const char *statename);

// ������������� ��������� ���������� �� ��������������
virtual bool Model_SetComponentState(const char *stringid, const char* buffer);

// ������������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
virtual void Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer);

// ���������� ����� ������ � ����������
virtual int Model_GetComponentNumInputs(const char *stringid);

// ���������� ������ ����� ���������� � ����� ���������
virtual int Model_GetComponentInputDataSize(const char *stringid, int index);

// ���������� ������ �������� ����� � ������
virtual int Model_GetComponentInputElementSize(const char *stringid, int index);

// ���������� ������ ����� ���������� � ������ ���������
virtual int Model_GetComponentInputByteSize(const char *stringid, int index);

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
virtual unsigned char* Model_GetComponentInputData(const char *stringid, int index);

// ���������� ����� ������� � ����������
virtual int Model_GetComponentNumOutputs(const char *stringid);

// ���������� ������ ������ ���������� � ����� ���������
virtual int Model_GetComponentOutputDataSize(const char *stringid, int index);

// ���������� ������ �������� ������ � ������
virtual int Model_GetComponentOutputElementSize(const char *stringid, int index);

// ���������� ������ ������ ���������� � ������ ���������
virtual int Model_GetComponentOutputByteSize(const char *stringid, int index);

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
virtual unsigned char* Model_GetComponentOutputData(const char *stringid, int index);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
virtual const char* Model_SaveComponent(const char *stringid);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
virtual int Model_LoadComponent(const char *stringid, char* buffer);

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
virtual const char* Model_SaveComponentParameters(const char *stringid);

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
virtual int Model_LoadComponentParameters(const char *stringid, char* buffer);

// ��������� ��������� ���������� � ��� �������� ��������� � xml
virtual const char* Model_SaveComponentState(const char *stringid);

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
virtual int Model_LoadComponentState(const char *stringid, char* buffer);

// ��������� ����� ����� ������ �� ���������
virtual int Model_GetDefaultTimeStep(void) const;
virtual void Model_SetDefaultTimeStep(int value);

// ��������� ����� ����� ����������
virtual int Model_GetTimeStep(const char *stringid) const;
virtual void Model_SetTimeStep(const char *stringid, int value);

// ������������� ��� ����� ���������� � ���� ��� �������� ���������
virtual void Model_SetGlobalTimeStep(const char *stringid, int value);

// ���������� ������� ����� ������
virtual long long Model_GetTime(void);
virtual double Model_GetDoubleTime(void);

// ������������� ������� ����� ������
virtual bool Model_SetTime(long long value);

// ���������� �������� �����
virtual long long Model_GetRealTime(void);
virtual double Model_GetDoubleRealTime(void);

// ������������� �������� �����
virtual bool Model_SetRealTime(long long value);

// ����������� �������� ����� �� �������� ��������
virtual bool Model_IncreaseRealTime(long long value);

// ���������� ���������� ��� � �������� �������
virtual long long Model_GetRealTimeStep(void);
virtual double Model_GetDoubleRealTimeStep(void);

// ���������� ����� ������� ���������� ��� ������� ������� �������� ��������� (��)
long long Model_GetStepDuration(const char *stringid) const;

// ���������� �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
long long Model_GetFullStepDuration(const char *stringid) const;

// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
double Model_GetInstantPerformance(const char *stringid) const;
// --------------------------

// --------------------------
// ������ ���������� ������������
// --------------------------
// ������������ ��������� ����������
virtual void ProcessException(Exception *exception) const;

// ��������� ������ ���� �� ����������
//virtual string CreateLogMessage(Exception *exception) const;

// ���������� ������ ������������������ ����������
const vector<USharedPtr<Exception> > GetExceptionsLog(void) const;

// ������������ ����� �������� ����������
// ���� 0, �� �������������
int GetMaxExceptionsLogSize(void) const;
void SetMaxExceptionsLogSize(int value);

// ���������� ������ ����� ����
const char* GetLog(void) const;

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
const char* GetUnreadLog(void);

// ���������� ��������-������������ ����������
PExceptionHandler GetExceptionHandler(void) const;
bool SetExceptionHandler(PExceptionHandler value);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
public:
// �������������� ������ ���������
void CreateStorage(void);

// �������������� �����
// ���� ������ ��������� 'storage', �� ��������� ��� �� ������.
// ���� ���� 'isinit' == true, �� �������������� ��������� ������������ ������������
// ���� ��������� �� ������ external_libs != 0, ������������� �������������� ��������� ����� �������������
void CreateEnvironment(bool isinit=true, list<UAContainer*>* external_classes=0, list<UALibrary*>* external_libs=0);

// ��������� ����� ����������������� ���������
virtual int LoadPredefinedLibraries(void);

// ��������� ����� �������
int LoadClasses(void);

// ��������� ����� ���������
int LoadLibraries(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// ���������� ��������� ���������� �� ��������������
virtual bool Model_GetComponentParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// ���������� ���������� ��������� ���������� �� ��������������
virtual bool Model_GetComponentSelectedParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// ���������� ��������� ���������� �� �������������� � ����������
virtual bool Model_GetComponentParametersEx(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// ������������� ��������� ���������� �� ��������������
virtual bool Model_SetComponentParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
virtual int Model_GetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
virtual int Model_SetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
virtual int Model_GetComponentInputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
virtual int Model_GetComponentOutputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual bool Model_GetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// ���������� ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual bool Model_GetComponentSelectedState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// ������������� ��������� ���������� �� ��������������
virtual bool Model_SetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
virtual int Model_SaveComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, bool links=true);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
virtual int Model_LoadComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, bool links=true);

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
virtual int Model_SaveComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
virtual int Model_LoadComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// ��������� ��������� ���������� � ��� �������� ��������� � xml
virtual int Model_SaveComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
virtual int Model_LoadComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);
// --------------------------

// --------------------------
// ������ ����������� ���������� ��������
// --------------------------
protected:
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// ������������ ����� ���������� �� �������� ���������� id
// ���� ��������� id �� ������, �� ���������� ��������� �� ������
UEPtr<UAContainer> FindComponent(const char *stringid) const;

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
	  /*
#ifndef RDK_CALL
#define RDK_CALL _cdecl
#endif

// ������������� dll
typedef bool (RDK_CALL *DLLPDllInit)(void* pfstorage,void* pfenvironment,void* pfengine);
extern "C" DLLPDllInit DLLDllInit;

// ��������� �� ������� ������������ ����� �������� � ����������
typedef int (RDK_CALL *DLLPGetNumStorages)(void);
extern "C" DLLPGetNumStorages DLLGetNumStorages;

// ��������� �� ������� ������������ ����� ���� � ����������
typedef int (RDK_CALL *DLLPGetNumEnvironments)(void);
extern "C" DLLPGetNumEnvironments DLLGetNumEnvironments;

// ��������� �� ������� ������������ ����� ������� � ����������
typedef int (RDK_CALL *DLLPGetNumEngines)(void);
extern "C" DLLPGetNumEngines DLLGetNumEngines;

// ���������� ��������� �� �������
typedef UAContainerStorage* (RDK_CALL *DLLPGetStorage)(size_t i);
extern "C" DLLPGetStorage DLLGetStorage;

// ���������� ����� �� �������
typedef UAContainerEnvironment* (RDK_CALL *DLLPGetEnvironment)(size_t i);
extern "C" DLLPGetEnvironment DLLGetEnvironment;

// ���������� ������ �� �������
typedef UEngine* (RDK_CALL *DLLPGetEngine)(size_t i);
extern "C" DLLPGetEngine DLLGetEngine;

// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
typedef UAContainerStorage* (RDK_CALL *DLLPAddNewStorage)(void);
extern "C" DLLPAddNewStorage DLLAddNewStorage;

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
typedef UAContainerEnvironment* (RDK_CALL *DLLPAddNewEnvironment)(UAContainerStorage *storage,bool isinit,list<UAContainer*>* external_classes, list<UALibrary*>* external_libs);
extern "C" DLLPAddNewEnvironment DLLAddNewEnvironment;

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
typedef UEngine* (RDK_CALL *DLLPAddNewEngine)(void);
extern "C" DLLPAddNewEngine DLLAddNewEngine;
          */

}
#endif
