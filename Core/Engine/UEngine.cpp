/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2009.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UEngine_CPP
#define UEngine_CPP

#include "UEngine.h"
//#include "../../Deploy/Include/rdk_init.h"
//#include "../../Deploy/Include/rdkdll_loader.h"


namespace RDK{

// ������������� dll
DLLPDllInit DLLDllInit=0;

// ��������� �� ������� ������������ ����� �������� � ����������
DLLPGetNumStorages DLLGetNumStorages=0;

// ��������� �� ������� ������������ ����� ���� � ����������
DLLPGetNumEnvironments DLLGetNumEnvironments=0;

// ��������� �� ������� ������������ ����� ������� � ����������
DLLPGetNumEngines DLLGetNumEngines=0;

// ���������� ��������� �� �������
DLLPGetStorage DLLGetStorage=0;

// ���������� ����� �� �������
DLLPGetEnvironment DLLGetEnvironment=0;

// ���������� ������ �� �������
DLLPGetEngine DLLGetEngine=0;

// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
DLLPAddNewStorage DLLAddNewStorage=0;

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
DLLPAddNewEnvironment DLLAddNewEnvironment=0;

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
DLLPAddNewEngine DLLAddNewEngine=0;


// --------------------------
// ������������ � �����������
// --------------------------
UEngine::UEngine(void)
{
 Runned=-1;
 Storage=0;
 Environment=0;
 FuncCreateNewStorage=0;
 FuncCreateNewEnvironment=0;
}

UEngine::~UEngine(void)
{
 Stop();
}
// --------------------------

// --------------------------
// ������ ���������� ����������� �������������
// --------------------------
// ��������� �� ������� �������� ����������� ��������� � �����
UEngine::PCreateNewStorage UEngine::GetFuncCreateNewStorage(void) const
{
 return FuncCreateNewStorage;
}

bool UEngine::SetFuncCreateNewStorage(UEngine::PCreateNewStorage value)
{
 if(FuncCreateNewStorage == value)
  return true;

 FuncCreateNewStorage=value;
 return true;
}

UEngine::PCreateNewEnvironment UEngine::GetFuncCreateNewEnvironment(void) const
{
 return FuncCreateNewEnvironment;
}

bool UEngine::SetFuncCreateNewEnvironment(UEngine::PCreateNewEnvironment value)
{
 if(FuncCreateNewEnvironment == value)
  return true;

 FuncCreateNewEnvironment=value;
 return true;
}

// ��� ����� �������������
const string& UEngine::GetOptionsFileName(void) const
{
 return OptionsFileName;
}

bool UEngine::SetOptionsFileName(const string& value)
{
 if(OptionsFileName == value)
  return true;

 OptionsFileName=value;
 return true;
}
// --------------------------

// --------------------------
// ������ ������� � ���������� ���������
// --------------------------
// ���� ������ �������
int UEngine::IsRunned(void)
{
 return Runned;
}
// --------------------------


// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ��������� �� �����
UAContainerEnvironment* UEngine::GetEnvironment(void)
{
 return Environment;
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� ������ ������
bool UEngine::Init(void)
{
 if(!Options.LoadFromFile(OptionsFileName))
 {
  if(!Default())
   return false;
 }

 StorageIndex=atoi(Options("General","StorageIndex",sntoa(StorageIndex).c_str()));
 EnvironmentIndex=atoi(Options("General","EnvironmentIndex",sntoa(EnvironmentIndex).c_str()));
/*
 if(LoadDll(Options(MainLibrarySectionName,MainLibraryName,"").c_str()))
  return false;
  */
 if(StorageIndex>=0)
  Storage=DLLGetStorage(StorageIndex);
 else
  Storage=DLLAddNewStorage();

 if(!Storage)
  return false;

 if(EnvironmentIndex>=0)
  Environment=DLLGetEnvironment(EnvironmentIndex);
 else
 {
//  if(LoadPredefinedLibraries())
//   return false;
  if(LoadClasses())
   return false;
  if(LoadLibraries())
   return false;
  Environment=DLLAddNewEnvironment(Storage,true,&ClassesList, &LibrariesList);
 }
/*
 Storage=FuncCreateNewStorage();
 Storage=CreateStorage(Storage);
 Environment=FuncCreateNewEnvironment();
 if(LoadPredefinedLibraries())
  return false;
 if(LoadLibraries())
  return false;
 CreateEnvironment(Environment, Storage, true, &LibrariesList);

 if(!Environment)
  return false;
  */
 if(!Storage || !Environment || Environment->GetStorage() != Storage)
 {
  return false;
 }

 return true;
}

// ���������������� ������ ������
// � ��������� ������� ���������
bool UEngine::UnInit(void)
{
 if(!Stop())
  return false;

 Options("General","StorageIndex",sntoa(StorageIndex));
 Options("General","EnvironmentIndex",sntoa(EnvironmentIndex));

 if(!Options.SaveToFile(OptionsFileName))
  return false;

 return true;
}

// ��������� �������
bool UEngine::Start(void)
{
 if(IsRunned() == 1)
 {
  return true;
 }

 if(!IsReady())
  if(!Reset())
  {
   return false;
  }


 Runned=1;
 return true;
}

// ���������������� �������
bool UEngine::Pause(void)
{
 if(IsRunned() == 0)
 {
  return true;
 }

 if(!IsReady())
  if(!Reset())
  {
   return false;
  }

 Runned=0;
 return true;
}

// ������������� �������
bool UEngine::Stop(void)
{
 if(IsRunned() == -1)
 {
  return true;
 }

 Runned=-1;
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
int UEngine::Storage_GetNumClasses(void)
{
 return Storage->GetNumClasses();
}

// ���������� id ������� � ���������. ������ ������ ���� ��������
void UEngine::Storage_GetClassesList(int *buffer) const
{
 Storage->GetClassIdList(buffer,Storage->GetNumClasses());
}

 // ���������� ��� ������ �� ��� id.
const char * UEngine::Storage_GetClassName(int id) const
{
 TempString=Storage->GetClassName(id);
 return TempString.c_str();
}

// ���������� Id ������ �� ��� �����
int UEngine::Storage_GetClassId(const char *name) const
{
 return Storage->GetClassId(name);
}

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
bool UEngine::Storage_DelClass(int classid)
{
 return Storage->DelClass(classid);
}

// �������� ��� ��������� ������� �� ���������
void UEngine::Storage_FreeObjectsStorage(void)
{
 Storage->FreeObjectsStorage();
}

// ������� ��� ������� �� ���������
void UEngine::Storage_ClearObjectsStorage(void)
{
 Storage->ClearObjectsStorage();
}

// ��������� ��������� ����� �������� � ���������
int UEngine::Storage_CalcNumObjects(void) const
{
 return Storage->CalcNumObjects();
}

int UEngine::Storage_CalcNumObjectsById(int classid) const
{
 return Storage->CalcNumObjects(classid);
}

int UEngine::Storage_CalcNumObjectsByName(const char* classname) const
{
 return Storage->CalcNumObjects(classname);
}


// ������ ���������� ������
// ----------------------------
// ������ ������������� �������� ������ ���������
int UEngine::Env_GetPredefinedStructure(void) const
{
 return Environment->GetPredefinedStructure();
}

bool UEngine::Env_SetPredefinedStructure(int value)
{
 return Environment->SetPredefinedStructure(value);
}

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool UEngine::Env_IsStoragePresent(void) const
{
 return Environment->IsStoragePresent();
}

// ���������� ��������� �������������
bool UEngine::Env_IsInit(void) const
{
 return Environment->IsInit();
}

// ������� ������� �������������� ���������
bool UEngine::Env_IsStructured(void) const
{
 return Environment->IsStructured();
}

// ������������� �����
bool UEngine::Env_Init(void)
{
 return Environment->Init();
}

// ��������������� �����
bool UEngine::Env_UnInit(void)
{
 return Environment->UnInit();
}

// ��������� �������������� �������� ������ ���������
bool UEngine::Env_CreateStructure(void)
{
 return Environment->CreateStructure();
}

// ���������� ������� ������ ���������
bool UEngine::Env_DestroyStructure(void)
{
 return Environment->DestroyStructure();
}

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
void UEngine::Env_Destroy(void)
{
 Environment->DestroyModel();
 Storage->ClearObjectsStorage();
 Storage->ClearClassesStorage();
 Environment->DelAllClassLibraries();
}

// ��������� ���������� �� ����� dll-�����
int UEngine::Env_LoadStorageLibrary(const char *filename)
{
 return 0;
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UEngine::Env_DelClassLibraryByIndex(int index)
{
 return Environment->DelClassLibrary(index);
}

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UEngine::Env_DelClassLibraryByName(const char *name)
{
 return Environment->DelClassLibrary(name);
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UEngine::Env_DelAllClassLibraries(void)
{
 return Environment->DelAllClassLibraries();
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool UEngine::Env_BuildStorage(void)
{
 return Environment->BuildStorage();
}

// ���������� ����� ���������
int UEngine::Env_GetNumClassLibraries(void) const
{
 return Environment->GetNumClassLibraries();
}

// ���������� ��� ���������� �� �������
const char * UEngine::Env_GetClassLibraryName(int index)
{
 TempString=Environment->GetClassLibraryName(index);
 return TempString.c_str();
}

// ���������� ������ ���������� �� �������
const char * UEngine::Env_GetClassLibraryVersion(int index)
{
 TempString=Environment->GetClassLibraryVersion(index);
 return TempString.c_str();
}


// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
// ���������� id ������ ������ � ���������
int UEngine::Env_CreateClass(const char* stringid)
{
 return 0;
}

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
int UEngine::Env_Calculate(const char* stringid)
{
 RDK::ULongId id;
 if(!stringid)
 {
  Environment->SetModelCalculationComponent(id);
 }
 else
 {
  RDK::operator<<(id,stringid);
  Environment->SetModelCalculationComponent(id);
 }

 if(!Environment->Calculate())
  return 1;

 return 0;
}

// ������ ���������� �������
// ----------------------------
// ������� ������
int UEngine::Model_Destroy(void)
{
 if(Environment->DestroyModel())
  return 0;

 return -1;
}

// ������� ����� ������ �� id ������ � ���������
// �������������� ������� ������������ ������
int UEngine::Model_Create(int classid)
{
 if(Environment->CreateModel(classid))
  return 0;

 return -1;
}

// ������� ������
int UEngine::Model_Clear(void)
{
 RDK::UAContainer *model=dynamic_cast<RDK::UAContainer *>(Environment->GetModel());

 if(!model)
  return -2;

 model->DelAllComponents();
 return 0;
}

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'classid'
// ���� stringid - ������ ������, �� ��������� � ���� ������
int UEngine::Model_AddComponent(char* stringid, int classid)
{
/* if(!stringid)
  return -10;

 RDK::UAContainer *model=dynamic_cast<RDK::UAContainer *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::UAContainer* cont=Storage->TakeObject(classid);

 if(!cont)
  return -3;

 RDK::UAContainer* destcont=0;
 RDK::ULongId id;
 if(stringid[0] != '\0')
  destcont=model->GetComponentL(RDK::operator<<(id,stringid));
 else
  destcont=model;
                */
 RDK::UAContainer* destcont=FindComponent(stringid);

 RDK::UAContainer* cont=Storage->TakeObject(classid);

 if(!cont)
  return -3;

 if(!destcont)
  return -4;

 return destcont->AddComponent(cont);
}

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'id'
// ���� stringid - ������ ������, �� ������� �� ����� ������
int UEngine::Model_DelComponent(char* stringid, int id)
{         /*
 if(!stringid)
  return -10;

 RDK::UAContainer *model=dynamic_cast<RDK::UAContainer *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::UAContainer* destcont=0;
 RDK::ULongId longid;
 if(stringid[0] != '\0')
  destcont=model->GetComponentL(RDK::operator<<(longid,stringid));
 else
  destcont=model;
*/
 RDK::UAContainer* destcont=FindComponent(stringid);

 if(!destcont)
  return -4;

 if(!destcont->DelComponent(id))
  return -5;

 return 0;
}

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
int UEngine::Model_GetNumComponents(char* stringid)
{      /*
 if(!stringid)
  return -10;

 RDK::UAContainer *model=dynamic_cast<RDK::UAContainer *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::UAContainer* destcont=0;
 RDK::ULongId longid;
 if(stringid[0] != '\0')
  destcont=model->GetComponentL(RDK::operator<<(longid,stringid));
 else
  destcont=model;
         */
 RDK::UAContainer* destcont=FindComponent(stringid);

 if(!destcont)
  return -4;

 return destcont->GetNumComponents();
}

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
int UEngine::Model_GetComponentsList(char* stringid, int *buffer)
{
/* if(!stringid)
  return -10;

 if(!buffer)
  return -20;

 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::UAContainer* destcont=0;
 RDK::ULongId longid;
 if(stringid[0] != '\0')
  destcont=model->GetComponentL(RDK::operator<<(longid,stringid));
 else
  destcont=model;
*/
 RDK::UAContainer* destcont=FindComponent(stringid);

 if(!destcont)
  return -4;

 destcont->GetComponentsList(buffer);

 return 0;
}

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
const char* UEngine::Model_GetComponentName(char* stringid)
{
/* if(!stringid)
  return 0;

 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return 0;

 RDK::UAContainer* destcont=0;
 RDK::ULongId longid;
 if(stringid[0] != '\0')
  destcont=model->GetComponentL(RDK::operator<<(longid,stringid));
 else
  destcont=model;
    */
 RDK::UAContainer* destcont=FindComponent(stringid);

 if(!destcont)
  return 0;

 return destcont->GetName().c_str();
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char* UEngine::Model_GetComponentParameters(const char *stringid)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return 0;

 RDK::ULongId id;
// id.Add(2);

// RDK::UBAbstract* cont=dynamic_cast<RDK::UBAbstract*>(model->GetComponentL(id));
 RDK::UAContainer* cont=model->GetComponentL(RDK::operator<<(id,stringid));
 if(!cont)
  return 0;

 XmlStorage.Create("Parameters");

 if(!Model_GetComponentParameters(cont,&XmlStorage))
  return 0;

 TempString="";
 XmlStorage.Save(TempString);
// strcpy(buffer,str.c_str());
 return TempString.c_str();
}

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char* UEngine::Model_GetComponentSelectedParameters(const char *stringid)
{
 return 0;
}

// ������������� ��������� ���������� �� ��������������
bool UEngine::Model_SetComponentParameters(const char *stringid, const char* buffer)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return false;

 RDK::ULongId id;
// id.Add(2);

// RDK::UBAbstract* cont=dynamic_cast<RDK::UBAbstract*>(model->GetComponentL(id));
 RDK::UAContainer* cont=model->GetComponentL(RDK::operator<<(id,stringid));
 if(!cont)
  return false;

 XmlStorage.Load(buffer,"Parameters");

 if(!Model_SetComponentParameters(cont,&XmlStorage))
  return false;

 return true;
}

// ��������� ��������� ���������� ���� � ������
int UEngine::Model_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 if(!stringid1)
  return -10;

 if(!stringid2)
  return -11;

 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::ULongId longid1, longid2;
 longid1<<stringid1;
 longid2<<stringid2;
 bool res=model->CreateLink(longid1,output_number,longid2,input_number);
 if(!res)
  return -3;

 return 0;
}

// ��������� ��������� �����
int UEngine::Model_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 if(!stringid1)
  return -10;

 if(!stringid2)
  return -11;

 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::ULongId longid1, longid2;
 longid1<<stringid1;
 longid2<<stringid2;
 bool res=model->BreakLink(longid1,output_number,longid2,input_number);
 if(!res)
  return -3;

 return 0;
}

// ��������� ��� �����
int UEngine::Model_BreakAllLinks(void)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return -2;

 model->BreakLinks();

 return 0;
}

// ��������� ��� ������� � �������� ����� ���������� ����������
int UEngine::Model_BreakAllComponentLinks(char* stringid)
{
 return 0;
}

// ��������� ��� ������� ����� ���������� ����������
int UEngine::Model_BreakAllComponentInputLinks(char* stringid)
{
 return 0;
}

// ��������� ��� �������� ����� ���������� ����������
int UEngine::Model_BreakAllComponentOutputLinks(char* stringid)
{
 return 0;
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
const char* UEngine::Model_GetComponentInternalLinks(char* stringid)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return 0;

 XmlStorage.Create("Links");

 if(!Model_GetComponentInternalLinks(cont,&XmlStorage))
  return 0;

 TempString="";
 XmlStorage.Save(TempString);

 return TempString.c_str();
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
int UEngine::Model_SetComponentInternalLinks(char* stringid,const char* buffer)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return -3;

 XmlStorage.Load(buffer,"Links");

 if(!Model_SetComponentInternalLinks(cont,&XmlStorage))
  return -4;

 return 0;
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
const char * UEngine::Model_GetComponentInputLinks(char* stringid)
{
 return 0;
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
const char * UEngine::Model_GetComponentOutputLinks(char* stringid)
{
 return 0;
}

// ���������� ��������� ���������� �� ��������������
const char * UEngine::Model_GetComponentState(const char *stringid)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return 0;

 RDK::ULongId id;
// id.Add(2);

// RDK::UBAbstract* cont=dynamic_cast<RDK::UBAbstract*>(model->GetComponentL(id));
 RDK::UAContainer* cont=model->GetComponentL(RDK::operator<<(id,stringid));
 if(!cont)
  return 0;

 XmlStorage.Create("State");

 if(!Model_GetComponentState(cont,&XmlStorage))
  return 0;

 TempString="";
 XmlStorage.Save(TempString);
 return TempString.c_str();
}

// ���������� ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * UEngine::Model_GetComponentSelectedState(const char *stringid)
{
 return 0;
}

// ������������� ��������� ���������� �� ��������������
bool UEngine::Model_SetComponentState(const char *stringid, const char* buffer)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return false;

 RDK::ULongId id;
// id.Add(2);

// RDK::UBAbstract* cont=dynamic_cast<RDK::UBAbstract*>(model->GetComponentL(id));
 RDK::UAContainer* cont=model->GetComponentL(RDK::operator<<(id,stringid));
 if(!cont)
  return false;

 XmlStorage.Load(buffer,"State");

 if(!Model_SetComponentState(cont,&XmlStorage))
  return false;

 return true;
}

// ���������� ����� ������ � ����������
int UEngine::Model_GetComponentNumInputs(const char *stringid)
{
 return 0;
}

// ���������� ������ ����� ���������� � ����� ���������
int UEngine::Model_GetComponentInputSize(const char *stringid, int index)
{
 return 0;
}

// ���������� ������ �������� ����� � ������
int UEngine::Model_GetComponentInputElementSize(const char *stringid, int index)
{
 return 0;
}

// ���������� ������ ����� ���������� � ������ ���������
int UEngine::Model_GetComponentInputByteSize(const char *stringid, int index)
{
 return 0;
}

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
unsigned char* UEngine::Model_GetComponentInputData(const char *stringid, int index)
{
 return 0;
}

// ���������� ����� ������� � ����������
int UEngine::Model_GetComponentNumOutputs(const char *stringid)
{
 return 0;
}

// ���������� ������ ������ ���������� � ����� ���������
int UEngine::Model_GetComponentOutputSize(const char *stringid, int index)
{
 return 0;
}

// ���������� ������ �������� ������ � ������
int UEngine::Model_GetComponentOutputElementSize(const char *stringid, int index)
{
 return 0;
}

// ���������� ������ ������ ���������� � ������ ���������
int UEngine::Model_GetComponentOutputByteSize(const char *stringid, int index)
{
 return 0;
}

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
unsigned char* UEngine::Model_GetComponentOutputData(const char *stringid, int index)
{
 return 0;
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
const char *  UEngine::Model_SaveComponent(const char *stringid)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return 0;

 XmlStorage.Create("Save");

 if(!Model_SaveComponent(cont,&XmlStorage))
  return 0;

 std::string str;
 XmlStorage.Save(str);
// strcpy(buffer,str.c_str());

 return str.c_str();
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int UEngine::Model_LoadComponent(const char *stringid, char* buffer)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return -3;

 XmlStorage.Load(buffer,"Save");
 XmlStorage.SelectNode(0);

 if(!Model_LoadComponent(cont,&XmlStorage))
  return -4;

 return 0;
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
const char * UEngine::Model_SaveComponentParameters(const char *stringid)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return 0;

 XmlStorage.Create("SaveParameters");

 if(!Model_SaveComponentParameters(cont,&XmlStorage))
  return 0;

 TempString="";
 XmlStorage.Save(TempString);

 return TempString.c_str();
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
int UEngine::Model_LoadComponentParameters(const char *stringid, char* buffer)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return -3;

 XmlStorage.Load(buffer,"SaveParameters");
 XmlStorage.SelectNode(0);

 if(!Model_LoadComponentParameters(cont,&XmlStorage))
  return -4;

 return 0;
}

// ��������� ��������� ���������� � ��� �������� ��������� � xml
const char * UEngine::Model_SaveComponentState(const char *stringid)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return 0;

 XmlStorage.Create("SaveState");

 if(!Model_SaveComponentState(cont,&XmlStorage))
  return 0;

 TempString="";
 XmlStorage.Save(TempString);

 return TempString.c_str();
}

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
int UEngine::Model_LoadComponentState(const char *stringid, char* buffer)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return -3;

