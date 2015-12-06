#ifndef RDK_INIT_CPP
#define RDK_INIT_CPP

#include <exception>
#include "rdk_init.h"
#include "rdk.h"
//#include "rdk_rpc.cpp"
#include "rdk_engine_support.h"
#include "rdk_exceptions.h"
#include "UEnvException.h"
#include "rdk_error_codes.h"
#include "../../System/UGenericMutex.h"
namespace RDK {

// ���������� ��������� �������
RDK::UBitmapFontCollection GlobalFonts;

// ������� ��������� ���������� �������
void ClearClobalFonts(void)
{
 GlobalFonts.DelAllFonts();
}

// ��������� ����� ���������� �����
bool AddGlobalFont(const std::string &font_file_name)
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
  return RDK::GlobalFonts.AddFont(font.GetName(),size,font);
 }
 return false;
}

/// ���������� RDK_UNHANDLED_EXCEPTION ���� �� ������� �������� ������ ����������
/// ����� ���������� RDK_EXCEPTION_CATCHED
bool ProcessException(int channel_index, UException &ex)
{
 UELockPtr<UEngine> engine=DllManager.GetEngineLock(channel_index);
 if(!engine)
  return RDK_UNHANDLED_EXCEPTION;
 engine->ProcessException(ex);
 return RDK_EXCEPTION_CATCHED;
}

}

// --------------------------------------
// ���������� �������������� �������
// --------------------------------------
// ������������� dll
bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine);
// --------------------------------------


/*****************************************************************************/
extern RDK::UStorage* CreateNewStorage(void);

extern RDK::UEnvironment* CreateNewEnvironment(void);

extern RDK::UEngine* CreateNewEngine(void);
/*****************************************************************************/

// ----------------------------
// ������� RPC
// � ������� ���� ������� �������� ������� ����� ������� ����������,
// ������������ ��������� ������
// ----------------------------
/// ��������� ������ � ���������� xml-�������� ������
/// request - xml �������� �������
/// return_value - ������������ �������� ��� ��� �������, ������� ��� �����
/// ��� ��������� ���������� 0
const char* RDK_CALL RemoteCall(const char *request, int &return_value, int &channel_index)
{
 return RDK::RemoteCallInternal(request, return_value, channel_index);
}
// ----------------------------

// ----------------------------
// ������ �������������
// ----------------------------

// ���������� ��� �������� �������� ������
const char* RDK_CALL Core_GetSystemDir(void)
{
 return RdkSystemDir.c_str();
}

const char* RDK_CALL GetSystemDir(void)
{
 return Core_GetSystemDir();
}

// ������������� ��� �������� �������� ������
int RDK_CALL Core_SetSystemDir(const char *dir)
{
 RdkSystemDir=dir;
 return RDK_SUCCESS;
}

int RDK_CALL SetSystemDir(const char *dir)
{
 return Core_SetSystemDir(dir);
}

