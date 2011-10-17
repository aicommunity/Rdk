#ifndef RDK_ENGINE_CPP
#define RDK_ENGINE_CPP

#include "rdk.h"
#include "rdk_engine.h"


RDK::UEngine* PEngine=0;
RDK::UAEnvironment* PEnvironment=0;
RDK::UAStorage* PStorage=0;

// ----------------------------
// ������ �������������
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
// ----------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
int RDKStorage_GetNumClasses(void)
{
 return PEngine->Storage_GetNumClasses();
}

// ���������� id ������� � ���������. ������ ������ ���� ��������
void RDKStorage_GetClassesList(int *buffer)
{
 PEngine->Storage_GetClassesList(buffer);
}

// ���������� ��� ������ �� ��� id.
const char * RDKStorage_GetClassName(int id)
{
 return PEngine->Storage_GetClassName(id);
}

// ���������� Id ������ �� ��� �����
int RDKStorage_GetClassId(const char *name)
{
 return PEngine->Storage_GetClassId(name);
}

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
bool RDKStorage_DelClass(int classid)
{
 return PEngine->Storage_DelClass(classid);
}

// �������� ��� ��������� ������� �� ���������
void RDKStorage_FreeObjectsStorage(void)
{
 PEngine->Storage_FreeObjectsStorage();
}

// ������� ��� ������� �� ���������
void RDKStorage_ClearObjectsStorage(void)
{
 PEngine->Storage_ClearObjectsStorage();
}

// ��������� ��������� ����� �������� � ���������
int RDKStorage_CalcNumObjects(void)
{
 return PEngine->Storage_CalcNumObjects();
}

int RDKStorage_CalcNumObjectsById(int classid)
{
 return PEngine->Storage_CalcNumObjectsById(classid);
}

int RDKStorage_CalcNumObjectsByName(const char* classname)
{
 return PEngine->Storage_CalcNumObjectsByName(classname);
}

// ������ ���������� ������
// ----------------------------
// ������ ������������� �������� ������ ���������
int RDKEnv_GetPredefinedStructure(void)
{
 return PEngine->Env_GetPredefinedStructure();
}

bool RDKEnv_SetPredefinedStructure(int value)
{
 return PEngine->Env_SetPredefinedStructure(value);
}

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool RDKEnv_IsStoragePresent(void)
{
 return PEngine->Env_IsStoragePresent();
}

// ���������� ��������� �������������
bool RDKEnv_IsInit(void)
{
 return PEngine->Env_IsInit();
}

// ������� ������� �������������� ���������
bool RDKEnv_IsStructured(void)
{
 return PEngine->Env_IsStructured();
}

// ������������� �����
bool RDKEnv_Init(void)
{
 return PEngine->Env_Init();
}

// ��������������� �����
bool RDKEnv_UnInit(void)
{
 return PEngine->Env_UnInit();
}

// ��������� �������������� �������� ������ ���������
bool RDKEnv_CreateStructure(void)
{
 return PEngine->Env_CreateStructure();
}

// ���������� ������� ������ ���������
bool RDKEnv_DestroyStructure(void)
{
 return PEngine->Env_DestroyStructure();
}

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
void RDKEnv_Destroy(void)
{
 return PEngine->Env_Destroy();
}

