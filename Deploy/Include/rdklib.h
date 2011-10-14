#ifndef RDKLIB_H
#define RDKLIB_H

#include "rdk.h"

extern "C"  {


// ��������� ���������
class RDKDllManager;
extern RDKDllManager DllManager;

// ������������� dll
extern bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine);

// ���������� ����� �������� � ����������
extern size_t GetNumStorages(void);

// ���������� ����� ���� � ����������
extern size_t GetNumEnvironments(void);

// ���������� ����� ������� � ����������
extern size_t GetNumEngines(void);

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
extern RDK::UAContainerEnvironment*  AddNewEnvironment(RDK::UAContainerStorage *storage=0,bool isinit=true, std::list<RDK::UAContainer*>* external_classes=0, std::list<RDK::UALibrary*>* external_libs=0);

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
extern RDK::UEngine*  AddNewEngine(void);

}

// �������� DLL
class RDKDllManager
{
public:
// ������ ��������
std::vector<RDK::UAContainerStorage*> StorageList;

// ������ ����
std::vector<RDK::UAContainerEnvironment*> EnvironmentList;

// ������ �������
std::vector<RDK::UEngine*> EngineList;

// ----------------------------------------------------------
// ���������� ��������� �� ������� �������� ��������� � �����
// ----------------------------------------------------------
// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
typedef RDK::UAContainerStorage* (*PCreateNewStorage)(void);
PCreateNewStorage FuncCreateNewStorage;

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
typedef RDK::UAContainerEnvironment* (*PCreateNewEnvironment)(void);
PCreateNewEnvironment FuncCreateNewEnvironment;

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
typedef RDK::UEngine* (*PCreateNewEngine)(void);
PCreateNewEngine FuncCreateNewEngine;
// ----------------------------------------------------------

public:
// --------------------------
// ������������ � �����������
// --------------------------
RDKDllManager(void);
~RDKDllManager(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
bool Init(PCreateNewStorage fCreateNewStorage,
            PCreateNewEnvironment fCreateNewEnvironment,
            PCreateNewEngine fCreateNewEngine);
// --------------------------
};


#endif
