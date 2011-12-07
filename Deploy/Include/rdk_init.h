#ifndef RDK_INIT_H
#define RDK_INIT_H

#include "rdk.h"
//#include "rdk_gengine.h"

extern "C"  {

// ----------------------------
// ������ �������������
// ----------------------------
// �������������� ������ (������� ������ ���� ������� ������!)
RDK_LIB_TYPE int RDK_EngineInit(int predefined_structure);

// �������������� ����������� ������ (������� ������ ���� ������� ������!)
RDK_LIB_TYPE int RDK_GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height);
// ----------------------------

// --------------------------
// ������ ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
RDK_LIB_TYPE int Storage_GetNumClasses(void);

// ���������� id ������� � ���������. ������ ������ ���� ��������
RDK_LIB_TYPE void Storage_GetClassesList(int *buffer);

// ���������� ��� ������ �� ��� id.
RDK_LIB_TYPE const char * Storage_GetClassName(int id);

// ���������� Id ������ �� ��� �����
RDK_LIB_TYPE int Storage_GetClassId(const char *name);

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
RDK_LIB_TYPE bool Storage_DelClass(int classid);

// ������� ��� ��������� ������� �� ���������
RDK_LIB_TYPE void Storage_FreeObjectsStorage(void);

// ������� ��� ������� �� ���������
RDK_LIB_TYPE void Storage_ClearObjectsStorage(void);

// ��������� ��������� ����� �������� � ���������
RDK_LIB_TYPE int Storage_CalcNumObjects(void);
RDK_LIB_TYPE int Storage_CalcNumObjectsById(int classid);
RDK_LIB_TYPE int Storage_CalcNumObjectsByName(const char* classname);
// --------------------------

// --------------------------
// ������ ���������� ������
// ----------------------------
// ������ �������������� �������� ������ ���������
RDK_LIB_TYPE int Env_GetPredefinedStructure(void);
RDK_LIB_TYPE bool Env_SetPredefinedStructure(int value);

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
RDK_LIB_TYPE bool Env_IsStoragePresent(void);

// ���������� ��������� �������������
RDK_LIB_TYPE bool Env_IsInit(void);

// ������� ������� �������������� ���������
RDK_LIB_TYPE bool Env_IsStructured(void);

// ������������� �����
RDK_LIB_TYPE bool Env_Init(void);

// ��������������� �����
RDK_LIB_TYPE bool Env_UnInit(void);

// ��������� �������������� �������� ������ ���������
RDK_LIB_TYPE bool Env_CreateStructure(void);

// ���������� ������� ������ ���������
RDK_LIB_TYPE bool Env_DestroyStructure(void);

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
RDK_LIB_TYPE void Env_Destroy(void);

// ��������� ���������� �� ����� dll-�����
RDK_LIB_TYPE int Env_LoadStorageLibrary(const char *filename);

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool Env_DelClassLibraryByIndex(int index);

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool Env_DelClassLibraryByName(const char *name);

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool Env_DelAllClassLibraries(void);

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
RDK_LIB_TYPE bool Env_BuildStorage(void);

// ���������� ����� ���������
RDK_LIB_TYPE int Env_GetNumClassLibraries(void);

// ���������� ��� ���������� �� �������
RDK_LIB_TYPE const char * Env_GetClassLibraryName(int index);

// ���������� ������ ���������� �� �������
RDK_LIB_TYPE const char * Env_GetClassLibraryVersion(int index);

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
// ���������� id ������ ������ � ���������
RDK_LIB_TYPE int Env_CreateClass(const char* stringid);

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
RDK_LIB_TYPE int Env_Calculate(const char* stringid=0);
// ----------------------------

// ----------------------------
// ������ ���������� �������
// ----------------------------
// ������� ������
RDK_LIB_TYPE int Model_Destroy(void);

// ������� ����� ������ �� ����� ������ � ���������
// �������������� ������� ������������ ������
RDK_LIB_TYPE int Model_Create(int classid);

// ������� ������
RDK_LIB_TYPE int Model_Clear(void);

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'classid'
// ���� stringid - ������ ������, �� ��������� � ���� ������
RDK_LIB_TYPE int Model_AddComponent(char* stringid, int classid);

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'id'
// ���� stringid - ������ ������, �� ������� �� ����� ������
RDK_LIB_TYPE int Model_DelComponent(char* stringid, int id);

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
RDK_LIB_TYPE int Model_GetNumComponents(char* stringid);

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
RDK_LIB_TYPE int Model_GetComponentsList(char* stringid, int *buffer);

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
RDK_LIB_TYPE const char* Model_GetComponentName(char* stringid);

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
RDK_LIB_TYPE const char * Model_GetComponentParameters(const char *stringid);

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
RDK_LIB_TYPE const char * Model_GetComponentSelectedParameters(const char *stringid);

// ������������� ��������� ���������� �� ��������������
RDK_LIB_TYPE bool Model_SetComponentParameters(const char *stringid, const char* buffer);

// ��������� ��������� ���������� ���� � ������
RDK_LIB_TYPE int Model_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number);

// ��������� ��������� �����
RDK_LIB_TYPE int Model_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number);

// ��������� ��� �����
RDK_LIB_TYPE int Model_BreakAllLinks(void);

// ��������� ��� ������� � �������� ����� ���������� ����������
RDK_LIB_TYPE int Model_BreakAllComponentLinks(char* stringid);

// ��������� ��� ������� ����� ���������� ����������
RDK_LIB_TYPE int Model_BreakAllComponentInputLinks(char* stringid);

