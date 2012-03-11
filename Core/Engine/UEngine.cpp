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
//#include "UEnvException.h"
#include "UXMLEnvSerialize.h"

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
 try
 {
     return Storage->GetNumClasses();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� id ������� � ���������. ������ ������ ���� ��������
void UEngine::Storage_GetClassesList(int *buffer) const
{
 try
 {
  std::vector<UId> temp;
  Storage->GetClassIdList(temp);
  if(temp.size())
   memcpy(buffer,&temp[0],temp.size()*sizeof(UId));
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
}

 // ���������� ��� ������ �� ��� id.
const char * UEngine::Storage_GetClassName(int id) const
{
 try
 {
  TempString=Storage->GetClassName(id);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ���������� Id ������ �� ��� �����
int UEngine::Storage_GetClassId(const char *name) const
{
 try
 {
  return Storage->GetClassId(name);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
bool UEngine::Storage_DelClass(int classid)
{
 try
 {
  Storage->DelClass(classid);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return true;
}

// �������� ��� ��������� ������� �� ���������
void UEngine::Storage_FreeObjectsStorage(void)
{
 try
 {
  Storage->FreeObjectsStorage();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
}

// ������� ��� ������� �� ���������
void UEngine::Storage_ClearObjectsStorage(void)
{
 try
 {
  Storage->ClearObjectsStorage();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
}

// ��������� ��������� ����� �������� � ���������
int UEngine::Storage_CalcNumObjects(void) const
{
 try
 {
  return Storage->CalcNumObjects();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

int UEngine::Storage_CalcNumObjectsById(int classid) const
{
 try
 {
  return Storage->CalcNumObjects(classid);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

int UEngine::Storage_CalcNumObjectsByName(const char* classname) const
{
 try
 {
  return Storage->CalcNumObjects(classname);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� �������� ������ �� ��� id � ������� xml
const char* UEngine::Storage_GetClassDescription(int classid)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Create(sntoa(classid));
  Storage->SaveClassDescription(classid,xml);
  xml.SelectUp();
  xml.Save(TempString);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ������������� �������� ������ �� ��� id, �������� ��� �� ������� xml
bool UEngine::Storage_SetClassDescription(int classid, const char* description)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Load(description, sntoa(classid));
  Storage->LoadClassDescription(classid,xml);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return true;
}

// ��������� �������� ���� ������� � xml
const char* UEngine::Storage_SaveClassesDescription(void)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Create("ClassesDescription");
  Storage->SaveClassesDescription(xml);
  xml.SelectUp();
  xml.Save(TempString);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ��������� �������� ���� ������� �� xml
bool UEngine::Storage_LoadClassesDescription(const char* xmltext)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Load(xmltext, "ClassesDescription");
  Storage->LoadClassesDescription(xml);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return true;
}

// ��������� ����� �������� ���� ������� � xml
const char* UEngine::Storage_SaveCommonClassesDescription(void)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Create("ClassesDescription");
  Storage->SaveCommonClassesDescription(xml);
  xml.SelectUp();
  xml.Save(TempString);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ��������� ����� �������� ���� ������� �� xml
bool UEngine::Storage_LoadCommonClassesDescription(const char* xmltext)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Load(xmltext, "ClassesDescription");
  return Storage->LoadCommonClassesDescription(xml);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}


// ������ ���������� ������
// ----------------------------
// ������ ������������� �������� ������ ���������
int UEngine::Env_GetPredefinedStructure(void) const
{
 try
 {
  return Environment->GetPredefinedStructure();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

bool UEngine::Env_SetPredefinedStructure(int value)
{
 try
 {
  return Environment->SetPredefinedStructure(value);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool UEngine::Env_IsStoragePresent(void) const
{
 try
 {
  return Environment->IsStoragePresent();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ���������� ��������� �������������
bool UEngine::Env_IsInit(void) const
{
 try
 {
  return Environment->IsInit();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ������� ������� �������������� ���������
bool UEngine::Env_IsStructured(void) const
{
 try
 {
  return Environment->IsStructured();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ������������� �����
bool UEngine::Env_Init(void)
{
 try
 {
  return Environment->Init();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ��������������� �����
bool UEngine::Env_UnInit(void)
{
 try
 {
  return Environment->UnInit();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ��������� �������������� �������� ������ ���������
bool UEngine::Env_CreateStructure(void)
{
 try
 {
  return Environment->CreateStructure();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ���������� ������� ������ ���������
bool UEngine::Env_DestroyStructure(void)
{
 try
 {
  return Environment->DestroyStructure();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
void UEngine::Env_Destroy(void)
{
 try
 {
  Environment->DestroyModel();
  Storage->ClearObjectsStorage();
  Storage->ClearClassesStorage();
  Environment->DelAllClassLibraries();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
}

// ��������� ���������� �� ����� dll-�����
int UEngine::Env_LoadStorageLibrary(const char *filename)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UEngine::Env_DelClassLibraryByIndex(int index)
{
 try
 {
  return Environment->DelClassLibrary(index);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UEngine::Env_DelClassLibraryByName(const char *name)
{
 try
 {
  return Environment->DelClassLibrary(name);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UEngine::Env_DelAllClassLibraries(void)
{
 try
 {
  return Environment->DelAllClassLibraries();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool UEngine::Env_BuildStorage(void)
{
 try
 {
  return Environment->BuildStorage();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ���������� ����� ���������
int UEngine::Env_GetNumClassLibraries(void) const
{
 try
 {
  return Environment->GetNumClassLibraries();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ��� ���������� �� �������
const char * UEngine::Env_GetClassLibraryName(int index)
{
 try
 {
  TempString=Environment->GetClassLibraryName(index);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ���������� ������ ���������� �� �������
const char * UEngine::Env_GetClassLibraryVersion(int index)
{
 try
 {
  TempString=Environment->GetClassLibraryVersion(index);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}


// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
// ���������� id ������ ������ � ���������
int UEngine::Env_CreateClass(const char* stringid)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
int UEngine::Env_Calculate(const char* stringid)
{
 try
 {
  RDK::ULongId id;
  if(!stringid)
  {
   Environment->SetModelCalculationComponent(id);
  }
  else
  {
   id.DecodeFromString(stringid);
   Environment->SetModelCalculationComponent(id);
  }

  if(!Environment->Calculate())
   return 1;
 }
 catch (RDK::Exception * exception)
 {
  ProcessException(exception);
 }


 return 0;
}


// ����� ������ �����
// ���� stringid == 0 �� ���������� ��� ������ �������,
// ����� - ������ ��������� ��������� ������
int UEngine::Env_Reset(const char* stringid)
{
 try
 {
  RDK::ULongId id;
  if(!stringid)
  {
   Environment->SetModelCalculationComponent(id);
  }
  else
  {
   id.DecodeFromString(stringid);
   Environment->SetModelCalculationComponent(id);
  }

  if(!Environment->Reset())
   return 1;
 }
 catch (RDK::Exception * exception)
 {
  ProcessException(exception);
 }


 return 0;
}

// !!! ��������� ������ ���������� ������� ����������� ������ �� ���
// ������, ������������ � ����������� �� ���������� id !!!
// ������������� ������� ��������� (��������� ������������ ����� - ������)
int UEngine::Env_SelectCurrentComponent(const char *stringid)
{
 try
 {
  Environment->ResetCurrentComponent();

  // ���� ������ ������ - �����, �� ��������� ��� ��������� id
  if(stringid[0]>=0x30 && stringid[0]<=0x39)
  {
   ULongId longid;
   longid.DecodeFromString(stringid);
   Environment->SelectCurrentComponent(longid);
  }
  else // ...����� ���������� ��� ��������� ���
  {
   Environment->SelectCurrentComponent(stringid);
  }
 }
 catch (RDK::Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ������� ��������� � ��������� �� ��������� (������)
int UEngine::Env_ResetCurrentComponent(const char *stringid)
{
 try
 {
  Environment->ResetCurrentComponent();
 }
 catch (RDK::Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ������ ������� ��������� �� ��� �������� (������ �� ������� �����)
// ���� ��� �� ������� ������, �� �� ������ ������
int UEngine::Env_UpCurrentComponent(void)
{
 try
 {
  Environment->UpCurrentComponent();
 }
 catch (RDK::Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ������ ������� ��������� �� ��� �������� �� ������������ ������ �����������
// (����� �� N ������� ���� ������������ �������� ����������)
int UEngine::Env_DownCurrentComponent(const char *stringid)
{
 try
 {
  // ���� ������ ������ - �����, �� ��������� ��� ��������� id
  if(stringid[0]>=0x30 && stringid[0]<=0x39)
  {
   ULongId longid;
   longid.DecodeFromString(stringid);
   Environment->DownCurrentComponent(longid);
  }
  else // ...����� ���������� ��� ��������� ���
  {
   Environment->DownCurrentComponent(stringid);
  }
 }
 catch (RDK::Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ������� ��� �������� ����������
const char* UEngine::Env_GetCurrentComponentName(void) const
{
 try
 {
  Environment->GetCurrentComponent()->GetLongName(Environment->GetModel(),TempString);
 }
 catch (RDK::Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ���������� ������� ��������� id �������� ����������
const char* UEngine::Env_GetCurrentComponentId(void) const
{
 try
 {
  ULongId longid;
  Environment->GetCurrentComponent()->GetLongId(Environment->GetModel(),longid);
  longid.EncodeToString(TempString);
 }
 catch (RDK::Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ������ ���������� �������
// ----------------------------
// ������� ������
int UEngine::Model_Destroy(void)
{
 try
 {
  if(Environment->DestroyModel())
   return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return -1;
}

// ������� ����� ������ �� id ������ � ���������
// �������������� ������� ������������ ������
int UEngine::Model_Create(int classid)
{
 try
 {
  if(Environment->CreateModel(classid))
   return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return -1;
}

// ������� ������
int UEngine::Model_Clear(void)
{
 try
 {
  UEPtr<RDK::UAContainer> model=dynamic_pointer_cast<RDK::UAContainer>(Environment->GetModel());

  if(!model)
   return -2;

  model->DelAllComponents();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'classid'
// ���� stringid - ������ ������, �� ��������� � ���� ������
int UEngine::Model_AddComponent(const char* stringid, int classid)
{
 try
 {
  UEPtr<RDK::UAContainer> destcont=FindComponent(stringid);

  UEPtr<RDK::UAContainer> cont=dynamic_pointer_cast<RDK::UAContainer>(Storage->TakeObject(classid));

  if(!cont)
   return -3;

  if(!destcont)
   return -4;

  return destcont->AddComponent(cont);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'id'
// ���� stringid - ������ ������, �� ������� �� ����� ������
int UEngine::Model_DelComponent(const char* stringid, int id)
{
 try
 {
  RDK::UAContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return -4;

  destcont->DelComponent(id);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
int UEngine::Model_GetNumComponents(const char* stringid)
{
 try
 {
  RDK::UAContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return -4;

  return destcont->GetNumComponents();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
int UEngine::Model_GetComponentsList(const char* stringid, int *buffer)
{
 try
 {
  RDK::UAContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return -4;

  std::vector<int> tempbuffer;

  destcont->GetComponentsList(tempbuffer);
  if(tempbuffer.size())
   memcpy(buffer,&tempbuffer[0],tempbuffer.size()*sizeof(int));
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
const char* UEngine::Model_GetComponentName(const char* stringid)
{
 try
 {
  RDK::UAContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return 0;

  return destcont->GetName().c_str();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ��������� ���������� �� ��������������
const char* UEngine::Model_GetComponentParameters(const char *stringid)
{
 try
 {
  TempString="";
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);

  if(!cont)
   return TempString.c_str();

  XmlStorage.Create(cont->GetLongName(Environment->GetCurrentComponent(),CompName));
  XmlStorage.AddNode("Parameters");

  if(!Model_GetComponentParameters(cont,&XmlStorage))
   return 0;

  XmlStorage.SelectUp();
  XmlStorage.Save(TempString);
  return TempString.c_str();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ���������� ��������� ���������� �� ��������������
const char* UEngine::Model_GetComponentSelectedParameters(const char *stringid)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ��������� ���������� �� �������������� � ����������
const char* UEngine::Model_GetComponentParametersEx(const char *stringid)
{
 try
 {
  TempString="";
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  if(!cont)
   return TempString.c_str();

  XmlStorage.Create(cont->GetLongName(Environment->GetCurrentComponent(),CompName));
  XmlStorage.AddNode("Parameters");

  if(!Model_GetComponentParametersEx(cont,&XmlStorage))
   return 0;

  XmlStorage.SelectUp();
  XmlStorage.Save(TempString);
  return TempString.c_str();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
const char * UEngine::Model_GetComponentParameterValue(const char *stringid, const char *paramname)
{
 try
 {
  TempString="";
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  if(!cont)
   return TempString.c_str();

  cont->GetPropertyValue(paramname,TempString);
  return TempString.c_str();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ������������� ��������� ���������� �� ��������������
bool UEngine::Model_SetComponentParameters(const char *stringid, const char* buffer)
{
 try
 {
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  if(!cont)
   return false;

  XmlStorage.Load(buffer, cont->GetLongName(Environment->GetCurrentComponent(),CompName));
  XmlStorage.SelectNode("Parameters");

  if(!Model_SetComponentParameters(cont,&XmlStorage))
   return false;
  XmlStorage.SelectUp();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ������������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
void UEngine::Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer)
{
 try
 {
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  if(!cont)
   return;

  cont->SetPropertyValue(paramname,buffer);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
}

// ��������� ��������� ���������� ���� � ������
int UEngine::Model_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 try
 {
  if(!stringid1)
   return -10;

  if(!stringid2)
   return -11;

  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return -2;

  RDK::ULongId longid1, longid2;
  longid1.DecodeFromString(stringid1);
  longid2.DecodeFromString(stringid2);
  bool res=model->CreateLink(longid1,output_number,longid2,input_number);
  if(!res)
   return -3;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��������� �����
int UEngine::Model_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 try
 {
  if(!stringid1)
   return -10;

  if(!stringid2)
   return -11;

  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return -2;

  RDK::ULongId longid1, longid2;
  longid1.DecodeFromString(stringid1);
  longid2.DecodeFromString(stringid2);
  bool res=model->BreakLink(longid1,output_number,longid2,input_number);
  if(!res)
   return -3;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ��������� ��� �����
int UEngine::Model_BreakAllLinks(void)
{
 try
 {
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return -2;

  model->BreakLinks();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ��������� ��� ������� � �������� ����� ���������� ����������
int UEngine::Model_BreakAllComponentLinks(char* stringid)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� ������� ����� ���������� ����������
int UEngine::Model_BreakAllComponentInputLinks(char* stringid)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� �������� ����� ���������� ����������
int UEngine::Model_BreakAllComponentOutputLinks(char* stringid)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
const char* UEngine::Model_GetComponentInternalLinks(char* stringid)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.Create("Links");

  if(!Model_GetComponentInternalLinks(cont,&XmlStorage))
   return 0;

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
int UEngine::Model_SetComponentInternalLinks(char* stringid,const char* buffer)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return -3;

  XmlStorage.Load(buffer,"Links");

  if(!Model_SetComponentInternalLinks(cont,&XmlStorage))
   return -4;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
const char * UEngine::Model_GetComponentInputLinks(char* stringid)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
const char * UEngine::Model_GetComponentOutputLinks(char* stringid)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ��������� ���������� �� ��������������
const char * UEngine::Model_GetComponentState(const char *stringid)
{
 try
 {
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return 0;

  RDK::ULongId id;
  string namebuffer;

  UEPtr<RDK::UAContainer> cont=model->GetComponentL(id.DecodeFromString(stringid));
  if(!cont)
   return 0;

  XmlStorage.Create(cont->GetLongName(model,namebuffer));
  XmlStorage.AddNode("State");

  if(!Model_GetComponentState(cont,&XmlStorage))
   return 0;
  XmlStorage.SelectUp();

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ���������� ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * UEngine::Model_GetComponentSelectedState(const char *stringid)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ���������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
const char * UEngine::Model_GetComponentStateValue(const char *stringid, const char *statename)
{
 try
 {
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return 0;

  RDK::ULongId id;
//  string namebuffer;

  UEPtr<RDK::UAContainer> cont=model->GetComponentL(id.DecodeFromString(stringid));
  if(!cont)
   return 0;

//  XmlStorage.Create(cont->GetLongName(model,namebuffer));
//  XmlStorage.AddNode("Parameters");

  cont->GetStateValue(statename,TempString);

//  XmlStorage.SelectUp();
//  TempString="";
//  XmlStorage.Save(TempString);
  return TempString.c_str();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ������������� ��������� ���������� �� ��������������
bool UEngine::Model_SetComponentState(const char *stringid, const char* buffer)
{
 try
 {
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return false;

  RDK::ULongId id;
  string namebuffer;

  UEPtr<RDK::UAContainer> cont=model->GetComponentL(id.DecodeFromString(stringid));
  if(!cont)
   return false;

  XmlStorage.Load(buffer, cont->GetLongName(model,namebuffer));
  XmlStorage.SelectNode("State");

  if(!Model_SetComponentState(cont,&XmlStorage))
   return false;
  XmlStorage.SelectUp();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return true;
}


// ������������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
void UEngine::Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer)
{
 try
 {
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return;

  RDK::ULongId id;
//  string namebuffer;

  UEPtr<RDK::UAContainer> cont=model->GetComponentL(id.DecodeFromString(stringid));
  if(!cont)
   return;

//  XmlStorage.Load(buffer, cont->GetLongName(model,namebuffer));
//  XmlStorage.SelectNode("Parameters");

  cont->SetStateValue(statename,buffer);
//  XmlStorage.SelectUp();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
}

// ���������� ����� ������ � ����������
int UEngine::Model_GetComponentNumInputs(const char *stringid)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ������ ����� ���������� � ����� ���������
int UEngine::Model_GetComponentInputSize(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ������ �������� ����� � ������
int UEngine::Model_GetComponentInputElementSize(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ������ ����� ���������� � ������ ���������
int UEngine::Model_GetComponentInputByteSize(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
unsigned char* UEngine::Model_GetComponentInputData(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ����� ������� � ����������
int UEngine::Model_GetComponentNumOutputs(const char *stringid)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ������ ������ ���������� � ����� ���������
int UEngine::Model_GetComponentOutputSize(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ������ �������� ������ � ������
int UEngine::Model_GetComponentOutputElementSize(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ������ ������ ���������� � ������ ���������
int UEngine::Model_GetComponentOutputByteSize(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
unsigned char* UEngine::Model_GetComponentOutputData(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
const char *  UEngine::Model_SaveComponent(const char *stringid)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.Create("Save");

  if(!Model_SaveComponent(cont,&XmlStorage))
   return 0;

  XmlStorage.Save(TempString);
 // strcpy(buffer,str.c_str());
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int UEngine::Model_LoadComponent(const char *stringid, char* buffer)
{
 try
 {
  XmlStorage.Load(buffer,"Save");
  XmlStorage.SelectNode(0);

  if(!Environment->GetModel())
  {
   if(!Model_LoadComponent(0,&XmlStorage))
	return -4;
  }
  else
  {
   UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

   if(!cont)
	return -3;

   if(!Model_LoadComponent(cont,&XmlStorage))
    return -4;
  }
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
const char * UEngine::Model_SaveComponentParameters(const char *stringid)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.Create("SaveParameters");

  if(!Model_SaveComponentParameters(cont,&XmlStorage))
   return 0;

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
int UEngine::Model_LoadComponentParameters(const char *stringid, char* buffer)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return -3;

  XmlStorage.Load(buffer,"SaveParameters");
  XmlStorage.SelectNode(0);

  if(!Model_LoadComponentParameters(cont,&XmlStorage))
   return -4;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��������� ���������� � ��� �������� ��������� � xml
const char * UEngine::Model_SaveComponentState(const char *stringid)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.Create("SaveState");

  if(!Model_SaveComponentState(cont,&XmlStorage))
   return 0;

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
int UEngine::Model_LoadComponentState(const char *stringid, char* buffer)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return -3;

  XmlStorage.Load(buffer,"SaveState");
  XmlStorage.SelectNode(0);

  if(!Model_LoadComponentState(cont,&XmlStorage))
   return -4;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
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
 try
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
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
bool UEngine::Model_GetComponentSelectedParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ���������� ��������� ���������� �� �������������� � ����������
// ������ ��� buffer ������ ���� ��������!
bool UEngine::Model_GetComponentParametersEx(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  RDK::UAContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  UEPtr<UContainerDescription> descr=dynamic_pointer_cast<UContainerDescription>(Storage->GetClassDescription(cont->GetClass()));

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->GetProperty(I->second.Id,serstorage);

   if(descr)
   {
    std::string paramname=I->second.Property->GetName();
    if(serstorage->SelectNode(paramname))
    {
     serstorage->SetNodeAttribute("Header",descr->GetParameter(paramname).Header);
     serstorage->SelectUp();
    }
   }
   ++I;
  }
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}


// ������������� ��������� ���������� �� ��������������
bool UEngine::Model_SetComponentParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  std::string name;

  RDK::UAContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->SetProperty(I->second.Id,serstorage);
   ++I;
  }
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return true;
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
int UEngine::Model_GetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  ULinksList linkslist;
  cont->GetLinks(linkslist, cont);

  *serstorage<<linkslist;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }
 return true;
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
int UEngine::Model_SetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  ULinksList linkslist;
  *serstorage>>linkslist;

  cont->BreakLinks();
  cont->CreateLinks(linkslist);
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
int UEngine::Model_GetComponentInputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
int UEngine::Model_GetComponentOutputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
bool UEngine::Model_GetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
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
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ���������� ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
bool UEngine::Model_GetComponentSelectedState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ������������� ��������� ���������� �� ��������������
bool UEngine::Model_SetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  std::string name;

  RDK::UAContainer::VariableMapT props=cont->GetStateList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->SetState(I->second.Id,serstorage);
   ++I;
  }
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
int UEngine::Model_SaveComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
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
   if(!Model_SaveComponent(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage))
    return false;
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int UEngine::Model_LoadComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!serstorage)
   return false;

  UId id=RDK::atoi(serstorage->GetNodeAttribute("Class"));

  if(!cont) // ������� ������
  {
   Model_Create(id);
   cont=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel()).Get();
  }
  else
  if(cont->GetClass() != id)
   return false;

  serstorage->SelectNode("Parameters");
  if(!Model_SetComponentParameters(cont, serstorage))
   return false;
  serstorage->SelectUp();

  cont->DelAllComponents();

  serstorage->SelectNode("Components");
  UAStorage* storage=cont->GetStorage();
  for(int i=0;i<serstorage->GetNumNodes();i++)
  {
   serstorage->SelectNode(i);
   std::string nodename=serstorage->GetNodeName();
   id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
   UEPtr<UANet> newcont=dynamic_pointer_cast<UANet>(storage->TakeObject(id));
   if(!newcont)
    return false;
   if(cont->AddComponent(static_pointer_cast<UAContainer>(newcont)) == ForbiddenId)
    return false;

   if(!Model_LoadComponent(newcont,serstorage))
    return false;
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

  serstorage->SelectNode("Links");
  if(!Model_SetComponentInternalLinks(cont,serstorage))
   return false;
  serstorage->SelectUp();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
int UEngine::Model_SaveComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
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
   if(!Model_SaveComponentParameters(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage))
    return false;
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
int UEngine::Model_LoadComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
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

   if(!Model_LoadComponentParameters(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage))
    return false;
   serstorage->SelectUp();
  }
  serstorage->SelectUp();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��������� ���������� � ��� �������� ��������� � xml
int UEngine::Model_SaveComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
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
   if(!Model_SaveComponentState(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage))
    return false;
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
int UEngine::Model_LoadComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
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

   if(!Model_LoadComponentState(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage))
    return false;
   serstorage->SelectUp();
  }
  serstorage->SelectUp();
 }
 catch (Exception * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ���������� ������� ����� ������
long long UEngine::Model_GetTime(void)
{
 return UTimeControl::GetTime();
}

double UEngine::Model_GetDoubleTime(void)
{
 return UTimeControl::GetDoubleTime();
}

// ������������� ������� ����� ������
bool UEngine::Model_SetTime(long long value)
{
 return UTimeControl::SetTime(value);
}

// ���������� �������� �����
long long UEngine::Model_GetRealTime(void)
{
 return UTimeControl::GetRealTime();
}

double UEngine::Model_GetDoubleRealTime(void)
{
 return UTimeControl::GetDoubleRealTime();
}

// ������������� �������� �����
bool UEngine::Model_SetRealTime(long long value)
{
 return UTimeControl::SetRealTime(value);
}

// ����������� �������� ����� �� �������� ��������
bool UEngine::Model_IncreaseRealTime(long long value)
{
 return UTimeControl::IncreaseRealTime(value);
}

// ���������� ���������� ��� � �������� �������
long long UEngine::Model_GetRealTimeStep(void)
{
 return UTimeControl::GetRealTimeStep();
}

double UEngine::Model_GetDoubleRealTimeStep(void)
{
 return UTimeControl::GetDoubleRealTimeStep();
}
// --------------------------

// --------------------------
// ������ ���������� ������������
// --------------------------
// ������������ ��������� ����������
void UEngine::ProcessException(Exception *exception) const
{
 if(!exception)
  throw exception;

 USharedPtr<Exception> ptr=exception;
 ExceptionsLog.push_back(ptr);

 TempLogString+=exception->CreateLogMessage();
 TempLogString+="\r\n";

 if(ExceptionHandler)
  ExceptionHandler();

// if(exception->GetType() == 1)
//  throw exception;
}


// ���������� ������ ������������������ ����������
const vector<USharedPtr<Exception> > UEngine::GetExceptionsLog(void) const
{
 return ExceptionsLog;
}


// ���������� ������ ����� ����
const char* UEngine::GetLog(void) const
{
 return TempLogString.c_str();
}

// ���������� ��������-������������ ����������
UEngine::PExceptionHandler UEngine::GetExceptionHandler(void) const
{
 return ExceptionHandler;
}

bool UEngine::SetExceptionHandler(PExceptionHandler value)
{
 if(ExceptionHandler == value)
  return true;

 ExceptionHandler=value;
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
UEPtr<UAContainer> UEngine::FindComponent(const char *stringid)
{
// UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());
 UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetCurrentComponent());

 if(!model)
  return 0;

 RDK::ULongId longid;

 UEPtr<RDK::UAContainer> cont;
 if(!stringid)
  cont=model;
 else
 {
  // ���� ������ ������ - �����, �� ��������� ��� ��������� id
  if(stringid[0]>=0x30 && stringid[0]<=0x39)
  {
   longid.DecodeFromString(stringid);
   if(!longid.GetSize() || longid[0] == ForbiddenId)
	cont=model;
   else
	cont=dynamic_pointer_cast<RDK::UAContainer>(model->GetComponentL(longid));
  }
  else // ...����� ���������� ��� ���
  {
   if(strlen(stringid) == 0)
	cont=model;
   else
    cont=dynamic_pointer_cast<RDK::UAContainer>(model->GetComponentL(stringid));
  }

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
