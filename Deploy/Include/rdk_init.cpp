#ifndef RDK_INIT_CPP
#define RDK_INIT_CPP

#include <exception>
#include "rdk_init.h"
#include "rdk.h"
#include "rdk_rpc.cpp"


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
// ������ ����������
// --------------------------
bool Init(PCreateNewStorage fCreateNewStorage,
            PCreateNewEnvironment fCreateNewEnvironment,
			PCreateNewEngine fCreateNewEngine);

/// ���������� ����� �������
int GetNumEngines(void) const;

/// ������� ��������� ����� ������ �������
int SetNumEngines(int num);

/// �������� ��������� ������
/// (���� ������ ��� ���������������, �� �� ������ ������
int EngineCreate(int index);

/// ���������� ��������� ������
/// (���� ������ ��� ���������, �� �� ������ ������
int EngineDestroy(int index);
// --------------------------
};

// ��������� ���������
RDKDllManager DllManager;


// --------------------------------------
// ���������� �������������� �������
// --------------------------------------
/*
// ���������� ��������� �� �������
RDK::UStorage* GetStorage(size_t i);

// ���������� ����� �� �������
RDK::UEnvironment*  GetEnvironment(size_t i);

// ���������� ������ �� �������
RDK::UEngine*  GetEngine(size_t i);

// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
RDK::UStorage*  AddNewStorage(void);

// ������� ������������ ���������
void DelStorage(RDK::UStorage* env);

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
RDK::UEnvironment*  AddNewEnvironment(void);

// ������� ������������ �����
void DelEnvironment(RDK::UEnvironment* env);

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
RDK::UEngine*  AddNewEngine(void);

// ������� ������������ ������
void DelEngine(RDK::UEngine* engine);
						  */
// ������������� ����������
//int Init(void* exception_handler);

// ��������������� ����������
//int UnInit(void);

// ������������� dll
bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine);
// --------------------------------------

RDK::UEPtr<RDK::UEngine> PEngine=0;
RDK::UEPtr<RDK::UEnvironment> PEnvironment=0;
RDK::UEPtr<RDK::UStorage> PStorage=0;

int SelectedEngineIndex=0;

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
const char* RDK_CALL RemoteCall(const char *request, int &return_value)
{
 return RDK::RemoteCallInternal(request, return_value);
}
// ----------------------------

// ----------------------------
// ������ �������������
// ----------------------------
// ���������� ����� �������
int RDK_CALL GetNumEngines(void)
{
 return DllManager.GetNumEngines();
}

// ������� ��������� ����� �������
// num > 0
int RDK_CALL SetNumEngines(int num)
{
 if(num<=0)
  return 1;

 if(num == GetNumEngines())
  return 0;

 if(!DllInit((void*)CreateNewStorage, (void*)CreateNewEnvironment, (void*)CreateNewEngine))
  return -2;


 int res=DllManager.SetNumEngines(num);
 if(res != 0)
  return res;

 if(SelectedEngineIndex>=num)
  SelectedEngineIndex=0;

 if(num>0)
 {
  PEngine=DllManager.EngineList[SelectedEngineIndex];
  PEnvironment=DllManager.EnvironmentList[SelectedEngineIndex];
  PStorage=DllManager.StorageList[SelectedEngineIndex];
 }

 RpcReturnString.resize(num);
 return 0;
}

// ���������� ������ �������� ���������� ������
int RDK_CALL GetSelectedEngineIndex(void)
{
 return SelectedEngineIndex;
}


// ����������� ������� ��������� �� ������ � �������� �������
// � ������ �������� ������, ��������� ������������� ����������������� �� 0 ������
int RDK_CALL SelectEngine(int index)
{
 if(index<0 || index>=GetNumEngines())
  return 1;

 if(SelectedEngineIndex == index)
  return 0;

 SelectedEngineIndex=index;
 PEngine=DllManager.EngineList[index];
 PEnvironment=DllManager.EnvironmentList[index];
 PStorage=DllManager.StorageList[index];

 return 0;
}

int RDK_CALL EngineInit(int predefined_structure, void* exception_handler)
{
 int res=0;
 if(GetNumEngines()<=SelectedEngineIndex)
  res=SetNumEngines(SelectedEngineIndex+1);

 if(res != 0)
  return res;

 res=MEngineInit(SelectedEngineIndex, predefined_structure, exception_handler);

 if(res != 0)
  return res;

 PEngine=DllManager.EngineList[SelectedEngineIndex];
 PEnvironment=DllManager.EnvironmentList[SelectedEngineIndex];
 PStorage=DllManager.StorageList[SelectedEngineIndex];

 return 0;
}

int RDK_CALL MEngineInit(int engine_index, int predefined_structure, void* exception_handler)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 MEngineUnInit(engine_index);

 int res=0;

 res=DllManager.EngineCreate(engine_index);
 if(res != 0)
  return res;

 MEngine_SetExceptionHandler(engine_index, exception_handler);

 MEnv_SetPredefinedStructure(engine_index, predefined_structure);
 MEnv_CreateStructure(engine_index);
 MEnv_Init(engine_index);

 return 0;
}


int RDK_CALL GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx,
		void* exception_handler)
{
 int res=0;
 if(GetNumEngines()<=SelectedEngineIndex)
  res=SetNumEngines(SelectedEngineIndex+1);

 if(res != 0)
  return res;

 res=MGraphicalEngineInit(SelectedEngineIndex, predefined_structure, num_inputs,
		num_outputs, input_width, input_height, reflectionx, exception_handler);
 if(res != 0)
  return res;

 PEngine=DllManager.EngineList[SelectedEngineIndex];
 PEnvironment=DllManager.EnvironmentList[SelectedEngineIndex];
 PStorage=DllManager.StorageList[SelectedEngineIndex];

 return 0;
}

