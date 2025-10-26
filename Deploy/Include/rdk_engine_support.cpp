#ifndef RDK_ENGINE_SUPPORT_CPP
#define RDK_ENGINE_SUPPORT_CPP

#include "rdk_engine_support.h"
#include "rdk_cpp_initdll.h"
#include "rdk_error_codes.h"
#include "rdk_exceptions.h"

// ��������� ���������
URdkCoreManager RdkCoreManager;

// �������� DLL
// --------------------------
// ������������ � �����������
// --------------------------
URdkCoreManager::URdkCoreManager(void)
{
 FuncCreateNewStorage=0;//&RDK::NewUStorage;
 FuncCreateNewEnvironment=0;//&RDK::NewUAContainerEnvironment;
 FuncCreateNewEngine=0;//&RDK::NewUEngine;
 GlobalMutex=UCreateMutex();
 SelectedChannelIndex=0;
 Engine=0;
 Environment=0;
 Storage=0;

 DebuggerMessageFlag=false;
 DebugMode=false;
 BufObjectsMode=1;
 // GlobalLogger удален - используется glog
 // SystemLogger.RegisterGlobalLogger удален - используется glog
 // SystemLogger удален - используется glog

 StorageBuildMode = 1;
}

URdkCoreManager::~URdkCoreManager(void)
{
 Destroy();
 if(GlobalMutex)
 {
  UDestroyMutex(GlobalMutex);
  GlobalMutex=0;
 }
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------

// ��������� ������������ ������ ������
void URdkCoreManager::SetStorageBuildMode(int mode)
{
    StorageBuildMode = mode;
}

// ��������� �������� ������ ������
int URdkCoreManager::GetStorageBuildMode()
{
    return StorageBuildMode;
}

// ��������� ���� � ������ ���������
void URdkCoreManager::SetLibrariesPath(const std::string& value)
{
    LibrariesPath = value;
}

// ��������� ���� � ������ ���������
const std::string URdkCoreManager::GetLibrariesPath(void) const
{
 return LibrariesPath;
}

// ��������� ���� � ����� � ���������� �������
void URdkCoreManager::SetClDescPath(const std::string &value)
{
    ClDescPath = value;
}

// ��������� ���� � ����� � ���������� �������
const std::string URdkCoreManager::GetClDescPath(void) const
{
 return ClDescPath;
}

// ���������� ��� �������� �������� ������
const char* URdkCoreManager::GetSystemDir(void)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 return SystemDir.c_str();
}

// ������������� ��� �������� �������� ������
int URdkCoreManager::SetSystemDir(const char *dir)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 SystemDir=dir;
 return RDK_SUCCESS;
}

// ���������� ��� �������� �����
const char* URdkCoreManager::GetLogDir(void)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 return LogDir.c_str();
}

// ������������� ��� �������� �����
int URdkCoreManager::SetLogDir(const char *dir)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);

 if(LogDir == dir)
  return RDK_SUCCESS;
 // GlobalLogger, SystemLogger, LoggerList удалены - используется glog
 LogDir=dir;
 return RDK_SUCCESS;
}

/// ���� ������ �������
bool URdkCoreManager::GetDebugMode(void) const
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 return DebugMode;
}

int URdkCoreManager::SetDebugMode(bool value)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 if(DebugMode == value)
  return RDK_SUCCESS;
 DebugMode=value;
 // LoggerList, SystemLogger, GlobalLogger удалены - используется glog
 return RDK_SUCCESS;
}

bool URdkCoreManager::GetDebuggerMessageFlag(void) const
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 return DebuggerMessageFlag;
}

int URdkCoreManager::SetDebuggerMessageFlag(bool value)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 if(DebuggerMessageFlag == value)
  return RDK_SUCCESS;
 DebuggerMessageFlag=value;
 // LoggerList, SystemLogger удалены - используется glog
 return RDK_SUCCESS;
}

int URdkCoreManager::GetBufObjectsMode(void)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 return BufObjectsMode;
}

