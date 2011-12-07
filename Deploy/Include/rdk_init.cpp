#ifndef RDK_INIT_CPP
#define RDK_INIT_CPP

#include "rdk_init.h"
//#include "rdk_gengine.h"


RDK::UEngine* PEngine=0;
RDK::UAEnvironment* PEnvironment=0;
RDK::UAStorage* PStorage=0;

/*****************************************************************************/
RDK::UAStorage* CreateNewStorage(void)
{
 return new RDK::UAStorage;
}

extern RDK::UAEnvironment* CreateNewEnvironment(void)
{
 return new RDK::UAEnvironment;
}

extern RDK::UEngine* CreateNewEngine(void)
{
 return new RDK::UEngine;
}

RDK::UAStorage* CreateNewGStorage(void)
{
 return new RDK::UBAStorage;
}

RDK::UAEnvironment* CreateNewGEnvironment(void)
{
 return new RDK::UBAEnvironment;
}

RDK::UEngine* CreateNewGEngine(void)
{
 return new RDK::UBEngine;
}
/*****************************************************************************/

// ----------------------------
// ������ �������������
// ----------------------------
int RDK_EngineInit(int predefined_structure)
{
 LoadEngine((void*)CreateNewStorage, (void*)CreateNewEnvironment, (void*)CreateNewEngine);
 RDKInit();
 RDKLoadPredefinedLibraries();

 Env_SetPredefinedStructure(predefined_structure);
 Env_CreateStructure();

 return 0;
}

int RDK_GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height)
{
 LoadEngine((void*)CreateNewGStorage, (void*)CreateNewGEnvironment, (void*)CreateNewGEngine);
 RDKInit();
 RDKLoadPredefinedLibraries();

 // ������ ����� ������ �����
 Env_SetNumInputImages(num_inputs);
 Env_SetNumOutputImages(num_outputs);

 // ������ ���������� �� ��������� (������� ����������)
 for(int i=0;i<num_inputs;i++)
  Env_SetInputRes(i, input_width, input_height);

 Env_SetPredefinedStructure(predefined_structure);
 Env_CreateStructure();

 return 0;
}
// ----------------------------

// --------------------------
// ������ ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
int Storage_GetNumClasses(void)
{
 return PEngine->Storage_GetNumClasses();
}

// ���������� id ������� � ���������. ������ ������ ���� ��������
void Storage_GetClassesList(int *buffer)
{
 PEngine->Storage_GetClassesList(buffer);
}

// ���������� ��� ������ �� ��� id.
const char * Storage_GetClassName(int id)
{
 return PEngine->Storage_GetClassName(id);
}

// ���������� Id ������ �� ��� �����
int Storage_GetClassId(const char *name)
{
 return PEngine->Storage_GetClassId(name);
}

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
bool Storage_DelClass(int classid)
{
 return PEngine->Storage_DelClass(classid);
}

// �������� ��� ��������� ������� �� ���������
void Storage_FreeObjectsStorage(void)
{
 PEngine->Storage_FreeObjectsStorage();
}

// ������� ��� ������� �� ���������
void Storage_ClearObjectsStorage(void)
{
 PEngine->Storage_ClearObjectsStorage();
}

// ��������� ��������� ����� �������� � ���������
int Storage_CalcNumObjects(void)
{
 return PEngine->Storage_CalcNumObjects();
}

int Storage_CalcNumObjectsById(int classid)
{
 return PEngine->Storage_CalcNumObjectsById(classid);
}

int Storage_CalcNumObjectsByName(const char* classname)
{
 return PEngine->Storage_CalcNumObjectsByName(classname);
}
// ----------------------------

// ----------------------------
// ������ ���������� ������
// ----------------------------
// ������ ������������� �������� ������ ���������
int Env_GetPredefinedStructure(void)
{
 return PEngine->Env_GetPredefinedStructure();
}

bool Env_SetPredefinedStructure(int value)
{
 return PEngine->Env_SetPredefinedStructure(value);
}

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool Env_IsStoragePresent(void)
{
 return PEngine->Env_IsStoragePresent();
}