// ��������� ���������� ������
int RDK_CALL Core_LoadFonts(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // ������ ������
   std::vector<std::string> font_names;
   std::string font_path=RdkSystemDir+"Fonts/";
   RDK::FindFilesList(font_path, "*.fnt", true, font_names);
   if(DllManager.GetEnvironment())
	DllManager.GetEnvironment()->LogMessage(RDK_EX_DEBUG, std::string("Loading fonts form ")+font_path+"\n");

   RDK::ClearClobalFonts();
   RDK::UBitmapFont font;
   for(size_t i=0;i<font_names.size();i++)
   {
	RDK::AddGlobalFont(font_path+font_names[i]);
	if(DllManager.GetEnvironment())
	 DllManager.GetEnvironment()->LogMessage(RDK_EX_DEBUG, std::string("Loaded font ")+font_names[i]+"\n");
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL Engine_LoadFonts(void)
{
 return Core_LoadFonts();
}

// ���������� ����� �������
int RDK_CALL Core_GetNumChannels(void)
{
 return DllManager.GetNumEngines();
}

int RDK_CALL GetNumEngines(void)
{
 return Core_GetNumChannels();
}

// ������� ��������� ����� �������
// num > 0
int RDK_CALL Core_SetNumChannels(int num)
{
 if(num<=0)
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 if(num == GetNumEngines())
  return RDK_SUCCESS;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!DllInit((void*)CreateNewStorage, (void*)CreateNewEnvironment, (void*)CreateNewEngine))
	return RDK_E_CORE_INIT_FAIL;

   res=DllManager.SetNumEngines(num);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL SetNumEngines(int num)
{
 return Core_SetNumChannels(num);
}

// ��������� ������ � ������� ��������� �������
// ���� ������� ����� ��� �������� ��������� ��
// ��������� � �����
int RDK_CALL Core_AddChannel(int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!DllInit((void*)CreateNewStorage, (void*)CreateNewEnvironment, (void*)CreateNewEngine))
	return RDK_E_CORE_INIT_FAIL;

   res=DllManager.Add(index);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL Engine_Add(int index)
{
 return Core_AddChannel(index);
}


// ������� ������ �� �������
int RDK_CALL Core_DelChannel(int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   res=DllManager.Del(index);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL Engine_Del(int index)
{
 return Core_DelChannel(index);
}

// ���������� ������ �������� ���������� ������
int RDK_CALL Core_GetSelectedChannelIndex(void)
{
 return DllManager.GetSelectedChannelIndex();
}

int RDK_CALL GetSelectedEngineIndex(void)
{
 return Core_GetSelectedChannelIndex();
}


// ����������� ������� ��������� �� ������ � �������� �������
// � ������ �������� ������, ��������� ������������� ����������������� �� 0 ������
int RDK_CALL Core_SelectChannel(int index)
{
 if(index<0 || index>=GetNumEngines())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(DllManager.GetSelectedChannelIndex() == index)
	return RDK_SUCCESS;

   RDK::UELockPtr<RDK::UEngine> ptr1(DllManager.GetEngineLock(DllManager.GetSelectedChannelIndex()));
   RDK::UELockPtr<RDK::UEngine> ptr2(DllManager.GetEngineLock(index));

   if(!DllManager.SetSelectedChannelIndex(index))
	res=RDK_E_CORE_CHANNEL_NOT_FOUND;
   else
    res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL SelectEngine(int index)
{
 return Core_SelectChannel(index);
}


/// ��������� ����� �� ������ ������� UnlockEngine
int RDK_CALL Core_LockChannel(void)
{
 return MLockEngine(DllManager.GetSelectedChannelIndex());
}

int RDK_CALL LockEngine(void)
{
 return Core_LockChannel();
}

int RDK_CALL MCore_LockChannel(int index)
{
 if(index<0 || index>=GetNumEngines())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(DllManager.LockerList[index])
	return RDK_SUCCESS;

   if(!DllManager.LockerList[index])
	DllManager.LockerList[index]=new UGenericMutexExclusiveLocker(DllManager.MutexList[index]);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL MLockEngine(int index)
{
 return MCore_LockChannel(index);
}

/// ������������ �����
int RDK_CALL Core_UnLockChannel(void)
{
 return MUnLockEngine(DllManager.GetSelectedChannelIndex());
}

int RDK_CALL UnLockEngine(void)
{
 return Core_UnLockChannel();
}

int RDK_CALL MCore_UnLockChannel(int index)
{
 if(index<0 || index>=GetNumEngines())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!DllManager.LockerList[index])
	return RDK_SUCCESS;

   delete DllManager.LockerList[index];
   DllManager.LockerList[index]=0;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL MUnLockEngine(int index)
{
 return MCore_UnLockChannel(index);
}

int RDK_CALL Core_ChannelInit(int predefined_structure, void* exception_handler)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(GetNumEngines()<=DllManager.GetSelectedChannelIndex())
    res=SetNumEngines(DllManager.GetSelectedChannelIndex()+1);

   if(res != RDK_SUCCESS)
	return res;

   res=MCore_ChannelInit(DllManager.GetSelectedChannelIndex(), predefined_structure, exception_handler);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL EngineInit(int predefined_structure, void* exception_handler)
{
 return Core_ChannelInit(predefined_structure, exception_handler);
}

int RDK_CALL MCore_ChannelInit(int engine_index, int predefined_structure, void* exception_handler)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(engine_index<0 || engine_index>=GetNumEngines())
    return RDK_E_CORE_CHANNEL_NOT_FOUND;

   res=MEngineUnInit(engine_index);
   if(res != RDK_SUCCESS)
	return res;

   res=DllManager.EngineCreate(engine_index);
   if(res != RDK_SUCCESS)
	return res;

   DllManager.GetEngineLock(engine_index)->SetChannelIndex(engine_index);
   DllManager.GetEngineLock(engine_index)->SetBufObjectsMode(BufObjectsMode);
   MEngine_SetExceptionHandler(engine_index, exception_handler);

   MEnv_SetPredefinedStructure(engine_index, predefined_structure);
   MEnv_CreateStructure(engine_index);
   MEnv_Init(engine_index);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL MEngineInit(int engine_index, int predefined_structure, void* exception_handler)
{
 return MCore_ChannelInit(engine_index, predefined_structure, exception_handler);
}


int RDK_CALL GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx,
		void* exception_handler)
{
 int res=RDK_SUCCESS;
 if(GetNumEngines()<=DllManager.GetSelectedChannelIndex())
  res=SetNumEngines(DllManager.GetSelectedChannelIndex()+1);

 if(res != RDK_SUCCESS)
  return res;

 res=MGraphicalEngineInit(DllManager.GetSelectedChannelIndex(), predefined_structure, num_inputs,
		num_outputs, input_width, input_height, reflectionx, exception_handler);

 return res;
}

int RDK_CALL MGraphicalEngineInit(int engine_index, int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx,
		void* exception_handler)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   MCore_ChannelUnInit(engine_index);
   res=DllManager.EngineCreate(engine_index);
   if(res != RDK_SUCCESS)
	return res;

   DllManager.GetEngineLock(engine_index)->SetChannelIndex(engine_index);
   DllManager.GetEngineLock(engine_index)->SetBufObjectsMode(BufObjectsMode);

   MEngine_SetExceptionHandler(engine_index, exception_handler);

   // ������ ����� ������ �����
   MEnv_SetNumInputImages(engine_index, num_inputs);
   MEnv_SetNumOutputImages(engine_index, num_outputs);

   // ������ ���������� �� ��������� (������� ����������)
   for(int i=0;i<num_inputs;i++)
	MEnv_SetInputRes(engine_index, i, input_width, input_height);

   MEnv_SetReflectionXFlag(engine_index, reflectionx);

   MEnv_SetPredefinedStructure(engine_index, predefined_structure);
   MEnv_CreateStructure(engine_index);
   MEnv_Init(engine_index);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

// ���������������� ������ (������� ������������� ���������� ��� ������ �������������)
int RDK_CALL Core_ChannelUnInit(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(DllManager.GetEngine())
   {
	if(!Env_UnInit())
     return RDK_E_CORE_ENVIRONMENT_UNINIT_FAIL;
   }

   res=DllManager.EngineDestroy(DllManager.GetSelectedChannelIndex());
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL EngineUnInit(void)
{
 return Core_ChannelUnInit();
}

int RDK_CALL MCore_ChannelUnInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(DllManager.EngineList[engine_index])
   {
	if(!MEnv_UnInit(engine_index))
     return RDK_E_CORE_ENVIRONMENT_UNINIT_FAIL;
   }

   res=DllManager.EngineDestroy(engine_index);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL MEngineUnInit(int engine_index)
{
 return MCore_ChannelUnInit(engine_index);
}

/// ��������� ��������������� �� ������
bool RDK_CALL Core_IsChannelInit(void)
{
 return (DllManager.GetEngine())?true:false;
}

bool RDK_CALL IsEngineInit(void)
{
 return Core_IsChannelInit();
}

bool RDK_CALL MCore_IsChannelInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;

 return (DllManager.GetEngine(engine_index))?true:false;
}

bool RDK_CALL MIsEngineInit(int engine_index)
{
 return MCore_IsChannelInit(engine_index);
}

/// ����� �������� ���������� ��������� ���������� ���
/// ������������ ��������
/// 0 - ���� ���������� ��� ���� �������, ������������ ����� ���
/// 1 - ���������� ���������� � �������������� ������� ������� �������
int RDK_CALL Engine_GetBufObjectsMode(void)
{
 UGenericMutexExclusiveLocker lock(DllManager.GlobalMutex);
 return BufObjectsMode;
}

int RDK_CALL Engine_SetBufObjectsMode(int mode)
{
 UGenericMutexExclusiveLocker lock(DllManager.GlobalMutex);
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(BufObjectsMode == mode)
	return RDK_SUCCESS;

   for(size_t i=0;i<DllManager.EngineList.size();i++)
   {
	DllManager.GetEngineLock(i)->SetBufObjectsMode(mode);
   }
   BufObjectsMode=mode;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

/// ������������ �������� ������ ������, �� ��������� ���������
int RDK_CALL Engine_FreeBufString(const char *pointer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   DllManager.GetEngineLock()->DestroyTempString(pointer);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

int RDK_CALL MEngine_FreeBufString(int engine_index,const char *pointer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(engine_index<0 || engine_index>=GetNumEngines())
	return RDK_E_CORE_CHANNEL_NOT_FOUND;
   DllManager.GetEngineLock(engine_index)->DestroyTempString(pointer);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(engine_index,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(engine_index,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(engine_index,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

int RDK_CALL Engine_FreeBufStringUnsafe(const char *pointer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   DllManager.GetEngine()->DestroyTempString(pointer);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(0,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(0,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(0,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

int RDK_CALL MEngine_FreeBufStringUnsafe(int engine_index,const char *pointer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   DllManager.GetEngineLock(engine_index)->DestroyTempString(pointer);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(engine_index,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(engine_index,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(engine_index,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

/// ���������� ����� �������� ����� ������
int RDK_CALL Engine_GetNumBufStrings(void)
{
 return DllManager.GetEngineLock()->GetNumTempStrings();
}

int RDK_CALL MEngine_GetNumBufStrings(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return DllManager.GetEngineLock()->GetNumTempStrings();
}

/// ������ � ��������
void* RDK_CALL Engine_GetMutex(void)
{
 return DllManager.GetEngineMutex();
}

void* RDK_CALL MEngine_GetMutex(int index)
{
 return DllManager.GetEngineMutex(index);
}
// ----------------------------

// --------------------------
// ������ ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
int RDK_CALL Storage_GetNumClasses(void)
{
 return DllManager.GetEngineLock()->Storage_GetNumClasses();
}

// ���������� id ������� � ���������. ������ ������ ���� ��������
int RDK_CALL Storage_GetClassesList(int *buffer)
{
 return DllManager.GetEngineLock()->Storage_GetClassesList(buffer);
}

// ���������� ����� ������� � ��������� � ���� ������ ����������� ��������
const char * RDK_CALL Storage_GetClassesNameList(void)
{
 return DllManager.GetEngineLock()->Storage_GetClassesNameList();
}

// ���������� ��� ������ �� ��� id.
const char * RDK_CALL Storage_GetClassName(int id)
{
 return DllManager.GetEngineLock()->Storage_GetClassName(id);
}

// ���������� Id ������ �� ��� �����
int RDK_CALL Storage_GetClassId(const char *name)
{
 return DllManager.GetEngineLock()->Storage_GetClassId(name);
}

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
int RDK_CALL Storage_DelClass(int classid)
{
 return DllManager.GetEngineLock()->Storage_DelClass(classid);
}

// �������� ��� ��������� ������� �� ���������
int RDK_CALL Storage_FreeObjectsStorage(void)
{
 return DllManager.GetEngineLock()->Storage_FreeObjectsStorage();
}

// ������� ��� ������� �� ���������
int RDK_CALL Storage_ClearObjectsStorage(void)
{
 return DllManager.GetEngineLock()->Storage_ClearObjectsStorage();
}

// ��������� ��������� ����� �������� � ���������
int RDK_CALL Storage_CalcNumObjects(void)
{
 return DllManager.GetEngineLock()->Storage_CalcNumObjects();
}

int RDK_CALL Storage_CalcNumObjectsById(int classid)
{
 return DllManager.GetEngineLock()->Storage_CalcNumObjectsById(classid);
}

int RDK_CALL Storage_CalcNumObjectsByName(const char* classname)
{
 return DllManager.GetEngineLock()->Storage_CalcNumObjectsByName(classname);
}

// ���������� �������� ������ �� ��� id � ������� xml
const char* RDK_CALL Storage_GetClassDescription(const char* classname)
{
 return DllManager.GetEngineLock()->Storage_GetClassDescription(classname);
}

// ������������� �������� ������ �� ��� id, �������� ��� �� ������� xml
bool RDK_CALL Storage_SetClassDescription(const char* classname, const char* description)
{
 return DllManager.GetEngineLock()->Storage_SetClassDescription(classname, description);
}

// ��������� �������� ���� ������� � xml
const char* RDK_CALL Storage_SaveClassesDescription(void)
{
 return DllManager.GetEngineLock()->Storage_SaveClassesDescription();
}

// ��������� �������� ���� ������� �� xml
int RDK_CALL Storage_LoadClassesDescription(const char* xmltext)
{
 if(!DllManager.GetEngineLock()->Storage_LoadClassesDescription(xmltext))
  return 10;
 return 0;
}

int RDK_CALL MStorage_LoadClassesDescription(int engine_index, const char* xmltext)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 if(!DllManager.GetEngineLock(engine_index)->Storage_LoadClassesDescription(xmltext))
  return 10;

 return 0;
}

// ��������� ����� �������� ���� ������� � xml
const char* RDK_CALL Storage_SaveCommonClassesDescription(void)
{
 return DllManager.GetEngineLock()->Storage_SaveCommonClassesDescription();
}

// ��������� ����� �������� ���� ������� �� xml
int RDK_CALL Storage_LoadCommonClassesDescription(const char* xmltext)
{
 if(!DllManager.GetEngineLock()->Storage_LoadCommonClassesDescription(xmltext))
  return 10;
 return 0;
}

int RDK_CALL MStorage_LoadCommonClassesDescription(int engine_index, const char* xmltext)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 if(!DllManager.GetEngineLock(engine_index)->Storage_LoadCommonClassesDescription(xmltext))
  return 10;

 return 0;
}


// ��������� �������� ���� ������� � xml ������� ����� ��������
const char* RDK_CALL Storage_SaveAllClassesDescription(void)
{
 return DllManager.GetEngineLock()->Storage_SaveAllClassesDescription();
}

// ��������� �������� ���� ������� �� xml ������� ����� ��������
bool RDK_CALL Storage_LoadAllClassesDescription(const char* xmltext)
{
 return DllManager.GetEngineLock()->Storage_LoadAllClassesDescription(xmltext);
}
// ----------------------------

// ----------------------------
// ������ ���������� ����������� ���������
// ----------------------------
// ���������� ����� ���������
int RDK_CALL Storage_GetNumClassLibraries(void)
{
 return DllManager.GetEngineLock()->Storage_GetNumClassLibraries();
}

// ���������� ������ ��������� � ���� ������, ����������� ��������
const char* RDK_CALL Storage_GetClassLibrariesList(void)
{
 return DllManager.GetEngineLock()->Storage_GetClassLibrariesList();
}

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// library_name - ��� ����������
const char* RDK_CALL Storage_GetLibraryClassNames(const char *library_name)
{
 return DllManager.GetEngineLock()->Storage_GetLibraryClassNames(library_name);
}

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// index - ������ ����������
const char* RDK_CALL Storage_GetLibraryClassNamesByIndex(int index)
{
 return DllManager.GetEngineLock()->Storage_GetLibraryClassNamesByIndex(index);
}

// ���������� ��� ���������� �� �������
const char * RDK_CALL Storage_GetClassLibraryNameByIndex(int index)
{
 return DllManager.GetEngineLock()->Storage_GetClassLibraryNameByIndex(index);
}

// ���������� ������ ���������� �� �������
const char * RDK_CALL Storage_GetClassLibraryVersionByIndex(int index)
{
 return DllManager.GetEngineLock()->Storage_GetClassLibraryVersionByIndex(index);
}

/// ������� ����� runtime-����������
int RDK_CALL Storage_CreateRuntimeCollection(const char *collection_name)
{
 return DllManager.GetEngineLock()->Storage_CreateRuntimeCollection(collection_name);
}

// ��������� ��������� �� ����� dll-�����
int RDK_CALL Storage_LoadBinaryCollectionFromFile(const char *filename)
{
 return DllManager.GetEngineLock()->Storage_LoadBinaryCollectionFromFile(filename);
}

// ��������� runtime-���������
int RDK_CALL Storage_LoadRuntimeCollectionFromFile(const char *filename)
{
 return DllManager.GetEngineLock()->Storage_LoadRuntimeCollectionFromFile(filename);
}

int RDK_CALL Storage_LoadRuntimeCollectionFromString(const char *buffer)
{
 return DllManager.GetEngineLock()->Storage_LoadRuntimeCollectionFromString(buffer);
}

// ��������� runtime-���������
int RDK_CALL Storage_SaveRuntimeCollectionToFile(const char *filename)
{
 return DllManager.GetEngineLock()->Storage_SaveRuntimeCollectionToFile(filename);
}

int RDK_CALL Storage_SaveRuntimeCollectionToString(const char *buffer)
{
 return DllManager.GetEngineLock()->Storage_SaveRuntimeCollectionToString(buffer);
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
int RDK_CALL Storage_DelClassLibraryByIndex(int index)
{
 return DllManager.GetEngineLock()->Storage_DelClassLibraryByIndex(index);
}

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
int RDK_CALL Storage_DelClassLibraryByName(const char *name)
{
 return DllManager.GetEngineLock()->Storage_DelClassLibraryByName(name);
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
int RDK_CALL Storage_DelAllClassLibraries(void)
{
 return DllManager.GetEngineLock()->Storage_DelAllClassLibraries();
}

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
int RDK_CALL Storage_CreateClass(const char* stringid, const char *classname, const char *collection_name)
{
 return DllManager.GetEngineLock()->Storage_CreateClass(stringid, classname, collection_name);
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
int RDK_CALL Storage_BuildStorage(void)
{
 return DllManager.GetEngineLock()->Storage_BuildStorage();
}
// ----------------------------


// ----------------------------
// ������ ���������� ������
// ----------------------------
// ������ ������������� �������� ������ ���������
int RDK_CALL Env_GetPredefinedStructure(void)
{
 return DllManager.GetEngineLock()->Env_GetPredefinedStructure();
}

int RDK_CALL MEnv_GetPredefinedStructure(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.GetEngineLock(engine_index)->Env_GetPredefinedStructure();
}

bool RDK_CALL Env_SetPredefinedStructure(int value)
{
 return DllManager.GetEngineLock()->Env_SetPredefinedStructure(value);
}

bool RDK_CALL MEnv_SetPredefinedStructure(int engine_index, int value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.GetEngineLock(engine_index)->Env_SetPredefinedStructure(value);
}

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool RDK_CALL Env_IsStoragePresent(void)
{
 return DllManager.GetEngineLock()->Env_IsStoragePresent();
}

bool RDK_CALL MEnv_IsStoragePresent(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.GetEngineLock(engine_index)->Env_IsStoragePresent();
}

// ���������� ��������� �������������
bool RDK_CALL Env_IsInit(void)
{
 return DllManager.GetEngineLock()->Env_IsInit();
}

bool RDK_CALL MEnv_IsInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.GetEngineLock(engine_index)->Env_IsInit();
}


// ������� ������� �������������� ���������
bool RDK_CALL Env_IsStructured(void)
{
 return DllManager.GetEngineLock()->Env_IsStructured();
}

bool RDK_CALL MEnv_IsStructured(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.GetEngineLock(engine_index)->Env_IsStructured();
}

// ������������� �����
bool RDK_CALL Env_Init(void)
{
 return DllManager.GetEngineLock()->Env_Init();
}

bool RDK_CALL MEnv_Init(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.GetEngineLock(engine_index)->Env_Init();
}

// ��������������� �����
bool RDK_CALL Env_UnInit(void)
{
 return DllManager.GetEngineLock()->Env_UnInit();
}

bool RDK_CALL MEnv_UnInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.GetEngineLock(engine_index)->Env_UnInit();
}

// ��������� �������������� �������� ������ ���������
bool RDK_CALL Env_CreateStructure(void)
{
 return DllManager.GetEngineLock()->Env_CreateStructure();
}

bool RDK_CALL MEnv_CreateStructure(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.GetEngineLock(engine_index)->Env_CreateStructure();
}

// ���������� ������� ������ ���������
bool RDK_CALL Env_DestroyStructure(void)
{
 return DllManager.GetEngineLock()->Env_DestroyStructure();
}

bool RDK_CALL MEnv_DestroyStructure(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.GetEngineLock(engine_index)->Env_DestroyStructure();
}

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
void RDK_CALL Env_Destroy(void)
{
 return DllManager.GetEngineLock()->Env_Destroy();
}

void RDK_CALL MEnv_Destroy(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;
 return DllManager.GetEngineLock(engine_index)->Env_Destroy();
}

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
int RDK_CALL Env_Calculate(const char* stringid)
{
#ifdef RDK_UNSAFE_CALCULATE
 return DllManager.GetEngine()->Env_Calculate(stringid);
#endif
 return DllManager.GetEngineLock()->Env_Calculate(stringid);
}

int RDK_CALL MEnv_Calculate(int engine_index, const char* stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
#ifdef RDK_UNSAFE_CALCULATE
 return DllManager.GetEngine(engine_index)->Env_Calculate(stringid);
#endif
 return DllManager.GetEngineLock(engine_index)->Env_Calculate(stringid);
}

int RDK_CALL MEnv_CalculateUnsafe(int engine_index, const char* stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.GetEngine(engine_index)->Env_Calculate(stringid);
}

// ������ ���� ������ � �������� �������
void RDK_CALL Env_RTCalculate(void)
{
 DllManager.GetEngineLock()->Env_RTCalculate();
}

void RDK_CALL MEnv_RTCalculate(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 DllManager.GetEngineLock(engine_index)->Env_RTCalculate();
}

// ����� ������ �����
// ���� stringid == 0 �� ���������� ��� ������ �������,
// ����� - ������ ��������� ��������� ������
int RDK_CALL Env_Reset(const char* stringid)
{
 return DllManager.GetEngineLock()->Env_Reset(stringid);
}

int RDK_CALL MEnv_Reset(int engine_index, const char* stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.GetEngineLock(engine_index)->Env_Reset(stringid);
}

/// ����� ������ ���������� �� �������� �� ���������
/// ���� stringid == 0 �� ���������� ��� ������ �������,
/// ����� - ������ ��������� ��������� ������
/// ���� subcomps == true �� ����� ���������� ��������� ���� �������� ���������
int RDK_CALL Env_Default(const char* stringid, bool subcomps)
{
 return DllManager.GetEngineLock()->Env_Default(stringid,subcomps);
}

int RDK_CALL MEnv_Default(int engine_index, const char* stringid, bool subcomps)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.GetEngineLock(engine_index)->Env_Default(stringid,subcomps);
}

// ���������� ���������� ������� ������ �� ��������� ��������
void RDK_CALL Env_IncreaseModelTimeByStep(void)
{
 return DllManager.GetEngineLock()->Env_IncreaseModelTimeByStep();
}

/// ������������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
int RDK_CALL Env_SetMinInterstepsInterval(unsigned long long value)
{
 return DllManager.GetEngineLock()->Env_SetMinInterstepsInterval(value);
}

int RDK_CALL MEnv_SetMinInterstepsInterval(int engine_index, unsigned long long value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.GetEngineLock(engine_index)->Env_SetMinInterstepsInterval(value);
}

/// ���������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
unsigned long long RDK_CALL Env_GetMinInterstepsInterval(void)
{
 return DllManager.GetEngineLock()->Env_GetMinInterstepsInterval();
}

unsigned long long RDK_CALL Env_GetMinInterstepsInterval(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngineLock(engine_index)->Env_GetMinInterstepsInterval();
}


// �����, ����������� �� ��������� RT-������
double RDK_CALL Env_GetRTLastDuration(void)
{
 return DllManager.GetEngineLock()->Env_GetRTLastDuration();
}

double RDK_CALL MEnv_GetRTLastDuration(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Env_GetRTLastDuration();
}

/// �����, ����������� � ������ �� ���� ����� RTCalculate;
double RDK_CALL Env_GetRTModelCalcTime(void)
{
 return DllManager.GetEngineLock()->Env_GetRTModelCalcTime();
}

double RDK_CALL MEnv_GetRTModelCalcTime(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Env_GetRTModelCalcTime();
}

/// ������������������ RT ������� (��������� RTModelCalcTime/RTLastDuration)
double RDK_CALL Env_CalcRTPerformance(void)
{
 return DllManager.GetEngineLock()->Env_CalcRTPerformance();
}

double RDK_CALL MEnv_CalcRTPerformance(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Env_CalcRTPerformance();
}

// ���������� ��� �������� �������� ��� �������� ������
const char* RDK_CALL Env_GetCurrentDataDir(void)
{
 return DllManager.GetEngineLock()->Env_GetCurrentDataDir();
}

// ������������� ��� �������� �������� ��� �������� ������
int RDK_CALL Env_SetCurrentDataDir(const char *dir)
{
 return DllManager.GetEngineLock()->Env_SetCurrentDataDir(dir);
}

/// ���������� ��������� ����� ����������� ������ �����
bool RDK_CALL Env_GetDebugMode(void)
{
 return DllManager.GetEngineLock()->Env_GetDebugMode();
}

bool RDK_CALL MEnv_GetDebugMode(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngineLock(engine_index)->Env_GetDebugMode();
}

/// ������������� ��������� ����� ����������� ������ �����
int RDK_CALL Env_SetDebugMode(bool value)
{
 return DllManager.GetEngineLock()->Env_SetDebugMode(value);
}

int RDK_CALL MEnv_SetDebugMode(int engine_index, bool value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngineLock(engine_index)->Env_SetDebugMode(value);
}


// ***********************************************
// ������ ���������� ������� �����������
// !!! ��������� ������ ������ �� ���
// ������, ������������ � ����������� �� ���������� id !!!
// ***********************************************
// ������������� ������� ��������� (��������� ������������ ����� - ������)
int RDK_CALL Env_SelectCurrentComponent(const char *stringid)
{
 return DllManager.GetEngineLock()->Env_SelectCurrentComponent(stringid);
}

// ���������� ������� ��������� � ��������� �� ��������� (������)
int RDK_CALL Env_ResetCurrentComponent(const char *stringid)
{
 return DllManager.GetEngineLock()->Env_ResetCurrentComponent(stringid);
}

// ������ ������� ��������� �� ��� �������� (������ �� ������� �����)
// ���� ��� �� ������� ������, �� �� ������ ������
int RDK_CALL Env_UpCurrentComponent(void)
{
 return DllManager.GetEngineLock()->Env_UpCurrentComponent();
}

// ������ ������� ��������� �� ��� �������� �� ������������ ������ �����������
// (����� �� N ������� ���� ������������ �������� ����������)
int RDK_CALL Env_DownCurrentComponent(const char *stringid)
{
 return DllManager.GetEngineLock()->Env_DownCurrentComponent(stringid);
}

// ���������� ������� ��� �������� ����������
const char* RDK_CALL Env_GetCurrentComponentName(void)
{
 return DllManager.GetEngineLock()->Env_GetCurrentComponentName();
}

// ���������� ������� ��������� id �������� ����������
const char* RDK_CALL Env_GetCurrentComponentId(void)
{
 return DllManager.GetEngineLock()->Env_GetCurrentComponentId();
}
// ***********************************************

/// ���������� ��������� � ���� � ������ ��������� ������
bool RDK_CALL Env_CallSourceController(void)
{
 return DllManager.GetEngineLock()->Env_CallSourceController();
}

bool RDK_CALL MEnv_CallSourceController(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.GetEngineLock(engine_index)->Env_CallSourceController();
}
// --------------------------

// --------------------------
// ������ ���������� �������
// ----------------------------
// ������� ������
int RDK_CALL Model_Destroy(void)
{
 return DllManager.GetEngineLock()->Model_Destroy();
}

int RDK_CALL MModel_Destroy(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.GetEngineLock(engine_index)->Model_Destroy();
}

// ������� ����� ������ �� ����� ������ � ���������
// �������������� ������� ������������ ������
int RDK_CALL Model_Create(const char *classname)
{
 return DllManager.GetEngineLock()->Model_Create(classname);
}

int RDK_CALL MModel_Create(int engine_index, const char *classname)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.GetEngineLock(engine_index)->Model_Create(classname);
}

// ������� ������
int RDK_CALL Model_Clear(void)
{
 return DllManager.GetEngineLock()->Model_Clear();
}

int RDK_CALL MModel_Clear(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.GetEngineLock(engine_index)->Model_Clear();
}


// ���������, ���������� �� ������
bool RDK_CALL Model_Check(void)
{
 if(!DllManager.GetEngine())
  return false;
 return DllManager.GetEngineLock()->Model_Check();
}

bool RDK_CALL MModel_Check(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
// UGenericMutexLocker locker(DllManager.MutexList[engine_index]);
 if(!DllManager.GetEngine(engine_index))
  return false;
 return DllManager.GetEngineLock(engine_index)->Model_Check();
}

// ���������, ���������� �� � ������ ��������� � ������ stringid)
bool RDK_CALL Model_CheckComponent(const char* stringid)
{
 return DllManager.GetEngineLock()->Model_CheckComponent(stringid);
}

bool RDK_CALL MModel_CheckComponent(int engine_index, const char* stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.GetEngineLock(engine_index)->Model_CheckComponent(stringid);
}

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'classname'
// ���� stringid - ������ ������, �� ��������� � ���� ������
// ���������� ��� ���������� � ������ ������
const char* RDK_CALL Model_AddComponent(const char* stringid, const char *classname)
{
 return DllManager.GetEngineLock()->Model_AddComponent(stringid, classname);
}

const char* RDK_CALL MModel_AddComponent(int engine_index, const char* stringid, const char *classname)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngineLock(engine_index)->Model_AddComponent(stringid, classname);
}

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'name'
// ���� stringid - ������ ������, �� ������� �� ����� ������
int RDK_CALL Model_DelComponent(const char* stringid, const char *name)
{
 return DllManager.GetEngineLock()->Model_DelComponent(stringid, name);
}

int RDK_CALL MModel_DelComponent(int engine_index, const char* stringid, const char *name)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.GetEngineLock(engine_index)->Model_DelComponent(stringid, name);
}

/// ���������� ���������� � ������ ���������
/// ���� comp �� ����������� ����� ����������, ��� target ����� �������� ��
/// ����� ���������� storage, ��� target �� ����� ������� � ���� ���������
/// �� ���������� false � �� ������ ������
int RDK_CALL Model_MoveComponent(const char* component, const char* target)
{
 return DllManager.GetEngineLock()->Model_MoveComponent(component, target);
}

int RDK_CALL MModel_MoveComponent(int engine_index, const char* component, const char* target)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.GetEngineLock(engine_index)->Model_MoveComponent(component, target);
}

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
int RDK_CALL Model_GetNumComponents(const char* stringid)
{
 return DllManager.GetEngineLock()->Model_GetNumComponents(stringid);
}

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
int RDK_CALL Model_GetComponentsList(const char* stringid, int *buffer)
{

 return DllManager.GetEngineLock()->Model_GetComponentsList(stringid, buffer);
}

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ','
const char* RDK_CALL Model_GetComponentsNameList(const char* stringid)
{

 return DllManager.GetEngineLock()->Model_GetComponentsNameList(stringid);
}

const char* RDK_CALL MModel_GetComponentsNameList(int engine_index, const char* stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->Model_GetComponentsNameList(stringid);
}

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ',' � ������� ��� ������ 'class_name'
// ���� find_all == true �� ����� ������� � �� ���� ��������������
const char* RDK_CALL Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all)
{

 return DllManager.GetEngineLock()->Model_FindComponentsByClassName(stringid,class_name,find_all);
}

// ���������� ��������� � ������� �������� index ��� ������ 'name' ����� ���
// ���� �� ������ �� �������� ����� ���������
// ����������� ��� ��������� ������� ������� ���������
// ���� �������� 'step' ������� �� ������� �������, �� ��������� ���������������
// �� ��� �������
int RDK_CALL Model_ChangeComponentPosition(const char* stringid, int step)
{

 return DllManager.GetEngineLock()->Model_ChangeComponentPosition(stringid, step);
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
const char* RDK_CALL Model_GetConnectorsList(const char* stringid,
						  int sublevel, const char* owner_level_stringid)
{

 return DllManager.GetEngineLock()->Model_GetConnectorsList(stringid, sublevel, owner_level_stringid);
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
const char* RDK_CALL Model_GetItemsList(const char* stringid,
							int sublevel, const char* owner_level_stringid)
{

 return DllManager.GetEngineLock()->Model_GetItemsList(stringid, sublevel, owner_level_stringid);
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
const char* RDK_CALL Model_GetNetsList(const char* stringid,
							int sublevel, const char* owner_level_stringid)
{

 return DllManager.GetEngineLock()->Model_GetNetsList(stringid, sublevel, owner_level_stringid);
}

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
const char* RDK_CALL Model_GetComponentName(const char* stringid)
{

 return DllManager.GetEngineLock()->Model_GetComponentName(stringid);
}

// ���������� ������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
const char* RDK_CALL Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid)
{

 return DllManager.GetEngineLock()->Model_GetComponentLongName(stringid,owner_level_stringid);
}

const char* RDK_CALL MModel_GetComponentLongName(int engine_index, const char* stringid, const char* owner_level_stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->Model_GetComponentLongName(stringid,owner_level_stringid);
}

// ���������� ������� id ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
const char* RDK_CALL Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid)
{

 return DllManager.GetEngineLock()->Model_GetComponentLongId(stringid,owner_level_stringid);
}

// ���������� ��� ������ ���������� � ��������� �� �������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������ ������
const char* RDK_CALL Model_GetComponentClassName(const char* stringid)
{

 return DllManager.GetEngineLock()->Model_GetComponentClassName(stringid);
}

const char* RDK_CALL MModel_GetComponentClassName(int engine_index, const char* stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->Model_GetComponentClassName(stringid);
}


// ���������� ������ ������� ���������� ����������� ��������
const char* RDK_CALL Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_GetComponentPropertiesList(stringid,type_mask);
}

// ���������� ������ ���� � �������� ������� ���������� ����������� ��������
// ������ ������� ����� ��� ���_��������:������_�����(������)
const char* RDK_CALL Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_GetComponentPropertiesLookupList(stringid,type_mask);
}

// ���������� �������� ���������� �� ��������������
const char * RDK_CALL Model_GetComponentProperties(const char *stringid, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_GetComponentProperties(stringid,type_mask);
}

// ���������� �������� ���������� �� �������������� � ����������
const char * RDK_CALL Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_GetComponentPropertiesEx(stringid, type_mask);
}

// ���������� ���������� �������� ���������� �� ��������������
const char * RDK_CALL Model_GetComponentSelectedProperties(const char *stringid, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_GetComponentSelectedProperties(stringid);
}

// ���������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
const char * RDK_CALL Model_GetComponentPropertyValue(const char *stringid, const char *paramname)
{

 return DllManager.GetEngineLock()->Model_GetComponentPropertyValue(stringid,paramname);
}

const char * RDK_CALL MModel_GetComponentPropertyValue(int engine_index, const char *stringid, const char *paramname)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;


 return DllManager.GetEngineLock(engine_index)->Model_GetComponentPropertyValue(stringid,paramname);
}

// ������������� �������� ���������� �� ��������������
int RDK_CALL Model_SetComponentProperties(const char *stringid, const char* buffer)
{

 return DllManager.GetEngineLock()->Model_SetComponentProperties(stringid, buffer);
}

// ������������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
void RDK_CALL Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer)
{

 DllManager.GetEngineLock()->Model_SetComponentPropertyValue(stringid,paramname,buffer);
}

int RDK_CALL MModel_SetComponentPropertyValue(int engine_index, const char *stringid, const char *paramname, const char *buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 DllManager.GetEngineLock(engine_index)->Model_SetComponentPropertyValue(stringid,paramname,buffer);
 return 0;
}

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
void RDK_CALL Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer)
{

 DllManager.GetEngineLock()->Model_SetGlobalComponentPropertyValue(stringid,class_stringid, paramname,buffer);
}

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
void RDK_CALL Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer)
{

 DllManager.GetEngineLock()->Model_SetGlobalOwnerComponentPropertyValue(stringid, class_stringid, class_owner_stringid, paramname,buffer);
}

// ���������� ��������� void* �� ������ �������� ����������
const void* RDK_CALL Model_GetComponentPropertyData(const char *stringid, const char *property_name)
{

 return DllManager.GetEngineLock()->Model_GetComponentPropertyData(stringid, property_name);
}

const void* RDK_CALL MModel_GetComponentPropertyData(int engine_index, const char *stringid, const char *property_name)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->Model_GetComponentPropertyData(stringid, property_name);
}

// �������� ������ 'data' � �������� �������� ����������
int RDK_CALL Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data)
{

 return DllManager.GetEngineLock()->Model_SetComponentPropertyData(stringid, property_name, data);
}

int RDK_CALL MModel_SetComponentPropertyData(int engine_index, const char *stringid, const char *property_name, const void *data)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_SetComponentPropertyData(stringid, property_name, data);
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentParameters(const char *stringid, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_GetComponentProperties(stringid,type_mask & 0xFFFFFF01);
}

const char * RDK_CALL MModel_GetComponentParameters(int engine_index, const char *stringid, unsigned int type_mask)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->Model_GetComponentProperties(stringid,type_mask & 0xFFFFFF01);
}

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid)
{

 return DllManager.GetEngineLock()->Model_GetComponentSelectedProperties(stringid);
}

// ���������� ��������� ���������� �� �������������� � ����������
const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_GetComponentPropertiesEx(stringid, type_mask & 0xFFFFFF01);
}

// ���������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname)
{

 return DllManager.GetEngineLock()->Model_GetComponentPropertyValue(stringid,paramname);
}

const char * RDK_CALL MModel_GetComponentParameterValue(int engine_index, const char *stringid, const char *paramname)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->Model_GetComponentPropertyValue(stringid,paramname);
}

// ������������� ��������� ���������� �� ��������������
int RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer)
{

 return DllManager.GetEngineLock()->Model_SetComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_SetComponentParameters(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_SetComponentProperties(stringid, buffer);
}

// ������������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
int RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer)
{

 DllManager.GetEngineLock()->Model_SetComponentPropertyValue(stringid,paramname,buffer);
 return 0;
}

int RDK_CALL MModel_SetComponentParameterValue(int engine_index, const char *stringid, const char *paramname, const char *buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 DllManager.GetEngineLock(engine_index)->Model_SetComponentPropertyValue(stringid,paramname,buffer);
 return 0;
}

// ��������� ��������� ���������� ���� � ������
int RDK_CALL Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 return DllManager.GetEngineLock()->Model_CreateLink(stringid1, output_number, stringid2, input_number);
}

int RDK_CALL Model_CreateLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 return DllManager.GetEngineLock()->Model_CreateLink(stringid1, item_property_name, stringid2, connector_property_name);
}