int URdkCoreManager::SetBufObjectsMode(int value)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(BufObjectsMode == value)
	return RDK_SUCCESS;

   for(size_t i=0;i<EngineList.size();i++)
   {
    GetEngineLock(int(i))->SetBufObjectsMode(value);
   }
   BufObjectsMode=value;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  // SystemLogger.ProcessException удален - используется glog
  res=RDK_EXCEPTION_CATCHED;
 }

 return res;
}

// ������� ��������� ���������� �������
int URdkCoreManager::ClearFonts(void)
{
 Fonts.DelAllFonts();
 return RDK_SUCCESS;
}

// ��������� ���������� ������
int URdkCoreManager::LoadFonts(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // ������ ������
   std::vector<std::string> font_names;
   std::string font_path=SystemDir+"Fonts/";
   RDK::FindFilesList(font_path, "*.fnt", true, font_names);
   // // SystemLogger.LogMessage удален - используется glog удален - используется glog

   ClearFonts();
   RDK::UBitmapFont font;
   for(size_t i=0;i<font_names.size();i++)
   {
    if(AddFont(font_path+font_names[i]))
    {
     // SystemLogger.LogMessage удален - используется glog
    }
    else
    {
     // SystemLogger.LogMessage удален - используется glog
    }
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  // SystemLogger.ProcessException удален - используется glog
  res=RDK_EXCEPTION_CATCHED;
 }

 return res;
}

// ��������� ����� ���������� �����
bool URdkCoreManager::AddFont(const std::string &font_file_name)
{
 //int res=RDK_SUCCESS;
 RDK_SYS_TRY
 {
  try
  {
   RDK::UBitmapFont font;
   std::size_t dir_sep_pos=font_file_name.find_last_of("\\/");
   std::string font_name;
   if(dir_sep_pos != std::string::npos)
	font_name=font_file_name.substr(dir_sep_pos+1);
   else
	font_name=font_file_name;
   std::size_t _pos=font_name.find_first_of("_");
   std::size_t _pos2=font_name.find_first_not_of("0123456789",_pos+1);
   if(_pos != std::string::npos)
   {
	std::string font_string_size=font_name.substr(_pos+1,_pos2-_pos-1);
	int size=RDK::atoi(font_string_size);
	if(!font.LoadFromFile(font_name.substr(0,_pos),font_file_name,size))
	 return false;
	return Fonts.AddFont(font.GetName(),size,font);
   }
  }
  catch (RDK::UException &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
  }
  catch (std::exception &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
  }
 }
 RDK_SYS_CATCH
 {
  // SystemLogger.ProcessException удален - используется glog
 }

 return false;
}

// ���������� ������ �� ��������� �������
RDK::UBitmapFontCollection& URdkCoreManager::GetFonts(void)
{
 return Fonts;
}


// ��� ����� �������� ���������� �������
const std::string& URdkCoreManager::GetClassesDescriptionFileName(void) const
{
 return ClassesDescriptionFileName;
}

bool URdkCoreManager::SetClassesDescriptionFileName(const std::string& value)
{
 if(ClassesDescriptionFileName == value)
  return true;

 ClassesDescriptionFileName=value;
 return true;
}

// ��� ����� �������� ����� ���������� �������
const std::string& URdkCoreManager::GetCommonClassesDescriptionFileName(void) const
{
 return CommonClassesDescriptionFileName;
}

bool URdkCoreManager::SetCommonClassesDescriptionFileName(const std::string& value)
{
 if(CommonClassesDescriptionFileName == value)
  return true;

 CommonClassesDescriptionFileName=value;
 return true;
}

/// ��������� ������ ����
const RDK::UVersion& URdkCoreManager::GetVersion(void) const
{
 return RDK::GetGlobalVersion();
}
// --------------------------

// --------------------------
// ������ ���������� ��������� �������
// --------------------------
bool URdkCoreManager::SetCoreElementsCreationFunctions(PCreateNewStorage fCreateNewStorage,
							PCreateNewEnvironment fCreateNewEnvironment,
							PCreateNewEngine fCreateNewEngine)
{
 FuncCreateNewStorage=fCreateNewStorage;
 FuncCreateNewEnvironment=fCreateNewEnvironment;
 FuncCreateNewEngine=fCreateNewEngine;

 return true;
}

