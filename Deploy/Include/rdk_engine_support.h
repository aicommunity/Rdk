#ifndef RDK_ENGINE_SUPPORT_H
#define RDK_ENGINE_SUPPORT_H

#include <vector>
#include "../../Core/Engine/UELockPtr.h"
#include "../../Core/Engine/UELockVar.h"
#include "../../Core/Engine/UStorage.h"
#include "../../Core/Engine/UEngine.h"
#include "../../Core/Engine/UEnvironment.h"

// �������� DLL
class RDK_LIB_TYPE URdkCoreManager
{
public:
/// ������ ��������
std::vector<RDK::UStorage*> StorageList;

/// ������ ����
std::vector<RDK::UEnvironment*> EnvironmentList;

/// ������ �������
std::vector<RDK::UEngine*> EngineList;

/// ������ ���������
std::vector<UGenericMutex*> MutexList;

/// ������ �������
std::vector<UGenericMutexExclusiveLocker*> LockerList;

/// ������ ��������
std::vector<RDK::ULoggerEnv*> LoggerList;

/// ��������� ������
RDK::ULoggerEnv SystemLogger;

/// ���������� ������ (����������� ���������� �� ���� ��������)
RDK::ULoggerEnv GlobalLogger;

UGenericMutex* GlobalMutex;

/// ������� �������� �����
RDK::UELockVar<int> SelectedChannelIndex;

/// ������� ����� �������
RDK::UELockVar<int> NumChannels;

/// ������ �������� ���������� ������
RDK::UEPtr<RDK::ULoggerEnv> Logger;
RDK::UEPtr<RDK::UEngine> Engine;
RDK::UEPtr<RDK::UEnvironment> Environment;
RDK::UEPtr<RDK::UStorage> Storage;

/// ���� �� ���������� � ��������� ������� ���� (����������)
std::string SystemDir;

/// ���� �� ���������� � ������
std::string LogDir;

/// ���� ������ �������
bool DebugMode;

/// ���� ��������� ������ ���� � ��������
bool DebuggerMessageFlag;

int BufObjectsMode;


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

// ���������� ��������� �������
RDK::UBitmapFontCollection Fonts;


public:
// --------------------------
// ������������ � �����������
// --------------------------
URdkCoreManager(void);
virtual ~URdkCoreManager(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ���������� ��� �������� �������� ������
const char* GetSystemDir(void);

// ������������� ��� �������� �������� ������
int SetSystemDir(const char *dir);

// ���������� ��� �������� �����
const char* GetLogDir(void);

// ������������� ��� �������� �����
int SetLogDir(const char *dir);

/// ���� ������ �������
bool GetDebugMode(void) const;
int SetDebugMode(bool value);

/// ���� ��������� ������ ���� � ��������
bool GetDebuggerMessageFlag(void) const;
int SetDebuggerMessageFlag(bool value);

int GetBufObjectsMode(void);
int SetBufObjectsMode(int value);

// ������� ��������� ���������� �������
int ClearFonts(void);

// ��������� ���������� ������
int LoadFonts(void);

// ��������� ����� ���������� �����
bool AddFont(const std::string &font_file_name);

// ���������� ������ �� ��������� �������
RDK::UBitmapFontCollection& GetFonts(void);
// --------------------------

// --------------------------
// ������ ���������� ��������� �������
// --------------------------
/// ������������� ��������� �� ���������� �������, ��������� ��������� ������, ����� � ���������
bool SetCoreElementsCreationFunctions(PCreateNewStorage fCreateNewStorage,
            PCreateNewEnvironment fCreateNewEnvironment,
			PCreateNewEngine fCreateNewEngine);

/// ���������� ����� �������
int GetNumChannels(void) const;

/// ������� ��������� ����� ������ �������
int SetNumChannels(int num);

/// ������ ������� ������ � �������� ��������
int SelectChannel(int index);

/// ���������� ������ �������� ��������� ������
int GetSelectedChannelIndex(void) const;

/// ��������� ����� ������ � ������� index
/// ���� index <0 ��� >= NumChannels �� ��������� � �����
int Add(int index);

/// ������� ������ �� ������� index
int Del(int index);

/// �������������� ����� (������� ������ ���� ������� ������!)
/// Upd: ������� ����� ���� ������� ����� SetNumChannels � SelectChannel
int ChannelInit(int channel_index, int predefined_structure, void* exception_handler);

/// ���������������� ����� (������� ������������� ���������� ��� ������ �������������)
int ChannelUnInit(int channel_index);

protected:
/// �������� ��������� �����
/// (���� ����� ��� ���������������, �� �� ������ ������
int ChannelCreate(int index);

/// ���������� ��������� �����
/// (���� ����� ��� ���������, �� �� ������ ������
int ChannelDestroy(int index);
// --------------------------

// --------------------------
// ������ ������� � �������
// --------------------------
public:
// ���������� ������ �� ��������� ������������ ����
RDK::UEPtr<RDK::UEngine>& GetEngine(void);
RDK::UEPtr<RDK::UEngine> GetEngine(int channel_index);

// ���������� ������ �� ��������� ����� ����������
RDK::UEPtr<RDK::UEnvironment>& GetEnvironment(void);
RDK::UEPtr<RDK::UEnvironment> GetEnvironment(int channel_index);

// ���������� ������ �� ��������� ���������
RDK::UEPtr<RDK::UStorage>& GetStorage(void);
RDK::UEPtr<RDK::UStorage> GetStorage(int channel_index);

// ���������� ��������� �� ������� ������
RDK::UEPtr<RDK::UContainer> GetModel(void);
RDK::UEPtr<RDK::UContainer> GetModel(int channel_index);
// --------------------------

// --------------------------
// ������ ������� � ������� � �����������
// --------------------------
/// ����� ������� � ����������� ��������
UGenericMutex* GetGlobalMutex(void);

/// ����� ������ � ��������
UGenericMutex* GetEngineMutex(void);
UGenericMutex* GetEngineMutex(int index);

// ���������� ������ �� ��������� ������������ ����
RDK::UELockPtr<RDK::UEngine> GetEngineLock(void);
RDK::UELockPtr<RDK::UEngine> GetEngineLock(int channel_index);

// ���������� ������ �� ��������� ����� ����������
RDK::UELockPtr<RDK::UEnvironment> GetEnvironmentLock(void);
RDK::UELockPtr<RDK::UEnvironment> GetEnvironmentLock(int channel_index);

// ���������� ������ �� ��������� ���������
RDK::UELockPtr<RDK::UStorage> GetStorageLock(void);
RDK::UELockPtr<RDK::UStorage> GetStorageLock(int channel_index);

// ���������� ��������� �� ������� ������
RDK::UELockPtr<RDK::UContainer> GetModelLock(void);
RDK::UELockPtr<RDK::UContainer> GetModelLock(int channel_index);
// --------------------------

// --------------------------
/// �������� ������������
// --------------------------
// ���������� ������ �� ��������� �� ������ �������� ������
RDK::UEPtr<RDK::ULoggerEnv>& GetLogger(void);

// ���������� ��������� �� ������ ���������� ������, ��� SystemLogger
RDK::UEPtr<RDK::ULoggerEnv> GetLogger(int channel_index);

/// ���������� ��������� �� ��������� ������
RDK::UEPtr<RDK::ULoggerEnv> GetSystemLogger(void);

/// ���������� ���������  �� ���������� ������ (����������� ���������� �� ���� ��������)
RDK::UEPtr<RDK::ULoggerEnv> GetGlobalLogger(void);
// --------------------------


// --------------------------
// ��������������� ������ ����������
// --------------------------
protected:
/// ������ ������� �������� �����
bool SetSelectedChannelIndex(int channel_index);
// --------------------------



};

//extern RDK::UEPtr<RDK::UEngine> PEngine;
//extern RDK::UEPtr<RDK::UEnvironment> PEnvironment;
//extern RDK::UEPtr<RDK::UStorage> PStorage;

//extern int SelectedEngineIndex;

//extern RDK_LIB_TYPE std::string RdkSystemDir;

// ��������� ���������
extern RDK_LIB_TYPE URdkCoreManager RdkCoreManager;

#endif