// ��������� ��� ���������� ���������� ���������� �� ����������� id � �������: 0 ����� � 0 �����
int RDK_CALL Model_ChainLinking(const char* stringid)
{
 return DllManager.GetEngineLock()->Model_ChainLinking(stringid);
}

// ��������� ��� ���������� ���������� ���������� �����������, ��������� �� � ������������ ����� ������� ������
// ������������ ��� ������������ ������������������
int RDK_CALL Model_ParallelLinking(const char* stringid)
{
 return DllManager.GetEngineLock()->Model_ParallelLinking(stringid);
}

// ��������� ��������� �����
int RDK_CALL Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 return DllManager.GetEngineLock()->Model_BreakLink(stringid1, output_number, stringid2, input_number);
}

int RDK_CALL Model_BreakLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 return DllManager.GetEngineLock()->Model_BreakLink(stringid1, item_property_name, stringid2, connector_property_name);
}

// ��������� ��� �����
int RDK_CALL Model_BreakAllLinks(void)
{
 return DllManager.GetEngineLock()->Model_BreakAllLinks();
}

// ��������� ����� �� ����� connector_index ���������� 'connectorid'
int RDK_CALL Model_BreakConnectorLink(const char* connectorname, int connector_index)
{
 return DllManager.GetEngineLock()->Model_BreakConnectorLink(connectorname, connector_index);
}