/// ���������� ����� �������
int URdkCoreManager::GetNumChannels(void) const
{
 return NumChannels;
}

/// ������� ��������� ����� ������ �������
int URdkCoreManager::SetNumChannels(int num)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   {
    UGenericMutexExclusiveLocker lock(GlobalMutex);

    if(num<0)
     return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

    for(int i=num;i<int(EngineList.size());i++)
    {
     ChannelDestroy(i);
     UDestroyMutex(MutexList[i]);
    }

    int old_num=int(EngineList.size());

    EngineList.resize(num,0);
    StorageList.resize(num,0);
    EnvironmentList.resize(num,0);
    MutexList.resize(num,0);
    LockerList.resize(num,0);
    // LoggerList удален - используется glog
    for(int i=old_num;i<num;i++)
    {
     MutexList[i]=UCreateMutex();
     #ifdef RDK_MUTEX_DEADLOCK_DEBUG
     MutexList[i]->DebugId=i;
     #endif
    }
    NumChannels=int(EngineList.size());
   }

   SetSelectedChannelIndex(num-1);

   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  // SystemLogger.ProcessException удален - используется glog
  res=RDK_EXCEPTION_CATCHED;
 }

 return res;
}

/// ������ ������� ����� � �������� ��������
int URdkCoreManager::SelectChannel(int index)
{
 if(index<0 || index>=NumChannels)
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(SelectedChannelIndex == index)
	return RDK_SUCCESS;

   RDK::UELockPtr<RDK::UEngine> ptr1(GetEngineLock(SelectedChannelIndex));
   RDK::UELockPtr<RDK::UEngine> ptr2(GetEngineLock(index));

   if(!SetSelectedChannelIndex(index))
	res=RDK_E_CORE_CHANNEL_NOT_FOUND;
   else
    res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  // SystemLogger.ProcessException удален - используется glog
  res=RDK_EXCEPTION_CATCHED;
 }


 return res;
}

/// ���������� ������ �������� ��������� ������
int URdkCoreManager::GetSelectedChannelIndex(void) const
{
 return SelectedChannelIndex;
}

/// ��������� ����� ������ � ������� index
/// ���� index <0 ��� >= NumChannels �� ��������� � �����
int URdkCoreManager::Add(int index)
{
 if(index<0 || index >= GetNumChannels())
  return SetNumChannels(GetNumChannels()+1);

 int old_num=GetNumChannels();
 int num=old_num+1;

 {
  UGenericMutexExclusiveLocker lock(GlobalMutex);
  EngineList.resize(num,0);
  StorageList.resize(num,0);
  EnvironmentList.resize(num,0);
  MutexList.resize(num,0);
  LockerList.resize(num,0);
  // LoggerList удален - используется glog
 }

 for(int i=int(EngineList.size())-1;i>index;i--)
 {
  UGenericMutexExclusiveLocker lock1(MutexList[i-1]);
  EngineList[i]=EngineList[i-1];
  StorageList[i]=StorageList[i-1];
  EnvironmentList[i]=EnvironmentList[i-1];
  MutexList[i]=MutexList[i-1];
  LockerList[i]=LockerList[i-1];
  // LoggerList[ удален - используется glogi]=// LoggerList[ удален - используется glogi-1];
 }

 EngineList[index]=0;
 StorageList[index]=0;
 EnvironmentList[index]=0;
 MutexList[index]=0;
 LockerList[index]=0;
 // LoggerList[ удален - используется glogindex]=0;
 MutexList[index]=UCreateMutex();
 #ifdef RDK_MUTEX_DEADLOCK_DEBUG
 MutexList[index]->DebugId=index;
 #endif

 SetSelectedChannelIndex(SelectedChannelIndex);
 {
  UGenericMutexExclusiveLocker lock(GlobalMutex);
  NumChannels=int(EngineList.size());
 }
 return RDK_SUCCESS;
}

