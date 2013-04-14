#ifndef RDK_INIT_H
#define RDK_INIT_H

#ifdef __cplusplus
extern "C"  {
#else
typedef int bool;
#endif

#ifndef RDK_PROPERTY_TYPES
#define RDK_PROPERTY_TYPES
// �������� ���� �������� (������� �����) pt - Property Type
// 0x1 - ��������
// 0x2 - ���������� ���������
// 0x4 - ��������� ����������
// 0x8 - ����
enum {ptParameter=1, ptState=2, ptTemp=4, ptInput=8, ptOutput=16, ptAny=255};

// �������� ����� �������� (������� �����) pg - Property Group
// 0x100 - �������������
// 0x200 - ���������
// 0x400 - ������� ������
// 0x800 - �������� ������
// 0x1000 - ���� ����� ������ ������ ����������
enum {pgPublic=0x100, pgSystem=0x200, pgInput=0x400, pgOutput=0x800, pgMode=0x1000, pgAny=0xFFFFFF};

// �������� ����� ������������ ��������� ���� � ������
enum {ptPubParameter=ptParameter|pgPublic, ptPubState=ptState|pgPublic, ptPubInput=ptInput|pgPublic, ptPubOutput=ptOutput|pgPublic};
#endif

// ----------------------------
// ������� �������������
// ----------------------------
// �������������� ������ (������� ������ ���� ������� ������!)
RDK_LIB_TYPE int RDK_CALL EngineInit(int predefined_structure, void* exception_handler=0);

// �������������� ����������� ������ (������� ������ ���� ������� ������!)
RDK_LIB_TYPE int RDK_CALL GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx=false,
		void* exception_handler=0);

// ���������������� ������ (������� ������������� ���������� ��� ������ �������������)
RDK_LIB_TYPE int RDK_CALL EngineUnInit(void);
// ----------------------------

// --------------------------
// ������� ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
RDK_LIB_TYPE int RDK_CALL Storage_GetNumClasses(void);

// ���������� id ������� � ���������. ������ ������ ���� ��������
RDK_LIB_TYPE void RDK_CALL Storage_GetClassesList(int *buffer);

// ���������� ����� ������� � ��������� � ���� ������ ����������� ��������
RDK_LIB_TYPE const char * RDK_CALL Storage_GetClassesNameList(void);

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
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassDescription(const char* classname);

// ������������� �������� ������ �� ��� id, �������� ��� �� ������� xml
RDK_LIB_TYPE bool RDK_CALL Storage_SetClassDescription(const char* classname, const char* description);

// ��������� �������� ���� ������� � xml
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveClassesDescription(void);

// ��������� �������� ���� ������� �� xml
RDK_LIB_TYPE bool RDK_CALL Storage_LoadClassesDescription(const char* xmltext);

// ��������� ����� �������� ���� ������� � xml
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveCommonClassesDescription(void);

// ��������� ����� �������� ���� ������� �� xml
RDK_LIB_TYPE bool RDK_CALL Storage_LoadCommonClassesDescription(const char* xmltext);

// ��������� �������� ���� ������� � xml ������� ����� ��������
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveAllClassesDescription(void);

// ��������� �������� ���� ������� �� xml ������� ����� ��������
RDK_LIB_TYPE bool RDK_CALL Storage_LoadAllClassesDescription(const char* xmltext);
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
RDK_LIB_TYPE int RDK_CALL Env_CreateClass(const char* stringid, const char *classname);

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
RDK_LIB_TYPE int RDK_CALL Env_Calculate(const char* stringid);

// ������ ���� ������ � �������� �������
RDK_LIB_TYPE void RDK_CALL Env_RTCalculate(void);

// ����� ������ �����
// ���� stringid == 0 �� ���������� ��� ������ �������,
// ����� - ������ ��������� ��������� ������
RDK_LIB_TYPE int RDK_CALL Env_Reset(const char* stringid);