// ��������� ��� ������� � �������� ����� ���������� ����������
int RDK_CALL Model_BreakAllComponentLinks(const char* stringid)
{
 return DllManager.GetEngineLock()->Model_BreakAllComponentLinks(stringid);
}

// ��������� ��� ������� ����� ���������� ����������
int RDK_CALL Model_BreakAllComponentInputLinks(const char* stringid)
{
 return DllManager.GetEngineLock()->Model_BreakAllComponentInputLinks(stringid);
}

// ��������� ��� �������� ����� ���������� ����������
int RDK_CALL Model_BreakAllComponentOutputLinks(const char* stringid)
{

 return DllManager.GetEngineLock()->Model_BreakAllComponentOutputLinks(stringid);
}

// ���������, ���������� �� ������� �����
bool RDK_CALL Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{

 return DllManager.GetEngineLock()->Model_CheckLink(stringid1, output_number, stringid2, input_number);
}

bool RDK_CALL Model_CheckLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 return DllManager.GetEngineLock()->Model_CheckLink(stringid1, item_property_name, stringid2, connector_property_name);
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char * RDK_CALL Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid)
{

 return DllManager.GetEngineLock()->Model_GetComponentInternalLinks(stringid, owner_level_stringid);
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� � ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����������� ������� �������� ����������
int RDK_CALL Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid)
{

 return DllManager.GetEngineLock()->Model_SetComponentInternalLinks(stringid,buffer, owner_level_stringid);
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char * RDK_CALL Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{

 return DllManager.GetEngineLock()->Model_GetComponentInputLinks(stringid,owner_level_stringid, sublevel);
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char * RDK_CALL Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{

 return DllManager.GetEngineLock()->Model_GetComponentOutputLinks(stringid,owner_level_stringid, sublevel);
}

// ��������� ��� ����� ����� ����� ������������ � ���� xml � ����� buffer
// ������� ����� ����� ����������
// ���� 'sublevel' == -1, �� ���������� ����� ��� ����� ����� �������� � ����� �������� �����������
// ������� �������. �������� ����������� � ��� �������.
// ���� 'sublevel' == 0, �� ���������� ����� ������ ����� ����� ���������
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char* RDK_CALL Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid)
{

 return DllManager.GetEngineLock()->Model_GetComponentPersonalLinks(stringid,owner_level_stringid);
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentState(const char *stringid, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_GetComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

const char * RDK_CALL MModel_GetComponentState(int engine_index, const char *stringid, unsigned int type_mask)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->Model_GetComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

// ���������� ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid)
{

 return DllManager.GetEngineLock()->Model_GetComponentSelectedProperties(stringid);
}

// ���������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename)
{

 return DllManager.GetEngineLock()->Model_GetComponentPropertyValue(stringid,statename);
}

const char * RDK_CALL MModel_GetComponentStateValue(int engine_index, const char *stringid, const char *statename)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->Model_GetComponentPropertyValue(stringid,statename);
}

// ������������� ��������� ���������� �� ��������������
int RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer)
{

 return DllManager.GetEngineLock()->Model_SetComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_SetComponentState(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_SetComponentProperties(stringid, buffer);
}

// ������������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
void RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer)
{

 DllManager.GetEngineLock()->Model_SetComponentPropertyValue(stringid,statename,buffer);
}

// ���������� ����� ������ � ����������
int RDK_CALL Model_GetComponentNumInputs(const char *stringid)
{

 return DllManager.GetEngineLock()->Model_GetComponentNumInputs(stringid);
}

// ���������� ������ ����� ���������� � ����� ���������
int RDK_CALL Model_GetComponentInputDataSize(const char *stringid, int index)
{

 return DllManager.GetEngineLock()->Model_GetComponentInputDataSize(stringid, index);
}

// ���������� ������ �������� ����� � ������
int RDK_CALL Model_GetComponentInputElementSize(const char *stringid, int index)
{

 return DllManager.GetEngineLock()->Model_GetComponentInputElementSize(stringid, index);
}

// ���������� ������ ����� ���������� � ������ ���������
int RDK_CALL Model_GetComponentInputByteSize(const char *stringid, int index)
{

 return DllManager.GetEngineLock()->Model_GetComponentInputByteSize(stringid, index);
}

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
unsigned char* RDK_CALL Model_GetComponentInputData(const char *stringid, int index)
{

 return DllManager.GetEngineLock()->Model_GetComponentInputData(stringid, index);
}

// ���������� ����� ������� � ����������
int RDK_CALL Model_GetComponentNumOutputs(const char *stringid)
{

 return DllManager.GetEngineLock()->Model_GetComponentNumOutputs(stringid);
}

// ���������� ������ ������ ���������� � ����� ���������
int RDK_CALL Model_GetComponentOutputDataSize(const char *stringid, int index)
{

 return DllManager.GetEngineLock()->Model_GetComponentOutputDataSize(stringid, index);
}

// ���������� ������ �������� ������ � ������
//int RDK_CALL Model_GetComponentOutputElementSize(const char *stringid, int index)
//{
//
// return DllManager.GetEngineLock()->Model_GetComponentOutputElementSize(stringid, index);
//}

// ���������� ������ ������ ���������� � ������ ���������
int RDK_CALL Model_GetComponentOutputByteSize(const char *stringid, int index)
{

 return DllManager.GetEngineLock()->Model_GetComponentOutputByteSize(stringid, index);
}

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
unsigned char* RDK_CALL Model_GetComponentOutputData(const char *stringid, int index)
{

 return DllManager.GetEngineLock()->Model_GetComponentOutputData(stringid, index);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
const char * RDK_CALL Model_SaveComponent(const char *stringid, unsigned int params_type_mask)
{

 return DllManager.GetEngineLock()->Model_SaveComponent(stringid, params_type_mask);
}

const char * RDK_CALL MModel_SaveComponent(int engine_index, const char *stringid, unsigned int params_type_mask)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->Model_SaveComponent(stringid, params_type_mask);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
int RDK_CALL Model_SaveComponentToFile(const char *stringid, const char* file_name, unsigned int params_type_mask)
{

 return DllManager.GetEngineLock()->Model_SaveComponentToFile(stringid, file_name, params_type_mask);
}

int RDK_CALL MModel_SaveComponentToFile(int engine_index, const char *stringid, const char* file_name, unsigned int params_type_mask)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->Model_SaveComponentToFile(stringid, file_name, params_type_mask);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int RDK_CALL Model_LoadComponent(const char *stringid, const char* buffer)
{

 return DllManager.GetEngineLock()->Model_LoadComponent(stringid, buffer);
}

int RDK_CALL MModel_LoadComponent(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_LoadComponent(stringid, buffer);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int RDK_CALL Model_LoadComponentFromFile(const char *stringid, const char* file_name)
{

 return DllManager.GetEngineLock()->Model_LoadComponentFromFile(stringid, file_name);
}

int RDK_CALL MModel_LoadComponentFromFile(int engine_index, const char *stringid, const char* file_name)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_LoadComponentFromFile(stringid, file_name);
}

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL Model_SaveComponentProperties(const char *stringid, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_SaveComponentProperties(stringid, type_mask);
}

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
int RDK_CALL Model_SaveComponentPropertiesToFile(const char *stringid, const char* file_name, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_SaveComponentPropertiesToFile(stringid, file_name, type_mask);
}

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
int RDK_CALL Model_LoadComponentProperties(const char *stringid, char* buffer)
{

 return DllManager.GetEngineLock()->Model_LoadComponentProperties(stringid, buffer);
}

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
int RDK_CALL Model_LoadComponentPropertiesFromFile(const char *stringid, const char* file_name)
{

 return DllManager.GetEngineLock()->Model_LoadComponentPropertiesFromFile(stringid, file_name);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL Model_SaveComponentParameters(const char *stringid, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF01);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL MModel_SaveComponentParameters(int engine_index, const char *stringid, unsigned int type_mask)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF01);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
int RDK_CALL Model_LoadComponentParameters(const char *stringid, const char* buffer)
{

 return DllManager.GetEngineLock()->Model_LoadComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_LoadComponentParameters(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_LoadComponentProperties(stringid, buffer);
}

// ��������� ��������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL Model_SaveComponentState(const char *stringid, unsigned int type_mask)
{

 return DllManager.GetEngineLock()->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
int RDK_CALL Model_LoadComponentState(const char *stringid, char* buffer)
{

 return DllManager.GetEngineLock()->Model_LoadComponentProperties(stringid, buffer);
}

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
const char* RDK_CALL Model_SaveComponentDrawInfo(const char *stringid)
{

 return DllManager.GetEngineLock()->Model_SaveComponentDrawInfo(stringid);
}

// ��������� ����� ����� ������ �� ���������
unsigned int RDK_CALL Model_GetDefaultTimeStep(void)
{

 return DllManager.GetEngineLock()->Model_GetDefaultTimeStep();
}

void RDK_CALL Model_SetDefaultTimeStep(unsigned int value)
{

 DllManager.GetEngineLock()->Model_SetDefaultTimeStep(value);
}

// ��������� ����� ����� ����������
unsigned int RDK_CALL Model_GetTimeStep(const char *stringid)
{

 return DllManager.GetEngineLock()->Model_GetTimeStep(stringid);
}

void RDK_CALL Model_SetTimeStep(const char *stringid, unsigned int value)
{

 DllManager.GetEngineLock()->Model_SetTimeStep(stringid, value);
}

// ������������� ��� ����� ���������� � ���� ��� �������� ���������
void RDK_CALL Model_SetGlobalTimeStep(const char *stringid, unsigned int value)
{

 DllManager.GetEngineLock()->Model_SetGlobalTimeStep(stringid, value);
}

// ���������� ������� ����� ������
unsigned long long RDK_CALL Model_GetTime(void)
{

 return DllManager.GetEngineLock()->Model_GetTime();
}

unsigned long long RDK_CALL MModel_GetTime(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0.0;

 return DllManager.GetEngineLock(engine_index)->Model_GetTime();
}

double RDK_CALL Model_GetDoubleTime(void)
{

 return DllManager.GetEngineLock()->Model_GetDoubleTime();
}

double RDK_CALL MModel_GetDoubleTime(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0.0;

 return DllManager.GetEngineLock(engine_index)->Model_GetDoubleTime();
}

// ������������� ������� ����� ������
bool RDK_CALL Model_SetTime(unsigned long long value)
{

 return DllManager.GetEngineLock()->Model_SetTime(value);
}

// ���������� �������� �����
unsigned long long RDK_CALL Model_GetRealTime(void)
{

 return DllManager.GetEngineLock()->Model_GetRealTime();
}

double RDK_CALL Model_GetDoubleRealTime(void)
{

 return DllManager.GetEngineLock()->Model_GetDoubleRealTime();
}

double RDK_CALL MModel_GetDoubleRealTime(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0.0;

 return DllManager.GetEngineLock(engine_index)->Model_GetDoubleRealTime();
}

// ������������� �������� �����
bool RDK_CALL Model_SetRealTime(unsigned long long value)
{

 return DllManager.GetEngineLock()->Model_SetRealTime(value);
}

// ����������� �������� ����� �� �������� ��������
bool RDK_CALL Model_IncreaseRealTime(unsigned long long value)
{

 return DllManager.GetEngineLock()->Model_IncreaseRealTime(value);
}

// ���������� ���������� ��� � �������� �������
unsigned long long RDK_CALL Model_GetRealTimeStep(void)
{

 return DllManager.GetEngineLock()->Model_GetRealTimeStep();
}

double RDK_CALL Model_GetDoubleRealTimeStep(void)
{

 return DllManager.GetEngineLock()->Model_GetDoubleRealTimeStep();
}

// ������� ����� ������� ���������� ������ � �������������
/*long long RDK_CALL Model_GetSourceTime(void)
{
 return DllManager.GetEngineLock()->Model_GetSourceTime();
}

long long RDK_CALL MModel_GetSourceTime(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_GetSourceTime();
} */

double RDK_CALL Model_GetDoubleSourceTime(void)
{

 return DllManager.GetEngineLock()->Model_GetDoubleSourceTime();
}

double RDK_CALL MModel_GetDoubleSourceTime(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_GetDoubleSourceTime();
}

// ������������� ����� ������� ���������� ������
bool RDK_CALL Model_SetDoubleSourceTime(double value)
{

 return DllManager.GetEngineLock()->Model_SetDoubleSourceTime(value);
}

bool RDK_CALL MModel_SetDoubleSourceTime(int engine_index, double value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;

 return DllManager.GetEngineLock(engine_index)->Model_SetDoubleSourceTime(value);
}

bool RDK_CALL Model_SetDoubleSourceTimeAll(double value)
{
 bool res=true;
 for(int i=0;i<GetNumEngines();i++)
 {
  res&=DllManager.GetEngineLock(i)->Model_SetDoubleSourceTime(value);
 }
 return true;
}

// ���������� ����� ������� ���������� ��� ������� ������� �������� ��������� (��)
unsigned long long RDK_CALL Model_GetStepDuration(const char *stringid)
{
 return DllManager.GetEngineLock()->Model_GetStepDuration(stringid);
}

unsigned long long RDK_CALL MModel_GetStepDuration(int engine_index, const char *stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_GetStepDuration(stringid);
}

// ���������� �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
unsigned long long RDK_CALL Model_GetFullStepDuration(const char *stringid)
{

 return DllManager.GetEngineLock()->Model_GetFullStepDuration(stringid);
}

unsigned long long RDK_CALL MModel_GetFullStepDuration(int engine_index, const char *stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_GetFullStepDuration(stringid);
}

// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
double RDK_CALL Model_GetInstantPerformance(const char *stringid)
{
 return DllManager.GetEngineLock()->Model_GetInstantPerformance(stringid);
}

double RDK_CALL MModel_GetInstantPerformance(int engine_index, const char *stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_GetInstantPerformance(stringid);
}

// �����, ��������� ����� ����� ���������� ���������� �����
unsigned long long RDK_CALL Model_GetInterstepsInterval(const char *stringid)
{
 return DllManager.GetEngineLock()->Model_GetInterstepsInterval(stringid);
}

unsigned long long RDK_CALL MModel_GetInterstepsInterval(int engine_index, const char *stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.GetEngineLock(engine_index)->Model_GetInterstepsInterval(stringid);
}
// --------------------------

// --------------------------
// ������ ���������� ������������
// ----------------------------
// ���������� ��������-������������ ����������
void* RDK_CALL Engine_GetExceptionHandler(void)
{

 return reinterpret_cast<void*>(DllManager.GetEngineLock()->GetExceptionHandler());
}

void* RDK_CALL MEngine_GetExceptionHandler(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return reinterpret_cast<void*>(DllManager.GetEngineLock(engine_index)->GetExceptionHandler());
}

bool RDK_CALL Engine_SetExceptionHandler(void* value)
{

 return DllManager.GetEngineLock()->SetExceptionHandler(reinterpret_cast<RDK::UEnvironment::PExceptionHandler>(value));
}

bool RDK_CALL MEngine_SetExceptionHandler(int engine_index, void* value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;

 return DllManager.GetEngineLock(engine_index)->SetExceptionHandler(reinterpret_cast<RDK::UEnvironment::PExceptionHandler>(value));
}

// ���������� ������ ����� ����
const char* RDK_CALL Engine_GetLog(int &error_level)
{

 return DllManager.GetEngineLock()->GetLog(error_level);
}

const char* RDK_CALL MEngine_GetLog(int engine_index, int &error_level)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.GetEngineLock(engine_index)->GetLog(error_level);
}

// ���������� � ��� ����� ���������
int RDK_CALL Engine_LogMessage(int log_level, const char *message)
{
// if(log_level == RDK_EX_DEBUG && !DllManager.GetEngine()->Env_GetDebugMode())
//  return 0;
 return DllManager.GetEngineLock()->Engine_LogMessage(log_level, message);
}

int RDK_CALL MEngine_LogMessage(int engine_index, int log_level, const char *message)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

// if(log_level == RDK_EX_DEBUG && !DllManager.GetEngine(engine_index)->Env_GetDebugMode())
//  return 0;
 return DllManager.GetEngineLock(engine_index)->Engine_LogMessage(log_level, message);
}


// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
const char* RDK_CALL Engine_GetUnreadLog(int &error_level)
{
 return DllManager.GetEngineLock()->GetUnreadLog(error_level);
}

const char* RDK_CALL MEngine_GetUnreadLog(int engine_index, int &error_level)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngineLock(engine_index)->GetUnreadLog(error_level);
}

const char* RDK_CALL Engine_GetUnreadLogUnsafe(int &error_level)
{
 return DllManager.GetEngine()->GetUnreadLog(error_level);
}

const char* RDK_CALL MEngine_GetUnreadLogUnsafe(int engine_index, int &error_level)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngine(engine_index)->GetUnreadLog(error_level);
}

