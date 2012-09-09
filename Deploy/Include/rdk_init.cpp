#ifndef RDK_INIT_CPP
#define RDK_INIT_CPP

#include <exception>
#include "rdk_init.h"
#include "rdk.h"

// --------------------------------------
// ���������� �������������� �������
// --------------------------------------
// ���������� ��������� �� �������
RDK::UAContainerStorage* GetStorage(size_t i);

// ���������� ����� �� �������
RDK::UAContainerEnvironment*  GetEnvironment(size_t i);

// ���������� ������ �� �������
RDK::UEngine*  GetEngine(size_t i);

// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
RDK::UAContainerStorage*  AddNewStorage(void);

// ������� ������������ ���������
void DelStorage(RDK::UAContainerStorage* env);

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
RDK::UAContainerEnvironment*  AddNewEnvironment(void);

// ������� ������������ �����
void DelEnvironment(RDK::UAContainerEnvironment* env);

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
RDK::UEngine*  AddNewEngine(void);

// ������� ������������ ������
void DelEngine(RDK::UEngine* engine);

// ������������� ����������
int Init(void* exception_handler);

// ��������������� ����������
int UnInit(void);

// ������������� dll
bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine);
// --------------------------------------

RDK::UEPtr<RDK::UEngine> PEngine=0;
RDK::UEPtr<RDK::UAContainerEnvironment> PEnvironment=0;
RDK::UEPtr<RDK::UAContainerStorage> PStorage=0;

/*****************************************************************************/
extern RDK::UAContainerStorage* CreateNewStorage(void);

extern RDK::UAContainerEnvironment* CreateNewEnvironment(void);

extern RDK::UEngine* CreateNewEngine(void);
/*****************************************************************************/

// ----------------------------
// ������ �������������
// ----------------------------
int RDK_CALL EngineInit(int predefined_structure, void* exception_handler)
{
 EngineUnInit();
 Init(exception_handler);

 Env_SetPredefinedStructure(predefined_structure);
 Env_CreateStructure();
 Env_Init();

 return 0;
}

int RDK_CALL GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx,
		void* exception_handler)
{
 EngineUnInit();
 Init(exception_handler);

 // ������ ����� ������ �����
 Env_SetNumInputImages(num_inputs);
 Env_SetNumOutputImages(num_outputs);

 // ������ ���������� �� ��������� (������� ����������)
 for(int i=0;i<num_inputs;i++)
  Env_SetInputRes(i, input_width, input_height);

 Env_SetReflectionXFlag(reflectionx);

 Env_SetPredefinedStructure(predefined_structure);
 Env_CreateStructure();
 Env_Init();

 return 0;
}

