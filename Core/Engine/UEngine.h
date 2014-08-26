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
#include "UEnvironment.h"
#include "UStorage.h"
#include "../Utilities/UIniFile.h"
#include "UNet.h"

namespace RDK{

//using namespace RDK;


class RDK_LIB_TYPE UEngine: public UModule
{
public:
// �������� ������� ��������� ������ ����������� ����������
//typedef void (*PExceptionHandler)(void);

protected: // ��������� �������������
// ��� ����� �������������
string OptionsFileName;

// ��� ����� �������� ���������� �������
string ClassesDescriptionFileName;

// ��� ������ ������ ��������� ���������
string ComponentLibrariesSectionName;

// ��� ������ ������ ������� ���������
string ComponentClassesSectionName;

/// ����� �������� ���������� ��������� ���������� ���
/// ������������ ��������
/// 0 - ���� ���������� ��� ���� �������, ������������ ����� ���
/// 1 - ���������� ���������� � �������������� ������� ������� �������
int BufObjectsMode;

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

/// ������ �������� ������ � �������������� ����������
int ChannelIndex;

protected: // ������
// ������ �������������
UIniFile<char> Options;

// ���������
RDK::UEPtr<UStorage> Storage;

// �����
RDK::UEPtr<UEnvironment> Environment;

protected: // ��������� ����������
// ��� ���������� �������
//mutable vector<USharedPtr<UException> > ExceptionsLog;

// ������� ����� ���������� �������
//mutable int CurrentExceptionsLogSize;

// ������������ ����� �������� ����������
// ���� 0, �� �������������
//int MaxExceptionsLogSize;

// ������� ���������� ����������
//PExceptionHandler ExceptionHandler;

protected: // ��������� ����������
// ������ ����������� ���������
std::list<ULibrary*> LibrariesList;

// ������ ����������� �������� �������
std::list<UContainer*> ClassesList;

RDK::USerStorageXML XmlStorage;

// ��������� ��������� �����
mutable std::list<UEPtr<string> > TempStrings;
mutable string DummyTempString;

// ��������� ��������� ������ ��� ����
//mutable string TempLogString;

// ��������� ��������� ����� ����������
mutable string CompName;

// ������ ���������� ���������� ������� ���� ����������
//int LastReadExceptionLogIndex;

UBitmap TempBmp;

public:
// ��������� ��������� �����
//mutable string PubTempString;

/// ��� �������� ������� � �����������
mutable std::map<std::string, UEPtr<UContainer> > AccessCache;



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
/// ����� �������� ���������� ��������� ���������� ���
/// ������������ ��������
/// 0 - ���� ���������� ��� ���� �������, ������������ ����� ���
/// 1 - ���������� ���������� � �������������� ������� ������� �������
int GetBufObjectsMode(void) const;
bool SetBufObjectsMode(int mode);

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
// ������ ���������� ���������� �����������
// --------------------------
/// ������� � ������ ��������� ������ ����� ������
/// � ���������� ������ �� ���
std::string& CreateTempString(void) const;

/// ���������� ��������� ������
/// �� ��������� �� �� ������
std::string& FindTempString(const char *str_data) const;

/// ������� ��������� ������
/// �� ��������� �� �� ������
void DestroyTempString(const char *str_data) const;

/// ������� ��������� ������
/// �� ������ �� ���
void DestroyTempString(const std::string &ref) const;

/// ������� ��� ��������� �����
void ClearAllTempStrings(void) const;

/// ���������� ����� ��������� �����
int GetNumTempStrings(void) const;
// --------------------------

// --------------------------
// ������ ������� � ���������� ���������
// --------------------------
// ���� ������ �������
int IsRunned(void);

/// ������ �������� ������ � �������������� ����������
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ��������� �� �����
virtual UEnvironment* GetEnvironment(void);

// ���������� ��������� �� ������
virtual UContainer* GetModel(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// ������ ����� � ��� �� �������������
// --------------------------
// �������������� ������ ������
virtual void Init(void);
virtual bool Init(UEPtr<UStorage> storage, UEPtr<UEnvironment> env);

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

// ���������� ����� ������� � ��������� � ���� ������, ����������� ��������
virtual const char* Storage_GetClassesNameList(void) const;

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
virtual const char* Storage_GetClassDescription(const char* classname);

// ������������� �������� ������ �� ��� id, �������� ��� �� ������� xml
virtual bool Storage_SetClassDescription(const char* classname, const char* description);

// ��������� �������� ���� ������� � xml
virtual const char* Storage_SaveClassesDescription(void);

// ��������� �������� ���� ������� �� xml
virtual bool Storage_LoadClassesDescription(const char* xmltext);

// ��������� ����� �������� ���� ������� � xml
virtual const char* Storage_SaveCommonClassesDescription(void);

// ��������� ����� �������� ���� ������� �� xml
virtual bool Storage_LoadCommonClassesDescription(const char* xmltext);

// ��������� �������� ���� ������� � xml ������� ����� ��������
virtual const char* Storage_SaveAllClassesDescription(void);

// ��������� �������� ���� ������� �� xml ������� ����� ��������
virtual bool Storage_LoadAllClassesDescription(const char* xmltext);
// ----------------------------

// ----------------------------
// ������ ���������� ����������� ���������
// ----------------------------
// ���������� ����� ���������
virtual int Storage_GetNumClassLibraries(void) const;

// ���������� ������ ��������� � ���� ������, ����������� ��������
virtual const char * Storage_GetClassLibrariesList(void) const;

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// library_name - ��� ����������
virtual const char * Storage_GetLibraryClassNames(const char *library_name) const;

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// index - ������ ����������
virtual const char * Storage_GetLibraryClassNamesByIndex(int index) const;

// ���������� ��� ���������� �� �������
virtual const char * Storage_GetClassLibraryNameByIndex(int index);

// ���������� ������ ���������� �� �������
virtual const char * Storage_GetClassLibraryVersionByIndex(int index);

// ��������� ��������� �� ����� dll-�����
virtual int Storage_LoadBinaryCollectionFromFile(const char *filename);

// ��������� runtime-���������
virtual int Storage_LoadRuntimeCollectionFromFile(const char *filename);
virtual int Storage_LoadRuntimeCollectionFromString(const char *buffer);

// ��������� runtime-���������
virtual int Storage_SaveRuntimeCollectionToFile(const char *filename);
virtual int Storage_SaveRuntimeCollectionToString(const char *buffer);

/// ������� ����� runtime-����������
virtual int Storage_CreateRuntimeCollection(const char *collection_name);

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
virtual int Storage_CreateClass(const char* stringid, const char *class_name, const char *collection_name);

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool Storage_DelClassLibraryByIndex(int index);

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool Storage_DelClassLibraryByName(const char *name);

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool Storage_DelAllClassLibraries(void);


// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
virtual bool Storage_BuildStorage(void);
// ----------------------------

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

/// ����� ������ ���������� �� �������� �� ���������
/// ���� stringid == 0 �� ���������� ��� ������ �������,
/// ����� - ������ ��������� ��������� ������
/// ���� subcomps == true �� ����� ���������� ��������� ���� �������� ���������
virtual int Env_Default(const char* stringid, bool subcomps=false);

// ���������� ���������� ������� ������ �� ��������� ��������
virtual void Env_IncreaseModelTimeByStep(void);

/// ������������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
virtual int Env_SetMinInterstepsInterval(unsigned long long value);

/// ���������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
virtual unsigned long long Env_GetMinInterstepsInterval(void) const;

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

// ���������� ��� �������� �������� ��� �������� ������
virtual const char* Env_GetCurrentDataDir(void) const;

// ������������� ��� �������� �������� ��� �������� ������
virtual int Env_SetCurrentDataDir(const char *dir);

// ���������� ��� �������� �������� ������
virtual const char* Env_GetSystemDir(void) const;

// ������������� ��� �������� �������� ������
virtual int Env_SetSystemDir(const char *dir);

/// ���������� ��������� ����� ����������� ������ �����
virtual int Env_GetDebugMode(void) const;

/// ������������� ��������� ����� ����������� ������ �����
virtual int Env_SetDebugMode(bool value);

// ���������� �� UBEngine
// ������ ����� ������ �����
virtual void Env_SetNumInputImages(int number);

// ������ ����� ������� �����
virtual void Env_SetNumOutputImages(int number);

// ���������� ����� ������ �����
virtual int Env_GetNumInputImages(void);

// ���������� ����� ������� �����
virtual int Env_GetNumOutputImages(void);

// ������ ���������� �� ��������� (������� ����������)
virtual void Env_SetInputRes(int number, int width, int height);

// ������ ���� ��������� �������� ����������� ������ �������������� ���
virtual void Env_SetReflectionXFlag(bool value);

// ���������� ���������� �� ��������� (������� ����������)
virtual int Env_GetInputImageWidth(int number);
virtual int Env_GetInputImageHeight(int number);
virtual int Env_GetInputImageColorModel(int number);

// ���������� ������� �������� ����������
virtual int Env_GetOutputImageWidth(int number);
virtual int Env_GetOutputImageHeight(int number);
virtual int Env_GetOutputImageColorModel(int number);

virtual void Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

virtual unsigned char* Env_GetInputImage(int index);

virtual unsigned char* Env_GetOutputImage(int index);

virtual unsigned char* Env_GetOutputImageY8(int index);

/// ���������� ��������� � ���� � ������ ��������� ������
virtual bool Env_CallSourceController(void);

// ������ ���������� �������
// ----------------------------
// ������� ������
virtual int Model_Destroy(void);

// ������� ����� ������ �� ����� ������ � ���������
// �������������� ������� ������������ ������
virtual int Model_Create(const char *classname);

// ������� ������
virtual int Model_Clear(void);

// ���������, ���������� �� ������
virtual bool Model_Check(void);

// ���������, ���������� �� � ������ ��������� � ������ stringid)
virtual bool Model_CheckComponent(const char* stringid) const;

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'classname'
// ���� stringid - ������ ������, �� ��������� � ���� ������
// ���������� ������� ��� ���������� � ������ ������
virtual const char*  Model_AddComponent(const char* stringid, const char *classname);

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'name'
// ���� stringid - ������ ������, �� ������� �� ����� ������
virtual int Model_DelComponent(const char* stringid, const char *name);

/// ���������� ���������� � ������ ���������
/// ���� comp �� ����������� ����� ����������, ��� target ����� �������� ��
/// ����� ���������� storage, ��� target �� ����� ������� � ���� ���������
/// �� ���������� false � �� ������ ������
virtual int Model_MoveComponent(const char* component, const char* target);

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
virtual int Model_GetNumComponents(const char* stringid);

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
virtual int Model_GetComponentsList(const char* stringid, int *buffer);

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ','
virtual const char* Model_GetComponentsNameList(const char* stringid);

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ',' � ������� ��� ������ 'class_name'
// ���� find_all == true �� ����� ������� � �� ���� ��������������
//virtual const char* Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all=false);
virtual const char* Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all=false);

// ���������� ��������� � ������� �������� index ��� ������ 'name' ����� ���
// ���� �� ������ �� �������� ����� ���������
// ����������� ��� ��������� ������� ������� ���������
// ���� �������� 'step' ������� �� ������� �������, �� ��������� ���������������
// �� ��� �������
virtual int Model_ChangeComponentPosition(const char* stringid, int step);

// ���������� xml-������ ������� ��������������� ���� ����������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ���������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ����������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ����������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
virtual const char* Model_GetConnectorsList(const char* stringid,
						  int sublevel=-1, const char* owner_level_stringid=0);

// ���������� xml-������ ������� ��������������� ���� ��������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// �������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ��������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
virtual const char* Model_GetItemsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// ���������� xml-������ ������� ��������������� ���� �������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
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

// ���������� ��� ������ ���������� � ��������� �� �������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������ ������
virtual const char* Model_GetComponentClassName(const char* stringid);

// ���������� ������ ������� ���������� ����������� ��������
virtual const char* Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask);

// ���������� ������ ���� � �������� ������� ���������� ����������� ��������
// ������ ������� ����� ��� ���_��������:������_�����(������)
virtual const char* Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask);

// ���������� �������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual const char* Model_GetComponentProperties(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// ���������� ���������� �������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual const char* Model_GetComponentSelectedProperties(const char *stringid);

// ���������� �������� ���������� �� �������������� � ����������
// ������ ��� buffer ������ ���� ��������!
virtual const char* Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// ���������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
virtual const char * Model_GetComponentPropertyValue(const char *stringid, const char *paramname);

// ������������� �������� ���������� �� ��������������
virtual int Model_SetComponentProperties(const char *stringid, const char* buffer);

// ������������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
virtual void Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer);

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
virtual void Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer);

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
virtual void Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer);

// ���������� ��������� �� ������ �������� ����������
template<class T>
const T& Model_GetComponentPropertyData(const char *stringid, const char *property_name);

// ���������� ��������� void* �� ������ �������� ����������
const void* Model_GetComponentPropertyData(const char *stringid, const char *property_name);

// �������� ������ 'data' � �������� �������� ����������
template<class T>
int Model_SetComponentPropertyData(const char *stringid, const char *property_name, const T &data);

// �������� ������ 'data' � �������� �������� ����������
int Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data);

// ��������� ��������� ���������� ���� � ������
virtual int Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number);
virtual int Model_CreateLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);

// ��������� ��� ���������� ���������� ���������� �� ����������� id � �������: 0 ����� � 0 �����
virtual int Model_ChainLinking(const char* stringid);