int RDK_CALL MGraphicalEngineInit(int engine_index, int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx,
		void* exception_handler)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 MEngineUnInit(engine_index);
// Init(exception_handler);

 int res=0;

 res=DllManager.EngineCreate(SelectedEngineIndex);
 if(res != 0)
  return res;

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

 return 0;
}

// ���������������� ������ (������� ������������� ���������� ��� ������ �������������)
int RDK_CALL EngineUnInit(void)
{
 if(PEngine)
  if(!Env_UnInit())
   return 1;

 return DllManager.EngineDestroy(SelectedEngineIndex);
}

int RDK_CALL MEngineUnInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 if(DllManager.EngineList[engine_index])
  if(!MEnv_UnInit(engine_index))
   return 1;

 return DllManager.EngineDestroy(engine_index);
}


/// ��������� ��������������� �� ������
bool RDK_CALL IsEngineInit(void)
{
 return (PEngine)?true:false;
}

bool RDK_CALL MIsEngineInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;

 return (DllManager.EngineList[engine_index])?true:false;
}
// ----------------------------

// --------------------------
// ������ ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
int RDK_CALL Storage_GetNumClasses(void)
{
 return PEngine->Storage_GetNumClasses();
}

// ���������� id ������� � ���������. ������ ������ ���� ��������
void RDK_CALL Storage_GetClassesList(int *buffer)
{
 PEngine->Storage_GetClassesList(buffer);
}

// ���������� ����� ������� � ��������� � ���� ������ ����������� ��������
const char * RDK_CALL Storage_GetClassesNameList(void)
{
 return PEngine->Storage_GetClassesNameList();
}

// ���������� ��� ������ �� ��� id.
const char * RDK_CALL Storage_GetClassName(int id)
{
 return PEngine->Storage_GetClassName(id);
}

// ���������� Id ������ �� ��� �����
int RDK_CALL Storage_GetClassId(const char *name)
{
 return PEngine->Storage_GetClassId(name);
}

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
bool RDK_CALL Storage_DelClass(int classid)
{
 return PEngine->Storage_DelClass(classid);
}

// �������� ��� ��������� ������� �� ���������
void RDK_CALL Storage_FreeObjectsStorage(void)
{
 PEngine->Storage_FreeObjectsStorage();
}

// ������� ��� ������� �� ���������
void RDK_CALL Storage_ClearObjectsStorage(void)
{
 PEngine->Storage_ClearObjectsStorage();
}

// ��������� ��������� ����� �������� � ���������
int RDK_CALL Storage_CalcNumObjects(void)
{
 return PEngine->Storage_CalcNumObjects();
}

int RDK_CALL Storage_CalcNumObjectsById(int classid)
{
 return PEngine->Storage_CalcNumObjectsById(classid);
}

int RDK_CALL Storage_CalcNumObjectsByName(const char* classname)
{
 return PEngine->Storage_CalcNumObjectsByName(classname);
}

// ���������� �������� ������ �� ��� id � ������� xml
const char* RDK_CALL Storage_GetClassDescription(const char* classname)
{
 return PEngine->Storage_GetClassDescription(classname);
}

// ������������� �������� ������ �� ��� id, �������� ��� �� ������� xml
bool RDK_CALL Storage_SetClassDescription(const char* classname, const char* description)
{
 return PEngine->Storage_SetClassDescription(classname, description);
}

// ��������� �������� ���� ������� � xml
const char* RDK_CALL Storage_SaveClassesDescription(void)
{
 return PEngine->Storage_SaveClassesDescription();
}

// ��������� �������� ���� ������� �� xml
bool RDK_CALL Storage_LoadClassesDescription(const char* xmltext)
{
 return PEngine->Storage_LoadClassesDescription(xmltext);
}

// ��������� ����� �������� ���� ������� � xml
const char* RDK_CALL Storage_SaveCommonClassesDescription(void)
{
 return PEngine->Storage_SaveCommonClassesDescription();
}

// ��������� ����� �������� ���� ������� �� xml
bool RDK_CALL Storage_LoadCommonClassesDescription(const char* xmltext)
{
 return PEngine->Storage_LoadCommonClassesDescription(xmltext);
}

// ��������� �������� ���� ������� � xml ������� ����� ��������
const char* RDK_CALL Storage_SaveAllClassesDescription(void)
{
 return PEngine->Storage_SaveAllClassesDescription();
}

// ��������� �������� ���� ������� �� xml ������� ����� ��������
bool RDK_CALL Storage_LoadAllClassesDescription(const char* xmltext)
{
 return PEngine->Storage_LoadAllClassesDescription(xmltext);
}