/// ������� ������ �� ������� index
int URdkCoreManager::Del(int index)
{
 if(index<0 || index >= GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 if(GetNumChannels() == 1)
  return RDK_E_CORE_ZERO_CHANNEL_MUST_EXIST;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   {
    UGenericMutexExclusiveLocker lock(MutexList[index]);
    ChannelDestroy(index);
   }
   UDestroyMutex(MutexList[index]);
   for(int i=index+1;i<int(EngineList.size());i++)
   {
    UGenericMutexExclusiveLocker lock2(MutexList[i]);
    EngineList[i-1]=EngineList[i];
    StorageList[i-1]=StorageList[i];
    EnvironmentList[i-1]=EnvironmentList[i];
    MutexList[i-1]=MutexList[i];
    LockerList[i-1]=LockerList[i];
    // LoggerList[ удален - используется glogi-1]=// LoggerList[ удален - используется glogi];
   }

   int new_num_size=0;
   int curr_selected_channel_index=0;

   {
    UGenericMutexExclusiveLocker lock(GlobalMutex);
    EngineList.resize(EngineList.size()-1);
    StorageList.resize(StorageList.size()-1);
    EnvironmentList.resize(EnvironmentList.size()-1);
    MutexList.resize(MutexList.size()-1);
    // LoggerList удален - используется glog
    LockerList.resize(LockerList.size()-1);
    new_num_size=NumChannels=int(EngineList.size());
    curr_selected_channel_index=SelectedChannelIndex;
   }

   if(curr_selected_channel_index>=new_num_size)
    SetSelectedChannelIndex(curr_selected_channel_index-1);
   else
    SetSelectedChannelIndex(curr_selected_channel_index);

   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  // SystemLogger.ProcessException удален - используется glog
  res=RDK_EXCEPTION_CATCHED;
 }

 return res;
}

/// �������� ��������� �����
/// (���� ����� ��� ���������������, �� �� ������ ������
int URdkCoreManager::ChannelCreate(int index)
{
 if(index<0 || index>=GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 if(EngineList[index])
  return RDK_SUCCESS;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // // SystemLogger.LogMessage удален - используется glog удален - используется glog
   // TODO: ����� ������������� ���������� ������� � ��� ������
   // LoggerList, GlobalLogger удалены - используется glog

   EngineList[index]=FuncCreateNewEngine();
   if(!EngineList[index])
   {
    ChannelDestroy(index);
    return RDK_E_CORE_ENGINE_CREATE_FAIL;
   }

    // EngineList[index]->SetLogger удален - используется glog

   StorageList[index]=FuncCreateNewStorage();
   if(!StorageList[index])
   {
    ChannelDestroy(index);
    return RDK_E_CORE_STORAGE_CREATE_FAIL;
   }
   //
   StorageList[index]->SetBuildMode(StorageBuildMode);
   StorageList[index]->SetLibrariesPath(LibrariesPath);
   StorageList[index]->SetClDescPath(ClDescPath);

   EnvironmentList[index]=FuncCreateNewEnvironment();
   if(!EnvironmentList[index])
   {
    ChannelDestroy(index);
    return RDK_E_CORE_ENVIRONMENT_CREATE_FAIL;
   }

   EnvironmentList[index]->SetFonts(Fonts);
   EngineList[index]->Default();

   EnvironmentList[index]->SetSystemDir(SystemDir);
   EngineList[index]->SetCommonClassesDescriptionFileName(CommonClassesDescriptionFileName);
   EngineList[index]->SetClassesDescriptionFileName(ClassesDescriptionFileName);
    if(!EngineList[index]->Init(std::shared_ptr<RDK::UStorage>(StorageList[index], [](RDK::UStorage*){}),std::shared_ptr<RDK::UEnvironment>(EnvironmentList[index], [](RDK::UEnvironment*){})))
   {
    ChannelDestroy(index);
    return RDK_E_CORE_ENGINE_INIT_FAIL;
   }

   {
    UGenericMutexExclusiveLocker lock(GlobalMutex);
    /// ������ �������� ���������� ������
    Engine=std::shared_ptr<RDK::UEngine>(EngineList[SelectedChannelIndex], [](RDK::UEngine*){});
    Environment=std::shared_ptr<RDK::UEnvironment>(EnvironmentList[SelectedChannelIndex], [](RDK::UEnvironment*){});
    Storage=std::shared_ptr<RDK::UStorage>(StorageList[SelectedChannelIndex], [](RDK::UStorage*){});
    // Logger удален - используется glog
   }

   // SystemLogger.LogMessage удален - используется glog
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  // SystemLogger.ProcessException удален - используется glog
  res=RDK_EXCEPTION_CATCHED;
 }

 return res;
}