 XmlStorage.Load(buffer,"SaveState");
 XmlStorage.SelectNode(0);

 if(!Model_LoadComponentState(cont,&XmlStorage))
  return -4;

 return 0;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
bool UEngine::Model_GetComponentParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 RDK::UAContainer::VariableMapT props=cont->GetPropertiesList();

 RDK::UAContainer::VariableMapCIteratorT I,J;

 I=props.begin();
 J=props.end();
 while(I != J)
 {
  cont->GetProperty(I->second.Id,serstorage);
  ++I;
 }

 return true;
}

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
bool UEngine::Model_GetComponentSelectedParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 return true;
}

// ������������� ��������� ���������� �� ��������������
bool UEngine::Model_SetComponentParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 std::string name;
// name=XmlStorage.GetNodeName();
// if(name != cont->GetName())
//  return false;

 RDK::UAContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->SetProperty(I->second.Id,serstorage);
   ++I;
  }

 return true;
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
int UEngine::Model_GetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 ULinksList linkslist;
 cont->GetLinks(linkslist, cont);

 *serstorage<<linkslist;

 return true;
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
int UEngine::Model_SetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 ULinksList linkslist;
 *serstorage>>linkslist;

 cont->BreakLinks();
 cont->CreateLinks(linkslist);

 return true;
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
int UEngine::Model_GetComponentInputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 return true;
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
int UEngine::Model_GetComponentOutputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 return true;
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
bool UEngine::Model_GetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 RDK::UAContainer::VariableMapT props=cont->GetStateList();

 RDK::UAContainer::VariableMapCIteratorT I,J;

 I=props.begin();
 J=props.end();
 while(I != J)
 {
  cont->GetState(I->second.Id,serstorage);
  ++I;
 }

 return true;
}

// ���������� ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
bool UEngine::Model_GetComponentSelectedState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 return true;
}

// ������������� ��������� ���������� �� ��������������
bool UEngine::Model_SetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 std::string name;
// name=XmlStorage.GetNodeName();
// if(name != cont->GetName())
//  return false;

 RDK::UAContainer::VariableMapT props=cont->GetStateList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->SetState(I->second.Id,serstorage);
   ++I;
  }

 return true;
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
int UEngine::Model_SaveComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 serstorage->AddNode(cont->GetName());
 serstorage->SetNodeAttribute("Class",RDK::sntoa(cont->GetClass()));
 serstorage->AddNode("Parameters");
 if(!Model_GetComponentParameters(cont, serstorage))
  return false;
 serstorage->SelectUp();

 serstorage->AddNode("Links");
 if(!Model_GetComponentInternalLinks(cont,serstorage))
  return false;
 serstorage->SelectUp();

 serstorage->AddNode("Components");
 for(int i=0;i<cont->GetNumComponents();i++)
 {
  if(!Model_SaveComponent(dynamic_cast<RDK::UANet*>(cont->GetComponentByIndex(i)),serstorage))
   return false;
 }
 serstorage->SelectUp();

 serstorage->SelectUp();

 return true;
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int UEngine::Model_LoadComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