// ��������� ���������� �� ����� dll-�����
int RDK_CALL Storage_LoadStorageLibrary(const char *filename)
{
 return PEngine->Storage_LoadStorageLibrary(filename);
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool RDK_CALL Storage_DelClassLibraryByIndex(int index)
{
 return PEngine->Storage_DelClassLibraryByIndex(index);
}

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool RDK_CALL Storage_DelClassLibraryByName(const char *name)
{
 return PEngine->Storage_DelClassLibraryByName(name);
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool RDK_CALL Storage_DelAllClassLibraries(void)
{
 return PEngine->Storage_DelAllClassLibraries();
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool RDK_CALL Storage_BuildStorage(void)
{
 return PEngine->Storage_BuildStorage();
}

// ���������� ����� ���������
int RDK_CALL Storage_GetNumClassLibraries(void)
{
 return PEngine->Storage_GetNumClassLibraries();
}

// ���������� ������ ��������� � ���� ������, ����������� ��������
const char* RDK_CALL Storage_GetClassLibrariesList(void)
{
 return PEngine->Storage_GetClassLibrariesList();
}

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// library_name - ��� ����������
const char* RDK_CALL Storage_GetLibraryClassNames(const char *library_name)
{
 return PEngine->Storage_GetLibraryClassNames(library_name);
}

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// index - ������ ����������
const char* RDK_CALL Storage_GetLibraryClassNamesByIndex(int index)
{
 return PEngine->Storage_GetLibraryClassNamesByIndex(index);
}

// ���������� ��� ���������� �� �������
const char * RDK_CALL Storage_GetClassLibraryNameByIndex(int index)
{
 return PEngine->Storage_GetClassLibraryNameByIndex(index);
}

// ���������� ������ ���������� �� �������
const char * RDK_CALL Storage_GetClassLibraryVersionByIndex(int index)
{
 return PEngine->Storage_GetClassLibraryVersionByIndex(index);
}

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
int RDK_CALL Storage_CreateClass(const char* stringid, const char *classname)
{
 return PEngine->Storage_CreateClass(stringid, classname);
}
// ----------------------------

// ----------------------------
// ������ ���������� ������
// ----------------------------
// ������ ������������� �������� ������ ���������
int RDK_CALL Env_GetPredefinedStructure(void)
{
 return PEngine->Env_GetPredefinedStructure();
}

int RDK_CALL MEnv_GetPredefinedStructure(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.EngineList[engine_index]->Env_GetPredefinedStructure();
}

bool RDK_CALL Env_SetPredefinedStructure(int value)
{
 return PEngine->Env_SetPredefinedStructure(value);
}

bool RDK_CALL MEnv_SetPredefinedStructure(int engine_index, int value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_SetPredefinedStructure(value);
}

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool RDK_CALL Env_IsStoragePresent(void)
{
 return PEngine->Env_IsStoragePresent();
}

bool RDK_CALL MEnv_IsStoragePresent(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_IsStoragePresent();
}

// ���������� ��������� �������������
bool RDK_CALL Env_IsInit(void)
{
 return PEngine->Env_IsInit();
}

bool RDK_CALL MEnv_IsInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_IsInit();
}


// ������� ������� �������������� ���������
bool RDK_CALL Env_IsStructured(void)
{
 return PEngine->Env_IsStructured();
}

bool RDK_CALL MEnv_IsStructured(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_IsStructured();
}

// ������������� �����
bool RDK_CALL Env_Init(void)
{
 return PEngine->Env_Init();
}

bool RDK_CALL MEnv_Init(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_Init();
}

// ��������������� �����
bool RDK_CALL Env_UnInit(void)
{
 return PEngine->Env_UnInit();
}

bool RDK_CALL MEnv_UnInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_UnInit();
}

// ��������� �������������� �������� ������ ���������
bool RDK_CALL Env_CreateStructure(void)
{
 return PEngine->Env_CreateStructure();
}

bool RDK_CALL MEnv_CreateStructure(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_CreateStructure();
}

// ���������� ������� ������ ���������
bool RDK_CALL Env_DestroyStructure(void)
{
 return PEngine->Env_DestroyStructure();
}

bool RDK_CALL MEnv_DestroyStructure(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_DestroyStructure();
}

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
void RDK_CALL Env_Destroy(void)
{
 return PEngine->Env_Destroy();
}

void RDK_CALL MEnv_Destroy(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.EngineList[engine_index]->Env_Destroy();
}

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
int RDK_CALL Env_Calculate(const char* stringid)
{
 return PEngine->Env_Calculate(stringid);
}

int RDK_CALL MEnv_Calculate(int engine_index, const char* stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.EngineList[engine_index]->Env_Calculate(stringid);
}


// ������ ���� ������ � �������� �������
void RDK_CALL Env_RTCalculate(void)
{
 PEngine->Env_RTCalculate();
}

void RDK_CALL MEnv_RTCalculate(int engine_index)
{
 DllManager.EngineList[engine_index]->Env_RTCalculate();
}

// ����� ������ �����
// ���� stringid == 0 �� ���������� ��� ������ �������,
// ����� - ������ ��������� ��������� ������
int RDK_CALL Env_Reset(const char* stringid)
{
 return PEngine->Env_Reset(stringid);
}

int RDK_CALL MEnv_Reset(int engine_index, const char* stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.EngineList[engine_index]->Env_Reset(stringid);
}

// ���������� ���������� ������� ������ �� ��������� ��������
void RDK_CALL Env_IncreaseModelTimeByStep(void)
{
 return PEngine->Env_IncreaseModelTimeByStep();
}

/// ������������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
int RDK_CALL Env_SetMinInterstepsInterval(long long value)
{
 return PEngine->Env_SetMinInterstepsInterval(value);
}

int RDK_CALL MEnv_SetMinInterstepsInterval(int engine_index, long long value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.EngineList[engine_index]->Env_SetMinInterstepsInterval(value);
}

/// ���������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
long long RDK_CALL Env_GetMinInterstepsInterval(void)
{
 return PEngine->Env_GetMinInterstepsInterval();
}

long long RDK_CALL Env_GetMinInterstepsInterval(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.EngineList[engine_index]->Env_GetMinInterstepsInterval();
}

// ���������� ��� �������� �������� ��� �������� ������
const char* RDK_CALL Env_GetCurrentDataDir(void)
{
 return PEngine->Env_GetCurrentDataDir();
}

// ������������� ��� �������� �������� ��� �������� ������
int RDK_CALL Env_SetCurrentDataDir(const char *dir)
{
 return PEngine->Env_SetCurrentDataDir(dir);
}

// ***********************************************
// ������ ���������� ������� �����������
// !!! ��������� ������ ������ �� ���
// ������, ������������ � ����������� �� ���������� id !!!
// ***********************************************
// ������������� ������� ��������� (��������� ������������ ����� - ������)
int RDK_CALL Env_SelectCurrentComponent(const char *stringid)
{
 return PEngine->Env_SelectCurrentComponent(stringid);
}

// ���������� ������� ��������� � ��������� �� ��������� (������)
int RDK_CALL Env_ResetCurrentComponent(const char *stringid)
{
 return PEngine->Env_ResetCurrentComponent(stringid);
}

// ������ ������� ��������� �� ��� �������� (������ �� ������� �����)
// ���� ��� �� ������� ������, �� �� ������ ������
int RDK_CALL Env_UpCurrentComponent(void)
{
 return PEngine->Env_UpCurrentComponent();
}

// ������ ������� ��������� �� ��� �������� �� ������������ ������ �����������
// (����� �� N ������� ���� ������������ �������� ����������)
int RDK_CALL Env_DownCurrentComponent(const char *stringid)
{
 return PEngine->Env_DownCurrentComponent(stringid);
}

// ���������� ������� ��� �������� ����������
const char* RDK_CALL Env_GetCurrentComponentName(void)
{
 return PEngine->Env_GetCurrentComponentName();
}

// ���������� ������� ��������� id �������� ����������
const char* RDK_CALL Env_GetCurrentComponentId(void)
{
 return PEngine->Env_GetCurrentComponentId();
}
// ***********************************************
// --------------------------

// --------------------------
// ������ ���������� �������
// ----------------------------
// ������� ������
int RDK_CALL Model_Destroy(void)
{
 return PEngine->Model_Destroy();
}

// ������� ����� ������ �� ����� ������ � ���������
// �������������� ������� ������������ ������
int RDK_CALL Model_Create(const char *classname)
{
 return PEngine->Model_Create(classname);
}

int RDK_CALL MModel_Create(int engine_index, const char *classname)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.EngineList[engine_index]->Model_Create(classname);
}