/// ���������� ��������� ������
/// (���� ������ ��� ���������, �� �� ������ ������
int URdkCoreManager::ChannelDestroy(int index)
{
 if(index<0 || index>=GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 UGenericMutexExclusiveLocker lock(MutexList[index]);

 if(EngineList[index])
 {
  if(Engine && EngineList[index] == Engine.get())
   Engine=0;
  delete EngineList[index];
  EngineList[index]=0;
 }

 if(EnvironmentList[index])
 {
  if(Environment && EnvironmentList[index] == Environment.get())
   Environment=0;
  delete EnvironmentList[index];
  EnvironmentList[index]=0;
 }

 if(StorageList[index])
 {
  if(Storage && StorageList[index] == Storage.get())
   Storage=0;
  delete StorageList[index];
  StorageList[index]=0;
 }

 // LoggerList удален - используется glog
 return RDK_SUCCESS;
}

// �������������� ����� (������� ������ ���� ������� ������!)
// Upd: ������� ����� ���� ������� ����� SetNumChannels � SelectChannel
int URdkCoreManager::ChannelInit(int channel_index, int predefined_structure, void* exception_handler)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(channel_index<0 || channel_index>=NumChannels)
    return RDK_E_CORE_CHANNEL_NOT_FOUND;

   // SystemLogger.LogMessage удален - используется glog(RDK_EX_DEBUG, std::string("Preparing to initialize channel ")+RDK::sntoa(channel_index));
   res=RDK_ASSERT_LOG(ChannelUnInit(channel_index));
   if(res != RDK_SUCCESS)
   {
	return res;
   }

   res=RDK_ASSERT_LOG(ChannelCreate(channel_index));
   if(res != RDK_SUCCESS)
	return res;

   GetEngineLock(channel_index)->SetChannelIndex(channel_index);
   GetEngineLock(channel_index)->SetBufObjectsMode(BufObjectsMode);
   RDK_ASSERT_LOG(MLog_SetExceptionHandler(channel_index, exception_handler));

   res=RDK_ASSERT_LOG(MEnv_SetPredefinedStructure(channel_index, predefined_structure));
   if(res != RDK_SUCCESS)
    return res;

   res=RDK_ASSERT_LOG(MEnv_CreateStructure(channel_index));
   if(res != RDK_SUCCESS)
    return res;

   res=RDK_ASSERT_LOG(MEnv_Init(channel_index));
   if(res != RDK_SUCCESS)
    return res;
   // SystemLogger.LogMessage удален - используется glog(RDK_EX_DEBUG, std::string("Channel ")+RDK::sntoa(channel_index)+" has been initialized successfully");
  }
  catch (RDK::UException &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  // SystemLogger.ProcessException удален - используется glog
  res=RDK_EXCEPTION_CATCHED;
 }

 return res;
}

// ���������������� ����� (������� ������������� ���������� ��� ������ �������������)
int URdkCoreManager::ChannelUnInit(int channel_index)
{
 if(channel_index<0 || channel_index>=NumChannels)
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(EngineList[channel_index])
   {
    res = RDK_ASSERT_LOG(MEnv_UnInit(channel_index));
	if(res==RDK_E_CORE_ENVIRONMENT_UNINIT_FAIL)
	 return res;
   }

   res=ChannelDestroy(channel_index);
  }
  catch (RDK::UException &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  // SystemLogger.ProcessException удален - используется glog
  res=RDK_EXCEPTION_CATCHED;
 }


 return res;
}