// ��������� ��� ���������� ���������� ���������� �����������, ��������� �� � ������������ ����� ������� ������
// ������������ ��� ������������ ������������������
virtual int Model_ParallelLinking(const char* stringid);

// ��������� ��������� �����
virtual int Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number);
virtual int Model_BreakLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);

// ��������� ��� �����
virtual int Model_BreakAllLinks(void);

// ��������� ����� �� ����� connector_index ���������� 'connectorid'
virtual int Model_BreakConnectorLink(const char* connectorname, int connector_index);

// ��������� ��� ������� � �������� ����� ���������� ����������
virtual int Model_BreakAllComponentLinks(const char* stringid);

// ��������� ��� ������� ����� ���������� ����������
virtual int Model_BreakAllComponentInputLinks(const char* stringid);

// ��������� ��� �������� ����� ���������� ����������
virtual int Model_BreakAllComponentOutputLinks(const char* stringid);

// ���������, ���������� �� ������� �����
virtual bool Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number);
virtual bool Model_CheckLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
virtual const char* Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid=0);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� � ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����������� ������� �������� ����������
virtual int Model_SetComponentInternalLinks(const char* stringid,const char* buffer, const char* owner_level_stringid=0);

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
virtual const char* Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid=0, int sublevel=-1);

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
virtual const char* Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid=0, int sublevel=-1);