/// ���������� ����� ������������� ����� ����
int RDK_CALL Engine_GetNumUnreadLogLines(void)
{
 return DllManager.GetEngineLock()->GetNumUnreadLogLines();
}

int RDK_CALL MEngine_GetNumUnreadLogLines(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngineLock(engine_index)->GetNumUnreadLogLines();
}

/// ���������� ����� ����� ����
int RDK_CALL Engine_GetNumLogLines(void)
{
 return DllManager.GetEngineLock()->GetNumLogLines();
}

int RDK_CALL MEngine_GetNumLogLines(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngineLock(engine_index)->GetNumLogLines();
}


/// ������� ��� ����������� ���������
void RDK_CALL Engine_ClearReadLog(void)
{
 return DllManager.GetEngineLock()->ClearReadLog();
}

void RDK_CALL MEngine_ClearReadLog(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;
 return DllManager.GetEngineLock(engine_index)->ClearReadLog();
}
// ----------------------------


// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ����� ������ �����
void RDK_CALL Env_SetNumInputImages(int number)
{

 return DllManager.GetEngineLock()->Env_SetNumInputImages(number);
}

void RDK_CALL MEnv_SetNumInputImages(int engine_index, int number)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.GetEngineLock(engine_index)->Env_SetNumInputImages(number);
}

