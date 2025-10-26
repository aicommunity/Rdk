#ifndef RDK_ENGINE_SUPPORT_H
#define RDK_ENGINE_SUPPORT_H

#include <vector>
#include "../../Core/Engine/UELockPtr.h"
#include "../../Core/Engine/UELockVar.h"
#include "../../Core/Engine/UStorage.h"
#include "../../Core/Engine/UEngine.h"
#include "../../Core/Engine/UEnvironment.h"
#include "rdk_version.h"

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
std::vector<RDK::UELockPtr<RDK::UEngine>*> LockerList;

/// ������ ��������
// LoggerList удален - используется glog

/// ��������� ������
// SystemLogger удален - используется glog

/// ���������� ������ (����������� ���������� �� ���� ��������)
// GlobalLogger удален - используется glog

UGenericMutex* GlobalMutex;

/// ������� �������� �����
RDK::UELockVar<int> SelectedChannelIndex;

/// ������� ����� �������
RDK::UELockVar<int> NumChannels;

/// ������ �������� ���������� ������
// Logger удален - используется glog
std::shared_ptr<RDK::UEngine> Engine;
std::shared_ptr<RDK::UEnvironment> Environment;
std::shared_ptr<RDK::UStorage> Storage;

/// ���� �� ���������� � ��������� ������� ���� (����������)
std::string SystemDir;

/// ���� �� ���������� � ������
std::string LogDir;

/// ���� ������ �������
bool DebugMode;

/// ���� ��������� ������ ���� � ��������
bool DebuggerMessageFlag;

int BufObjectsMode;

// ��� ����� �������� ���������� �������
std::string ClassesDescriptionFileName;

// ��� ����� �������� ����� ���������� �������
std::string CommonClassesDescriptionFileName;

// ������ ������ ���������
int StorageBuildMode;

// ��������� ������������ ������ ������
void SetStorageBuildMode(int mode);

// ��������� �������� ������ ������
int GetStorageBuildMode();

// ���� � ������ ���������
std::string LibrariesPath;

// ���� � ����� � ���������� �������
std::string ClDescPath;

// ��������� ���� � ������ ���������
void SetLibrariesPath(const std::string& value);

// ��������� ���� � ������ ���������
const std::string GetLibrariesPath() const;

// ��������� ���� � ����� � ���������� �������
void SetClDescPath(const std::string& value);

// ��������� ���� � ����� � ���������� �������
const std::string GetClDescPath() const;

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

// ��� ����� �������� ���������� �������
const std::string& GetClassesDescriptionFileName(void) const;
bool SetClassesDescriptionFileName(const std::string& value);

// ��� ����� �������� ����� ���������� �������
const std::string& GetCommonClassesDescriptionFileName(void) const;
bool SetCommonClassesDescriptionFileName(const std::string& value);

/// ��������� ������ ����
const RDK::UVersion& GetVersion(void) const;
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

public:
/// ���������� ���
void Destroy(void);
// --------------------------

// --------------------------
// ������ ������� � �������
// --------------------------
public:
// ���������� ������ �� ��������� ������������ ����
std::shared_ptr<RDK::UEngine>& GetEngine(void);
std::shared_ptr<RDK::UEngine> GetEngine(int channel_index);

// ���������� ������ �� ��������� ����� ����������
std::shared_ptr<RDK::UEnvironment>& GetEnvironment(void);
std::shared_ptr<RDK::UEnvironment> GetEnvironment(int channel_index);

// ���������� ������ �� ��������� ���������
std::shared_ptr<RDK::UStorage>& GetStorage(void);
std::shared_ptr<RDK::UStorage> GetStorage(int channel_index);

// ���������� ��������� �� ������� ������
std::shared_ptr<RDK::UContainer> GetModel(void);
std::shared_ptr<RDK::UContainer> GetModel(int channel_index);
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
RDK::UELockPtr<RDK::UEngine> GetEngineLockTimeout(unsigned timeout);
RDK::UELockPtr<RDK::UEngine> GetEngineLockTimeout(int channel_index, unsigned timeout);