// ��������� ��� ������� ����� c ����������� cont � ��� ��������� ������������ � ���� xml � ����� buffer
// ���������� � ������ ����������� ������������ ��������� ���������� cont!
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
virtual const char* Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid=0);

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
virtual const char* Model_SaveComponent(const char *stringid, unsigned int params_type_mask=0xFFFFFFFF);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
virtual int Model_SaveComponentToFile(const char *stringid, const char* file_name, unsigned int params_type_mask=0xFFFFFFFF);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
virtual int Model_LoadComponent(const char *stringid, const char* buffer);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
virtual int Model_LoadComponentFromFile(const char *stringid, const char* file_name);

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
virtual const char* Model_SaveComponentProperties(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
virtual int Model_SaveComponentPropertiesToFile(const char *stringid, const char* file_name, unsigned int type_mask=0xFFFFFFFF);

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
virtual int Model_LoadComponentProperties(const char *stringid, const char* buffer);

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
virtual int Model_LoadComponentPropertiesFromFile(const char *stringid, const char* file_name);

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
virtual const char* Model_SaveComponentDrawInfo(const char *stringid);

// ��������� ����� ����� ������ �� ���������
virtual unsigned int Model_GetDefaultTimeStep(void) const;
virtual void Model_SetDefaultTimeStep(unsigned int value);

// ��������� ����� ����� ����������
virtual unsigned int Model_GetTimeStep(const char *stringid) const;
virtual void Model_SetTimeStep(const char *stringid, unsigned int value);

// ������������� ��� ����� ���������� � ���� ��� �������� ���������
virtual void Model_SetGlobalTimeStep(const char *stringid, unsigned int value);

// ���������� ������� ����� ������
virtual unsigned long long Model_GetTime(void);
virtual double Model_GetDoubleTime(void);

// ������������� ������� ����� ������
virtual bool Model_SetTime(unsigned long long value);

// ���������� �������� �����
virtual unsigned long long Model_GetRealTime(void);
virtual double Model_GetDoubleRealTime(void);

// ������������� �������� �����
virtual bool Model_SetRealTime(unsigned long long value);

// ����������� �������� ����� �� �������� ��������
virtual bool Model_IncreaseRealTime(unsigned long long value);

// ���������� ���������� ��� � �������� �������
virtual unsigned long long Model_GetRealTimeStep(void);
virtual double Model_GetDoubleRealTimeStep(void);

// ������� ����� ������� ���������� ������ � �������������
virtual double Model_GetDoubleSourceTime(void) const;

// ������������� ����� ������� ���������� ������ � ����
virtual bool Model_SetDoubleSourceTime(double value);

// ���������� ����� ������� ���������� ��� ������� ������� �������� ��������� (��)
unsigned long long Model_GetStepDuration(const char *stringid) const;

// ���������� �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
unsigned long long Model_GetFullStepDuration(const char *stringid) const;

// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
double Model_GetInstantPerformance(const char *stringid) const;

// �����, ��������� ����� ����� ���������� ���������� �����
unsigned long long Model_GetInterstepsInterval(const char *stringid) const;
// --------------------------

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
// ������������ �������� ����� ����������� ��� RDK::MDMatrix*
// ���� ����� �� �������� ������ ������ ����, �� ���������� 0
const /* RDK::MDMatrix* */void* Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name);
const /* RDK::MDMatrix* */void* Model_GetComponentOutputAsMatrix(const char *stringid, int index);

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
virtual const RDK::UBitmap* Model_GetComponentOutput(const char *stringid, const char *property_name);
virtual const RDK::UBitmap* Model_GetComponentOutput(const char *stringid, int index);

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
virtual const RDK::UBitmap* Model_GetComponentBitmapOutput(const char *stringid, const char *property_name);
virtual const RDK::UBitmap* Model_GetComponentBitmapOutput(const char *stringid, int index);