// ������ ����� ������� �����
void RDK_CALL Env_SetNumOutputImages(int number)
{

 return DllManager.GetEngineLock()->Env_SetNumOutputImages(number);
}

void RDK_CALL MEnv_SetNumOutputImages(int engine_index, int number)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.GetEngineLock(engine_index)->Env_SetNumOutputImages(number);
}

// ������ ����� ������ �����
int RDK_CALL Env_GetNumInputImages(void)
{

 return DllManager.GetEngineLock()->Env_GetNumInputImages();
}

// ������ ����� ������� �����
int RDK_CALL Env_GetNumOutputImages(void)
{

 return DllManager.GetEngineLock()->Env_GetNumInputImages();
}

// ������ ���������� �� ��������� (������� ����������)
void RDK_CALL Env_SetInputRes(int number, int width, int height)
{

 return DllManager.GetEngineLock()->Env_SetInputRes(number, width, height);
}

void RDK_CALL MEnv_SetInputRes(int engine_index, int number, int width, int height)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.GetEngineLock(engine_index)->Env_SetInputRes(number, width, height);
}

// ������ ������ �����������
void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{

 return DllManager.GetEngineLock()->Env_SetInputImage(number, image, width, height,cmodel);
}

// ������ ���� ��������� �������� ����������� ������ �������������� ���
RDK_LIB_TYPE void Env_SetReflectionXFlag(bool value)
{

 return DllManager.GetEngineLock()->Env_SetReflectionXFlag(value);
}

RDK_LIB_TYPE void MEnv_SetReflectionXFlag(int engine_index, bool value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.GetEngineLock(engine_index)->Env_SetReflectionXFlag(value);
}


// ���������� ���������� �� ��������� (������� ����������)
int RDK_CALL Env_GetInputImageWidth(int number)
{

 return DllManager.GetEngineLock()->Env_GetInputImageWidth(number);
}

int RDK_CALL Env_GetInputImageHeight(int number)
{

 return DllManager.GetEngineLock()->Env_GetInputImageHeight(number);
}

int RDK_CALL Env_GetInputImageColorModel(int number)
{

 return DllManager.GetEngineLock()->Env_GetInputImageColorModel(number);
}

// ���������� ������� �������� ����������
int RDK_CALL Env_GetOutputImageWidth(int number)
{

 return DllManager.GetEngineLock()->Env_GetOutputImageWidth(number);
}

int RDK_CALL Env_GetOutputImageHeight(int number)
{

 return DllManager.GetEngineLock()->Env_GetOutputImageHeight(number);
}

int RDK_CALL Env_GetOutputImageColorModel(int number)
{

 return DllManager.GetEngineLock()->Env_GetOutputImageColorModel(number);
}

unsigned char* RDK_CALL Env_GetInputImage(int index)
{

 return DllManager.GetEngineLock()->Env_GetInputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImage(int index)
{

 return DllManager.GetEngineLock()->Env_GetOutputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImageY8(int index)
{

 return DllManager.GetEngineLock()->Env_GetOutputImageY8(index);
}
// --------------------------

// --------------------------
// ������ ���������� ����������� �������
// --------------------------
// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
// ������������ �������� ����� ����������� ��� RDK::MDMatrix*
// ���� ����� �� �������� ������ ������ ����, �� ���������� 0
const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name)
{

 return DllManager.GetEngineLock()->Model_GetComponentOutputAsMatrix(stringid, property_name);
}

const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrixByIndex(const char *stringid, int  index)
{

 return DllManager.GetEngineLock()->Model_GetComponentOutputAsMatrix(stringid, index);
}

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
const /* RDK::UBitmap* */ void* RDK_CALL Model_GetComponentOutput(const char *stringid, const char *property_name)
{

 return DllManager.GetEngineLock()->Model_GetComponentOutput(stringid, property_name);
}

const /* RDK::UBitmap* */ void* RDK_CALL Model_GetComponentOutputByIndex(const char *stringid, int index)
{
 return DllManager.GetEngineLock()->Model_GetComponentOutput(stringid, index);
}
const /* RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentOutput(int engine_index, const char *stringid, const char *property_name)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngineLock()->Model_GetComponentOutput(stringid, property_name);
}

const /* RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentOutputByIndex(int engine_index,const char *stringid, int index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngineLock()->Model_GetComponentOutput(stringid, index);
}

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutput(const char *stringid, const char *property_name)
{

 return DllManager.GetEngineLock()->Model_GetComponentBitmapOutput(stringid, property_name);
}

const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutputByIndex(const char *stringid, int index)
{

 return DllManager.GetEngineLock()->Model_GetComponentBitmapOutput(stringid, index);
}

const /*RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentBitmapOutput(int engine_index,const char *stringid, const char *property_name)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngineLock()->Model_GetComponentBitmapOutput(stringid, property_name);

}
const /*RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentBitmapOutputByIndex(int engine_index,const char *stringid, int index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.GetEngineLock()->Model_GetComponentBitmapOutput(stringid, index);
}

// ���������� ��������� �� ���� � �������� 'index' ���������� 'id'
const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInput(const char *stringid, const char *property_name)
{

 return DllManager.GetEngineLock()->Model_GetComponentBitmapInput(stringid, property_name);
}

const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInputByIndex(const char *stringid, int index)
{

 return DllManager.GetEngineLock()->Model_GetComponentBitmapInput(stringid, index);
}

/// �������� ������ � ���������� ����������� ������ � �������� 'index' ���������� 'id'
/// � �������� bmp_param
int RDK_CALL Model_CopyComponentBitmapOutputHeader(const char *stringid, const char *property_name, /*RDK::UBitmapParam* */ void* bmp_param)
{
 return DllManager.GetEngineLock()->Model_CopyComponentBitmapOutputHeader(stringid, property_name, (RDK::UBitmapParam*)bmp_param);
}

