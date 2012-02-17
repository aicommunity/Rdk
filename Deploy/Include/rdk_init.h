#ifndef RDK_INIT_H
#define RDK_INIT_H

#ifdef __cplusplus
extern "C"  {
#else
typedef int bool;
#endif

// ----------------------------
// ������� �������������
// ----------------------------
// �������������� ������ (������� ������ ���� ������� ������!)
RDK_LIB_TYPE int RDK_CALL EngineInit(int predefined_structure);

// �������������� ����������� ������ (������� ������ ���� ������� ������!)
RDK_LIB_TYPE int RDK_CALL GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx=false);
// ----------------------------

// --------------------------
// ������� ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
RDK_LIB_TYPE int RDK_CALL Storage_GetNumClasses(void);

// ���������� id ������� � ���������. ������ ������ ���� ��������
RDK_LIB_TYPE void RDK_CALL Storage_GetClassesList(int *buffer);

// ���������� ��� ������ �� ��� id.
RDK_LIB_TYPE const char * RDK_CALL Storage_GetClassName(int id);

// ���������� Id ������ �� ��� �����
RDK_LIB_TYPE int RDK_CALL Storage_GetClassId(const char *name);

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
RDK_LIB_TYPE bool RDK_CALL Storage_DelClass(int classid);

// ������� ��� ��������� ������� �� ���������
RDK_LIB_TYPE void RDK_CALL Storage_FreeObjectsStorage(void);

// ������� ��� ������� �� ���������
RDK_LIB_TYPE void RDK_CALL Storage_ClearObjectsStorage(void);

// ��������� ��������� ����� �������� � ���������
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjects(void);
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjectsById(int classid);
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjectsByName(const char* classname);

// ���������� �������� ������ �� ��� id � ������� xml
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassDescription(int classid);

// ������������� �������� ������ �� ��� id, �������� ��� �� ������� xml
RDK_LIB_TYPE bool RDK_CALL Storage_SetClassDescription(int classid, const char* description);

// ��������� �������� ���� ������� � xml
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveClassesDescription(void);

// ��������� �������� ���� ������� �� xml
RDK_LIB_TYPE bool RDK_CALL Storage_LoadClassesDescription(const char* xmltext);

// ��������� ����� �������� ���� ������� � xml
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveCommonClassesDescription(void);

// ��������� ����� �������� ���� ������� �� xml
RDK_LIB_TYPE bool RDK_CALL Storage_LoadCommonClassesDescription(const char* xmltext);
// --------------------------

// --------------------------
// ������� ���������� ������
// ----------------------------
// ��������� ���������� �� ����� dll-�����
RDK_LIB_TYPE int RDK_CALL Env_LoadStorageLibrary(const char *filename);

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool RDK_CALL Env_DelClassLibraryByIndex(int index);

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool RDK_CALL Env_DelClassLibraryByName(const char *name);

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool RDK_CALL Env_DelAllClassLibraries(void);

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
RDK_LIB_TYPE bool RDK_CALL Env_BuildStorage(void);

// ���������� ����� ���������
RDK_LIB_TYPE int RDK_CALL Env_GetNumClassLibraries(void);

// ���������� ��� ���������� �� �������
RDK_LIB_TYPE const char * RDK_CALL Env_GetClassLibraryName(int index);

// ���������� ������ ���������� �� �������
RDK_LIB_TYPE const char * RDK_CALL Env_GetClassLibraryVersion(int index);

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
// ���������� id ������ ������ � ���������
RDK_LIB_TYPE int RDK_CALL Env_CreateClass(const char* stringid);

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
RDK_LIB_TYPE int RDK_CALL Env_Calculate(const char* stringid);

// ***********************************************
// ��������������� ������� ���������� ������. ������ ����� �� ���������
// ***********************************************
// ������ �������������� �������� ������ ���������
RDK_LIB_TYPE int RDK_CALL Env_GetPredefinedStructure(void);
RDK_LIB_TYPE bool RDK_CALL Env_SetPredefinedStructure(int value);

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
RDK_LIB_TYPE bool RDK_CALL Env_IsStoragePresent(void);