// serstorage->SelectNode(cont->GetName());
 UId id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
 if(cont->GetClass() != id)
  return false;

 serstorage->SelectNode("Parameters");
 if(!Model_SetComponentParameters(cont, serstorage))
  return false;
 serstorage->SelectUp();

// cont->SetName(serstorage->GetNodeName());

 cont->DelAllComponents();

 serstorage->SelectNode("Components");
 UAStorage* storage=cont->GetStorage();
 for(int i=0;i<serstorage->GetNumNodes();i++)
 {
  serstorage->SelectNode(i);
  std::string nodename=serstorage->GetNodeName();
  id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
  UANet *newcont=dynamic_cast<UANet*>(storage->TakeObject(id));
  if(!newcont)
   return false;
  if(cont->AddComponent(newcont) == ForbiddenId)
   return false;

//  newcont->SetName(serstorage->GetNodeName());
//  serstorage->SelectUp();
  if(!Model_LoadComponent(newcont,serstorage))
   return false;
  serstorage->SelectUp();
 }
 serstorage->SelectUp();

 serstorage->SelectNode("Links");
 if(!Model_SetComponentInternalLinks(cont,serstorage))
  return false;
 serstorage->SelectUp();

// serstorage->SelectUp();

 return true;
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
int UEngine::Model_SaveComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 serstorage->AddNode(cont->GetName());
 serstorage->SetNodeAttribute("Class",RDK::sntoa(cont->GetClass()));
 serstorage->AddNode("Parameters");
 if(!Model_GetComponentParameters(cont, serstorage))
  return false;
 serstorage->SelectUp();

 serstorage->AddNode("Components");
 for(int i=0;i<cont->GetNumComponents();i++)
 {
  if(!Model_SaveComponentParameters(dynamic_cast<RDK::UANet*>(cont->GetComponentByIndex(i)),serstorage))
   return false;
 }
 serstorage->SelectUp();

 serstorage->SelectUp();

 return true;
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
int UEngine::Model_LoadComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 UId id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
 if(cont->GetClass() != id)
  return false;

 serstorage->SelectNode("Parameters");
 if(!Model_SetComponentParameters(cont, serstorage))
  return false;
 serstorage->SelectUp();

 serstorage->SelectNode("Components");
 for(int i=0;i<cont->GetNumComponents();i++)
 {
  if(!serstorage->SelectNode(cont->GetComponentByIndex(i)->GetName()))
   continue;
  std::string nodename=serstorage->GetNodeName();

  if(!Model_LoadComponentParameters(dynamic_cast<RDK::UANet*>(cont->GetComponentByIndex(i)),serstorage))
   return false;
  serstorage->SelectUp();
 }
 serstorage->SelectUp();

 return true;
}