/// �������� ������ � ���������� ����������� ������ � �������� 'index' ���������� 'id'
/// � �������� bmp_param
virtual int Model_CopyComponentBitmapOutputHeader(const char *stringid, const char *property_name, RDK::UBitmapParam* bmp_param);
virtual int Model_CopyComponentBitmapOutputHeaderByIndex(const char *stringid, int index, RDK::UBitmapParam* bmp_param);

/// �������� ����������� ������ � �������� 'index' ���������� 'id'
/// ����� ������������, ��� bmp ��� ����� ���������� ������ ��� ���������� ���������� �������
virtual int Model_CopyComponentBitmapOutput(const char *stringid, const char *property_name, RDK::UBitmap* bmp);
virtual int Model_CopyComponentBitmapOutput(const char *stringid, int index, RDK::UBitmap* bmp);

// ���������� ��������� �� ���� � �������� 'index' ���������� 'id'
virtual const RDK::UBitmap* Model_GetComponentBitmapInput(const char *stringid, const char *property_name);
virtual const RDK::UBitmap* Model_GetComponentBitmapInput(const char *stringid, int index);

// �������� ����������� ������ � �������� 'index' ���������� 'id'
virtual void Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const RDK::UBitmap* bmp, bool reflect=false);
virtual void Model_SetComponentBitmapOutput(const char *stringid, int index, const RDK::UBitmap* bmp, bool reflect=false);