// ���������� ��������� �������������
bool Env_IsInit(void)
{
 return PEngine->Env_IsInit();
}

// ������� ������� �������������� ���������
bool Env_IsStructured(void)
{
 return PEngine->Env_IsStructured();
}

// ������������� �����
bool Env_Init(void)
{
 return PEngine->Env_Init();
}

// ��������������� �����
bool Env_UnInit(void)
{
 return PEngine->Env_UnInit();
}

// ��������� �������������� �������� ������ ���������
bool Env_CreateStructure(void)
{
 return PEngine->Env_CreateStructure();
}

// ���������� ������� ������ ���������
bool Env_DestroyStructure(void)
{
 return PEngine->Env_DestroyStructure();
}

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
void Env_Destroy(void)
{
 return PEngine->Env_Destroy();
}

// ��������� ���������� �� ����� dll-�����
int Env_LoadStorageLibrary(const char *filename)
{
 return PEngine->Env_LoadStorageLibrary(filename);
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool Env_DelClassLibraryByIndex(int index)
{
 return PEngine->Env_DelClassLibraryByIndex(index);
}

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool Env_DelClassLibraryByName(const char *name)
{
 return PEngine->Env_DelClassLibraryByName(name);
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool Env_DelAllClassLibraries(void)
{
 return PEngine->Env_DelAllClassLibraries();
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool Env_BuildStorage(void)
{
 return PEngine->Env_BuildStorage();
}

// ���������� ����� ���������
int Env_GetNumClassLibraries(void)
{
 return PEngine->Env_GetNumClassLibraries();
}

// ���������� ��� ���������� �� �������
const char * Env_GetClassLibraryName(int index)
{
 return PEngine->Env_GetClassLibraryName(index);
}

// ���������� ������ ���������� �� �������
const char * Env_GetClassLibraryVersion(int index)
{
 return PEngine->Env_GetClassLibraryVersion(index);
}

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
// ���������� id ������ ������ � ���������
int Env_CreateClass(const char* stringid)
{
 return PEngine->Env_CreateClass(stringid);
}

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
int Env_Calculate(const char* stringid)
{
 return PEngine->Env_Calculate(stringid);
}
// --------------------------

// --------------------------
// ������ ���������� �������
// ----------------------------
// ������� ������
int Model_Destroy(void)
{
 return PEngine->Model_Destroy();
}

// ������� ����� ������ �� ����� ������ � ���������
// �������������� ������� ������������ ������
int Model_Create(int classid)
{
 return PEngine->Model_Create(classid);
}

// ������� ������
int Model_Clear(void)
{
 return PEngine->Model_Clear();
}

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'classid'
// ���� stringid - ������ ������, �� ��������� � ���� ������
int Model_AddComponent(char* stringid, int classid)
{
 return PEngine->Model_AddComponent(stringid, classid);
}

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'id'
// ���� stringid - ������ ������, �� ������� �� ����� ������
int Model_DelComponent(char* stringid, int id)
{
 return PEngine->Model_DelComponent(stringid, id);
}

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
int Model_GetNumComponents(char* stringid)
{
 return PEngine->Model_GetNumComponents(stringid);
}

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
int Model_GetComponentsList(char* stringid, int *buffer)
{
 return PEngine->Model_GetComponentsList(stringid, buffer);
}

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
const char* Model_GetComponentName(char* stringid)
{
 return PEngine->Model_GetComponentName(stringid);
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * Model_GetComponentParameters(const char *stringid)
{
 return PEngine->Model_GetComponentParameters(stringid);
}

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * Model_GetComponentSelectedParameters(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedParameters(stringid);
}

// ������������� ��������� ���������� �� ��������������
bool Model_SetComponentParameters(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentParameters(stringid, buffer);
}

// ��������� ��������� ���������� ���� � ������
int Model_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 return PEngine->Model_CreateLink(stringid1, output_number, stringid2, input_number);
}

// ��������� ��������� �����
int Model_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 return PEngine->Model_BreakLink(stringid1, output_number, stringid2, input_number);
}

// ��������� ��� �����
int Model_BreakAllLinks(void)
{
 return PEngine->Model_BreakAllLinks();
}


// ��������� ��� ������� � �������� ����� ���������� ����������
int Model_BreakAllComponentLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentLinks(stringid);
}

// ��������� ��� ������� ����� ���������� ����������
int Model_BreakAllComponentInputLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentInputLinks(stringid);
}