// ��������� ��������� ���������� � ��� �������� ��������� � xml
int UEngine::Model_SaveComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 serstorage->AddNode(cont->GetName());
 serstorage->SetNodeAttribute("Class",RDK::sntoa(cont->GetClass()));
 serstorage->AddNode("State");
 if(!Model_GetComponentState(cont, serstorage))
  return false;
 serstorage->SelectUp();

 serstorage->AddNode("Components");
 for(int i=0;i<cont->GetNumComponents();i++)
 {
  if(!Model_SaveComponentState(dynamic_cast<RDK::UANet*>(cont->GetComponentByIndex(i)),serstorage))
   return false;
 }
 serstorage->SelectUp();

 serstorage->SelectUp();

 return true;
}

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
int UEngine::Model_LoadComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 UId id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
 if(cont->GetClass() != id)
  return false;

 serstorage->SelectNode("State");
 if(!Model_SetComponentState(cont, serstorage))
  return false;
 serstorage->SelectUp();

 serstorage->SelectNode("Components");
 for(int i=0;i<cont->GetNumComponents();i++)
 {
  if(!serstorage->SelectNode(cont->GetComponentByIndex(i)->GetName()))
   continue;
  std::string nodename=serstorage->GetNodeName();

  if(!Model_LoadComponentState(dynamic_cast<RDK::UANet*>(cont->GetComponentByIndex(i)),serstorage))
   return false;
  serstorage->SelectUp();
 }
 serstorage->SelectUp();

 return true;

}
// --------------------------


