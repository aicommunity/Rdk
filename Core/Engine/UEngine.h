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
typedef RDK::UAContainerStorage* (*PCreateNewStorage)(void);

// ������� ����� �����
typedef RDK::UAContainerEnvironment* (*PCreateNewEnvironment)(void);

protected: // ��������� �������������
// ��������� �� ������� �������� ����������� ��������� � �����
PCreateNewStorage FuncCreateNewStorage;
PCreateNewEnvironment FuncCreateNewEnvironment;

// ��� ����� �������������
string OptionsFileName;

// ��� ������ ������ �������� ����������
string MainLibrarySectionName;

// ��� ���������� �������� ����������
string MainLibraryName;

// ��� ������ ������ ��������� ���������
string ComponentLibrariesSectionName;

// ��� ������ ������ ������� ���������
string ComponentClassesSectionName;

// ������ ������������� ��������� � ����������
// ���� < 0, �� ����� ��������� ����� �������
int StorageIndex;

// ������ ������������ ����� � ����������
// ���� < 0, �� ����� ����� ����� �������
int EnvironmentIndex;

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
UAContainerStorage* Storage;

// �����
UAContainerEnvironment* Environment;

protected: // ��������� ����������
// ������ ����������� ���������
std::list<UALibrary*> LibrariesList;

// ������ ����������� �������� �������
std::list<UAContainer*> ClassesList;

RDK::Serialize::USerStorageXML XmlStorage;

// ��������� ��������� �����
mutable string TempString;


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
UEngine::PCreateNewStorage GetFuncCreateNewStorage(void) const;
bool SetFuncCreateNewStorage(UEngine::PCreateNewStorage value);

UEngine::PCreateNewEnvironment GetFuncCreateNewEnvironment(void) const;
bool SetFuncCreateNewEnvironment(UEngine::PCreateNewEnvironment value);

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
// --------------------------

// --------------------------
// ������ ���������� �������
// ������ ����� � ��� �� �������������
// --------------------------
// �������������� ������ ������
virtual bool Init(void);

// ���������������� ������ ������
// � ��������� ������� ���������
virtual bool UnInit(void);

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

// ������ ���������� �������
// ----------------------------
// ������� ������
virtual int Model_Destroy(void);

// ������� ����� ������ �� id ������ � ���������
// �������������� ������� ������������ ������
virtual int Model_Create(int classid);

// ������� ������
virtual int Model_Clear(void);

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'classid'
// ���� stringid - ������ ������, �� ��������� � ���� ������
virtual int Model_AddComponent(char* stringid, int classid);

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'id'
// ���� stringid - ������ ������, �� ������� �� ����� ������
virtual int Model_DelComponent(char* stringid, int id);

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
virtual int Model_GetNumComponents(char* stringid);

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
virtual int Model_GetComponentsList(char* stringid, int *buffer);

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
virtual const char* Model_GetComponentName(char* stringid);

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual const char* Model_GetComponentParameters(const char *stringid);

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual const char* Model_GetComponentSelectedParameters(const char *stringid);

// ������������� ��������� ���������� �� ��������������
virtual bool Model_SetComponentParameters(const char *stringid, const char* buffer);

// ��������� ��������� ���������� ���� � ������
virtual int Model_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number);

// ��������� ��������� �����
virtual int Model_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number);

// ��������� ��� �����
virtual int Model_BreakAllLinks(void);

// ��������� ��� ������� � �������� ����� ���������� ����������
virtual int Model_BreakAllComponentLinks(char* stringid);

// ��������� ��� ������� ����� ���������� ����������
virtual int Model_BreakAllComponentInputLinks(char* stringid);

// ��������� ��� �������� ����� ���������� ����������
virtual int Model_BreakAllComponentOutputLinks(char* stringid);

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
virtual const char* Model_GetComponentInternalLinks(char* stringid);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
virtual int Model_SetComponentInternalLinks(char* stringid,const char* buffer);

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
virtual const char* Model_GetComponentInputLinks(char* stringid);

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
virtual const char* Model_GetComponentOutputLinks(char* stringid);

// ���������� ��������� ���������� �� ��������������
virtual const char* Model_GetComponentState(const char *stringid);

// ���������� ���������� ������ ��������� ���������� �� ��������������
virtual const char* Model_GetComponentSelectedState(const char *stringid);

// ������������� ��������� ���������� �� ��������������
virtual bool Model_SetComponentState(const char *stringid, const char* buffer);

// ���������� ����� ������ � ����������
virtual int Model_GetComponentNumInputs(const char *stringid);

// ���������� ������ ����� ���������� � ����� ���������
virtual int Model_GetComponentInputSize(const char *stringid, int index);

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
virtual int Model_GetComponentOutputSize(const char *stringid, int index);

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
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
public:
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
// ������ ��� buffer ������ ���� ��������!
virtual bool Model_GetComponentParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual bool Model_GetComponentSelectedParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

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
virtual int Model_SaveComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
virtual int Model_LoadComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

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
UAContainer* FindComponent(const char *stringid);

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

// ������������� dll
typedef bool(*DLLPDllInit)(void* pfstorage,void* pfenvironment,void* pfengine);
extern DLLPDllInit DLLDllInit;

// ��������� �� ������� ������������ ����� �������� � ����������
typedef std::size_t(*DLLPGetNumStorages)(void);
extern DLLPGetNumStorages DLLGetNumStorages;

// ��������� �� ������� ������������ ����� ���� � ����������
typedef std::size_t(*DLLPGetNumEnvironments)(void);
extern DLLPGetNumEnvironments DLLGetNumEnvironments;

// ��������� �� ������� ������������ ����� ������� � ����������
typedef std::size_t(*DLLPGetNumEngines)(void);
extern DLLPGetNumEngines DLLGetNumEngines;

// ���������� ��������� �� �������
typedef UAContainerStorage* (*DLLPGetStorage)(size_t i);
extern DLLPGetStorage DLLGetStorage;

// ���������� ����� �� �������
typedef UAContainerEnvironment* (*DLLPGetEnvironment)(size_t i);
extern DLLPGetEnvironment DLLGetEnvironment;

// ���������� ������ �� �������
typedef UEngine* (*DLLPGetEngine)(size_t i);
extern DLLPGetEngine DLLGetEngine;

// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
typedef UAContainerStorage* (*DLLPAddNewStorage)(void);
extern DLLPAddNewStorage DLLAddNewStorage;

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
typedef UAContainerEnvironment* (*DLLPAddNewEnvironment)(UAContainerStorage *storage,bool isinit,list<UAContainer*>* external_classes, list<UALibrary*>* external_libs);
extern DLLPAddNewEnvironment DLLAddNewEnvironment;

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
typedef UEngine* (*DLLPAddNewEngine)(void);
extern DLLPAddNewEngine DLLAddNewEngine;


}
#endif