// ������� ������
int RDK_CALL Model_Clear(void)
{
 return PEngine->Model_Clear();
}

// ���������, ���������� �� ������
bool RDK_CALL Model_Check(void)
{
 return PEngine->Model_Check();
}

bool RDK_CALL MModel_Check(int engine_index)
{
 return DllManager.EngineList[engine_index]->Model_Check();
}

// ���������, ���������� �� � ������ ��������� � ������ stringid)
bool RDK_CALL Model_CheckComponent(const char* stringid)
{
 return PEngine->Model_CheckComponent(stringid);
}

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'classname'
// ���� stringid - ������ ������, �� ��������� � ���� ������
// ���������� ��� ���������� � ������ ������
const char* RDK_CALL Model_AddComponent(const char* stringid, const char *classname)
{
 return PEngine->Model_AddComponent(stringid, classname);
}

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'name'
// ���� stringid - ������ ������, �� ������� �� ����� ������
int RDK_CALL Model_DelComponent(const char* stringid, const char *name)
{
 return PEngine->Model_DelComponent(stringid, name);
}

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
int RDK_CALL Model_GetNumComponents(const char* stringid)
{
 return PEngine->Model_GetNumComponents(stringid);
}

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
int RDK_CALL Model_GetComponentsList(const char* stringid, int *buffer)
{
 return PEngine->Model_GetComponentsList(stringid, buffer);
}

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ','
const char* RDK_CALL Model_GetComponentsNameList(const char* stringid)
{
 return PEngine->Model_GetComponentsNameList(stringid);
}

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ',' � ������� ��� ������ 'class_name'
// ���� find_all == true �� ����� ������� � �� ���� ��������������
const char* RDK_CALL Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all)
{
 return PEngine->Model_FindComponentsByClassName(stringid,class_name,find_all);
}

// ���������� ��������� � ������� �������� index ��� ������ 'name' ����� ���
// ���� �� ������ �� �������� ����� ���������
// ����������� ��� ��������� ������� ������� ���������
// ���� �������� 'step' ������� �� ������� �������, �� ��������� ���������������
// �� ��� �������
int RDK_CALL Model_ChangeComponentPosition(const char* stringid, int step)
{
 return PEngine->Model_ChangeComponentPosition(stringid, step);
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
 return PEngine->Model_GetConnectorsList(stringid, sublevel, owner_level_stringid);
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
 return PEngine->Model_GetItemsList(stringid, sublevel, owner_level_stringid);
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
 return PEngine->Model_GetNetsList(stringid, sublevel, owner_level_stringid);
}

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
const char* RDK_CALL Model_GetComponentName(const char* stringid)
{
 return PEngine->Model_GetComponentName(stringid);
}

// ���������� ������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
const char* RDK_CALL Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid)
{
 return PEngine->Model_GetComponentLongName(stringid,owner_level_stringid);
}


// ���������� ������� id ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
const char* RDK_CALL Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid)
{
 return PEngine->Model_GetComponentLongId(stringid,owner_level_stringid);
}

// ���������� ��� ������ ���������� � ��������� �� �������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������ ������
const char* RDK_CALL Model_GetComponentClassName(const char* stringid)
{
 return PEngine->Model_GetComponentClassName(stringid);
}

// ���������� ������ ������� ���������� ����������� ��������
const char* RDK_CALL Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentPropertiesList(stringid,type_mask);
}