// ���������������� ������ (������� ������������� ���������� ��� ������ �������������)
int RDK_CALL EngineUnInit(void)
{
 if(PEngine)
  if(Env_UnInit())
   return 1;

 return UnInit();
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
const char* RDK_CALL Storage_GetClassDescription(int classid)
{
 return PEngine->Storage_GetClassDescription(classid);
}

// ������������� �������� ������ �� ��� id, �������� ��� �� ������� xml
bool RDK_CALL Storage_SetClassDescription(int classid, const char* description)
{
 return PEngine->Storage_SetClassDescription(classid, description);
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
// ----------------------------

// ----------------------------
// ������ ���������� ������
// ----------------------------
// ������ ������������� �������� ������ ���������
int RDK_CALL Env_GetPredefinedStructure(void)
{
 return PEngine->Env_GetPredefinedStructure();
}

bool RDK_CALL Env_SetPredefinedStructure(int value)
{
 return PEngine->Env_SetPredefinedStructure(value);
}

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool RDK_CALL Env_IsStoragePresent(void)
{
 return PEngine->Env_IsStoragePresent();
}

// ���������� ��������� �������������
bool RDK_CALL Env_IsInit(void)
{
 return PEngine->Env_IsInit();
}

// ������� ������� �������������� ���������
bool RDK_CALL Env_IsStructured(void)
{
 return PEngine->Env_IsStructured();
}

// ������������� �����
bool RDK_CALL Env_Init(void)
{
 return PEngine->Env_Init();
}

// ��������������� �����
bool RDK_CALL Env_UnInit(void)
{
 return PEngine->Env_UnInit();
}

// ��������� �������������� �������� ������ ���������
bool RDK_CALL Env_CreateStructure(void)
{
 return PEngine->Env_CreateStructure();
}

// ���������� ������� ������ ���������
bool RDK_CALL Env_DestroyStructure(void)
{
 return PEngine->Env_DestroyStructure();
}

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
void RDK_CALL Env_Destroy(void)
{
 return PEngine->Env_Destroy();
}

// ��������� ���������� �� ����� dll-�����
int RDK_CALL Env_LoadStorageLibrary(const char *filename)
{
 return PEngine->Env_LoadStorageLibrary(filename);
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool RDK_CALL Env_DelClassLibraryByIndex(int index)
{
 return PEngine->Env_DelClassLibraryByIndex(index);
}

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool RDK_CALL Env_DelClassLibraryByName(const char *name)
{
 return PEngine->Env_DelClassLibraryByName(name);
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool RDK_CALL Env_DelAllClassLibraries(void)
{
 return PEngine->Env_DelAllClassLibraries();
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool RDK_CALL Env_BuildStorage(void)
{
 return PEngine->Env_BuildStorage();
}

// ���������� ����� ���������
int RDK_CALL Env_GetNumClassLibraries(void)
{
 return PEngine->Env_GetNumClassLibraries();
}

// ���������� ��� ���������� �� �������
const char * RDK_CALL Env_GetClassLibraryName(int index)
{
 return PEngine->Env_GetClassLibraryName(index);
}

// ���������� ������ ���������� �� �������
const char * RDK_CALL Env_GetClassLibraryVersion(int index)
{
 return PEngine->Env_GetClassLibraryVersion(index);
}

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
// ���������� id ������ ������ � ���������
int RDK_CALL Env_CreateClass(const char* stringid)
{
 return PEngine->Env_CreateClass(stringid);
}

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
int RDK_CALL Env_Calculate(const char* stringid)
{
 return PEngine->Env_Calculate(stringid);
}

// ������ ���� ������ � �������� �������
void RDK_CALL Env_RTCalculate(void)
{
 PEngine->Env_RTCalculate();
}

// ����� ������ �����
// ���� stringid == 0 �� ���������� ��� ������ �������,
// ����� - ������ ��������� ��������� ������
int RDK_CALL Env_Reset(const char* stringid)
{
 return PEngine->Env_Reset(stringid);
}

// ���������� ���������� ������� ������ �� ��������� ��������
void RDK_CALL Env_IncreaseModelTimeByStep(void)
{
 return PEngine->Env_IncreaseModelTimeByStep();
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
int RDK_CALL Model_Create(int classid)
{
 return PEngine->Model_Create(classid);
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

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'classid'
// ���� stringid - ������ ������, �� ��������� � ���� ������
int RDK_CALL Model_AddComponent(const char* stringid, int classid)
{
 return PEngine->Model_AddComponent(stringid, classid);
}

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'id'
// ���� stringid - ������ ������, �� ������� �� ����� ������
int RDK_CALL Model_DelComponent(const char* stringid, int id)
{
 return PEngine->Model_DelComponent(stringid, id);
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

// ���������� xml-������ ������� ��������������� ���� ����������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ���������� ����� ��������� � ������.
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

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentParameters(const char *stringid)
{
 return PEngine->Model_GetComponentParameters(stringid);
}

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedParameters(stringid);
}

// ���������� ��������� ���������� �� �������������� � ����������
const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid)
{
 return PEngine->Model_GetComponentParametersEx(stringid);
}

// ���������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname)
{
 return PEngine->Model_GetComponentParameterValue(stringid,paramname);
}

// ������������� ��������� ���������� �� ��������������
bool RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentParameters(stringid, buffer);
}

// ������������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
void RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer)
{
 PEngine->Model_SetComponentParameterValue(stringid,paramname,buffer);
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

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
const char * RDK_CALL Model_GetComponentInternalLinks(const char* stringid)
{
 return PEngine->Model_GetComponentInternalLinks(stringid);
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
int RDK_CALL Model_SetComponentInternalLinks(const char* stringid, const char* buffer)
{
 return PEngine->Model_SetComponentInternalLinks(stringid,buffer);
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
const char * RDK_CALL Model_GetComponentInputLinks(const char* stringid)
{
 return PEngine->Model_GetComponentInputLinks(stringid);
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
const char * RDK_CALL Model_GetComponentOutputLinks(const char* stringid)
{
 return PEngine->Model_GetComponentOutputLinks(stringid);
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentState(const char *stringid)
{
 return PEngine->Model_GetComponentState(stringid);
}

// ���������� ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedState(stringid);
}

// ���������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename)
{
 return PEngine->Model_GetComponentStateValue(stringid,statename);
}

// ������������� ��������� ���������� �� ��������������
bool RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentState(stringid, buffer);
}

// ������������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
void RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer)
{
 PEngine->Model_SetComponentStateValue(stringid,statename,buffer);
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
const char * RDK_CALL Model_SaveComponent(const char *stringid)
{
 return PEngine->Model_SaveComponent(stringid);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int RDK_CALL Model_LoadComponent(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponent(stringid, buffer);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL Model_SaveComponentParameters(const char *stringid)
{
 return PEngine->Model_SaveComponentParameters(stringid);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
int RDK_CALL Model_LoadComponentParameters(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentParameters(stringid, buffer);
}

// ��������� ��������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL Model_SaveComponentState(const char *stringid)
{
 return PEngine->Model_SaveComponentState(stringid);
}

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
int RDK_CALL Model_LoadComponentState(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentState(stringid, buffer);
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

// ���������� �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
long long RDK_CALL Model_GetFullStepDuration(const char *stringid)
{
 return PEngine->Model_GetFullStepDuration(stringid);
}

// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
double RDK_CALL Model_GetInstantPerformance(const char *stringid)
{
 return PEngine->Model_GetInstantPerformance(stringid);
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

bool RDK_CALL Engine_SetExceptionHandler(void* value)
{
 return PEngine->SetExceptionHandler(reinterpret_cast<RDK::UEngine::PExceptionHandler>(value));
}

// ���������� ������ ����� ����
const char* RDK_CALL Engine_GetLog(void)
{
 return PEngine->GetLog();
}

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
const char* RDK_CALL Engine_GetUnreadLog(void)
{
 return PEngine->GetUnreadLog();
}
// ----------------------------


// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ����� ������ �����
void RDK_CALL Env_SetNumInputImages(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_SetNumInputImages(number);
}

// ������ ����� ������� �����
void RDK_CALL Env_SetNumOutputImages(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_SetNumOutputImages(number);
}

// ������ ����� ������ �����
int RDK_CALL Env_GetNumInputImages(void)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetNumInputImages();
}

// ������ ����� ������� �����
int RDK_CALL Env_GetNumOutputImages(void)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetNumInputImages();
}

// ������ ���������� �� ��������� (������� ����������)
void RDK_CALL Env_SetInputRes(int number, int width, int height)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_SetInputRes(number, width, height);
}

// ������ ������ �����������
void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_SetInputImage(number, image, width, height,cmodel);
}

// ������ ���� ��������� �������� ����������� ������ �������������� ���
RDK_LIB_TYPE void Env_SetReflectionXFlag(bool value)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_SetReflectionXFlag(value);
}


// ���������� ���������� �� ��������� (������� ����������)
int RDK_CALL Env_GetInputImageWidth(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetInputImageWidth(number);
}

int RDK_CALL Env_GetInputImageHeight(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetInputImageHeight(number);
}

int RDK_CALL Env_GetInputImageColorModel(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetInputImageColorModel(number);
}

// ���������� ������� �������� ����������
int RDK_CALL Env_GetOutputImageWidth(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetOutputImageWidth(number);
}

int RDK_CALL Env_GetOutputImageHeight(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetOutputImageHeight(number);
}

int RDK_CALL Env_GetOutputImageColorModel(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetOutputImageColorModel(number);
}

unsigned char* RDK_CALL Env_GetInputImage(int index)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetInputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImage(int index)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetOutputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImageY8(int index)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetOutputImageY8(index);
}
// --------------------------

// --------------------------
// ������ ���������� ����������� �������
// --------------------------
// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
const /* RDK::UBitmap* */ void* const RDK_CALL Model_GetComponentOutput(const char *stringid, int index)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Model_GetComponentOutput(stringid, index);
}

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
const /*RDK::UBitmap* */ void* const RDK_CALL Model_GetComponentBitmapOutput(const char *stringid, int index)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Model_GetComponentBitmapOutput(stringid, index);
}

// ���������� ��������� �� ���� � �������� 'index' ���������� 'id'
const /*RDK::UBitmap* */ void* const RDK_CALL Model_GetComponentBitmapInput(const char *stringid, int index)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Model_GetComponentBitmapInput(stringid, index);
}

// �������� ����������� ������ � �������� 'index' ���������� 'id'
void RDK_CALL Model_SetComponentBitmapOutput(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp)
{
 RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Model_SetComponentBitmapOutput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp));
}

// �������� ����������� ����� � �������� 'index' ���������� 'id'
void RDK_CALL Model_SetComponentBitmapInput(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp)
{
 RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Model_SetComponentBitmapInput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp));
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