// ���������� ���������� ������� ������ �� ��������� ��������
RDK_LIB_TYPE void RDK_CALL Env_IncreaseModelTimeByStep(void);


// ***********************************************
// ������ ���������� ������� �����������
// !!! ��������� ������ ������ �� ���
// ������, ������������ � ����������� �� ���������� id !!!
// ***********************************************
// ������������� ������� ��������� (��������� ������������ ����� - ������)
RDK_LIB_TYPE int RDK_CALL Env_SelectCurrentComponent(const char *stringid);

// ���������� ������� ��������� � ��������� �� ��������� (������)
RDK_LIB_TYPE int RDK_CALL Env_ResetCurrentComponent(const char *stringid);

// ������ ������� ��������� �� ��� �������� (������ �� ������� �����)
// ���� ��� �� ������� ������, �� �� ������ ������
RDK_LIB_TYPE int RDK_CALL Env_UpCurrentComponent(void);

// ������ ������� ��������� �� ��� �������� �� ������������ ������ �����������
// (����� �� N ������� ���� ������������ �������� ����������)
RDK_LIB_TYPE int RDK_CALL Env_DownCurrentComponent(const char *stringid);

// ���������� ������� ��� �������� ����������
RDK_LIB_TYPE const char* RDK_CALL Env_GetCurrentComponentName(void);

// ���������� ������� ��������� id �������� ����������
RDK_LIB_TYPE const char* RDK_CALL Env_GetCurrentComponentId(void);
// ***********************************************

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
RDK_LIB_TYPE int RDK_CALL Model_Create(const char *classname);

// ������� ������
RDK_LIB_TYPE int RDK_CALL Model_Clear(void);

// ���������, ���������� �� ������
RDK_LIB_TYPE bool RDK_CALL Model_Check(void);

// ���������, ���������� �� � ������ ��������� � ������ stringid)
RDK_LIB_TYPE bool RDK_CALL Model_CheckComponent(const char* stringid);

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'classname'
// ���� stringid - ������ ������, �� ��������� � ���� ������
// ���������� ��� ���������� � ������ ������
RDK_LIB_TYPE const char* RDK_CALL Model_AddComponent(const char* stringid, const char *classname);

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'name'
// ���� stringid - ������ ������, �� ������� �� ����� ������
RDK_LIB_TYPE int RDK_CALL Model_DelComponent(const char* stringid, const char *name);

// ���������� ����� ���� ��������� � �������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
RDK_LIB_TYPE int RDK_CALL Model_GetNumComponents(const char* stringid);

// ���������� ������ id ���� ��������� ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentsList(const char* stringid, int *buffer);

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ','
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentsNameList(const char* stringid);

// ���������� xml-������ ������� ��������������� ���� ����������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ���������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ����������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ����������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
RDK_LIB_TYPE const char* RDK_CALL Model_GetConnectorsList(const char* stringid,
						  int sublevel=-1, const char* owner_level_stringid=0);

// ���������� xml-������ ������� ��������������� ���� ��������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// �������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ��������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
RDK_LIB_TYPE const char* RDK_CALL Model_GetItemsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// ���������� xml-������ ������� ��������������� ���� �������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� �������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
RDK_LIB_TYPE const char* RDK_CALL Model_GetNetsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// ���������� �������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentName(const char* stringid);

// ���������� ������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid=0);

// ���������� ������� id ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid=0);

// ���������� ��� ������ ���������� � ��������� �� �������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������ ������
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentClassName(const char* stringid);

// ���������� �������� ���������� �� ��������������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentProperties(const char *stringid, unsigned int type_mask);

// ���������� �������� ���������� �� �������������� � ����������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask);

// ���������� ���������� �������� ���������� �� ��������������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedProperties(const char *stringid, unsigned int type_mask);

// ���������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentPropertyValue(const char *stringid, const char *paramname);

// ������������� �������� ���������� �� ��������������
RDK_LIB_TYPE int RDK_CALL Model_SetComponentProperties(const char *stringid, const char* buffer);

// ������������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
RDK_LIB_TYPE void RDK_CALL Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer);

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
RDK_LIB_TYPE void RDK_CALL Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer);

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
RDK_LIB_TYPE void RDK_CALL Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer);

// ���������� ��������� void* �� ������ �������� ����������
RDK_LIB_TYPE const void* RDK_CALL Model_GetComponentPropertyData(const char *stringid, const char *property_name);

// �������� ������ 'data' � �������� �������� ����������
RDK_LIB_TYPE int RDK_CALL Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data);

// ���������� ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameters(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// ���������� ��������� ���������� �� �������������� � ����������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// ���������� ���������� ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid);

// ���������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname);

// ������������� ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer);

// ������������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
// Deprecated
RDK_LIB_TYPE void RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer);

// ���������� ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentState(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// ���������� ���������� ������ ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid);

// ���������� �������� ��������� ��������� ��������� �� �������������� ���������� � ����� ����������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename);

// ������������� ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer);

// ������������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
// Deprecated
RDK_LIB_TYPE void RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer);

// ��������� ��������� ���������� ���� � ������
RDK_LIB_TYPE int RDK_CALL Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number);

// ��������� ��� ���������� ���������� ���������� �� ����������� id � �������: 0 ����� � 0 �����
RDK_LIB_TYPE int RDK_CALL Model_ChainLinking(const char* stringid);

// ��������� ��� ���������� ���������� ���������� �����������, ��������� �� � ������������ ����� ������� ������
// ������������ ��� ������������ ������������������
RDK_LIB_TYPE int RDK_CALL Model_ParallelLinking(const char* stringid);

// ��������� ��������� �����
RDK_LIB_TYPE int RDK_CALL Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number);

// ��������� ��� �����
RDK_LIB_TYPE int RDK_CALL Model_BreakAllLinks(void);

// ��������� ��� ������� � �������� ����� ���������� ����������
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentLinks(const char* stringid);

// ��������� ��� ������� ����� ���������� ����������
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentInputLinks(const char* stringid);

// ��������� ��� �������� ����� ���������� ����������
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentOutputLinks(const char* stringid);

// ���������, ���������� �� ������� �����
RDK_LIB_TYPE bool RDK_CALL Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number);

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid=0);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� � ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����������� ������� �������� ����������
RDK_LIB_TYPE int RDK_CALL Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid=0);

// ���������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid=0, int sublevel=-1);

// ���������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid=0, int sublevel=-1);

// ��������� ��� ������� ����� c ����������� cont � ��� ��������� ������������ � ���� xml � ����� buffer
// ���������� � ������ ����������� ������������ ��������� ���������� cont!
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid=0);

// ���������� ����� ������ � ����������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentNumInputs(const char *stringid);

// ���������� ������ ����� ���������� � ����� ���������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputDataSize(const char *stringid, int index);

// ���������� ������ �������� ����� � ������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputElementSize(const char *stringid, int index);

// ���������� ������ ����� ���������� � ������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputByteSize(const char *stringid, int index);

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
RDK_LIB_TYPE unsigned char* RDK_CALL Model_GetComponentInputData(const char *stringid, int index);

// ���������� ����� ������� � ����������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentNumOutputs(const char *stringid);

// ���������� ������ ������ ���������� � ����� ���������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputDataSize(const char *stringid, int index);

// ���������� ������ �������� ������ � ������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputElementSize(const char *stringid, int index);