// ��������� ��� �������� ����� ���������� ����������
RDK_LIB_TYPE int Model_BreakAllComponentOutputLinks(char* stringid);

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
RDK_LIB_TYPE const char * Model_GetComponentInternalLinks(char* stringid);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
RDK_LIB_TYPE int Model_SetComponentInternalLinks(char* stringid, char* buffer);

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
RDK_LIB_TYPE const char * Model_GetComponentInputLinks(char* stringid);

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
RDK_LIB_TYPE const char * Model_GetComponentOutputLinks(char* stringid);

// ���������� ��������� ���������� �� ��������������
RDK_LIB_TYPE const char * Model_GetComponentState(const char *stringid);

// ���������� ���������� ������ ��������� ���������� �� ��������������
RDK_LIB_TYPE const char * Model_GetComponentSelectedState(const char *stringid);

// ������������� ��������� ���������� �� ��������������
RDK_LIB_TYPE bool Model_SetComponentState(const char *stringid, const char* buffer);

// ���������� ����� ������ � ����������
RDK_LIB_TYPE int Model_GetComponentNumInputs(const char *stringid);

// ���������� ������ ����� ���������� � ����� ���������
RDK_LIB_TYPE int Model_GetComponentInputSize(const char *stringid, int index);

// ���������� ������ �������� ����� � ������
RDK_LIB_TYPE int Model_GetComponentInputElementSize(const char *stringid, int index);

// ���������� ������ ����� ���������� � ������ ���������
RDK_LIB_TYPE int Model_GetComponentInputByteSize(const char *stringid, int index);

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
RDK_LIB_TYPE unsigned char* Model_GetComponentInputData(const char *stringid, int index);

// ���������� ����� ������� � ����������
RDK_LIB_TYPE int Model_GetComponentNumOutputs(const char *stringid);

// ���������� ������ ������ ���������� � ����� ���������
RDK_LIB_TYPE int Model_GetComponentOutputSize(const char *stringid, int index);

// ���������� ������ �������� ������ � ������
RDK_LIB_TYPE int Model_GetComponentOutputElementSize(const char *stringid, int index);

// ���������� ������ ������ ���������� � ������ ���������
RDK_LIB_TYPE int Model_GetComponentOutputByteSize(const char *stringid, int index);

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
RDK_LIB_TYPE unsigned char* Model_GetComponentOutputData(const char *stringid, int index);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
RDK_LIB_TYPE const char * Model_SaveComponent(const char *stringid);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
RDK_LIB_TYPE int Model_LoadComponent(const char *stringid, char* buffer);

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
RDK_LIB_TYPE const char * Model_SaveComponentParameters(const char *stringid);

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
RDK_LIB_TYPE int Model_LoadComponentParameters(const char *stringid, char* buffer);

// ��������� ��������� ���������� � ��� �������� ��������� � xml
RDK_LIB_TYPE const char * Model_SaveComponentState(const char *stringid);

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
RDK_LIB_TYPE int Model_LoadComponentState(const char *stringid, char* buffer);
// --------------------------

// --------------------------
// ������ ���������� ������������
// ----------------------------
// ���������� ������ ����� ����
RDK_LIB_TYPE const char* RDKEngine_GetLog(void);
// ----------------------------


// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ����� ������ �����
RDK_LIB_TYPE void Env_SetNumInputImages(int number);

// ������ ����� ������� �����
RDK_LIB_TYPE void Env_SetNumOutputImages(int number);

// ���������� ����� ������ �����
RDK_LIB_TYPE int Env_GetNumInputImages(void);

// ���������� ����� ������� �����
RDK_LIB_TYPE int Env_GetNumOutputImages(void);

// ������ ���������� �� ��������� (������� ����������)
RDK_LIB_TYPE void Env_SetInputRes(int number, int width, int height);

RDK_LIB_TYPE void Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

// ���������� ���������� �� ��������� (������� ����������)
RDK_LIB_TYPE int Env_GetInputImageWidth(int number);
RDK_LIB_TYPE int Env_GetInputImageHeight(int number);
RDK_LIB_TYPE int Env_GetInputImageColorModel(int number);

// ���������� ������� �������� ����������
RDK_LIB_TYPE int Env_GetOutputImageWidth(int number);
RDK_LIB_TYPE int Env_GetOutputImageHeight(int number);
RDK_LIB_TYPE int Env_GetOutputImageColorModel(int number);

RDK_LIB_TYPE unsigned char* Env_GetInputImage(int index);

RDK_LIB_TYPE unsigned char* Env_GetOutputImage(int index);

RDK_LIB_TYPE unsigned char* Env_GetOutputImageY8(int index);

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
RDK_LIB_TYPE const RDK::UBitmap* const Env_GetComponentOutput(const char *stringid, int index);
// --------------------------

// ----------------------------
// ���������� ������ �������������
// ----------------------------
RDK_LIB_TYPE int LoadEngine(void *create_storage, void *create_environment, void *create_engine);

// ������������� ����������
RDK_LIB_TYPE int RDKInit(void);

// ��������������� ����������
RDK_LIB_TYPE int RDKUnInit(void);

// ��������� ����� ����������������� ���������
RDK_LIB_TYPE int RDKLoadPredefinedLibraries(void);

// ���������� ���������� ����������
// ������ ���� ������ � ���������� ����������� ����������������� ��
RDK_LIB_TYPE int RDKExceptionDispatcher(void *exception);

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
// ----------------------------


}

#endif