// ��������� ���������� �� ����� dll-�����
int RDKEnv_LoadStorageLibrary(const char *filename)
{
 return PEngine->Env_LoadStorageLibrary(filename);
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool RDKEnv_DelClassLibraryByIndex(int index)
{
 return PEngine->Env_DelClassLibraryByIndex(index);
}

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool RDKEnv_DelClassLibraryByName(const char *name)
{
 return PEngine->Env_DelClassLibraryByName(name);
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool RDKEnv_DelAllClassLibraries(void)
{
 return PEngine->Env_DelAllClassLibraries();
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool RDKEnv_BuildStorage(void)
{
 return PEngine->Env_BuildStorage();
}

// ���������� ����� ���������
int RDKEnv_GetNumClassLibraries(void)
{
 return PEngine->Env_GetNumClassLibraries();
}

// ���������� ��� ���������� �� �������
const char * RDKEnv_GetClassLibraryName(int index)
{
 return PEngine->Env_GetClassLibraryName(index);
}

// ���������� ������ ���������� �� �������
const char * RDKEnv_GetClassLibraryVersion(int index)
{
 return PEngine->Env_GetClassLibraryVersion(index);
}

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
// ���������� id ������ ������ � ���������
int RDKEnv_CreateClass(const char* stringid)
{
 return PEngine->Env_CreateClass(stringid);
}

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
int RDKEnv_Calculate(const char* stringid)
{
 return PEngine->Env_Calculate(stringid);
}

// ������ ���������� �������
// ----------------------------
// ������� ������
int RDKModel_Destroy(void)
{
 return PEngine->Model_Destroy();
}

// ������� ����� ������ �� ����� ������ � ���������
// �������������� ������� ������������ ������
int RDKModel_Create(int classid)
{
 return PEngine->Model_Create(classid);
}

// ������� ������
int RDKModel_Clear(void)
{
 return PEngine->Model_Clear();
}

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'classid'
// ���� stringid - ������ ������, �� ��������� � ���� ������
int RDKModel_AddComponent(char* stringid, int classid)
{
 return PEngine->Model_AddComponent(stringid, classid);
}

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'id'
// ���� stringid - ������ ������, �� ������� �� ����� ������
int RDKModel_DelComponent(char* stringid, int id)
{
 return PEngine->Model_DelComponent(stringid, id);
}

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
int RDKModel_GetNumComponents(char* stringid)
{
 return PEngine->Model_GetNumComponents(stringid);
}

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
int RDKModel_GetComponentsList(char* stringid, int *buffer)
{
 return PEngine->Model_GetComponentsList(stringid, buffer);
}

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
const char* RDKModel_GetComponentName(char* stringid)
{
 return PEngine->Model_GetComponentName(stringid);
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDKModel_GetComponentParameters(const char *stringid)
{
 return PEngine->Model_GetComponentParameters(stringid);
}

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDKModel_GetComponentSelectedParameters(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedParameters(stringid);
}

// ������������� ��������� ���������� �� ��������������
bool RDKModel_SetComponentParameters(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentParameters(stringid, buffer);
}

// ��������� ��������� ���������� ���� � ������
int RDKModel_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 return PEngine->Model_CreateLink(stringid1, output_number, stringid2, input_number);
}

// ��������� ��������� �����
int RDKModel_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 return PEngine->Model_BreakLink(stringid1, output_number, stringid2, input_number);
}

// ��������� ��� �����
int RDKModel_BreakAllLinks(void)
{
 return PEngine->Model_BreakAllLinks();
}


// ��������� ��� ������� � �������� ����� ���������� ����������
int RDKModel_BreakAllComponentLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentLinks(stringid);
}

// ��������� ��� ������� ����� ���������� ����������
int RDKModel_BreakAllComponentInputLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentInputLinks(stringid);
}

// ��������� ��� �������� ����� ���������� ����������
int RDKModel_BreakAllComponentOutputLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentOutputLinks(stringid);
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
const char * RDKModel_GetComponentInternalLinks(char* stringid)
{
 return PEngine->Model_GetComponentInternalLinks(stringid);
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
int RDKModel_SetComponentInternalLinks(char* stringid,char* buffer)
{
 return PEngine->Model_SetComponentInternalLinks(stringid,buffer);
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
const char * RDKModel_GetComponentInputLinks(char* stringid)
{
 return PEngine->Model_GetComponentInputLinks(stringid);
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
const char * RDKModel_GetComponentOutputLinks(char* stringid)
{
 return PEngine->Model_GetComponentOutputLinks(stringid);
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDKModel_GetComponentState(const char *stringid)
{
 return PEngine->Model_GetComponentState(stringid);
}

// ���������� ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDKModel_GetComponentSelectedState(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedState(stringid);
}

// ������������� ��������� ���������� �� ��������������
bool RDKModel_SetComponentState(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentState(stringid, buffer);
}

// ���������� ����� ������ � ����������
int RDKModel_GetComponentNumInputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumInputs(stringid);
}

// ���������� ������ ����� ���������� � ����� ���������
int RDKModel_GetComponentInputSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputSize(stringid, index);
}

// ���������� ������ �������� ����� � ������
int RDKModel_GetComponentInputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputElementSize(stringid, index);
}

// ���������� ������ ����� ���������� � ������ ���������
int RDKModel_GetComponentInputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputByteSize(stringid, index);
}

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
unsigned char* RDKModel_GetComponentInputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputData(stringid, index);
}

// ���������� ����� ������� � ����������
int RDKModel_GetComponentNumOutputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumOutputs(stringid);
}

// ���������� ������ ������ ���������� � ����� ���������
int RDKModel_GetComponentOutputSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputSize(stringid, index);
}

// ���������� ������ �������� ������ � ������
int RDKModel_GetComponentOutputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputElementSize(stringid, index);
}

// ���������� ������ ������ ���������� � ������ ���������
int RDKModel_GetComponentOutputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputByteSize(stringid, index);
}

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
unsigned char* RDKModel_GetComponentOutputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputData(stringid, index);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
const char * RDKModel_SaveComponent(const char *stringid)
{
 return PEngine->Model_SaveComponent(stringid);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int RDKModel_LoadComponent(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponent(stringid, buffer);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
const char * RDKModel_SaveComponentParameters(const char *stringid)
{
 return PEngine->Model_SaveComponentParameters(stringid);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
int RDKModel_LoadComponentParameters(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentParameters(stringid, buffer);
}

// ��������� ��������� ���������� � ��� �������� ��������� � xml
const char * RDKModel_SaveComponentState(const char *stringid)
{
 return PEngine->Model_SaveComponentState(stringid);
}

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
int RDKModel_LoadComponentState(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentState(stringid, buffer);
}
// --------------------------

#endif