// ���������� ������ ���� � �������� ������� ���������� ����������� ��������
// ������ ������� ����� ��� ���_��������:������_�����(������)
const char* RDK_CALL Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentPropertiesLookupList(stringid,type_mask);
}

// ���������� �������� ���������� �� ��������������
const char * RDK_CALL Model_GetComponentProperties(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentProperties(stringid,type_mask);
}

// ���������� �������� ���������� �� �������������� � ����������
const char * RDK_CALL Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentPropertiesEx(stringid, type_mask);
}

// ���������� ���������� �������� ���������� �� ��������������
const char * RDK_CALL Model_GetComponentSelectedProperties(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentSelectedProperties(stringid);
}

// ���������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
const char * RDK_CALL Model_GetComponentPropertyValue(const char *stringid, const char *paramname)
{
 return PEngine->Model_GetComponentPropertyValue(stringid,paramname);
}

// ������������� �������� ���������� �� ��������������
int RDK_CALL Model_SetComponentProperties(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentProperties(stringid, buffer);
}

// ������������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
void RDK_CALL Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer)
{
 PEngine->Model_SetComponentPropertyValue(stringid,paramname,buffer);
}

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
void RDK_CALL Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer)
{
 PEngine->Model_SetGlobalComponentPropertyValue(stringid,class_stringid, paramname,buffer);
}

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
void RDK_CALL Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer)
{
 PEngine->Model_SetGlobalOwnerComponentPropertyValue(stringid, class_stringid, class_owner_stringid, paramname,buffer);
}

// ���������� ��������� void* �� ������ �������� ����������
const void* RDK_CALL Model_GetComponentPropertyData(const char *stringid, const char *property_name)
{
 return PEngine->Model_GetComponentPropertyData(stringid, property_name);
}

// �������� ������ 'data' � �������� �������� ����������
int RDK_CALL Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data)
{
 return PEngine->Model_SetComponentPropertyData(stringid, property_name, data);
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentParameters(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentProperties(stringid,type_mask & 0xFFFFFF01);
}

const char * RDK_CALL MModel_GetComponentParameters(int engine_index, const char *stringid, unsigned int type_mask)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->Model_GetComponentProperties(stringid,type_mask & 0xFFFFFF01);
}

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedProperties(stringid);
}

// ���������� ��������� ���������� �� �������������� � ����������
const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentPropertiesEx(stringid, type_mask & 0xFFFFFF01);
}

// ���������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname)
{
 return PEngine->Model_GetComponentPropertyValue(stringid,paramname);
}

const char * RDK_CALL MModel_GetComponentParameterValue(int engine_index, const char *stringid, const char *paramname)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->Model_GetComponentPropertyValue(stringid,paramname);
}

// ������������� ��������� ���������� �� ��������������
int RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_SetComponentParameters(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.EngineList[engine_index]->Model_SetComponentProperties(stringid, buffer);
}

// ������������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
void RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer)
{
 PEngine->Model_SetComponentPropertyValue(stringid,paramname,buffer);
}

// ��������� ��������� ���������� ���� � ������
int RDK_CALL Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 return PEngine->Model_CreateLink(stringid1, output_number, stringid2, input_number);
}

// ��������� ��� ���������� ���������� ���������� �� ����������� id � �������: 0 ����� � 0 �����
int RDK_CALL Model_ChainLinking(const char* stringid)
{
 return PEngine->Model_ChainLinking(stringid);
}

// ��������� ��� ���������� ���������� ���������� �����������, ��������� �� � ������������ ����� ������� ������
// ������������ ��� ������������ ������������������
int RDK_CALL Model_ParallelLinking(const char* stringid)
{
 return PEngine->Model_ParallelLinking(stringid);
}

// ��������� ��������� �����
int RDK_CALL Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 return PEngine->Model_BreakLink(stringid1, output_number, stringid2, input_number);
}

// ��������� ��� �����
int RDK_CALL Model_BreakAllLinks(void)
{
 return PEngine->Model_BreakAllLinks();
}

// ��������� ����� �� ����� connector_index ���������� 'connectorid'
int RDK_CALL Model_BreakConnectorLink(const char* connectorname, int connector_index)
{
 return PEngine->Model_BreakConnectorLink(connectorname, connector_index);
}

// ��������� ��� ������� � �������� ����� ���������� ����������
int RDK_CALL Model_BreakAllComponentLinks(const char* stringid)
{
 return PEngine->Model_BreakAllComponentLinks(stringid);
}

// ��������� ��� ������� ����� ���������� ����������
int RDK_CALL Model_BreakAllComponentInputLinks(const char* stringid)
{
 return PEngine->Model_BreakAllComponentInputLinks(stringid);
}

// ��������� ��� �������� ����� ���������� ����������
int RDK_CALL Model_BreakAllComponentOutputLinks(const char* stringid)
{
 return PEngine->Model_BreakAllComponentOutputLinks(stringid);
}

// ���������, ���������� �� ������� �����
bool RDK_CALL Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 return PEngine->Model_CheckLink(stringid1, output_number, stringid2, input_number);
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char * RDK_CALL Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid)
{
 return PEngine->Model_GetComponentInternalLinks(stringid, owner_level_stringid);
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� � ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����������� ������� �������� ����������
int RDK_CALL Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid)
{
 return PEngine->Model_SetComponentInternalLinks(stringid,buffer, owner_level_stringid);
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
 return PEngine->Model_GetComponentInputLinks(stringid,owner_level_stringid, sublevel);
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
 return PEngine->Model_GetComponentOutputLinks(stringid,owner_level_stringid, sublevel);
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
 return PEngine->Model_GetComponentPersonalLinks(stringid,owner_level_stringid);
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentState(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

const char * RDK_CALL MModel_GetComponentState(int engine_index, const char *stringid, unsigned int type_mask)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->Model_GetComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

// ���������� ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedProperties(stringid);
}

// ���������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename)
{
 return PEngine->Model_GetComponentPropertyValue(stringid,statename);
}

const char * RDK_CALL MModel_GetComponentStateValue(int engine_index, const char *stringid, const char *statename)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->Model_GetComponentPropertyValue(stringid,statename);
}