/// ���������� ���
void URdkCoreManager::Destroy(void)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);

 for(int i=0;i<NumChannels;i++)
 {
  // SystemLogger.LogMessage удален - используется glog(RDK_EX_DEBUG, std::string("Prepearing to Uninitialize channel ")+RDK::sntoa(i));
  ChannelUnInit(i);
  // SystemLogger.LogMessage удален - используется glog(RDK_EX_DEBUG, std::string("Channel ")+RDK::sntoa(i)+std::string(" has been successfully uninitialized"));
 }

 RDK_SYS_TRY
 {
  try
  {
   for(size_t i=0;i<EnvironmentList.size();i++)
	if(EnvironmentList[i])
    {
     // SystemLogger.LogMessage удален - используется glog(RDK_EX_DEBUG, std::string("Deleting previously undeleted environment ")+RDK::sntoa(i));
     delete EnvironmentList[i];
    }

   EnvironmentList.resize(0);

   for(size_t i=0;i<StorageList.size();i++)
	if(StorageList[i])
    {
     // SystemLogger.LogMessage удален - используется glog(RDK_EX_DEBUG, std::string("Deleting previously undeleted storage ")+RDK::sntoa(i));
     delete StorageList[i];
    }

   StorageList.resize(0);

   for(size_t i=0;i<EngineList.size();i++)
	if(EngineList[i])
    {
     // SystemLogger.LogMessage удален - используется glog(RDK_EX_DEBUG, std::string("Deleting previously undeleted engine ")+RDK::sntoa(i));
     delete EngineList[i];
    }

   EngineList.resize(0);

   // LoggerList удален - используется glog

   for(size_t i=0;i<MutexList.size();i++)
	if(MutexList[i])
	 UDestroyMutex(MutexList[i]);

   MutexList.resize(0);

   LockerList.resize(0);
   NumChannels = 0;
  }
  catch (RDK::UException &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
  }
  catch (std::exception &exception)
  {
   // SystemLogger.ProcessException удален - используется glog
  }
 }
 RDK_SYS_CATCH
 {
  // SystemLogger.ProcessException удален - используется glog
 }
}
// --------------------------

// --------------------------
// ������ ������� � �������
// --------------------------
// ���������� ������ �� ��������� ������������ ����
std::shared_ptr<RDK::UEngine>& URdkCoreManager::GetEngine(void)
{
 return Engine;
}

std::shared_ptr<RDK::UEngine> URdkCoreManager::GetEngine(int channel_index)
{
 if(channel_index<0 || channel_index>=int(EngineList.size()))
  return 0;

 return std::shared_ptr<RDK::UEngine>(EngineList[channel_index], [](RDK::UEngine*){});
}

// ���������� ������ �� ��������� ����� ����������
std::shared_ptr<RDK::UEnvironment>& URdkCoreManager::GetEnvironment(void)
{
 return Environment;
}

std::shared_ptr<RDK::UEnvironment> URdkCoreManager::GetEnvironment(int channel_index)
{
 if(channel_index<0 || channel_index>=int(EnvironmentList.size()))
  return 0;

 return std::shared_ptr<RDK::UEnvironment>(EnvironmentList[channel_index], [](RDK::UEnvironment*){});
}

// ���������� ������ �� ��������� ���������
std::shared_ptr<RDK::UStorage>& URdkCoreManager::GetStorage(void)
{
 return Storage;
}

std::shared_ptr<RDK::UStorage> URdkCoreManager::GetStorage(int channel_index)
{
 if(channel_index<0 || channel_index>=int(StorageList.size()))
  return 0;

 return std::shared_ptr<RDK::UStorage>(StorageList[channel_index], [](RDK::UStorage*){});
}

// ���������� ��������� �� ������� ������
std::shared_ptr<RDK::UContainer> URdkCoreManager::GetModel(void)
{
 if(Environment)
  return std::shared_ptr<RDK::UContainer>(Environment->GetModel().get(), [](RDK::UContainer*){});

 return 0;
}