// ���������� ��������� �������������
RDK_LIB_TYPE bool RDK_CALL Env_IsInit(void);

// ������� ������� �������������� ���������
RDK_LIB_TYPE bool RDK_CALL Env_IsStructured(void);

// ������������� �����
RDK_LIB_TYPE bool RDK_CALL Env_Init(void);

// ��������������� �����
RDK_LIB_TYPE bool RDK_CALL Env_UnInit(void);

// ��������� �������������� �������� ������ ���������
RDK_LIB_TYPE bool RDK_CALL Env_CreateStructure(void);

// ���������� ������� ������ ���������
RDK_LIB_TYPE bool RDK_CALL Env_DestroyStructure(void);

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
RDK_LIB_TYPE void RDK_CALL Env_Destroy(void);
// ***********************************************
// ----------------------------


// --------------------------
// ������� ���������� ������ ��������������
// --------------------------
// ������ ����� ������ �����
RDK_LIB_TYPE void RDK_CALL Env_SetNumInputImages(int number);

// ������ ����� ������� �����
RDK_LIB_TYPE void RDK_CALL Env_SetNumOutputImages(int number);

// ���������� ����� ������ �����
RDK_LIB_TYPE int RDK_CALL Env_GetNumInputImages(void);

// ���������� ����� ������� �����
RDK_LIB_TYPE int RDK_CALL Env_GetNumOutputImages(void);

// ������ ���������� �� ��������� (������� ����������)
RDK_LIB_TYPE void RDK_CALL Env_SetInputRes(int number, int width, int height);

// ������ ������ �����������
RDK_LIB_TYPE void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

// ������ ���� ��������� �������� ����������� ������ �������������� ���
RDK_LIB_TYPE void Env_SetReflectionXFlag(bool value);

// ���������� ���������� �� ��������� (������� ����������)
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageWidth(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageHeight(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageColorModel(int number);

// ���������� ������� �������� ����������
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageWidth(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageHeight(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageColorModel(int number);

// ���������� ������ ��������� �����������
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetInputImage(int index);
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetOutputImage(int index);
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetOutputImageY8(int index);
// --------------------------

// ----------------------------
// ������ ���������� �������
// ----------------------------
// ������� ������
RDK_LIB_TYPE int RDK_CALL Model_Destroy(void);

// ������� ����� ������ �� ����� ������ � ���������
// �������������� ������� ������������ ������
RDK_LIB_TYPE int RDK_CALL Model_Create(int classid);

// ������� ������
RDK_LIB_TYPE int RDK_CALL Model_Clear(void);

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'classid'
// ���� stringid - ������ ������, �� ��������� � ���� ������
RDK_LIB_TYPE int RDK_CALL Model_AddComponent(char* stringid, int classid);

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'id'
// ���� stringid - ������ ������, �� ������� �� ����� ������
RDK_LIB_TYPE int RDK_CALL Model_DelComponent(char* stringid, int id);

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
RDK_LIB_TYPE int RDK_CALL Model_GetNumComponents(const char* stringid);

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentsList(const char* stringid, int *buffer);

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentName(const char* stringid);

// ���������� ��������� ���������� �� ��������������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameters(const char *stringid);

// ���������� ��������� ���������� �� �������������� � ����������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid);

// ���������� ���������� ��������� ���������� �� ��������������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid);

// ���������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname);

// ������������� ��������� ���������� �� ��������������
RDK_LIB_TYPE bool RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer);

// ������������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
RDK_LIB_TYPE void RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer);

// ��������� ��������� ���������� ���� � ������
RDK_LIB_TYPE int RDK_CALL Model_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number);

// ��������� ��������� �����
RDK_LIB_TYPE int RDK_CALL Model_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number);

// ��������� ��� �����
RDK_LIB_TYPE int RDK_CALL Model_BreakAllLinks(void);

// ��������� ��� ������� � �������� ����� ���������� ����������
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentLinks(char* stringid);

