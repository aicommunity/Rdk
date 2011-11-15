#ifndef RDK_ENGINE_H
#define RDK_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------
// ������ �������������
// ----------------------------
RDK_LIB_TYPE int LoadEngine(void *create_storage, void *create_environment, void *create_engine);

// �������������� ������ (������� ������ ���� ������� ������!)
RDK_LIB_TYPE int RDK_EngineInit(int predefined_structure);

// ������������� ����������
RDK_LIB_TYPE int RDKInit(void);

// ��������������� ����������
RDK_LIB_TYPE int RDKUnInit(void);

// ��������� ����� ����������������� ���������
RDK_LIB_TYPE int RDKLoadPredefinedLibraries(void);
// ----------------------------

// --------------------------
// ������ ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
RDK_LIB_TYPE int RDKStorage_GetNumClasses(void);

// ���������� id ������� � ���������. ������ ������ ���� ��������
RDK_LIB_TYPE void RDKStorage_GetClassesList(int *buffer);

// ���������� ��� ������ �� ��� id.
RDK_LIB_TYPE const char * RDKStorage_GetClassName(int id);

// ���������� Id ������ �� ��� �����
RDK_LIB_TYPE int RDKStorage_GetClassId(const char *name);

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
RDK_LIB_TYPE bool RDKStorage_DelClass(int classid);

// ������� ��� ��������� ������� �� ���������
RDK_LIB_TYPE void RDKStorage_FreeObjectsStorage(void);

// ������� ��� ������� �� ���������
RDK_LIB_TYPE void RDKStorage_ClearObjectsStorage(void);

// ��������� ��������� ����� �������� � ���������
RDK_LIB_TYPE int RDKStorage_CalcNumObjects(void);
RDK_LIB_TYPE int RDKStorage_CalcNumObjectsById(int classid);
RDK_LIB_TYPE int RDKStorage_CalcNumObjectsByName(const char* classname);
// --------------------------

// --------------------------
// ������ ���������� ������
// ----------------------------
// ������ �������������� �������� ������ ���������
RDK_LIB_TYPE int RDKEnv_GetPredefinedStructure(void);
RDK_LIB_TYPE bool RDKEnv_SetPredefinedStructure(int value);

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
RDK_LIB_TYPE bool RDKEnv_IsStoragePresent(void);

// ���������� ��������� �������������
RDK_LIB_TYPE bool RDKEnv_IsInit(void);

// ������� ������� �������������� ���������
RDK_LIB_TYPE bool RDKEnv_IsStructured(void);

// ������������� �����
RDK_LIB_TYPE bool RDKEnv_Init(void);

// ��������������� �����
RDK_LIB_TYPE bool RDKEnv_UnInit(void);

// ��������� �������������� �������� ������ ���������
RDK_LIB_TYPE bool RDKEnv_CreateStructure(void);

// ���������� ������� ������ ���������
RDK_LIB_TYPE bool RDKEnv_DestroyStructure(void);

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
RDK_LIB_TYPE void RDKEnv_Destroy(void);

// ��������� ���������� �� ����� dll-�����
RDK_LIB_TYPE int RDKEnv_LoadStorageLibrary(const char *filename);

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool RDKEnv_DelClassLibraryByIndex(int index);

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool RDKEnv_DelClassLibraryByName(const char *name);

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool RDKEnv_DelAllClassLibraries(void);

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
RDK_LIB_TYPE bool RDKEnv_BuildStorage(void);

// ���������� ����� ���������
RDK_LIB_TYPE int RDKEnv_GetNumClassLibraries(void);

// ���������� ��� ���������� �� �������
RDK_LIB_TYPE const char * RDKEnv_GetClassLibraryName(int index);

// ���������� ������ ���������� �� �������
RDK_LIB_TYPE const char * RDKEnv_GetClassLibraryVersion(int index);

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
// ���������� id ������ ������ � ���������
RDK_LIB_TYPE int RDKEnv_CreateClass(const char* stringid);

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
RDK_LIB_TYPE int RDKEnv_Calculate(const char* stringid=0);
// ----------------------------

// ----------------------------
// ������ ���������� �������
// ----------------------------
// ������� ������
RDK_LIB_TYPE int RDKModel_Destroy(void);

// ������� ����� ������ �� ����� ������ � ���������
// �������������� ������� ������������ ������
RDK_LIB_TYPE int RDKModel_Create(int classid);