// �������� ����������� ����� � �������� 'index' ���������� 'id'
virtual void Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const RDK::UBitmap* const bmp, bool reflect=false);
virtual void Model_SetComponentBitmapInput(const char *stringid, int index, const RDK::UBitmap* const bmp, bool reflect=false);

// --------------------------
// ������ ���������� ������������
// --------------------------
// ������������ ��������� ����������
virtual void ProcessException(UException &exception) const;

// ��������� ������ ���� �� ����������
//virtual string CreateLogMessage(UException &exception) const;

// ���������� ������ ������������������ ����������
//const vector<USharedPtr<UException> > GetExceptionsLog(void) const;

// ������������ ����� �������� ����������
// ���� 0, �� �������������
int GetMaxExceptionsLogSize(void) const;
void SetMaxExceptionsLogSize(int value);

// ���������� ������ ����� ����
const char* GetLog(int &error_level) const;

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
const char* GetUnreadLog(int &error_level);

/// ���������� � ��� ����� ���������
int Engine_LogMessage(int log_level, const char *message);

// ���������� ��������-������������ ����������
UEnvironment::PExceptionHandler GetExceptionHandler(void) const;
bool SetExceptionHandler(UEnvironment::PExceptionHandler value);

/// ������� ���
void ClearLog(void);

/// ���������� ����� ������������� ����� ����
int GetNumUnreadLogLines(void) const;

// ���������� ������ ���� � �������� i �� ���������� ������� ����� ���� �
// ������� ���������� ���������� ���� ���� ��������
const char* GetUnreadLogLine(int &error_level);

// �������� ������ ���� � �������� i �� ���������� ������� ����� ���� �
// ������� ���������� ���������� ��� �����������
//void MarkUnreadLogLineAsRead(int i);

/// ������� ��� ����������� ���������
void ClearReadLog(void);
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
void CreateEnvironment(bool isinit=true, list<UContainer*>* external_classes=0, list<ULibrary*>* external_libs=0);

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
/// ����� ������ ���������� �� �������� �� ���������
/// ���� subcomps == true �� ����� ���������� ��������� ���� �������� ���������
//virtual bool Env_Default(RDK::UContainer* cont, bool subcomps);
     /*
// ���������� �������� ���������� �� ��������������
virtual bool Model_GetComponentProperties(RDK::UContainer* cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ���������� ���������� �������� ���������� �� ��������������
virtual bool Model_GetComponentSelectedProperties(RDK::UContainer* cont, RDK::USerStorageXML *serstorage);

// ���������� �������� ���������� �� �������������� � ����������
virtual bool Model_GetComponentPropertiesEx(RDK::UContainer* cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ������������� �������� ���������� �� ��������������
virtual int Model_SetComponentProperties(RDK::UContainer* cont, RDK::USerStorageXML *serstorage);
   */
// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
//virtual void Model_SetGlobalComponentPropertyValue(RDK::UContainer* cont, UId classid, const char *paramname, const char *buffer);

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
//virtual void Model_SetGlobalOwnerComponentPropertyValue(RDK::UContainer* cont, UId classid, UId owner_classid, const char *paramname, const char *buffer);
/*
// ��������� ��� ����� ������ ���������� cont � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int Model_GetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int Model_SetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int Model_GetComponentInputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int Model_GetComponentOutputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// ��������� ��� ������� ����� c ����������� cont � ��� ��������� ������������ � ���� xml � ����� buffer
// ���������� � ������ ����������� ������������ ��������� ���������� cont!
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int Model_GetComponentPersonalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
virtual bool Model_SaveComponent(RDK::UNet* cont, RDK::USerStorageXML *serstorage, bool links, unsigned int params_type_mask);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
virtual bool Model_LoadComponent(RDK::UNet* cont, RDK::USerStorageXML *serstorage, bool links);

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
virtual bool Model_SaveComponentProperties(RDK::UNet* cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
virtual bool Model_LoadComponentProperties(RDK::UNet* cont, RDK::USerStorageXML *serstorage);
*/
// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
//virtual bool Model_SaveComponentDrawInfo(RDK::UNet* cont, RDK::USerStorageXML *serstorage);
// --------------------------