// ���������� ������ ������ ���������� � ������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputByteSize(const char *stringid, int index);

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
RDK_LIB_TYPE unsigned char* RDK_CALL Model_GetComponentOutputData(const char *stringid, int index);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponent(const char *stringid, unsigned int params_type_mask=ptParameter | pgPublic);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponent(const char *stringid, char* buffer);

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentProperties(const char *stringid, unsigned int type_mask);

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentProperties(const char *stringid, char* buffer);

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentParameters(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentParameters(const char *stringid, char* buffer);

// ��������� ��������� ���������� � ��� �������� ��������� � xml
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentState(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentState(const char *stringid, char* buffer);

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
RDK_LIB_TYPE const char* RDK_CALL Model_SaveComponentDrawInfo(const char *stringid);

// ��������� ����� ����� ������ �� ���������
RDK_LIB_TYPE int RDK_CALL Model_GetDefaultTimeStep(void);
RDK_LIB_TYPE void RDK_CALL Model_SetDefaultTimeStep(int value);

// ��������� ����� ����� ����������
RDK_LIB_TYPE int RDK_CALL Model_GetTimeStep(const char *stringid);
RDK_LIB_TYPE void RDK_CALL Model_SetTimeStep(const char *stringid, int value);

// ������������� ��� ����� ���������� � ���� ��� �������� ���������
RDK_LIB_TYPE void RDK_CALL Model_SetGlobalTimeStep(const char *stringid, int value);

// ���������� ������� ����� ������
RDK_LIB_TYPE long long RDK_CALL Model_GetTime(void);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleTime(void);

// ������������� ������� ����� ������
RDK_LIB_TYPE bool RDK_CALL Model_SetTime(long long value);

// ���������� �������� �����
RDK_LIB_TYPE long long RDK_CALL Model_GetRealTime(void);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleRealTime(void);

// ������������� �������� �����
RDK_LIB_TYPE bool RDK_CALL Model_SetRealTime(long long value);

// ����������� �������� ����� �� �������� ��������
RDK_LIB_TYPE bool RDK_CALL Model_IncreaseRealTime(long long value);

// ���������� ���������� ��� � �������� �������
RDK_LIB_TYPE long long RDK_CALL Model_GetRealTimeStep(void);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleRealTimeStep(void);

// ���������� ����� ������� ���������� ��� ������� ������� �������� ��������� (��)
RDK_LIB_TYPE long long RDK_CALL Model_GetStepDuration(const char *stringid);

// ���������� �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ���������) (��)
RDK_LIB_TYPE long long RDK_CALL Model_GetFullStepDuration(const char *stringid);

// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
RDK_LIB_TYPE double RDK_CALL Model_GetInstantPerformance(const char *stringid);

// �����, ��������� ����� ����� ���������� ���������� �����
RDK_LIB_TYPE long long RDK_CALL Model_GetInterstepsInterval(const char *stringid);
// --------------------------

// --------------------------
// ������� ���������� ������� ��������������
// --------------------------
// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
// ������������ �������� ����� ����������� ��� RDK::MDMatrix*
// ���� ����� �� �������� ������ ������ ����, �� ���������� 0
RDK_LIB_TYPE const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrix(const char *stringid, int index);

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
// ������������ �������� ����� ����������� ��� RDK::UBitmap*
RDK_LIB_TYPE const /* RDK::UBitmap* */void* RDK_CALL Model_GetComponentOutput(const char *stringid, int index);

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutput(const char *stringid, int index);

// ���������� ��������� �� ���� � �������� 'index' ���������� 'id'
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInput(const char *stringid, int index);

// �������� ����������� ������ � �������� 'index' ���������� 'id'
RDK_LIB_TYPE void RDK_CALL Model_SetComponentBitmapOutput(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);

// �������� ����������� ����� � �������� 'index' ���������� 'id'
RDK_LIB_TYPE void RDK_CALL Model_SetComponentBitmapInput(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
// --------------------------

// --------------------------
// ������� ���������� ������������
// ----------------------------
// ���������� ��������-������������ ����������
RDK_LIB_TYPE void* RDK_CALL Engine_GetExceptionHandler(void);
RDK_LIB_TYPE bool RDK_CALL Engine_SetExceptionHandler(void* value);

// ���������� ������ ����� ����
RDK_LIB_TYPE const char* RDK_CALL Engine_GetLog(void);

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
RDK_LIB_TYPE const char* RDK_CALL Engine_GetUnreadLog(void);
// ----------------------------

#ifdef __cplusplus
}
#endif

#endif