// --------------------------
// ������ ����������� ���������� �������
// --------------------------
// ��������� ����� ����������������� ���������
int UEngine::LoadPredefinedLibraries(void)
{
 return 0;
}

// ��������� ����� �������
int UEngine::LoadClasses(void)
{
 vector<string> variables;
 int res=0;
 UAContainer *dllclass=0;

 Options.GetVariableList(ComponentClassesSectionName, variables);

 ClassesList.clear();
 for(size_t i=0;i<variables.size();i++)
 {
  //dllclass=LoadUClass(Options(ComponentClassesSectionName,variables[i],""),variables[i]);
  if(!dllclass)
   res|=1;
  ClassesList.push_back(dllclass);
 }
 return res;
}

// ��������� ����� ��������� � ��������� ��������� �� ��� � ������ external_libs
// ���������� 0 � ������ ������
// ������ ���� ��������� ������� �� ini-����� 'inifile', ������ 'section'
int UEngine::LoadLibraries(void)
{
 vector<string> variables;
 int res=0;
 UALibrary *library=0;

 Options.GetVariableList(ComponentLibrariesSectionName, variables);

 LibrariesList.clear();
 for(size_t i=0;i<variables.size();i++)
 {
  //library=LoadULibrary(Options(ComponentLibrariesSectionName,variables[i],""),variables[i]);
  if(!library)
   res|=1;
  LibrariesList.push_back(library);
 }
 return res;
}
// --------------------------