// ������� ������
RDK_LIB_TYPE int RDKModel_Clear(void);

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'classid'
// ���� stringid - ������ ������, �� ��������� � ���� ������
RDK_LIB_TYPE int RDKModel_AddComponent(char* stringid, int classid);

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ��������� ���������� � �������� 'id'
// ���� stringid - ������ ������, �� ������� �� ����� ������
RDK_LIB_TYPE int RDKModel_DelComponent(char* stringid, int id);

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
RDK_LIB_TYPE int RDKModel_GetNumComponents(char* stringid);

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
RDK_LIB_TYPE int RDKModel_GetComponentsList(char* stringid, int *buffer);

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
RDK_LIB_TYPE const char* RDKModel_GetComponentName(char* stringid);

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
RDK_LIB_TYPE const char * RDKModel_GetComponentParameters(const char *stringid);

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
RDK_LIB_TYPE const char * RDKModel_GetComponentSelectedParameters(const char *stringid);

// ������������� ��������� ���������� �� ��������������
RDK_LIB_TYPE bool RDKModel_SetComponentParameters(const char *stringid, const char* buffer);

// ��������� ��������� ���������� ���� � ������
RDK_LIB_TYPE int RDKModel_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number);

// ��������� ��������� �����
RDK_LIB_TYPE int RDKModel_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number);

// ��������� ��� �����
RDK_LIB_TYPE int RDKModel_BreakAllLinks(void);

// ��������� ��� ������� � �������� ����� ���������� ����������
RDK_LIB_TYPE int RDKModel_BreakAllComponentLinks(char* stringid);

// ��������� ��� ������� ����� ���������� ����������
RDK_LIB_TYPE int RDKModel_BreakAllComponentInputLinks(char* stringid);

// ��������� ��� �������� ����� ���������� ����������
RDK_LIB_TYPE int RDKModel_BreakAllComponentOutputLinks(char* stringid);

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
RDK_LIB_TYPE const char * RDKModel_GetComponentInternalLinks(char* stringid);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
RDK_LIB_TYPE int RDKModel_SetComponentInternalLinks(char* stringid, char* buffer);

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
RDK_LIB_TYPE const char * RDKModel_GetComponentInputLinks(char* stringid);

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
RDK_LIB_TYPE const char * RDKModel_GetComponentOutputLinks(char* stringid);

// ���������� ��������� ���������� �� ��������������
RDK_LIB_TYPE const char * RDKModel_GetComponentState(const char *stringid);

// ���������� ���������� ������ ��������� ���������� �� ��������������
RDK_LIB_TYPE const char * RDKModel_GetComponentSelectedState(const char *stringid);

// ������������� ��������� ���������� �� ��������������
RDK_LIB_TYPE bool RDKModel_SetComponentState(const char *stringid, const char* buffer);

// ���������� ����� ������ � ����������
RDK_LIB_TYPE int RDKModel_GetComponentNumInputs(const char *stringid);

// ���������� ������ ����� ���������� � ����� ���������
RDK_LIB_TYPE int RDKModel_GetComponentInputSize(const char *stringid, int index);

// ���������� ������ �������� ����� � ������
RDK_LIB_TYPE int RDKModel_GetComponentInputElementSize(const char *stringid, int index);

// ���������� ������ ����� ���������� � ������ ���������
RDK_LIB_TYPE int RDKModel_GetComponentInputByteSize(const char *stringid, int index);

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
RDK_LIB_TYPE unsigned char* RDKModel_GetComponentInputData(const char *stringid, int index);

// ���������� ����� ������� � ����������
RDK_LIB_TYPE int RDKModel_GetComponentNumOutputs(const char *stringid);

// ���������� ������ ������ ���������� � ����� ���������
RDK_LIB_TYPE int RDKModel_GetComponentOutputSize(const char *stringid, int index);

// ���������� ������ �������� ������ � ������
RDK_LIB_TYPE int RDKModel_GetComponentOutputElementSize(const char *stringid, int index);

// ���������� ������ ������ ���������� � ������ ���������
RDK_LIB_TYPE int RDKModel_GetComponentOutputByteSize(const char *stringid, int index);

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
RDK_LIB_TYPE unsigned char* RDKModel_GetComponentOutputData(const char *stringid, int index);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
RDK_LIB_TYPE const char * RDKModel_SaveComponent(const char *stringid);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
RDK_LIB_TYPE int RDKModel_LoadComponent(const char *stringid, char* buffer);

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
RDK_LIB_TYPE const char * RDKModel_SaveComponentParameters(const char *stringid);

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
RDK_LIB_TYPE int RDKModel_LoadComponentParameters(const char *stringid, char* buffer);

// ��������� ��������� ���������� � ��� �������� ��������� � xml
RDK_LIB_TYPE const char * RDKModel_SaveComponentState(const char *stringid);

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
RDK_LIB_TYPE int RDKModel_LoadComponentState(const char *stringid, char* buffer);
// --------------------------


#ifdef __cplusplus
}
#endif

#endif