// ���������� ������ �� ��������� ����� ����������
RDK::UELockPtr<RDK::UEnvironment> GetEnvironmentLock(void);
RDK::UELockPtr<RDK::UEnvironment> GetEnvironmentLock(int channel_index);

// ���������� ������ �� ��������� ���������
RDK::UELockPtr<RDK::UStorage> GetStorageLock(void);
RDK::UELockPtr<RDK::UStorage> GetStorageLock(int channel_index);

// ���������� ��������� �� ������� ������
RDK::UELockPtr<RDK::UContainer> GetModelLock(void);
RDK::UELockPtr<RDK::UContainer> GetModelLock(int channel_index);
RDK::UELockPtr<RDK::UContainer> GetModelLockTimeout(unsigned timeout);
RDK::UELockPtr<RDK::UContainer> GetModelLockTimeout(int channel_index, unsigned timeout);

template<class T>
RDK::UELockPtr<T> GetEngineLock(int channel_index);

template<class T>
RDK::UELockPtr<T> GetEngineLockTimeout(int channel_index, unsigned timeout);

template<class T>
RDK::UELockPtr<T> GetModelLock(int channel_index);

template<class T>
RDK::UELockPtr<T> GetModelLockTimeout(int channel_index, unsigned timeout);

/// ����� ������ ���������� ������
int LockChannel(int index);

/// ����� ������ ������ ���������� ������
int UnLockChannel(int index);
// --------------------------

// --------------------------
/// �������� ������������
// --------------------------
// ���������� ������ �� ��������� �� ������ �������� ������
// GetLogger функции удалены - используется glog

// ���������� ��������� �� ������ ���������� ������, ��� SystemLogger
// GetLogger функции удалены - используется glog

/// ���������� ��������� �� ��������� ������
// GetSystemLogger удален - используется glog

/// ���������� ���������  �� ���������� ������ (����������� ���������� �� ���� ��������)
// GetGlobalLogger удален - используется glog
// --------------------------


// --------------------------
// ��������������� ������ ����������
// --------------------------
protected:
/// ������ ������� �������� �����
bool SetSelectedChannelIndex(int channel_index);
// --------------------------
};

template<class T>
RDK::UELockPtr<T> URdkCoreManager::GetEngineLock(int channel_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<T>(0,GetEngine(channel_index));
#else
 return (channel_index<int(MutexList.size()))?RDK::UELockPtr<T>(MutexList[channel_index],std::dynamic_pointer_cast<T>(GetEngine(channel_index))):RDK::UELockPtr<T>(0,0);
#endif
}

template<class T>
RDK::UELockPtr<T> URdkCoreManager::GetEngineLockTimeout(int channel_index, unsigned timeout)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<T>(0,GetEngine(channel_index));
#else
 return (channel_index<int(MutexList.size()))?RDK::UELockPtr<T>(MutexList[channel_index],std::dynamic_pointer_cast<T>(GetEngine(channel_index)), timeout):RDK::UELockPtr<T>(0,0);
#endif
}

template<class T>
RDK::UELockPtr<T> URdkCoreManager::GetModelLock(int channel_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<T>(0,GetModel(channel_index));
#else
 return (channel_index<int(MutexList.size()))?RDK::UELockPtr<T>(MutexList[channel_index],std::dynamic_pointer_cast<T>(GetModel(channel_index))):RDK::UELockPtr<T>(0,0);
#endif
}

template<class T>
RDK::UELockPtr<T> URdkCoreManager::GetModelLockTimeout(int channel_index, unsigned timeout)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<T>(0,GetModel(channel_index));
#else
 return (channel_index<int(MutexList.size()))?RDK::UELockPtr<T>(MutexList[channel_index],std::dynamic_pointer_cast<T>(GetModel(channel_index)), timeout):RDK::UELockPtr<T>(0,0);
#endif
}


//extern std::shared_ptr<UEngine> PEngine;
//extern std::shared_ptr<UEnvironment> PEnvironment;
//extern std::shared_ptr<UStorage> PStorage;

//extern int SelectedEngineIndex;

//extern RDK_LIB_TYPE std::string RdkSystemDir;

// ��������� ���������
extern RDK_LIB_TYPE URdkCoreManager RdkCoreManager;

#endif