int RDK_CALL MModel_CopyComponentBitmapOutputHeader(int engine_index, const char *stringid, const char *property_name, /*RDK::UBitmapParam* */ void* bmp_param)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1;

 return DllManager.GetEngineLock(engine_index)->Model_CopyComponentBitmapOutputHeader(stringid, property_name, (RDK::UBitmapParam*)bmp_param);
}

int RDK_CALL Model_CopyComponentBitmapOutputHeaderByIndex(const char *stringid, int index, /*RDK::UBitmapParam* */ void* bmp_param)
{
 return DllManager.GetEngineLock()->Model_CopyComponentBitmapOutputHeaderByIndex(stringid, index, (RDK::UBitmapParam*)bmp_param);
}

int RDK_CALL MModel_CopyComponentBitmapOutputHeaderByIndex(int engine_index, const char *stringid, int index, /*RDK::UBitmapParam* */ void* bmp_param)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1;

 return DllManager.GetEngineLock(engine_index)->Model_CopyComponentBitmapOutputHeaderByIndex(stringid, index, (RDK::UBitmapParam*)bmp_param);
}

/// �������� ����������� ������ � �������� 'index' ���������� 'id'
/// ����� ������������, ��� bmp ��� ����� ���������� ������ ��� ���������� ���������� �������
int RDK_CALL Model_CopyComponentBitmapOutput(const char *stringid, const char *property_name, /*RDK::UBitmap* */ void* bmp)
{
 return DllManager.GetEngineLock()->Model_CopyComponentBitmapOutput(stringid, property_name, (RDK::UBitmap*)bmp);
}

RDK_LIB_TYPE int RDK_CALL MModel_CopyComponentBitmapOutput(int engine_index, const char *stringid, const char *property_name, /*RDK::UBitmap**/void* bmp)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1;

  return DllManager.GetEngineLock(engine_index)->Model_CopyComponentBitmapOutput(stringid, property_name, (RDK::UBitmap*)bmp);
}

int RDK_CALL Model_CopyComponentBitmapOutputByIndex(const char *stringid, int index, /*RDK::UBitmap* */ void* bmp)
{
 return DllManager.GetEngineLock()->Model_CopyComponentBitmapOutput(stringid, index, (RDK::UBitmap*)bmp);
}

int RDK_CALL MModel_CopyComponentBitmapOutputByIndex(int engine_index, const char *stringid, int index, /*RDK::UBitmap* */ void* bmp)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1;
 return DllManager.GetEngineLock(engine_index)->Model_CopyComponentBitmapOutput(stringid, index, (RDK::UBitmap*)bmp);
}

// �������� ����������� ������ � �������� 'index' ���������� 'id'
void RDK_CALL Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{

 DllManager.GetEngineLock()->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL MModel_SetComponentBitmapOutput(int engine_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 DllManager.GetEngineLock(engine_index)->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL MModel_SetComponentBitmapOutputUnsafe(int engine_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 DllManager.GetEngine(engine_index)->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL Model_SetComponentBitmapOutputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{

 DllManager.GetEngineLock()->Model_SetComponentBitmapOutput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL MModel_SetComponentBitmapOutputByIndex(int engine_index, const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 DllManager.GetEngineLock(engine_index)->Model_SetComponentBitmapOutput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

// �������� ����������� ����� � �������� 'index' ���������� 'id'
void RDK_CALL Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 DllManager.GetEngineLock()->Model_SetComponentBitmapInput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL MModel_SetComponentBitmapInput(int engine_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 DllManager.GetEngineLock(engine_index)->Model_SetComponentBitmapInput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL Model_SetComponentBitmapInputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{

 DllManager.GetEngineLock()->Model_SetComponentBitmapInput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}
// --------------------------

/* ************************************************************************** */




// ----------------------------
// ���������� ������ �������������
// ----------------------------
/*
int Init(void* exception_handler)
{
 UnInit();
 if(!DllInit((void*)CreateNewStorage, (void*)CreateNewEnvironment, (void*)CreateNewEngine))
  return -2;

 PEngine=dynamic_cast<RDK::UEngine*>(AddNewEngine());

 if(!PEngine)
  return -3;

 DllManager.GetEngineLock()->Default();
 Engine_SetExceptionHandler(exception_handler);

 try {
  PEnvironment=AddNewEnvironment();
  PStorage=AddNewStorage();
  if(!PEnvironment)
  {
   UnInit();
   return 3;
  }

  if(!PStorage)
  {
   UnInit();
   return 4;
  }

  if(!DllManager.GetEngineLock()->Init(PStorage,PEnvironment))
  {
   UnInit();
   return 3;
  }
 }
 catch (RDK::UException &exception)
 {
  DllManager.GetEngineLock()->ProcessException(exception);
 }
 return 0;
}

int UnInit(void)
{
 try
 {
  if(PEngine)
  {
   DllManager.GetEngineLock()->UnInit();
  }
 }
 catch (RDK::UException &exception)
 {
  DllManager.GetEngineLock()->ProcessException(exception);
 }

 DelEngine(PEngine);
 PEngine=0;
 DelEnvironment(PEnvironment);
 PEnvironment=0;
 DelStorage(PStorage);
 PStorage=0;

 return 0;
}
        */
// ���������� ���������� ����������
// ������ ���� ������ � ���������� ����������� ����������������� ��
int RDK_CALL ExceptionDispatcher(void *exception)
{
 if(!DllManager.GetEngine())
  return 1;

 if(!exception)
  return 2;

 RDK::UException *exc=reinterpret_cast<RDK::UException*>(exception);
 DllManager.GetEngineLock()->ProcessException(*exc);

 return 0;
}

// ������������� dll
bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine)
{
 return DllManager.Init(reinterpret_cast<RDKDllManager::PCreateNewStorage>(pfstorage),
						reinterpret_cast<RDKDllManager::PCreateNewEnvironment>(pfenvironment),
						reinterpret_cast<RDKDllManager::PCreateNewEngine>(pfengine));
}
	   /*
// ���������� ����� �������� � ����������
int GetNumStorages(void)
{
 return DllManager.StorageList.size();
}

// ���������� ����� ���� � ����������
int GetNumEnvironments(void)
{
 return DllManager.EnvironmentList.size();
}                     */
 /*
// ���������� ����� ������� � ����������
int GetNumEngines(void)
{
 return DllManager.EngineList.size();
}
	  */      /*
// ���������� ��������� �� �������
RDK::UStorage* GetStorage(size_t i)
{
 if(i>=DllManager.StorageList.size())
  return 0;

 return DllManager.StorageList[i];
}

// ���������� ����� �� �������
RDK::UEnvironment* GetEnvironment(size_t i)
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
RDK::UStorage* AddNewStorage(void)
{
 if(!DllManager.FuncCreateNewStorage)
  return 0;

 RDK::UStorage* storage=DllManager.FuncCreateNewStorage();
 if(storage)
 {
//  CreateStorage(storage);
  DllManager.StorageList.push_back(storage);
 }
 return storage;
}

// ������� ������������ ���������
void DelStorage(RDK::UStorage* storage)
{
 if(storage)
 {
  for(size_t i=0;i<DllManager.StorageList.size();i++)
   if(DllManager.StorageList[i] == storage)
   {
	DllManager.StorageList.erase(DllManager.StorageList.begin()+i);
	delete storage;
   }
 }
}

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
RDK::UEnvironment* AddNewEnvironment(void)
{
 if(!DllManager.FuncCreateNewEnvironment)
  return 0;

 RDK::UEnvironment* environment=DllManager.FuncCreateNewEnvironment();
 if(environment)
 {
//  CreateEnvironment(environment, storage,isinit,external_classes, external_libs);
  DllManager.EnvironmentList.push_back(environment);
 }
 return environment;
}

// ������� ������������ �����
void DelEnvironment(RDK::UEnvironment* env)
{
 if(env)
 {
  for(size_t i=0;i<DllManager.EnvironmentList.size();i++)
   if(DllManager.EnvironmentList[i] == env)
   {
	DllManager.EnvironmentList.erase(DllManager.EnvironmentList.begin()+i);
	delete env;
   }
 }
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

// ������� ��������� �����
void DelEngine(RDK::UEngine* engine)
{
 if(engine)
 {
  for(size_t i=0;i<DllManager.EngineList.size();i++)
   if(DllManager.EngineList[i] == engine)
   {
	DllManager.EngineList.erase(DllManager.EngineList.begin()+i);
	delete engine;
   }
 }
}                    */
// ----------------------------

namespace boost {

void tss_cleanup_implemented(void)
{
	/*
	This function's sole purpose is to cause a link
error in cases where
	automatic tss cleanup is not implemented by
Boost.Threads as a
	reminder that user code is responsible for calling
the necessary
	functions at the appropriate times (and for
implementing an a
	tss_cleanup_implemented() function to eliminate the
linker's
	missing symbol error).
	If Boost.Threads later implements automatic tss
cleanup in cases
	where it currently doesn't (which is the plan), the
duplicate
	symbol error will warn the user that their custom
solution is no
	longer needed and can be removed.
	*/
}

}

void tss_cleanup_implemented(void)
{
	/*
	This function's sole purpose is to cause a link
error in cases where
	automatic tss cleanup is not implemented by
Boost.Threads as a
	reminder that user code is responsible for calling
the necessary
	functions at the appropriate times (and for
implementing an a
	tss_cleanup_implemented() function to eliminate the
linker's
	missing symbol error).
	If Boost.Threads later implements automatic tss
cleanup in cases
	where it currently doesn't (which is the plan), the
duplicate
	symbol error will warn the user that their custom
solution is no
	longer needed and can be removed.
	*/
}


#endif