// ������������� ��������� ���������� �� ��������������
int RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_SetComponentState(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.EngineList[engine_index]->Model_SetComponentProperties(stringid, buffer);
}

// ������������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
void RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer)
{
 PEngine->Model_SetComponentPropertyValue(stringid,statename,buffer);
}

// ���������� ����� ������ � ����������
int RDK_CALL Model_GetComponentNumInputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumInputs(stringid);
}

// ���������� ������ ����� ���������� � ����� ���������
int RDK_CALL Model_GetComponentInputDataSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputDataSize(stringid, index);
}

// ���������� ������ �������� ����� � ������
int RDK_CALL Model_GetComponentInputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputElementSize(stringid, index);
}

// ���������� ������ ����� ���������� � ������ ���������
int RDK_CALL Model_GetComponentInputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputByteSize(stringid, index);
}

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
unsigned char* RDK_CALL Model_GetComponentInputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputData(stringid, index);
}

// ���������� ����� ������� � ����������
int RDK_CALL Model_GetComponentNumOutputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumOutputs(stringid);
}

// ���������� ������ ������ ���������� � ����� ���������
int RDK_CALL Model_GetComponentOutputDataSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputDataSize(stringid, index);
}

// ���������� ������ �������� ������ � ������
int RDK_CALL Model_GetComponentOutputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputElementSize(stringid, index);
}

// ���������� ������ ������ ���������� � ������ ���������
int RDK_CALL Model_GetComponentOutputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputByteSize(stringid, index);
}

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
unsigned char* RDK_CALL Model_GetComponentOutputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputData(stringid, index);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
const char * RDK_CALL Model_SaveComponent(const char *stringid, unsigned int params_type_mask)
{
 return PEngine->Model_SaveComponent(stringid, params_type_mask);
}

const char * RDK_CALL MModel_SaveComponent(int engine_index, const char *stringid, unsigned int params_type_mask)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->Model_SaveComponent(stringid, params_type_mask);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int RDK_CALL Model_LoadComponent(const char *stringid, const char* buffer)
{
 return PEngine->Model_LoadComponent(stringid, buffer);
}