std::shared_ptr<RDK::UContainer> URdkCoreManager::GetModel(int channel_index)
{
 if(channel_index<0 || channel_index>=int(EnvironmentList.size()))
  return 0;

 std::shared_ptr<RDK::UEnvironment> environment(EnvironmentList[channel_index], [](RDK::UEnvironment*){});
 if(environment)
  return std::shared_ptr<RDK::UContainer>(environment->GetModel().get(), [](RDK::UContainer*){});

 return 0;
}
// --------------------------


// --------------------------
// ������ ������� � ������� � ������������
// --------------------------
/// ����� ������� � ����������� ��������
UGenericMutex* URdkCoreManager::GetGlobalMutex(void)
{
 return GlobalMutex;
}

/// ����� ������ � ��������
UGenericMutex* URdkCoreManager::GetEngineMutex(void)
{
 if(MutexList.empty())
  return 0;
 return MutexList[0];
}

UGenericMutex* URdkCoreManager::GetEngineMutex(int index)
{
 if(index<0 || index>=int(MutexList.size()))
  return 0;

 return MutexList[index];
}

// ���������� ������ �� ��������� ������������ ����
RDK::UELockPtr<RDK::UEngine> URdkCoreManager::GetEngineLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEngine>(0,GetEngine());
#else
 return RDK::UELockPtr<RDK::UEngine>(MutexList[SelectedChannelIndex],GetEngine());
#endif
}

RDK::UELockPtr<RDK::UEngine> URdkCoreManager::GetEngineLock(int channel_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEngine>(0,GetEngine(channel_index));
#else
 if(channel_index<0 || channel_index>=int(MutexList.size()))
  return RDK::UELockPtr<RDK::UEngine>(0,0);
 return RDK::UELockPtr<RDK::UEngine>(MutexList[channel_index],GetEngine(channel_index));
#endif
}

RDK::UELockPtr<RDK::UEngine> URdkCoreManager::GetEngineLockTimeout(unsigned timeout)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEngine>(0,GetEngine());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UEngine>(MutexList[SelectedChannelIndex],GetEngine(),timeout);
#endif
}

RDK::UELockPtr<RDK::UEngine> URdkCoreManager::GetEngineLockTimeout(int channel_index, unsigned timeout)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEngine>(0,GetEngine(channel_index));
#else
 if(channel_index<0 || channel_index>=int(MutexList.size()))
  return RDK::UELockPtr<RDK::UEngine>(0,0);
 return RDK::UELockPtr<RDK::UEngine>(MutexList[channel_index],GetEngine(channel_index),timeout);
#endif
}

// ���������� ������ �� ��������� ����� ����������
RDK::UELockPtr<RDK::UEnvironment> URdkCoreManager::GetEnvironmentLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEnvironment>(0,GetEnvironment());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UEnvironment>(MutexList[SelectedChannelIndex],GetEnvironment());
#endif
}

RDK::UELockPtr<RDK::UEnvironment> URdkCoreManager::GetEnvironmentLock(int channel_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEnvironment>(0,GetEnvironment(channel_index));
#else
 if(channel_index<0 || channel_index>=int(MutexList.size()))
  return RDK::UELockPtr<RDK::UEnvironment>(0,0);
 return RDK::UELockPtr<RDK::UEnvironment>(MutexList[channel_index],GetEnvironment(channel_index));
#endif
}

// ���������� ������ �� ��������� ���������
RDK::UELockPtr<RDK::UStorage> URdkCoreManager::GetStorageLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UStorage>(0,GetStorage());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UStorage>(MutexList[SelectedChannelIndex],GetStorage());
#endif
}

RDK::UELockPtr<RDK::UStorage> URdkCoreManager::GetStorageLock(int channel_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UStorage>(0,GetStorage(channel_index));
#else
 if(channel_index<0 || channel_index>=int(MutexList.size()))
  return RDK::UELockPtr<RDK::UStorage>(0,0);
 return RDK::UELockPtr<RDK::UStorage>(MutexList[channel_index],GetStorage(channel_index));
#endif
}