// --------------------------
// ������ ����������� ���������� ��������
// --------------------------
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ������������ ����� ���������� �� �������� ���������� id
// ���� ��������� id �� ������, �� ���������� ��������� �� ������
UAContainer* UEngine::FindComponent(const char *stringid)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return 0;

 RDK::ULongId longid;

 RDK::UAContainer *cont=0;
 if(!stringid)
  cont=model;
 else
 {
  longid<<stringid;
  if(!longid.GetSize() || longid[0] == ForbiddenId)
   cont=model;
  else
   cont=dynamic_cast<RDK::UAContainer *>(model->GetComponentL(longid));
 }

 return cont;
}

// �������������� �������� �� ��������� � ����� �������� �����
bool UEngine::ADefault(void)
{
 // ��� ����� �������������
 OptionsFileName="options.ini";

 // ��� ������ ������ �������� ����������
 MainLibrarySectionName="MainDLL";

 // ��� ���������� �������� ����������
 MainLibraryName="MainLibraryName";
 Options(MainLibrarySectionName,MainLibraryName,string("nmsdk.dll"));

 // ��� ������ ������ ��������� ���������
 ComponentLibrariesSectionName="ComponentLibraries";

 ComponentClassesSectionName="ComponentClasses";

 // ������ ������������� ��������� � ����������
 // ���� < 0, �� ����� ��������� ����� �������
 StorageIndex=-1;
 Options("General","StorageIndex",sntoa(StorageIndex));

 // ������ ������������ ����� � ����������
 // ���� < 0, �� ����� ����� ����� �������
 EnvironmentIndex=-1;
 Options("General","EnvironmentIndex",sntoa(EnvironmentIndex));

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UEngine::ABuild(void)
{
/* if(!Environment)
  return false;

 return Environment->Build();
*/
 return true;
}

// ����� �������� �����.
bool UEngine::AReset(void)
{
/* if(!Environment)
  return false;

 return Environment->Reset(); */

 return true;
}

// ��������� ������ ����� �������
bool UEngine::ACalculate(void)
{
/* if(!Environment)
  return false;

 return Environment->Calculate();*/
 return true;
}
// --------------------------

}
#endif