int RDK_CALL MModel_LoadComponent(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.EngineList[engine_index]->Model_LoadComponent(stringid, buffer);
}

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL Model_SaveComponentProperties(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_SaveComponentProperties(stringid, type_mask);
}

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
int RDK_CALL Model_LoadComponentProperties(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentProperties(stringid, buffer);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL Model_SaveComponentParameters(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF01);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
int RDK_CALL Model_LoadComponentParameters(const char *stringid, const char* buffer)
{
 return PEngine->Model_LoadComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_LoadComponentParameters(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.EngineList[engine_index]->Model_LoadComponentProperties(stringid, buffer);
}

// ��������� ��������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL Model_SaveComponentState(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
int RDK_CALL Model_LoadComponentState(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentProperties(stringid, buffer);
}

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
const char* RDK_CALL Model_SaveComponentDrawInfo(const char *stringid)
{
 return PEngine->Model_SaveComponentDrawInfo(stringid);
}

// ��������� ����� ����� ������ �� ���������
int RDK_CALL Model_GetDefaultTimeStep(void)
{
 return PEngine->Model_GetDefaultTimeStep();
}

void RDK_CALL Model_SetDefaultTimeStep(int value)
{
 PEngine->Model_SetDefaultTimeStep(value);
}

// ��������� ����� ����� ����������
int RDK_CALL Model_GetTimeStep(const char *stringid)
{
 return PEngine->Model_GetTimeStep(stringid);
}

void RDK_CALL Model_SetTimeStep(const char *stringid, int value)
{
 PEngine->Model_SetTimeStep(stringid, value);
}

// ������������� ��� ����� ���������� � ���� ��� �������� ���������
void RDK_CALL Model_SetGlobalTimeStep(const char *stringid, int value)
{
 PEngine->Model_SetGlobalTimeStep(stringid, value);
}

// ���������� ������� ����� ������
long long RDK_CALL Model_GetTime(void)
{
 return PEngine->Model_GetTime();
}

double RDK_CALL Model_GetDoubleTime(void)
{
 return PEngine->Model_GetDoubleTime();
}

// ������������� ������� ����� ������
bool RDK_CALL Model_SetTime(long long value)
{
 return PEngine->Model_SetTime(value);
}

// ���������� �������� �����
long long RDK_CALL Model_GetRealTime(void)
{
 return PEngine->Model_GetRealTime();
}

double RDK_CALL Model_GetDoubleRealTime(void)
{
 return PEngine->Model_GetDoubleRealTime();
}

// ������������� �������� �����
bool RDK_CALL Model_SetRealTime(long long value)
{
 return PEngine->Model_SetRealTime(value);
}

// ����������� �������� ����� �� �������� ��������
bool RDK_CALL Model_IncreaseRealTime(long long value)
{
 return PEngine->Model_IncreaseRealTime(value);
}

// ���������� ���������� ��� � �������� �������
long long RDK_CALL Model_GetRealTimeStep(void)
{
 return PEngine->Model_GetRealTimeStep();
}

double RDK_CALL Model_GetDoubleRealTimeStep(void)
{
 return PEngine->Model_GetDoubleRealTimeStep();
}

// ���������� ����� ������� ���������� ��� ������� ������� �������� ��������� (��)
long long RDK_CALL Model_GetStepDuration(const char *stringid)
{
 return PEngine->Model_GetStepDuration(stringid);
}

long long RDK_CALL MModel_GetStepDuration(int engine_index, const char *stringid)
{
 return DllManager.EngineList[engine_index]->Model_GetStepDuration(stringid);
}

// ���������� �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
long long RDK_CALL Model_GetFullStepDuration(const char *stringid)
{
 return PEngine->Model_GetFullStepDuration(stringid);
}

long long RDK_CALL MModel_GetFullStepDuration(int engine_index, const char *stringid)
{
 return DllManager.EngineList[engine_index]->Model_GetFullStepDuration(stringid);
}

// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
double RDK_CALL Model_GetInstantPerformance(const char *stringid)
{
 return PEngine->Model_GetInstantPerformance(stringid);
}

double RDK_CALL MModel_GetInstantPerformance(int engine_index, const char *stringid)
{
 return DllManager.EngineList[engine_index]->Model_GetInstantPerformance(stringid);
}

// �����, ��������� ����� ����� ���������� ���������� �����
long long RDK_CALL Model_GetInterstepsInterval(const char *stringid)
{
 return PEngine->Model_GetInterstepsInterval(stringid);
}

long long RDK_CALL MModel_GetInterstepsInterval(int engine_index, const char *stringid)
{
 return DllManager.EngineList[engine_index]->Model_GetInterstepsInterval(stringid);
}
// --------------------------

// --------------------------
// ������ ���������� ������������
// ----------------------------
// ���������� ��������-������������ ����������
void* RDK_CALL Engine_GetExceptionHandler(void)
{
 return reinterpret_cast<void*>(PEngine->GetExceptionHandler());
}

void* RDK_CALL MEngine_GetExceptionHandler(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return reinterpret_cast<void*>(DllManager.EngineList[engine_index]->GetExceptionHandler());
}

bool RDK_CALL Engine_SetExceptionHandler(void* value)
{
 return PEngine->SetExceptionHandler(reinterpret_cast<RDK::UEnvironment::PExceptionHandler>(value));
}

bool RDK_CALL MEngine_SetExceptionHandler(int engine_index, void* value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->SetExceptionHandler(reinterpret_cast<RDK::UEnvironment::PExceptionHandler>(value));
}

// ���������� ������ ����� ����
const char* RDK_CALL Engine_GetLog(void)
{
 return PEngine->GetLog();
}

const char* RDK_CALL MEngine_GetLog(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->GetLog();
}

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
const char* RDK_CALL Engine_GetUnreadLog(void)
{
 return PEngine->GetUnreadLog();
}

const char* RDK_CALL MEngine_GetUnreadLog(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->GetUnreadLog();
}
// ----------------------------


// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ����� ������ �����
void RDK_CALL Env_SetNumInputImages(int number)
{
 return PEngine->Env_SetNumInputImages(number);
}

void RDK_CALL MEnv_SetNumInputImages(int engine_index, int number)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.EngineList[engine_index]->Env_SetNumInputImages(number);
}

// ������ ����� ������� �����
void RDK_CALL Env_SetNumOutputImages(int number)
{
 return PEngine->Env_SetNumOutputImages(number);
}

void RDK_CALL MEnv_SetNumOutputImages(int engine_index, int number)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.EngineList[engine_index]->Env_SetNumOutputImages(number);
}

// ������ ����� ������ �����
int RDK_CALL Env_GetNumInputImages(void)
{
 return PEngine->Env_GetNumInputImages();
}

// ������ ����� ������� �����
int RDK_CALL Env_GetNumOutputImages(void)
{
 return PEngine->Env_GetNumInputImages();
}

// ������ ���������� �� ��������� (������� ����������)
void RDK_CALL Env_SetInputRes(int number, int width, int height)
{
 return PEngine->Env_SetInputRes(number, width, height);
}

void RDK_CALL MEnv_SetInputRes(int engine_index, int number, int width, int height)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.EngineList[engine_index]->Env_SetInputRes(number, width, height);
}

// ������ ������ �����������
void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{
 return PEngine->Env_SetInputImage(number, image, width, height,cmodel);
}

// ������ ���� ��������� �������� ����������� ������ �������������� ���
RDK_LIB_TYPE void Env_SetReflectionXFlag(bool value)
{
 return PEngine->Env_SetReflectionXFlag(value);
}

RDK_LIB_TYPE void MEnv_SetReflectionXFlag(int engine_index, bool value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.EngineList[engine_index]->Env_SetReflectionXFlag(value);
}


// ���������� ���������� �� ��������� (������� ����������)
int RDK_CALL Env_GetInputImageWidth(int number)
{
 return PEngine->Env_GetInputImageWidth(number);
}

int RDK_CALL Env_GetInputImageHeight(int number)
{
 return PEngine->Env_GetInputImageHeight(number);
}

int RDK_CALL Env_GetInputImageColorModel(int number)
{
 return PEngine->Env_GetInputImageColorModel(number);
}

// ���������� ������� �������� ����������
int RDK_CALL Env_GetOutputImageWidth(int number)
{
 return PEngine->Env_GetOutputImageWidth(number);
}

int RDK_CALL Env_GetOutputImageHeight(int number)
{
 return PEngine->Env_GetOutputImageHeight(number);
}

int RDK_CALL Env_GetOutputImageColorModel(int number)
{
 return PEngine->Env_GetOutputImageColorModel(number);
}