// ��������� ��� �������� ����� ���������� ����������
int Model_BreakAllComponentOutputLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentOutputLinks(stringid);
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
const char * Model_GetComponentInternalLinks(char* stringid)
{
 return PEngine->Model_GetComponentInternalLinks(stringid);
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
int Model_SetComponentInternalLinks(char* stringid,char* buffer)
{
 return PEngine->Model_SetComponentInternalLinks(stringid,buffer);
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
const char * Model_GetComponentInputLinks(char* stringid)
{
 return PEngine->Model_GetComponentInputLinks(stringid);
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
const char * Model_GetComponentOutputLinks(char* stringid)
{
 return PEngine->Model_GetComponentOutputLinks(stringid);
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * Model_GetComponentState(const char *stringid)
{
 return PEngine->Model_GetComponentState(stringid);
}

// ���������� ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * Model_GetComponentSelectedState(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedState(stringid);
}

// ������������� ��������� ���������� �� ��������������
bool Model_SetComponentState(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentState(stringid, buffer);
}

// ���������� ����� ������ � ����������
int Model_GetComponentNumInputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumInputs(stringid);
}

// ���������� ������ ����� ���������� � ����� ���������
int Model_GetComponentInputSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputSize(stringid, index);
}

// ���������� ������ �������� ����� � ������
int Model_GetComponentInputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputElementSize(stringid, index);
}

// ���������� ������ ����� ���������� � ������ ���������
int Model_GetComponentInputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputByteSize(stringid, index);
}

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
unsigned char* Model_GetComponentInputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputData(stringid, index);
}

// ���������� ����� ������� � ����������
int Model_GetComponentNumOutputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumOutputs(stringid);
}

// ���������� ������ ������ ���������� � ����� ���������
int Model_GetComponentOutputSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputSize(stringid, index);
}

// ���������� ������ �������� ������ � ������
int Model_GetComponentOutputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputElementSize(stringid, index);
}

// ���������� ������ ������ ���������� � ������ ���������
int Model_GetComponentOutputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputByteSize(stringid, index);
}

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
unsigned char* Model_GetComponentOutputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputData(stringid, index);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
const char * Model_SaveComponent(const char *stringid)
{
 return PEngine->Model_SaveComponent(stringid);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int Model_LoadComponent(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponent(stringid, buffer);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
const char * Model_SaveComponentParameters(const char *stringid)
{
 return PEngine->Model_SaveComponentParameters(stringid);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
int Model_LoadComponentParameters(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentParameters(stringid, buffer);
}

// ��������� ��������� ���������� � ��� �������� ��������� � xml
const char * Model_SaveComponentState(const char *stringid)
{
 return PEngine->Model_SaveComponentState(stringid);
}

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
int Model_LoadComponentState(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentState(stringid, buffer);
}
// --------------------------

// --------------------------
// ������ ���������� ������������
// ----------------------------
// ���������� ������ ����� ����
const char* RDKEngine_GetLog(void)
{
 return PEngine->GetLog();
}
// ----------------------------


// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ����� ������ �����
void Env_SetNumInputImages(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetNumInputImages(number);
}

// ������ ����� ������� �����
void Env_SetNumOutputImages(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetNumOutputImages(number);
}

// ������ ����� ������ �����
int Env_GetNumInputImages(void)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetNumInputImages();
}

// ������ ����� ������� �����
int Env_GetNumOutputImages(void)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetNumInputImages();
}

// ������ ���������� �� ��������� (������� ����������)
void Env_SetInputRes(int number, int width, int height)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetInputRes(number, width, height);
}

void Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetInputImage(number, image, width, height,cmodel);
}


// ���������� ���������� �� ��������� (������� ����������)
int Env_GetInputImageWidth(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImageWidth(number);
}

int Env_GetInputImageHeight(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImageHeight(number);
}

int Env_GetInputImageColorModel(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImageColorModel(number);
}

// ���������� ������� �������� ����������
int Env_GetOutputImageWidth(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageWidth(number);
}

int Env_GetOutputImageHeight(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageHeight(number);
}

int Env_GetOutputImageColorModel(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageColorModel(number);
}

unsigned char* Env_GetInputImage(int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImage(index);
}

unsigned char* Env_GetOutputImage(int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImage(index);
}

unsigned char* Env_GetOutputImageY8(int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageY8(index);
}

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
const RDK::UBitmap* const Env_GetComponentOutput(const char *stringid, int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetComponentOutput(stringid, index);
}
// --------------------------

/* ************************************************************************** */

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

// ��������� ���������
RDKDllManager DllManager;

extern RDK::UEngine* PEngine;
extern RDK::UAEnvironment* PEnvironment;
extern RDK::UAStorage* PStorage;


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



// ----------------------------
// ���������� ������ �������������
// ----------------------------
// ��������������� ������
int Engine_Destroy(void)
{
 PEngine=0;
 PEnvironment=0;
 PStorage=0;
 return 0;
}

int RDKInit(void)
{
  if(!PEngine->Init())
  {
   Engine_Destroy();
   return 3;
  }

  PEnvironment=PEngine->GetEnvironment();
  if(PEnvironment)
   PStorage=PEnvironment->GetStorage();
  else
  {
   Engine_Destroy();
   return 4;
  }

  if(!PStorage)
  {
   Engine_Destroy();
   return 5;
  }

 return 0;
}

int RDKUnInit(void)
{
 if(!PEngine->UnInit())
  return 1;

 return 0;
}

// ��������� ����� ����������������� ���������
int RDKLoadPredefinedLibraries(void)
{
 return PEngine->LoadPredefinedLibraries();
}

// ���������� ���������� ����������
// ������ ���� ������ � ���������� ����������� ����������������� ��
int RDKExceptionDispatcher(void *exception)
{
 if(!PEngine)
  return 1;

 RDK::UException *exc=reinterpret_cast<RDK::UException*>(exception);
 PEngine->ProcessException(exc);

 return 0;
}

// ��������������� ������
extern int Engine_Destroy(void);

// ������������� ������
int Engine_Create(RDK::UEngine *engine, const char *inifilename, void *pCreateNewStorage, void *pCreateNewEnvironment)
{
 if(!engine)
  return 1;

 if(PEngine != engine)
 {
  Engine_Destroy();

  PEngine=engine;
 }

 if(inifilename)
 {
  if(!PEngine->SetOptionsFileName(inifilename))
  {
   Engine_Destroy();
   return 2;
  }

  if(!PEngine->SetFuncCreateNewStorage(reinterpret_cast<RDK::UEngine::PCreateNewStorage>(pCreateNewStorage)))
   return 10;

  if(!PEngine->SetFuncCreateNewEnvironment(reinterpret_cast<RDK::UEngine::PCreateNewEnvironment>(pCreateNewEnvironment)))
   return 11;
 }
 return 0;
}


int LoadEngine(void *create_storage, void *create_environment, void *create_engine)
{
 if(!DllInit(create_storage, create_environment, create_engine))
  return -2;

 RDK::UEngine* pengine=dynamic_cast<RDK::UEngine*>(AddNewEngine());

 if(!pengine)
  return -3;

 pengine->Default();

 if(Engine_Create(pengine, "options.ini",(void*)AddNewStorage,(void*)AddNewEnvironment))
  return -4;

 return 0;
}

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
// ----------------------------




#endif