// --------------------------
// ������ ����������� ���������� ��������
// --------------------------
protected:
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
public:
// ������������ ����� ���������� �� �������� ���������� id
// ���� ��������� id �� ������, �� ���������� ��������� �� ������
virtual UEPtr<UContainer> FindComponent(const char *stringid) const;

// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

protected:
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

public: // ����������
// ����� ������ ������
struct EErrorEngine: public EError
{
protected: // ������ ����������
int EngineIndex;

public:
EErrorEngine(void)//int engine_index)
: EError(), EngineIndex(0) /*EngineIndex(engine_index) */{};
/*
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" EngineIndex=")+sntoa(EngineIndex);
} */
};

// ��� ������ �� ��������� � ���������
struct EErrorEngineModelNameDontMatch: public EErrorEngine
{
protected: // ������ ����������
/// ���������� ���
std::string GotModelName;

/// ��������� ���
std::string ExpectedModelName;

public:
EErrorEngineModelNameDontMatch(const std::string &got_name, const std::string &expected_name)
: EErrorEngine(), GotModelName(got_name), ExpectedModelName(expected_name) {};

// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const
{
 return EErrorEngine::CreateLogMessage()+std::string(" GotModelName=")+GotModelName+
 				std::string(" ExpectedModelName=")+ExpectedModelName;
}
};
/*
// ��� ������ ���������� �� ����� ���� �������� � ����������
struct EErrorEngineIncompatibleCast: public EErrorEngine
{
protected: // ������ ����������
/// ���������� ���
std::string GotModelName;

/// ��������� ���
std::string ExpectedModelName;

public:
EErrorEngineModelNameDontMatch(const std::string &got_name, const std::string &expected_name)
: EErrorEngine(), GotModelName(got_name), ExpectedModelName(expected_name) {};

// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const
{
 return EErrorEngine::CreateLogMessage()+std::string(" GotModelName=")+GotModelName+
				std::string(" ExpectedModelName=")+ExpectedModelName;
}
};  */
/*
// ��� ������ �� ��������� � ���������
struct EErrorEngineModelNameDontMatch: public EErrorEngine
{
protected: // ������ ����������
/// ���������� ���
std::string GotModelName;

/// ��������� ���
std::string ExpectedModelName;

public:
EErrorEngineModelNameDontMatch(const std::string &got_name, const std::string &expected_name)
: EErrorEngine(), GotModelName(got_name), ExpectedModelName(expected_name) {};

// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const
{
 return EErrorEngine::CreateLogMessage()+std::string(" GotModelName=")+GotModelName+
 				std::string(" ExpectedModelName=")+ExpectedModelName;
}
};
    */
};

// ���������� ������ �� ������ �������� ����������
template<class T>
const T& UEngine::Model_GetComponentPropertyData(const char *stringid, const char *property_name)
{
 try
 {
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
  UEPtr<UVBaseDataProperty<T> > property=dynamic_pointer_cast<UVBaseDataProperty<T> >(iproperty);
  return property.GetData();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// �������� ������ 'data' � �������� �������� ����������
template<class T>
int UEngine::Model_SetComponentPropertyData(const char *stringid, const char *property_name, const T &data)
{
 try
 {
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
  UEPtr<UVBaseDataProperty<T> > property=dynamic_pointer_cast<UVBaseDataProperty<T> >(iproperty);
  property.SetData(data);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

}
#endif