unsigned char* RDK_CALL Env_GetInputImage(int index)
{
 return PEngine->Env_GetInputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImage(int index)
{
 return PEngine->Env_GetOutputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImageY8(int index)
{
 return PEngine->Env_GetOutputImageY8(index);
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
 return (PEngine)->Model_GetComponentOutputAsMatrix(stringid, property_name);
}

const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrixByIndex(const char *stringid, int  index)
{
 return (PEngine)->Model_GetComponentOutputAsMatrix(stringid, index);
}

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
const /* RDK::UBitmap* */ void* RDK_CALL Model_GetComponentOutput(const char *stringid, const char *property_name)
{
 return PEngine->Model_GetComponentOutput(stringid, property_name);
}

const /* RDK::UBitmap* */ void* RDK_CALL Model_GetComponentOutputByIndex(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutput(stringid, index);
}

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutput(const char *stringid, const char *property_name)
{
 return PEngine->Model_GetComponentBitmapOutput(stringid, property_name);
}

const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutputByIndex(const char *stringid, int index)
{
 return PEngine->Model_GetComponentBitmapOutput(stringid, index);
}

// ���������� ��������� �� ���� � �������� 'index' ���������� 'id'
const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInput(const char *stringid, const char *property_name)
{
 return PEngine->Model_GetComponentBitmapInput(stringid, property_name);
}

const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInputByIndex(const char *stringid, int index)
{
 return PEngine->Model_GetComponentBitmapInput(stringid, index);
}

// �������� ����������� ������ � �������� 'index' ���������� 'id'
void RDK_CALL Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 PEngine->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL MModel_SetComponentBitmapOutput(int engine_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 DllManager.EngineList[engine_index]->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL Model_SetComponentBitmapOutputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 PEngine->Model_SetComponentBitmapOutput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL MModel_SetComponentBitmapOutputByIndex(int engine_index, const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 DllManager.EngineList[engine_index]->Model_SetComponentBitmapOutput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

// �������� ����������� ����� � �������� 'index' ���������� 'id'
void RDK_CALL Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 PEngine->Model_SetComponentBitmapInput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL Model_SetComponentBitmapInputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 PEngine->Model_SetComponentBitmapInput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}
// --------------------------

/* ************************************************************************** */

extern RDK::UEPtr<RDK::UEngine> PEngine;
extern RDK::UEPtr<RDK::UEnvironment> PEnvironment;
extern RDK::UEPtr<RDK::UStorage> PStorage;


// �������� DLL
// --------------------------
// ������������ � �����������
// --------------------------
RDKDllManager::RDKDllManager(void)
{
 FuncCreateNewStorage=0;//&RDK::NewUStorage;
 FuncCreateNewEnvironment=0;//&RDK::NewUAContainerEnvironment;
 FuncCreateNewEngine=0;//&RDK::NewUEngine;
}

RDKDllManager::~RDKDllManager(void)
{
 EngineUnInit();

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

/// ���������� ����� �������
int RDKDllManager::GetNumEngines(void) const
{
 return int(EngineList.size());
}

/// ������� ��������� ����� ������ �������
int RDKDllManager::SetNumEngines(int num)
{
 if(num<0)
  return 1;

 for(int i=num;i<int(EngineList.size());i++)
  EngineDestroy(i);

 EngineList.resize(num);
 StorageList.resize(num);
 EnvironmentList.resize(num);
 return 0;
}

/// �������� ��������� ������
/// (���� ������ ��� ���������������, �� �� ������ ������
int RDKDllManager::EngineCreate(int index)
{
 if(index<0 || index>=GetNumEngines())
  return 1;

 if(EngineList[index])
  return 0;

 EngineList[index]=FuncCreateNewEngine();
 if(!EngineList[index])
 {
  EngineDestroy(index);
  return 2;
 }

 try
 {
  StorageList[index]=FuncCreateNewStorage();
  if(!StorageList[index])
  {
   EngineDestroy(index);
   return 3;
  }

  EnvironmentList[index]=FuncCreateNewEnvironment();
  if(!EnvironmentList[index])
  {
   EngineDestroy(index);
   return 4;
  }

  EngineList[index]->Default();
  if(!EngineList[index]->Init(StorageList[index],EnvironmentList[index]))
  {
   EngineDestroy(index);
   return 5;
  }
 }
 catch (RDK::UException &exception)
 {
  EngineList[index]->ProcessException(exception);
 }
 return 0;
}

/// ���������� ��������� ������
/// (���� ������ ��� ���������, �� �� ������ ������
int RDKDllManager::EngineDestroy(int index)
{
 if(index<0 || index>=GetNumEngines())
  return 1;

 if(EngineList[index])
 {
  delete EngineList[index];
  EngineList[index]=0;
 }

 if(EnvironmentList[index])
 {
  delete EnvironmentList[index];
  EnvironmentList[index]=0;
 }

 if(StorageList[index])
 {
  delete StorageList[index];
  StorageList[index]=0;
 }
 return 0;
}
// --------------------------



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

 PEngine->Default();
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

  if(!PEngine->Init(PStorage,PEnvironment))
  {
   UnInit();
   return 3;
  }
 }
 catch (RDK::UException &exception)
 {
  PEngine->ProcessException(exception);
 }
 return 0;
}

int UnInit(void)
{
 try
 {
  if(PEngine)
  {
   PEngine->UnInit();
  }
 }
 catch (RDK::UException &exception)
 {
  PEngine->ProcessException(exception);
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
 if(!PEngine)
  return 1;

 if(!exception)
  return 2;

 RDK::UException *exc=reinterpret_cast<RDK::UException*>(exception);
 PEngine->ProcessException(*exc);

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




#endif

