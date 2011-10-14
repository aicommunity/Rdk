#ifndef RDK_LIB_CPP
#define RDK_LIB_CPP

#include "rdklib.h"
#include "rdkdll_loader.h"

// ��������� ���������
RDKDllManager DllManager;

// ������������� dll
bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine)
{
 // ������������� dll
 RDK::DLLDllInit=DllInit;

 // ��������� �� ������� ������������ ����� �������� � ����������
 RDK::DLLGetNumStorages=GetNumStorages;

 // ��������� �� ������� ������������ ����� ���� � ����������
 RDK::DLLGetNumEnvironments=GetNumEnvironments;

 // ��������� �� ������� ������������ ����� ������� � ����������
 RDK::DLLGetNumEngines=GetNumEngines;

 // ���������� ��������� �� �������
 RDK::DLLGetStorage=GetStorage;

 // ���������� ����� �� �������
 RDK::DLLGetEnvironment=GetEnvironment;

 // ���������� ������ �� �������
 RDK::DLLGetEngine=GetEngine;

 // ������� ����� ��������� � �������� � ����� �������
 // ���������� ��������� �� ���������
 RDK::DLLAddNewStorage=AddNewStorage;

 // ������� ����� ����� � �������� � ����� �������
 // ���������� ��������� �� �����
 RDK::DLLAddNewEnvironment=AddNewEnvironment;

 // ������� ����� ������ � �������� � ����� �������
 // ���������� ��������� �� ������
 RDK::DLLAddNewEngine=AddNewEngine;

 return DllManager.Init(reinterpret_cast<RDKDllManager::PCreateNewStorage>(pfstorage),
                        reinterpret_cast<RDKDllManager::PCreateNewEnvironment>(pfenvironment),
                        reinterpret_cast<RDKDllManager::PCreateNewEngine>(pfengine));
}

// ���������� ����� �������� � ����������
size_t GetNumStorages(void)
{
 return DllManager.StorageList.size();
}

// ���������� ����� ���� � ����������
size_t GetNumEnvironments(void)
{
 return DllManager.EnvironmentList.size();
}

// ���������� ����� ������� � ����������
size_t GetNumEngines(void)
{
 return DllManager.EngineList.size();
}

// ���������� ��������� �� �������
RDK::UAContainerStorage* GetStorage(size_t i)
{
 if(i>=DllManager.StorageList.size())
  return 0;

 return DllManager.StorageList[i];
}

// ���������� ����� �� �������
RDK::UAContainerEnvironment* GetEnvironment(size_t i)
{
 if(i>=DllManager.EnvironmentList.size())
  return 0;

 return DllManager.EnvironmentList[i];
}

// ���������� ������ �� �������
RDK::UEngine* GetEngine(size_t i)
{
 if(i>=DllManager.EngineList.size())
  return 0;

 return DllManager.EngineList[i];
}


// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
RDK::UAContainerStorage* AddNewStorage(void)
{
 if(!DllManager.FuncCreateNewStorage)
  return 0;

 RDK::UAContainerStorage* storage=DllManager.FuncCreateNewStorage();
 if(storage)
 {
  CreateStorage(storage);
  DllManager.StorageList.push_back(storage);
 }
 return storage;
}

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
RDK::UAContainerEnvironment* AddNewEnvironment(RDK::UAContainerStorage *storage,bool isinit, std::list<RDK::UAContainer*>* external_classes, std::list<RDK::UALibrary*>* external_libs)
{
 if(!DllManager.FuncCreateNewEnvironment)
  return 0;

 RDK::UAContainerEnvironment* environment=DllManager.FuncCreateNewEnvironment();
 if(environment)
 {
  CreateEnvironment(environment, storage,isinit,external_classes, external_libs);
  DllManager.EnvironmentList.push_back(environment);
 }
 return environment;
}

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
RDK::UEngine* AddNewEngine(void)
{
 if(!DllManager.FuncCreateNewEngine)
  return 0;

 RDK::UEngine* engine=DllManager.FuncCreateNewEngine();
 if(engine)
 {
  DllManager.EngineList.push_back(engine);
 }
 return engine;
}


// �������� DLL
// --------------------------
// ������������ � �����������
// --------------------------
RDKDllManager::RDKDllManager(void)
{
 FuncCreateNewStorage=&RDK::NewUStorage;
 FuncCreateNewEnvironment=&RDK::NewUAContainerEnvironment;
 FuncCreateNewEngine=&RDK::NewUEngine;
}

RDKDllManager::~RDKDllManager(void)
{
 for(size_t i=0;i<EnvironmentList.size();i++)
  if(EnvironmentList[i])
   delete EnvironmentList[i];

 EnvironmentList.resize(0);

 for(size_t i=0;i<StorageList.size();i++)
  if(StorageList[i])
   delete StorageList[i];

 StorageList.resize(0);

 for(size_t i=0;i<EngineList.size();i++)
  if(EngineList[i])
   delete EngineList[i];

 EngineList.resize(0);
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
bool RDKDllManager::Init(PCreateNewStorage fCreateNewStorage,
                            PCreateNewEnvironment fCreateNewEnvironment,
                            PCreateNewEngine fCreateNewEngine)
{
 FuncCreateNewStorage=fCreateNewStorage;
 FuncCreateNewEnvironment=fCreateNewEnvironment;
 FuncCreateNewEngine=fCreateNewEngine;

 return true;
}
// --------------------------


#endif