extern RDK::UEPtr<RDK::UEngine> PEngine;
extern RDK::UEPtr<RDK::UAContainerEnvironment> PEnvironment;
extern RDK::UEPtr<RDK::UAContainerStorage> PStorage;


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
/*int Engine_Destroy(void)
{
 PEngine=0;
 PEnvironment=0;
 PStorage=0;
 return 0;
} */

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

// ���������� ����� �������� � ����������
int GetNumStorages(void)
{
 return DllManager.StorageList.size();
}

// ���������� ����� ���� � ����������
int GetNumEnvironments(void)
{
 return DllManager.EnvironmentList.size();
}

// ���������� ����� ������� � ����������
int GetNumEngines(void)
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
//  CreateStorage(storage);
  DllManager.StorageList.push_back(storage);
 }
 return storage;
}

// ������� ������������ ���������
void DelStorage(RDK::UAContainerStorage* storage)
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
RDK::UAContainerEnvironment* AddNewEnvironment(void)
{
 if(!DllManager.FuncCreateNewEnvironment)
  return 0;

 RDK::UAContainerEnvironment* environment=DllManager.FuncCreateNewEnvironment();
 if(environment)
 {
//  CreateEnvironment(environment, storage,isinit,external_classes, external_libs);
  DllManager.EnvironmentList.push_back(environment);
 }
 return environment;
}

// ������� ������������ �����
void DelEnvironment(RDK::UAContainerEnvironment* env)
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
}
// ----------------------------




#endif

