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
#include "Libraries/IO/UFileIO.h"

// --------------------------------------
// ���������� �������������� �������
// --------------------------------------
// ���������� ��������� �� �������
extern RDK::UAContainerStorage* GetStorage(size_t i);

// ���������� ����� �� �������
extern RDK::UAContainerEnvironment*  GetEnvironment(size_t i);

// ���������� ������ �� �������
extern RDK::UEngine*  GetEngine(size_t i);

// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
extern RDK::UAContainerStorage*  AddNewStorage(void);

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
extern RDK::UAContainerEnvironment*  AddNewEnvironment(void);

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
extern RDK::UEngine*  AddNewEngine(void);
// --------------------------------------

namespace RDK{

// --------------------------
// ������������ � �����������
// --------------------------
UEngine::UEngine(void)
{
 Runned=-1;
 Storage=0;
 Environment=0;

 LastReadExceptionLogIndex=-1;
 MaxExceptionsLogSize=1000;
}

UEngine::~UEngine(void)
{
 Stop();
}
// --------------------------

// --------------------------
// ������ ���������� ����������� �������������
// --------------------------
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

// ���������� ��������� �� ������
UAContainer* UEngine::GetModel(void)
{
 return Environment->GetModel();
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� ������ ������
void UEngine::Init(void)
{
 // ��������
}

bool UEngine::Init(UEPtr<UAContainerStorage> storage, UEPtr<UAContainerEnvironment> env)
{
 LastReadExceptionLogIndex=-1;
 //ExceptionsLog.clear();
 CurrentExceptionsLogSize=0;
 TempLogString.clear();
// if(!Options.LoadFromFile(OptionsFileName))
// {
  if(!Default())
   return false;
// }

// StorageIndex=atoi(Options("General","StorageIndex",sntoa(StorageIndex).c_str()));
// EnvironmentIndex=atoi(Options("General","EnvironmentIndex",sntoa(EnvironmentIndex).c_str()));

// if(StorageIndex>=0)
//  Storage=DLLGetStorage(StorageIndex);
// else
//  Storage=AddNewStorage();

 Storage=storage;
 Environment=env;

 CreateStorage();

 if(!Storage)
  return false;

// if(EnvironmentIndex>=0)
//  Environment=DLLGetEnvironment(EnvironmentIndex);
// else
// {
  LibrariesList.clear();
  ClassesList.clear();
  if(LoadPredefinedLibraries())
   return false;
  if(LoadClasses())
   return false;
  if(LoadLibraries())
   return false;
//  Environment=AddNewEnvironment();
  CreateEnvironment(true,&ClassesList, &LibrariesList);
// }

 if(!Storage || !Environment || Environment->GetStorage() != Storage)
 {
  return false;
 }

 UFileIO FileIO;
 FileIO.Default();
 FileIO.SetDirection(0);
 FileIO.SetFileName(ClassesDescriptionFileName);
 FileIO.Calculate();
 if(!FileIO.GetDataString().empty())
 {
  Storage_LoadCommonClassesDescription(FileIO.GetDataString().c_str());
  Storage_LoadClassesDescription(FileIO.GetDataString().c_str());
 }

 return true;
}

// ���������������� ������ ������
// � ��������� ������� ���������
void UEngine::UnInit(void)
{
 if(!Stop())
  return;

/* Options("General","StorageIndex",sntoa(StorageIndex));
 Options("General","EnvironmentIndex",sntoa(EnvironmentIndex));

 if(!Options.SaveToFile(OptionsFileName))
  return false;
  */
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
 catch (UException &exception)
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
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

 // ���������� ��� ������ �� ��� id.
const char * UEngine::Storage_GetClassName(int id) const
{
 try
 {
  TempString=Storage->FindClassName(id);
 }
 catch (UException &exception)
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
  return Storage->FindClassId(name);
 }
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
  xml.Create("Root");
  xml.AddNode("ClassesDescription");
  Storage->SaveClassesDescription(xml);
  xml.SelectRoot();
  xml.Save(TempString);
 }
 catch (UException &exception)
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
  xml.Load(xmltext, "Root");
  if(!xml.SelectNode("ClassesDescription"))
   return false;
  Storage->LoadClassesDescription(xml);
 }
 catch (UException &exception)
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
  xml.Create("Root");
  xml.AddNode("CommonClassesDescription");
  Storage->SaveCommonClassesDescription(xml);
  xml.SelectRoot();
  xml.Save(TempString);
 }
 catch (UException &exception)
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
  xml.Load(xmltext, "Root");
  if(!xml.SelectNode("CommonClassesDescription"))
   return false;
  return Storage->LoadCommonClassesDescription(xml);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ��������� �������� ���� ������� � xml ������� ����� ��������
const char* UEngine::Storage_SaveAllClassesDescription(void)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Create("Root");
  xml.AddNode("CommonClassesDescription");
  Storage->SaveCommonClassesDescription(xml);
  xml.SelectUp();
  xml.AddNode("ClassesDescription");
  Storage->SaveClassesDescription(xml);
  xml.SelectRoot();
  xml.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ��������� �������� ���� ������� �� xml ������� ����� ��������
bool UEngine::Storage_LoadAllClassesDescription(const char* xmltext)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Load(xmltext, "Root");
  if(!xml.SelectNode("CommonClassesDescription"))
   return false;
  if(!Storage->LoadCommonClassesDescription(xml))
   return false;
  xml.SelectUp();
  if(!xml.SelectNode("ClassesDescription"))
   return false;
  Storage->LoadClassesDescription(xml);
  return true;
 }
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
  Environment->Init();
   return true;
 }
 catch (UException &exception)
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
  Environment->UnInit();
  return true;
 }
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }


 return 0;
}

// ������ ���� ������ � �������� �������
void UEngine::Env_RTCalculate(void)
{
 try
 {
  Environment->RTCalculate();
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
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
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ���������� ���������� ������� ������ �� ��������� ��������
void UEngine::Env_IncreaseModelTimeByStep(void)
{
 try
 {
  Environment->IncreaseModelTimeByStep();
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
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
 catch (RDK::UException &exception)
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
 catch (RDK::UException &exception)
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
 catch (RDK::UException &exception)
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
 catch (RDK::UException &exception)
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
 catch (RDK::UException &exception)
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
 catch (RDK::UException &exception)
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
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return -1;
}

// ������� ����� ������ �� id ������ � ���������
// �������������� ������� ������������ ������
int UEngine::Model_Create(const char *classid)
{
 try
 {
  if(Environment->CreateModel(Storage->FindClassId(classid)))
   return 0;
 }
 catch (UException &exception)
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
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������, ���������� �� ������
bool UEngine::Model_Check(void)
{
 try
 {
  UEPtr<RDK::UAContainer> model=dynamic_pointer_cast<RDK::UAContainer>(Environment->GetModel());

  if(!model)
   return false;

  return true;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ���������, ���������� �� � ������ ��������� � ������ stringid)
bool UEngine::Model_CheckComponent(const char* stringid) const
{
 try
 {
  UEPtr<RDK::UAContainer> destcont=FindComponent(stringid);
  if(destcont)
   return true;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'classid'
// ���� stringid - ������ ������, �� ��������� � ���� ������
// ���������� ��� ���������� � ������ ������
const char* UEngine::Model_AddComponent(const char* stringid, int classid)
{
 try
 {
  UEPtr<RDK::UAContainer> destcont=FindComponent(stringid);

  UEPtr<RDK::UAContainer> cont=dynamic_pointer_cast<RDK::UAContainer>(Storage->TakeObject(classid));

  if(!cont)
   return 0;

  if(!destcont)
   return 0;

  if(destcont->AddComponent(cont))
  {
   TempString=cont->GetName();
  }
  else
   TempString.clear();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
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
 catch (UException &exception)
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
   return 0;

  return destcont->GetNumComponents();
 }
 catch (UException &exception)
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
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ���������� xml-������ ������� ��������������� ���� ����������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ���������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ����������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ����������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
const char* UEngine::Model_GetConnectorsList(const char* stringid,
						  int sublevel, const char* owner_level_stringid)
{
 try
 {
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  UEPtr<RDK::UAContainer> owner_level=FindComponent(owner_level_stringid);

  TempString="";
  if(!cont)
   return TempString.c_str();


  ULongIdVector buffer;
  cont->GetConnectorsList(buffer,sublevel,owner_level);

  XmlStorage.Create("Connectors");
  XmlStorage<<buffer;
  XmlStorage.SelectRoot();

  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ���������� xml-������ ������� ��������������� ���� ��������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// �������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ��������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
const char* UEngine::Model_GetItemsList(const char* stringid,
							int sublevel, const char* owner_level_stringid)
{
 try
 {
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  UEPtr<RDK::UAContainer> owner_level=FindComponent(owner_level_stringid);

  TempString="";
  if(!cont)
   return TempString.c_str();


  ULongIdVector buffer;
  cont->GetItemsList(buffer,sublevel,owner_level);

  XmlStorage.Create("Items");
  XmlStorage<<buffer;

  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ���������� xml-������ ������� ��������������� ���� �������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� �������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
const char* UEngine::Model_GetNetsList(const char* stringid,
							int sublevel, const char* owner_level_stringid)
{
 try
 {
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  UEPtr<RDK::UAContainer> owner_level=FindComponent(owner_level_stringid);

  TempString="";
  if(!cont)
   return TempString.c_str();

  ULongIdVector buffer;
  cont->GetNetsList(buffer,sublevel,owner_level);

  XmlStorage.Create("Nets");
  XmlStorage<<buffer;

  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();

}


// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
const char* UEngine::Model_GetComponentName(const char* stringid)
{
 try
 {
  TempString="";
  UEPtr<RDK::UAContainer> destcont=FindComponent(stringid);

  if(!destcont)
   return TempString.c_str();

  return destcont->GetName().c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}


// ���������� ������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
const char* UEngine::Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid)
{
 try
 {
  TempString="";
  UEPtr<RDK::UAContainer> destcont=FindComponent(stringid);
  UEPtr<RDK::UAContainer> owner_level=FindComponent(owner_level_stringid);

  if(!destcont)
   return TempString.c_str();

  return destcont->GetLongName(owner_level,TempString).c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ���������� ������� id ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
const char* UEngine::Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid)
{
 try
 {
  TempString="";
  UEPtr<RDK::UAContainer> destcont=FindComponent(stringid);
  UEPtr<RDK::UAContainer> owner_level=FindComponent(owner_level_stringid);

  if(!destcont)
   return TempString.c_str();

  ULongId id;

  destcont->GetLongId(owner_level,id);
  id.EncodeToString(TempString);
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ���������� ��� ������ ���������� � ��������� �� �������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������ ������
const char* UEngine::Model_GetComponentClassName(const char* stringid)
{
 try
 {
  TempString="";
  UEPtr<RDK::UAContainer> destcont=FindComponent(stringid);

  if(!destcont)
   return TempString.c_str();

  TempString=Storage->FindClassName(destcont->GetClass());
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ���������� �������� ���������� �� ��������������
const char* UEngine::Model_GetComponentProperties(const char *stringid, unsigned int type_mask)
{
 try
 {
  TempString="";
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);

  if(!cont)
   return TempString.c_str();

  XmlStorage.Create(cont->GetLongName(Environment->GetCurrentComponent(),CompName));
  XmlStorage.AddNode(UVariable::GetPropertyTypeNameByType(type_mask));

  if(!Model_GetComponentProperties(cont,&XmlStorage, type_mask))
   return 0;

  XmlStorage.SelectUp();
  XmlStorage.Save(TempString);
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ���������� �������� ���������� �� ��������������
const char* UEngine::Model_GetComponentSelectedProperties(const char *stringid)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� �������� ���������� �� �������������� � ����������
const char* UEngine::Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask)
{
 try
 {
  TempString="";
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  if(!cont)
   return TempString.c_str();

  XmlStorage.Create(cont->GetLongName(Environment->GetCurrentComponent(),CompName));
  XmlStorage.AddNode(UVariable::GetPropertyTypeNameByType(type_mask));

  if(!Model_GetComponentPropertiesEx(cont,&XmlStorage, type_mask))
   return 0;

  XmlStorage.SelectUp();
  XmlStorage.Save(TempString);
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
const char * UEngine::Model_GetComponentPropertyValue(const char *stringid, const char *paramname)
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
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ������������� �������� ���������� �� ��������������
int UEngine::Model_SetComponentProperties(const char *stringid, const char* buffer)
{
 try
 {
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  if(!cont)
   return 1;

  XmlStorage.Load(buffer, cont->GetLongName(Environment->GetCurrentComponent(),CompName));
  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(XmlStorage.SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
	if(Model_SetComponentProperties(cont,&XmlStorage))
	 return 2;

	XmlStorage.SelectUp();
   }
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ������������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
void UEngine::Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer)
{
 try
 {
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  if(!cont)
   return;

  cont->SetPropertyValue(paramname,buffer);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}


// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
void UEngine::Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer)
{
 try
 {
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  if(!cont)
   return;

  UId classid=Storage->FindClassId(class_stringid);
  if(classid == ForbiddenId)
   return;

  Model_SetGlobalComponentPropertyValue(cont, classid, paramname, buffer);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
void UEngine::Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer)
{
 try
 {
  UEPtr<RDK::UAContainer> cont=FindComponent(stringid);
  if(!cont)
   return;

  UId classid=Storage->FindClassId(class_stringid);
  UId owner_classid=Storage->FindClassId(class_owner_stringid);
  if(classid == ForbiddenId || owner_classid == ForbiddenId)
   return;

  Model_SetGlobalOwnerComponentPropertyValue(cont, classid, owner_classid, paramname, buffer);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// ��������� ��������� ���������� ���� � ������
int UEngine::Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 try
 {
  if(!stringid1)
   return -10;

  if(!stringid2)
   return -11;

  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetCurrentComponent());

  if(!model)
   return -2;

  RDK::ULongId longid1, longid2;
  if(stringid1[0]>=0x30 && stringid1[0]<=0x39)
   longid1.DecodeFromString(stringid1);
  else
  {
   UEPtr<UAContainer> cont=FindComponent(stringid1);
   longid1=cont->GetLongId(model);
  }

  if(stringid2[0]>=0x30 && stringid2[0]<=0x39)
   longid2.DecodeFromString(stringid2);
  else
  {
   UEPtr<UAContainer> cont=FindComponent(stringid2);
   longid2=cont->GetLongId(model);
  }
  bool res=model->CreateLink(longid1,output_number,longid2,input_number);
  if(!res)
   return -3;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� ���������� ���������� ���������� �� ����������� id � �������: 0 ����� � 0 �����
int UEngine::Model_ChainLinking(const char* stringid)
{
 try
 {
  UEPtr<UANet> cont=dynamic_pointer_cast<UANet>(FindComponent(stringid));

  cont->BreakLinks(cont);

  if(cont->GetNumComponents() == 0)
   return 0;

  ULongId id1,id2;


  UEPtr<UANet> item=dynamic_pointer_cast<UANet>(cont->GetComponentByIndex(0));
  item->GetLongId(cont,id1);

  // ���������� ������ ������
  int minsize=cont->GetNumOutputs();
  if(minsize>item->GetNumInputs())
   minsize=item->GetNumInputs();
  for(int j=0;j<minsize;j++)
   cont->CreateLink(ForbiddenId,j,id1,j);

  for(int i=1;i<cont->GetNumComponents();i++)
  {
   UEPtr<UANet> connector=dynamic_pointer_cast<UANet>(cont->GetComponentByIndex(i));
   connector->GetLongId(cont,id2);
   minsize=item->GetNumOutputs();
   if(minsize>connector->GetNumInputs())
	minsize=connector->GetNumInputs();
   for(int j=0;j<minsize;j++)
	cont->CreateLink(id1,j,id2,j);
   id1=id2;
   item=connector;
  }
 /*   // ��������! ���������, ����� �� ��� ����� �����-������
  // ���������� ����� ������
  minsize=cont->GetNumInputs();
  if(minsize>item->GetNumOutputs())
   minsize=item->GetNumOutputs();
  for(int j=0;j<minsize;j++)
   cont->CreateLink(id1,j,ForbiddenId,j);
   */
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� ���������� ���������� ���������� �����������, ��������� �� � ������������ ����� ������� ������
// ������������ ��� ������������ ������������������
int UEngine::Model_ParallelLinking(const char* stringid)
{
 try
 {
  UEPtr<UANet> cont=dynamic_pointer_cast<UANet>(FindComponent(stringid));

  cont->BreakLinks(cont);

  if(cont->GetNumComponents() == 0)
   return 0;

  ULongId id1,id2;


  UEPtr<UANet> item=cont;//dynamic_pointer_cast<UANet>(cont->GetComponentByIndex(0));
  //item->GetLongId(cont,id1);

  for(int i=0;i<cont->GetNumComponents();i++)
  {
   UEPtr<UANet> connector=dynamic_pointer_cast<UANet>(cont->GetComponentByIndex(i));
   connector->GetLongId(cont,id2);
   int minsize=item->GetNumOutputs();
   if(minsize>connector->GetNumInputs())
	minsize=connector->GetNumInputs();
   for(int j=0;j<minsize;j++)
	cont->CreateLink(id1,j,id2,j);
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��������� �����
int UEngine::Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 try
 {
  if(!stringid1)
   return -10;

  if(!stringid2)
   return -11;

  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetCurrentComponent());

  if(!model)
   return -2;

  RDK::ULongId longid1, longid2;
  if(stringid1[0]>=0x30 && stringid1[0]<=0x39)
   longid1.DecodeFromString(stringid1);
  else
  {
   UEPtr<UAContainer> cont=FindComponent(stringid1);
   longid1=cont->GetLongId(model);
  }

  if(stringid2[0]>=0x30 && stringid2[0]<=0x39)
   longid2.DecodeFromString(stringid2);
  else
  {
   UEPtr<UAContainer> cont=FindComponent(stringid2);
   longid2=cont->GetLongId(model);
  }

  bool res=model->BreakLink(longid1,output_number,longid2,input_number);
  if(!res)
   return -3;
 }
 catch (UException &exception)
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
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetCurrentComponent());

  if(!model)
   return -2;

  model->BreakLinks();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ��������� ��� ������� � �������� ����� ���������� ����������
int UEngine::Model_BreakAllComponentLinks(const char* stringid)
{
 try
 {
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!model)
   return -2;

  model->BreakLinks();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ��������� ��� ������� ����� ���������� ����������
int UEngine::Model_BreakAllComponentInputLinks(const char* stringid)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� �������� ����� ���������� ����������
int UEngine::Model_BreakAllComponentOutputLinks(const char* stringid)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char* UEngine::Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));
  UEPtr<RDK::UANet> owner;
  if(owner_level_stringid)
   dynamic_pointer_cast<RDK::UANet>(FindComponent(owner_level_stringid));

  TempString="";
  if(!cont)
   return TempString.c_str();

  XmlStorage.Create("Links");

  if(Model_GetComponentInternalLinks(cont,&XmlStorage,owner))
   return TempString.c_str();

  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� � ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����������� ������� �������� ����������
int UEngine::Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));
  UEPtr<RDK::UANet> owner;
  if(owner_level_stringid)
   dynamic_pointer_cast<RDK::UANet>(FindComponent(owner_level_stringid));

  if(!cont)
   return -3;

  XmlStorage.Load(buffer,"Links");

  if(!Model_SetComponentInternalLinks(cont,&XmlStorage,owner))
   return -4;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char * UEngine::Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));
  UEPtr<RDK::UANet> owner;
  if(owner_level_stringid)
   dynamic_pointer_cast<RDK::UANet>(FindComponent(owner_level_stringid));

  TempString="";
  if(!cont)
   return TempString.c_str();

  XmlStorage.Create("Links");

  if(!Model_GetComponentInputLinks(cont,&XmlStorage,owner,sublevel))
   return 0;

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char * UEngine::Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));
  UEPtr<RDK::UANet> owner;
  if(owner_level_stringid)
   dynamic_pointer_cast<RDK::UANet>(FindComponent(owner_level_stringid));

  TempString="";
  if(!cont)
   return TempString.c_str();

  XmlStorage.Create("Links");

  if(!Model_GetComponentOutputLinks(cont,&XmlStorage,owner, sublevel))
   return 0;

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ��������� ��� ������� ����� c ����������� cont � ��� ��������� ������������ � ���� xml � ����� buffer
// ���������� � ������ ����������� ������������ ��������� ���������� cont!
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char* UEngine::Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));
  UEPtr<RDK::UANet> owner;
  if(owner_level_stringid)
   owner=dynamic_pointer_cast<RDK::UANet>(FindComponent(owner_level_stringid));

  TempString="";
  if(!cont)
   return TempString.c_str();

  XmlStorage.Create("Links");

  if(Model_GetComponentPersonalLinks(cont,&XmlStorage,owner))
   return TempString.c_str();

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ���������� ����� ������ � ����������
int UEngine::Model_GetComponentNumInputs(const char *stringid)
{
 try
 {
  UEPtr<RDK::UAItem> cont=dynamic_pointer_cast<RDK::UAItem>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetNumInputs();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ������ ����� ���������� � ����� ���������
int UEngine::Model_GetComponentInputDataSize(const char *stringid, int index)
{
 try
 {
  UEPtr<RDK::UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetInputDataSize(index);
 }
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
 catch (UException &exception)
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
  UEPtr<RDK::UAItem> cont=dynamic_pointer_cast<RDK::UAItem>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetNumOutputs();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� ������ ������ ���������� � ����� ���������
int UEngine::Model_GetComponentOutputDataSize(const char *stringid, int index)
{
 try
 {
  UEPtr<RDK::UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetOutputDataSize(index);
 }
 catch (UException &exception)
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
  UEPtr<RDK::UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetOutputDataElementSize(index);
 }
 catch (UException &exception)
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
 catch (UException &exception)
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
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetOutputData(index).UChar;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
const char *  UEngine::Model_SaveComponent(const char *stringid, unsigned int params_type_mask)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.DelNode();
  XmlStorage.Create("Save");
  XmlStorage.SetNodeAttribute("ModelName",Environment->GetModel()->GetName());

  if(!Model_SaveComponent(cont,&XmlStorage, true, params_type_mask))
   return 0;

  XmlStorage.Save(TempString);
 // strcpy(buffer,str.c_str());
 }
 catch (UException &exception)
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

  Environment->Reset();

  if(!Environment->GetModel())
  {
   if(!Model_LoadComponent(0,&XmlStorage,true))
	return -4;
  }
  else
  {
   if(XmlStorage.GetNodeAttribute("ModelName") != Environment->GetModel()->GetName())
    return -10;

   UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

   if(!cont)
	return -3;

   if(!Model_LoadComponent(cont,&XmlStorage,true))
    return -4;
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
const char * UEngine::Model_SaveComponentProperties(const char *stringid, unsigned int type_mask)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.DelNode();
  XmlStorage.Create("SaveProperties");
  XmlStorage.SetNodeAttribute("ModelName",Environment->GetModel()->GetName());

  if(!Model_SaveComponentProperties(cont,&XmlStorage, type_mask))
   return 0;

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
int UEngine::Model_LoadComponentProperties(const char *stringid, char* buffer)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return -3;

  XmlStorage.Load(buffer,"SaveProperties");
  if(XmlStorage.GetNodeAttribute("ModelName") != Environment->GetModel()->GetName())
   return -10;

  XmlStorage.SelectNode(0);

  if(!Model_LoadComponentProperties(cont,&XmlStorage))
   return -4;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
const char* UEngine::Model_SaveComponentDrawInfo(const char *stringid)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.DelNode();
  XmlStorage.Create("Save");
  XmlStorage.SetNodeAttribute("ModelName",Environment->GetModel()->GetName());

  if(!Model_SaveComponentDrawInfo(cont,&XmlStorage))
   return 0;

  XmlStorage.Save(TempString);
 // strcpy(buffer,str.c_str());
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}



// ��������� ����� ����� ������ �� ���������
int UEngine::Model_GetDefaultTimeStep(void) const
{
 return DefaultTimeStep;
}

void UEngine::Model_SetDefaultTimeStep(int value)
{
 DefaultTimeStep=value;
}

// ��������� ����� ����� ����������
int UEngine::Model_GetTimeStep(const char *stringid) const
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  return cont->GetTimeStep();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

void UEngine::Model_SetTimeStep(const char *stringid, int value)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  cont->SetTimeStep(value);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// ������������� ��� ����� ���������� � ���� ��� �������� ���������
void UEngine::Model_SetGlobalTimeStep(const char *stringid, int value)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  cont->SetGlobalTimeStep(value);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
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

// ���������� ����� ������� ���������� ��� ������� ������� �������� ��������� (��)
long long UEngine::Model_GetStepDuration(const char *stringid) const
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  return cont->GetStepDuration();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ���������� �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
long long UEngine::Model_GetFullStepDuration(const char *stringid) const
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  return cont->GetFullStepDuration();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}


// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
double UEngine::Model_GetInstantPerformance(const char *stringid) const
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  return cont->GetInstantPerformance();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// �����, ��������� ����� ����� ���������� ���������� �����
long long UEngine::Model_GetInterstepsInterval(const char *stringid) const
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  return cont->GetInterstepsInterval();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ���������� �������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
bool UEngine::Model_GetComponentProperties(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage, unsigned int type_mask)
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
   if(I->second.CheckMask(type_mask))
   {
	cont->GetProperty(I->second.Id,serstorage);
	std::string paramname=I->second.Property->GetName();
	if(serstorage->SelectNode(paramname))
	{
	 serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
	 serstorage->SelectUp();
    }
   }
   ++I;
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ���������� ���������� �������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
bool UEngine::Model_GetComponentSelectedProperties(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ���������� �������� ���������� �� �������������� � ����������
// ������ ��� buffer ������ ���� ��������!
bool UEngine::Model_GetComponentPropertiesEx(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage, unsigned int type_mask)
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
   if(I->second.CheckMask(type_mask))
   {
	cont->GetProperty(I->second.Id,serstorage);

	std::string paramname=I->second.Property->GetName();
	if(serstorage->SelectNode(paramname))
	{
	 serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
	 if(descr)
	 {
	  serstorage->SetNodeAttribute("Header",descr->GetProperty(paramname).Header);
	 }
	 serstorage->SelectUp();
	}
   }
   ++I;
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}


// ������������� �������� ���������� �� ��������������
int UEngine::Model_SetComponentProperties(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

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
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
void UEngine::Model_SetGlobalComponentPropertyValue(RDK::UAContainer* cont, UId classid, const char *paramname, const char *buffer)
{
 try
 {
  if(!cont || classid == ForbiddenId)
   return;

  if(cont->GetClass() == classid)
   cont->SetPropertyValue(paramname,buffer);

  for(int i=0;i<cont->GetNumComponents();i++)
  {
   Model_SetGlobalComponentPropertyValue(cont->GetComponentByIndex(i), classid, paramname, buffer);
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
void UEngine::Model_SetGlobalOwnerComponentPropertyValue(RDK::UAContainer* cont, UId classid, UId owner_classid, const char *paramname, const char *buffer)
{
 try
 {
  if(!cont || classid == ForbiddenId)
   return;

  if(cont->GetClass() == classid && cont->GetOwner() && cont->GetOwner()->GetClass() == owner_classid)
   cont->SetPropertyValue(paramname,buffer);

  for(int i=0;i<cont->GetNumComponents();i++)
  {
   Model_SetGlobalOwnerComponentPropertyValue(cont->GetComponentByIndex(i), classid, owner_classid, paramname, buffer);
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
int UEngine::Model_GetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, RDK::UANet* owner_level)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
  if(owner_level)
   cont->GetLinks(linkslist, owner_level);
  else
   cont->GetLinks(linkslist, cont);


  *serstorage<<linkslist;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
int UEngine::Model_SetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, RDK::UANet* owner_level)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
  *serstorage>>linkslist;

  cont->BreakLinks();
  cont->CreateLinks(linkslist, owner_level);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
int UEngine::Model_GetComponentInputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, RDK::UANet* owner_level, int sublevel)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
//  cont->GetInputLinks(linkslist, cont);

  *serstorage<<linkslist;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
int UEngine::Model_GetComponentOutputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, RDK::UANet* owner_level, int sublevel)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
//  cont->GetOutputLinks(linkslist, cont);

  *serstorage<<linkslist;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� ������� ����� c ����������� cont � ��� ��������� ������������ � ���� xml � ����� buffer
// ���������� � ������ ����������� ������������ ��������� ���������� cont!
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
int UEngine::Model_GetComponentPersonalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, RDK::UANet* owner_level)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
  if(owner_level)
   cont->GetLinks(linkslist, owner_level, true, cont);
  else
   cont->GetLinks(linkslist, cont->GetOwner(), true, cont);

  *serstorage<<linkslist;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
int UEngine::Model_SaveComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, bool links, unsigned int params_type_mask)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",/*RDK::sntoa(cont->GetClass())*/Storage->FindClassName(cont->GetClass()));
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(ptParameter));
  if(!Model_GetComponentProperties(cont, serstorage,params_type_mask))
   return false;
  serstorage->SelectUp();

  if(links)
  {
   serstorage->AddNode("Links");
   if(Model_GetComponentInternalLinks(cont,serstorage,0))
	return false;
   serstorage->SelectUp();
  }

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!Model_SaveComponent(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage,false,params_type_mask))
    return false;
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int UEngine::Model_LoadComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, bool links)
{
 try
 {
  if(!serstorage)
   return false;

  std::string name=serstorage->GetNodeAttribute("Class");
  UId id=Storage->FindClassId(name);

  if(!cont) // ������� ������
  {
   Model_Create(name.c_str());
   cont=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel()).Get();
  }
  else
  if(cont->GetClass() != id)
   return false;

  cont->SetName(serstorage->GetNodeName());

  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(serstorage->SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
	if(Model_SetComponentProperties(cont, serstorage))
	 return false;
    serstorage->SelectUp();
   }
  }

  cont->DelAllComponents();

  serstorage->SelectNode("Components");
  UAStorage* storage=cont->GetStorage();
  for(int i=0;i<serstorage->GetNumNodes();i++)
  {
   serstorage->SelectNode(i);
   std::string nodename=serstorage->GetNodeName();
   name=serstorage->GetNodeAttribute("Class");
   id=Storage->FindClassId(name);
   //id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
   UEPtr<UANet> newcont=dynamic_pointer_cast<UANet>(storage->TakeObject(id));
   if(!newcont)
    return false;
   if(cont->AddComponent(static_pointer_cast<UAContainer>(newcont)) == ForbiddenId)
    return false;

//   if(!Model_LoadComponent(newcont,serstorage))
   if(!Model_LoadComponent(newcont,serstorage,false))
	return false;
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

  if(links)
  {
   serstorage->SelectNode("Links");
   if(!Model_SetComponentInternalLinks(cont,serstorage,0))
	return false;
   serstorage->SelectUp();
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
int UEngine::Model_SaveComponentProperties(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, unsigned int type_mask)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",Storage->FindClassName(cont->GetClass()));
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(type_mask));
  if(!Model_GetComponentProperties(cont, serstorage,type_mask))
   return false;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!Model_SaveComponentProperties(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage,type_mask))
    return false;
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
int UEngine::Model_LoadComponentProperties(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  std::string name=serstorage->GetNodeAttribute("Class");
  UId id=Storage->FindClassId(name);
  if(cont->GetClass() != id)
   return false;

  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(serstorage->SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
	if(Model_SetComponentProperties(cont, serstorage))
	 return false;
    serstorage->SelectUp();
   }
  }

  serstorage->SelectNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!serstorage->SelectNode(cont->GetComponentByIndex(i)->GetName()))
    continue;
   std::string nodename=serstorage->GetNodeName();

   if(!Model_LoadComponentProperties(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage))
    return false;
   serstorage->SelectUp();
  }
  serstorage->SelectUp();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
int UEngine::Model_SaveComponentDrawInfo(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",/*RDK::sntoa(cont->GetClass())*/Storage->FindClassName(cont->GetClass()));

  serstorage->AddNode("Links");

  UStringLinksList linkslist;
//  cont->GetLinks(linkslist, cont);

  for(int i=0;i<cont->GetNumComponents();i++)
   static_pointer_cast<UANet>(cont->GetComponentByIndex(i))->GetLinks(linkslist, cont,true,cont->GetComponentByIndex(i));
  *serstorage<<linkslist;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   XmlStorage.AddNode(cont->GetComponentByIndex(i)->GetName());
   XmlStorage.AddNode("Parameters");
   if(!Model_GetComponentProperties(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage,ptParameter|pgAny))
	return false;
   XmlStorage.SelectUp();
   XmlStorage.SelectUp();
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ������������
// --------------------------
// ������������ ��������� ����������
void UEngine::ProcessException(UException &exception) const
{
// if(!exception)
//  throw exception;

// USharedPtr<UException> ptr=exception;
// ExceptionsLog.push_back(ptr);

 ++CurrentExceptionsLogSize;
 if(CurrentExceptionsLogSize/*ExceptionsLog.size()*/ > MaxExceptionsLogSize)
 {
//  ExceptionsLog.erase(ExceptionsLog.begin());
  size_t i=TempLogString.find_first_of("\n");
  if(i != string::npos)
  {
   TempLogString.erase(0,i);
  }
 }
 TempLogString+=exception.CreateLogMessage();
 TempLogString+="\r\n";

 if(ExceptionHandler)
  ExceptionHandler();
}


// ���������� ������ ������������������ ����������
/*const vector<USharedPtr<UException> > UEngine::GetExceptionsLog(void) const
{
 return ExceptionsLog;
}*/


// ���������� ������ ����� ����
const char* UEngine::GetLog(void) const
{
 return TempLogString.c_str();
}

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
const char* UEngine::GetUnreadLog(void)
{
 if(LastReadExceptionLogIndex<=0 && TempLogString.size())
 {
  LastReadExceptionLogIndex=TempLogString.size();
  return TempLogString.c_str();
 }

 if(LastReadExceptionLogIndex<int(TempLogString.size()))
 {
  TempString=TempLogString.substr(LastReadExceptionLogIndex);
  LastReadExceptionLogIndex=TempLogString.size();
  return TempString.c_str();
 }

 TempString="";
 return TempString.c_str();
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

// ������������ ����� �������� ����������
// ���� 0, �� �������������
int UEngine::GetMaxExceptionsLogSize(void) const
{
 return MaxExceptionsLogSize;
}

void UEngine::SetMaxExceptionsLogSize(int value)
{
 if(MaxExceptionsLogSize == value)
  return;

 MaxExceptionsLogSize=value;
 if(MaxExceptionsLogSize>0 && /*ExceptionsLog.size()*/CurrentExceptionsLogSize>MaxExceptionsLogSize)
 {
  //ExceptionsLog.erase(ExceptionsLog.begin(), ExceptionsLog.begin()+int(ExceptionsLog.size())-MaxExceptionsLogSize);
  CurrentExceptionsLogSize=MaxExceptionsLogSize;
 }
}
// --------------------------



// --------------------------
// ������ ����������� ���������� �������
// --------------------------

// ������� ������ ��������� � ���������� ��������� �� ����
void UEngine::CreateStorage(void)
{
}

// ������� ����� � ���������� ��������� �� ���.
// ���� ������ ��������� 'storage', �� ��������� ��� �� ������.
// ���� ���� 'isinit' == true, �� �������������� ��������� ������������ ������������
// ���� ��������� �� ������ external_libs != 0, ������������� �������������� ��������� ����� �������������
void UEngine::CreateEnvironment(bool isinit, list<UAContainer*>* external_classes, list<UALibrary*>* external_libs)
{
 if(!Storage)
  return;

 if(!Environment)
  return;

 Environment->Default();

 if(!Environment->SetStorage(Storage) || !isinit)
  return;

 if(external_classes != 0)
 {
  list<UAContainer*>::iterator I,J;
  I=external_classes->begin();
  J=external_classes->end();
  while(I != J)
  {
   Environment->AddClass(*I);
   ++I;
  }
 }

 if(external_libs != 0)
 {
  list<UALibrary*>::iterator I,J;
  I=external_libs->begin();
  J=external_libs->end();
  while(I != J)
  {
   Environment->AddClassLibrary(*I);
   ++I;
  }
 }

 Environment->BuildStorage();
}

// ��������� ����� ����������������� ���������
int UEngine::LoadPredefinedLibraries(void)
{
 LibrariesList.push_back(&BCLLibrary);
 LibrariesList.push_back(&IOLibrary);
 LibrariesList.push_back(&CRLibrary);
 LibrariesList.push_back(&StatisticLibrary);

 return 0;
}

// ��������� ����� �������
int UEngine::LoadClasses(void)
{
 vector<string> variables;
 int res=0;
 UAContainer *dllclass=0;

 Options.GetVariableList(ComponentClassesSectionName, variables);

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
UEPtr<UAContainer> UEngine::FindComponent(const char *stringid) const
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

 // ��� ����� �������� ���������� �������
 ClassesDescriptionFileName="ClassesDescription.xml";

 // ��� ������ ������ �������� ����������
// MainLibrarySectionName="MainDLL";

 // ��� ���������� �������� ����������
// MainLibraryName="MainLibraryName";
// Options(MainLibrarySectionName,MainLibraryName,string("nmsdk.dll"));

 // ��� ������ ������ ��������� ���������
 ComponentLibrariesSectionName="ComponentLibraries";

 ComponentClassesSectionName="ComponentClasses";

 // ������ ������������� ��������� � ����������
 // ���� < 0, �� ����� ��������� ����� �������
// StorageIndex=-1;
// Options("General","StorageIndex",sntoa(StorageIndex));

 // ������ ������������ ����� � ����������
 // ���� < 0, �� ����� ����� ����� �������
// EnvironmentIndex=-1;
// Options("General","EnvironmentIndex",sntoa(EnvironmentIndex));

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