// ��������� ��� ������� ����� ���������� ����������
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentInputLinks(char* stringid);

// ��������� ��� �������� ����� ���������� ����������
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentOutputLinks(char* stringid);

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentInternalLinks(char* stringid);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
RDK_LIB_TYPE int RDK_CALL Model_SetComponentInternalLinks(char* stringid, char* buffer);

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentInputLinks(char* stringid);

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentOutputLinks(char* stringid);

// ���������� ��������� ���������� �� ��������������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentState(const char *stringid);

// ���������� ���������� ������ ��������� ���������� �� ��������������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid);

// ���������� �������� ��������� ��������� ��������� �� �������������� ���������� � ����� ����������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename);

// ������������� ��������� ���������� �� ��������������
RDK_LIB_TYPE bool RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer);

// ������������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
RDK_LIB_TYPE void RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer);

// ���������� ����� ������ � ����������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentNumInputs(const char *stringid);

// ���������� ������ ����� ���������� � ����� ���������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputSize(const char *stringid, int index);

// ���������� ������ �������� ����� � ������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputElementSize(const char *stringid, int index);

// ���������� ������ ����� ���������� � ������ ���������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputByteSize(const char *stringid, int index);

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
RDK_LIB_TYPE unsigned char* RDK_CALL Model_GetComponentInputData(const char *stringid, int index);

// ���������� ����� ������� � ����������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentNumOutputs(const char *stringid);

// ���������� ������ ������ ���������� � ����� ���������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputSize(const char *stringid, int index);

// ���������� ������ �������� ������ � ������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputElementSize(const char *stringid, int index);

// ���������� ������ ������ ���������� � ������ ���������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputByteSize(const char *stringid, int index);

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
RDK_LIB_TYPE unsigned char* RDK_CALL Model_GetComponentOutputData(const char *stringid, int index);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponent(const char *stringid);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponent(const char *stringid, char* buffer);

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentParameters(const char *stringid);

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentParameters(const char *stringid, char* buffer);

// ��������� ��������� ���������� � ��� �������� ��������� � xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentState(const char *stringid);

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentState(const char *stringid, char* buffer);
// --------------------------

// --------------------------
// ������� ���������� ������� ��������������
// --------------------------
// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
// ������������ �������� ����� ����������� ��� RDK::UBitmap*
RDK_LIB_TYPE const /* RDK::UBitmap* */void* const RDK_CALL Model_GetComponentOutput(const char *stringid, int index);
// --------------------------

// --------------------------
// ������� ���������� ������������
// ----------------------------
// ���������� ������ ����� ����
RDK_LIB_TYPE const char* RDK_CALL Engine_GetLog(void);
// ----------------------------

// ----------------------------
// ���������� ������� �������������
// ----------------------------
RDK_LIB_TYPE int RDK_CALL LoadEngine(void *create_storage, void *create_environment, void *create_engine);

// ������������� ����������
RDK_LIB_TYPE int RDK_CALL Init(void);

// ��������������� ����������
RDK_LIB_TYPE int RDK_CALL UnInit(void);

// ��������� ����� ����������������� ���������
RDK_LIB_TYPE int RDK_CALL LoadPredefinedLibraries(void);

// ���������� ���������� ����������
// ������ ���� ������ � ���������� ����������� ����������������� ��
RDK_LIB_TYPE int RDK_CALL ExceptionDispatcher(void *exception);

// ������������� dll
RDK_LIB_TYPE bool RDK_CALL DllInit(void* pfstorage,void* pfenvironment,void* pfengine);

// ���������� ����� �������� � ����������
RDK_LIB_TYPE int RDK_CALL GetNumStorages(void);

// ���������� ����� ���� � ����������
RDK_LIB_TYPE int RDK_CALL GetNumEnvironments(void);

// ���������� ����� ������� � ����������
RDK_LIB_TYPE int RDK_CALL GetNumEngines(void);
// ----------------------------

#ifdef __cplusplus
}
#endif

#endif
