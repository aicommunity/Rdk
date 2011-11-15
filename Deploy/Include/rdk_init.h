#ifndef RDK_INIT_H
#define RDK_INIT_H

#include "rdk.h"
#include "rdk_gengine.h"

extern "C"  {


// ��������� ���������
class RDKDllManager;
extern RDKDllManager DllManager;

// ������������� dll
RDK_LIB_TYPE bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine);

// ���������� ����� �������� � ����������
RDK_LIB_TYPE size_t GetNumStorages(void);

// ���������� ����� ���� � ����������
RDK_LIB_TYPE size_t GetNumEnvironments(void);

// ���������� ����� ������� � ����������
RDK_LIB_TYPE size_t GetNumEngines(void);

// ���������� ��������� �� �������
RDK_LIB_TYPE RDK::UAContainerStorage* GetStorage(size_t i);

// ���������� ����� �� �������
RDK_LIB_TYPE RDK::UAContainerEnvironment*  GetEnvironment(size_t i);

// ���������� ������ �� �������
RDK_LIB_TYPE RDK::UEngine*  GetEngine(size_t i);

// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
RDK_LIB_TYPE RDK::UAContainerStorage*  AddNewStorage(void);

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
RDK_LIB_TYPE RDK::UAContainerEnvironment*  AddNewEnvironment(RDK::UAContainerStorage *storage=0,bool isinit=true, std::list<RDK::UAContainer*>* external_classes=0, std::list<RDK::UALibrary*>* external_libs=0);

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
RDK_LIB_TYPE RDK::UEngine*  AddNewEngine(void);
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
