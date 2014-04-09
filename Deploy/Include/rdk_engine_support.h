#ifndef RDK_ENGINE_SUPPORT_H
#define RDK_ENGINE_SUPPORT_H

#include "../../Core/Engine/UELockPtr.h"

// �������� DLL
class RDKDllManager
{
public:
// ������ ��������
std::vector<RDK::UStorage*> StorageList;

// ������ ����
std::vector<RDK::UEnvironment*> EnvironmentList;

// ������ �������
std::vector<RDK::UEngine*> EngineList;

// ������ ���������
std::vector<UGenericMutex*> MutexList;

// ������ �������
std::vector<UGenericMutexLocker*> LockerList;

UGenericMutex* GlobalMutex;

/// ������� �������� �����
int SelectedChannelIndex;

/// ������ �������� ���������� ������
RDK::UEPtr<RDK::UEngine> Engine;
RDK::UEPtr<RDK::UEnvironment> Environment;
RDK::UEPtr<RDK::UStorage> Storage;


// ----------------------------------------------------------
// ���������� ��������� �� ������� �������� ��������� � �����
// ----------------------------------------------------------
// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
typedef RDK::UStorage* (*PCreateNewStorage)(void);
PCreateNewStorage FuncCreateNewStorage;

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
typedef RDK::UEnvironment* (*PCreateNewEnvironment)(void);
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
// ������ ���������� ��������� �������
// --------------------------
bool Init(PCreateNewStorage fCreateNewStorage,
            PCreateNewEnvironment fCreateNewEnvironment,
			PCreateNewEngine fCreateNewEngine);

/// ���������� ����� �������
int GetNumEngines(void) const;

/// ������� ��������� ����� ������ �������
int SetNumEngines(int num);

/// ��������� ����� ������ � ������� index
/// ���� index <0 ��� >= NumEngines �� ��������� � �����
int Add(int index);

/// ������� ������ �� ������� index
int Del(int index);

/// �������� ��������� ������
/// (���� ������ ��� ���������������, �� �� ������ ������
int EngineCreate(int index);

/// ���������� ��������� ������
/// (���� ������ ��� ���������, �� �� ������ ������
int EngineDestroy(int index);
// --------------------------

// --------------------------
// ������ ������� � �������
// --------------------------
/// ������� �������� �����
int GetSelectedChannelIndex(void) const;
bool SetSelectedChannelIndex(int channel_index);

// ���������� ������ �� ��������� ������������ ����
RDK::UEPtr<RDK::UEngine>& GetEngine(void);
RDK::UEPtr<RDK::UEngine> GetEngine(int engine_index);

// ���������� ������ �� ��������� ����� ����������
RDK::UEPtr<RDK::UEnvironment>& GetEnvironment(void);
RDK::UEPtr<RDK::UEnvironment> GetEnvironment(int engine_index);

// ���������� ������ �� ��������� ���������
RDK::UEPtr<RDK::UStorage>& GetStorage(void);
RDK::UEPtr<RDK::UStorage> GetStorage(int engine_index);

// ���������� ��������� �� ������� ������
RDK::UEPtr<RDK::UContainer> GetModel(void);
RDK::UEPtr<RDK::UContainer> GetModel(int engine_index);
// --------------------------

// --------------------------
// ������ ������� � ������� � �����������
// --------------------------
// ���������� ������ �� ��������� ������������ ����
RDK::UELockPtr<RDK::UEngine> GetEngineLock(void);
RDK::UELockPtr<RDK::UEngine> GetEngineLock(int engine_index);

// ���������� ������ �� ��������� ����� ����������
RDK::UELockPtr<RDK::UEnvironment> GetEnvironmentLock(void);
RDK::UELockPtr<RDK::UEnvironment> GetEnvironmentLock(int engine_index);

// ���������� ������ �� ��������� ���������
RDK::UELockPtr<RDK::UStorage> GetStorageLock(void);
RDK::UELockPtr<RDK::UStorage> GetStorageLock(int engine_index);

// ���������� ��������� �� ������� ������
RDK::UELockPtr<RDK::UContainer> GetModelLock(void);
RDK::UELockPtr<RDK::UContainer> GetModelLock(int engine_index);
// --------------------------

};

//extern RDK::UEPtr<RDK::UEngine> PEngine;
//extern RDK::UEPtr<RDK::UEnvironment> PEnvironment;
//extern RDK::UEPtr<RDK::UStorage> PStorage;

extern int SelectedEngineIndex;

extern std::string RdkSystemDir;

// ��������� ���������
extern RDKDllManager DllManager;

extern int BufObjectsMode;

#endif