// ���������� ��������� �� ������� ������
RDK::UELockPtr<RDK::UContainer> URdkCoreManager::GetModelLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UContainer>(0,GetModel());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UContainer>(MutexList[SelectedChannelIndex],GetModel());
#endif
}

RDK::UELockPtr<RDK::UContainer> URdkCoreManager::GetModelLock(int channel_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UContainer>(0,GetModel(channel_index));
#else
 if(channel_index<0 || channel_index>=int(MutexList.size()))
  return RDK::UELockPtr<RDK::UContainer>(0,0);
 return RDK::UELockPtr<RDK::UContainer>(MutexList[channel_index],GetModel(channel_index));
#endif
}

RDK::UELockPtr<RDK::UContainer> URdkCoreManager::GetModelLockTimeout(unsigned timeout)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UContainer>(0,GetModel(SelectedChannelIndex));
#else
 return RDK::UELockPtr<RDK::UContainer>(MutexList[SelectedChannelIndex],GetModel(),timeout);
#endif
}

RDK::UELockPtr<RDK::UContainer> URdkCoreManager::GetModelLockTimeout(int channel_index, unsigned timeout)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UContainer>(0,GetModel(channel_index));
#else
 if(channel_index<0 || channel_index>=int(MutexList.size()))
  return RDK::UELockPtr<RDK::UContainer>(0,0);
 return RDK::UELockPtr<RDK::UContainer>(MutexList[channel_index],GetModel(channel_index), timeout);
#endif
}

/// ����� ������ ���������� ������
int URdkCoreManager::LockChannel(int index)
{
 if(RdkCoreManager.LockerList[index])
  return RDK_E_CORE_LOCK_FAIL;

 if(!RdkCoreManager.LockerList[index])
  RdkCoreManager.LockerList[index]=new RDK::UELockPtr<RDK::UEngine>(RDK::GetEngineLock(index));//new UGenericMutexExclusiveLocker(RdkCoreManager.MutexList[index]);

 return RDK_SUCCESS;
}

/// ����� ������ ������ ���������� ������
int URdkCoreManager::UnLockChannel(int index)
{
 if(!RdkCoreManager.LockerList[index])
  return RDK_E_CORE_UNLOCK_FAIL;

 if(RdkCoreManager.LockerList[index])
 {
  delete RdkCoreManager.LockerList[index];//new UGenericMutexExclusiveLocker(RdkCoreManager.MutexList[index]);
  RdkCoreManager.LockerList[index]=0;
 }
 return RDK_SUCCESS;
}
// --------------------------

// --------------------------
/// �������� ������������
// --------------------------
// ���������� ������ �� ��������� �� ������
// GetLogger методы удалены - используется glog

// GetLogger методы удалены - используется glog

/// ���������� ������ �� ��������� ������
// GetLogger методы удалены - используется glog

/// ���������� ���������  �� ���������� ������ (����������� ���������� �� ���� ��������)
// GetLogger методы удалены - используется glog
// --------------------------

// --------------------------
// ��������������� ������ ����������
// --------------------------
/// ������ ������� �������� �����
bool URdkCoreManager::SetSelectedChannelIndex(int channel_index)
{
// UGenericMutexExclusiveLocker lock(GlobalMutex);
 if(channel_index<0 || channel_index>=GetNumChannels())
  return false;

 SelectedChannelIndex=channel_index;
// ::SelectedEngineIndex=SelectedChannelIndex;
 /// ������ �������� ���������� ������
 Engine=std::shared_ptr<RDK::UEngine>(EngineList[channel_index], [](RDK::UEngine*){});
 Environment=std::shared_ptr<RDK::UEnvironment>(EnvironmentList[channel_index], [](RDK::UEnvironment*){});
 Storage=std::shared_ptr<RDK::UStorage>(StorageList[channel_index], [](RDK::UStorage*){});
 // Logger удален - используется glog

 return true;
}
// --------------------------

#endif
