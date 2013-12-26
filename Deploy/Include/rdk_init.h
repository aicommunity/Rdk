#ifndef RDK_INIT_H
#define RDK_INIT_H

#ifdef __cplusplus
extern "C"  {
#else
typedef int bool;
#endif

#ifndef RDK_EX_UNKNOWN
#define RDK_EX_UNKNOWN 0
#define RDK_EX_FATAL 1
#define RDK_EX_ERROR 2
#define RDK_EX_WARNING 3
#define RDK_EX_INFO 4
#define RDK_EX_DEBUG 5
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

enum { ipData=1, ipComp=2 };
enum { ipSingle=16, ipRange=32, ipList=64 };
enum { ipDataSingle=ipData|ipSingle, ipDataRange=ipData|ipRange,
       ipDataList=ipData|ipList, ipCompSingle=ipComp|ipSingle,
       ipCompRange=ipComp|ipRange, ipCompList=ipComp|ipList };
#endif

// ----------------------------
// ������� RPC
// � ������� ���� ������� �������� ������� ����� ������� ����������,
// ������������ ��������� ������
// ----------------------------
/// ��������� ������ � ���������� xml-�������� ������
/// request - xml �������� �������
/// return_value - ������������ �������� ��� ��� �������, ������� ��� �����
/// ��� ��������� ���������� 0
/// ������ request ����� ��������� ��� (����� ����� ����� ������������ � �����������
/// �� ���������� �������):
/// <RpcRequest>
///	    <Id>���������� ������������� �������</Id>
///     <Channel>������ ������</Channel>
///     <Cmd>��� ���������� �������</Cmd>
///     <Component>��� ����������</Component>
///     <Class>��� ������</Class>
///     <Data>xml-�������� ������ �������, �������� xml � ����������� ����������</Data>
/// </RpcRequest>
/// ����� ������������ ����� ������ � ��������� ����:
/// <RpcResponse>
///	    <Id>���������� ������������� �������</Id>
///     <Data>xml-�������� ������ �������, �������� xml � ����������� ����������</Data>
///     <Res>������������� ������������ ������ ��� 0 ���� ����� �������</Res>
/// </RpcResponse>
RDK_LIB_TYPE const char* RDK_CALL RemoteCall(const char *request, int &return_value);
// ----------------------------

// ----------------------------
// ������� �������������
// ----------------------------
// ���������� ��� �������� �������� ������
RDK_LIB_TYPE const char* RDK_CALL GetSystemDir(void);

// ������������� ��� �������� �������� ������
RDK_LIB_TYPE int RDK_CALL SetSystemDir(const char *dir);

// ��������� ���������� ������
RDK_LIB_TYPE int RDK_CALL Engine_LoadFonts(void);

// ���������� ����� �������
RDK_LIB_TYPE int RDK_CALL GetNumEngines(void);

// ������� ��������� ����� �������
// num > 0
RDK_LIB_TYPE int RDK_CALL SetNumEngines(int num);

// ������� ������ �� �������
RDK_LIB_TYPE int RDK_CALL DelEngine(int index);

// ���������� ������ �������� ���������� ������
RDK_LIB_TYPE int RDK_CALL GetSelectedEngineIndex(void);

// ����������� ������� ��������� �� ������ � �������� �������
// � ������ �������� ������, ��������� ������������� ����������������� �� 0 ������
RDK_LIB_TYPE int RDK_CALL SelectEngine(int index);

// �������������� ������ (������� ������ ���� ������� ������!)
// Upd: ������� ����� ���� ������� ����� SetNumEngines � SelectEngine
RDK_LIB_TYPE int RDK_CALL EngineInit(int predefined_structure, void* exception_handler=0);
RDK_LIB_TYPE int RDK_CALL MEngineInit(int engine_index, int predefined_structure, void* exception_handler=0);

// �������������� ����������� ������ (������� ������ ���� ������� ������!)
// Upd: ������� ����� ���� ������� ����� SetNumEngines � SelectEngine
RDK_LIB_TYPE int RDK_CALL GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx=false,
		void* exception_handler=0);
RDK_LIB_TYPE int RDK_CALL MGraphicalEngineInit(int engine_index, int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx=false,
		void* exception_handler=0);

// ���������������� ������ (������� ������������� ���������� ��� ������ �������������)
RDK_LIB_TYPE int RDK_CALL EngineUnInit(void);
RDK_LIB_TYPE int RDK_CALL MEngineUnInit(int engine_index);

/// ��������� ��������������� �� ������
RDK_LIB_TYPE bool RDK_CALL IsEngineInit(void);
RDK_LIB_TYPE bool RDK_CALL MIsEngineInit(int engine_index);
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

// ��������� ���������� �� ����� dll-�����
RDK_LIB_TYPE int RDK_CALL Storage_LoadStorageLibrary(const char *filename);

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool RDK_CALL Storage_DelClassLibraryByIndex(int index);

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool RDK_CALL Storage_DelClassLibraryByName(const char *name);

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
RDK_LIB_TYPE bool RDK_CALL Storage_DelAllClassLibraries(void);

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
RDK_LIB_TYPE bool RDK_CALL Storage_BuildStorage(void);

// ���������� ����� ���������
RDK_LIB_TYPE int RDK_CALL Storage_GetNumClassLibraries(void);

// ���������� ������ ��������� � ���� ������, ����������� ��������
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassLibrariesList(void);

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// library_name - ��� ����������
RDK_LIB_TYPE const char* RDK_CALL Storage_GetLibraryClassNames(const char *library_name);

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// index - ������ ����������
RDK_LIB_TYPE const char* RDK_CALL Storage_GetLibraryClassNamesByIndex(int index);

// ���������� ��� ���������� �� �������
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassLibraryNameByIndex(int index);

// ���������� ������ ���������� �� �������
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassLibraryVersionByIndex(int index);

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
// ���������� id ������ ������ � ���������
RDK_LIB_TYPE int RDK_CALL Storage_CreateClass(const char* stringid, const char *classname);
// --------------------------

// --------------------------
// ������� ���������� ������
// ----------------------------
// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
RDK_LIB_TYPE int RDK_CALL Env_Calculate(const char* stringid);
RDK_LIB_TYPE int RDK_CALL MEnv_Calculate(int engine_index, const char* stringid);

// ������ ���� ������ � �������� �������
RDK_LIB_TYPE void RDK_CALL Env_RTCalculate(void);
RDK_LIB_TYPE void RDK_CALL MEnv_RTCalculate(int engine_index);

// ����� ������ �����
// ���� stringid == 0 �� ���������� ��� ������ �������,
// ����� - ������ ��������� ��������� ������
RDK_LIB_TYPE int RDK_CALL Env_Reset(const char* stringid);
RDK_LIB_TYPE int RDK_CALL MEnv_Reset(int engine_index, const char* stringid);

/// ����� ������ ���������� �� �������� �� ���������
/// ���� stringid == 0 �� ���������� ��� ������ �������,
/// ����� - ������ ��������� ��������� ������
/// ���� subcomps == true �� ����� ���������� ��������� ���� �������� ���������
RDK_LIB_TYPE int RDK_CALL Env_Default(const char* stringid, bool subcomps=false);
RDK_LIB_TYPE int RDK_CALL MEnv_Default(int engine_index, const char* stringid, bool subcomps=false);

// ���������� ���������� ������� ������ �� ��������� ��������
RDK_LIB_TYPE void RDK_CALL Env_IncreaseModelTimeByStep(void);

/// ������������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
RDK_LIB_TYPE int RDK_CALL Env_SetMinInterstepsInterval(long long value);
RDK_LIB_TYPE int RDK_CALL MEnv_SetMinInterstepsInterval(int engine_index, long long value);

/// ���������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
RDK_LIB_TYPE long long RDK_CALL Env_GetMinInterstepsInterval(void);
RDK_LIB_TYPE long long RDK_CALL MEnv_GetMinInterstepsInterval(int engine_index);

// ���������� ��� �������� �������� ��� �������� ������
RDK_LIB_TYPE const char* RDK_CALL Env_GetCurrentDataDir(void);

// ������������� ��� �������� �������� ��� �������� ������
RDK_LIB_TYPE int RDK_CALL Env_SetCurrentDataDir(const char *dir);

/// ���������� ��������� ����� ����������� ������ �����
RDK_LIB_TYPE bool RDK_CALL Env_GetDebugMode(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_GetDebugMode(int engine_index);

/// ������������� ��������� ����� ����������� ������ �����
RDK_LIB_TYPE int RDK_CALL Env_SetDebugMode(bool value);
RDK_LIB_TYPE int RDK_CALL MEnv_SetDebugMode(int engine_index, bool value);

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
RDK_LIB_TYPE int RDK_CALL MEnv_GetPredefinedStructure(int engine_index);
RDK_LIB_TYPE bool RDK_CALL Env_SetPredefinedStructure(int value);
RDK_LIB_TYPE bool RDK_CALL MEnv_SetPredefinedStructure(int engine_index, int value);

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
RDK_LIB_TYPE bool RDK_CALL Env_IsStoragePresent(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_IsStoragePresent(int engine_index);

// ���������� ��������� �������������
RDK_LIB_TYPE bool RDK_CALL Env_IsInit(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_IsInit(int engine_index);

// ������� ������� �������������� ���������
RDK_LIB_TYPE bool RDK_CALL Env_IsStructured(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_IsStructured(int engine_index);

// ������������� �����
RDK_LIB_TYPE bool RDK_CALL Env_Init(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_Init(int engine_index);

// ��������������� �����
RDK_LIB_TYPE bool RDK_CALL Env_UnInit(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_UnInit(int engine_index);

// ��������� �������������� �������� ������ ���������
RDK_LIB_TYPE bool RDK_CALL Env_CreateStructure(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_CreateStructure(int engine_index);

// ���������� ������� ������ ���������
RDK_LIB_TYPE bool RDK_CALL Env_DestroyStructure(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_DestroyStructure(int engine_index);

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
RDK_LIB_TYPE void RDK_CALL Env_Destroy(void);
RDK_LIB_TYPE void RDK_CALL MEnv_Destroy(int engine_index);
// ***********************************************
// ----------------------------


// --------------------------
// ������� ���������� ������ ��������������
// --------------------------
// ������ ����� ������ �����
RDK_LIB_TYPE void RDK_CALL Env_SetNumInputImages(int number);
RDK_LIB_TYPE void RDK_CALL MEnv_SetNumInputImages(int engine_index, int number);

// ������ ����� ������� �����
RDK_LIB_TYPE void RDK_CALL Env_SetNumOutputImages(int number);
RDK_LIB_TYPE void RDK_CALL MEnv_SetNumOutputImages(int engine_index, int number);

// ���������� ����� ������ �����
RDK_LIB_TYPE int RDK_CALL Env_GetNumInputImages(void);

// ���������� ����� ������� �����
RDK_LIB_TYPE int RDK_CALL Env_GetNumOutputImages(void);

// ������ ���������� �� ��������� (������� ����������)
RDK_LIB_TYPE void RDK_CALL Env_SetInputRes(int number, int width, int height);
RDK_LIB_TYPE void RDK_CALL MEnv_SetInputRes(int engine_index, int number, int width, int height);

// ������ ������ �����������
RDK_LIB_TYPE void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

// ������ ���� ��������� �������� ����������� ������ �������������� ���
RDK_LIB_TYPE void Env_SetReflectionXFlag(bool value);
RDK_LIB_TYPE void MEnv_SetReflectionXFlag(int engine_index, bool value);

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

/// ���������� ��������� � ���� � ������ ��������� ������
RDK_LIB_TYPE bool RDK_CALL Env_CallSourceController(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_CallSourceController(int engine_index);
// --------------------------

// ----------------------------
// ������ ���������� �������
// ----------------------------
// ������� ������
RDK_LIB_TYPE int RDK_CALL Model_Destroy(void);
RDK_LIB_TYPE int RDK_CALL MModel_Destroy(int engine_index);

// ������� ����� ������ �� ����� ������ � ���������
// �������������� ������� ������������ ������
RDK_LIB_TYPE int RDK_CALL Model_Create(const char *classname);
RDK_LIB_TYPE int RDK_CALL MModel_Create(int engine_index, const char *classname);

// ������� ������
RDK_LIB_TYPE int RDK_CALL Model_Clear(void);
RDK_LIB_TYPE int RDK_CALL MModel_Clear(int engine_index);

// ���������, ���������� �� ������
RDK_LIB_TYPE bool RDK_CALL Model_Check(void);
RDK_LIB_TYPE bool RDK_CALL MModel_Check(int engine_index);

// ���������, ���������� �� � ������ ��������� � ������ stringid)
RDK_LIB_TYPE bool RDK_CALL Model_CheckComponent(const char* stringid);
RDK_LIB_TYPE bool RDK_CALL MModel_CheckComponent(int engine_index, const char* stringid);

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'classname'
// ���� stringid - ������ ������, �� ��������� � ���� ������
// ���������� ��� ���������� � ������ ������
RDK_LIB_TYPE const char* RDK_CALL Model_AddComponent(const char* stringid, const char *classname);
RDK_LIB_TYPE const char* RDK_CALL MModel_AddComponent(int engine_index, const char* stringid, const char *classname);

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'name'
// ���� stringid - ������ ������, �� ������� �� ����� ������
RDK_LIB_TYPE int RDK_CALL Model_DelComponent(const char* stringid, const char *name);
RDK_LIB_TYPE int RDK_CALL MModel_DelComponent(int engine_index, const char* stringid, const char *name);

// ���������� ����� ���� ��������� � �������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
RDK_LIB_TYPE int RDK_CALL Model_GetNumComponents(const char* stringid);

// ���������� ������ id ���� ��������� ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
RDK_LIB_TYPE int RDK_CALL Model_GetComponentsList(const char* stringid, int *buffer);

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ','
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentsNameList(const char* stringid);

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ',' � ������� ��� ������ 'class_name'
// ���� find_all == true �� ����� ������� � �� ���� ��������������
RDK_LIB_TYPE const char* RDK_CALL Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all);

// ���������� ��������� � ������� �������� index ��� ������ 'name' ����� ���
// ���� �� ������ �� �������� ����� ���������
// ����������� ��� ��������� ������� ������� ���������
// ���� �������� 'step' ������� �� ������� �������, �� ��������� ���������������
// �� ��� �������
RDK_LIB_TYPE int RDK_CALL Model_ChangeComponentPosition(const char* stringid, int step);

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
RDK_LIB_TYPE const char* RDK_CALL MModel_GetComponentClassName(int engine_index, const char* stringid);

// ���������� ������ ������� ���������� ����������� ��������
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask);

// ���������� ������ ���� � �������� ������� ���������� ����������� ��������
// ������ ������� ����� ��� ���_��������:������_�����(������)
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask);

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
RDK_LIB_TYPE const void* RDK_CALL MModel_GetComponentPropertyData(int engine_index, const char *stringid, const char *property_name);

// �������� ������ 'data' � �������� �������� ����������
RDK_LIB_TYPE int RDK_CALL Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentPropertyData(int engine_index, const char *stringid, const char *property_name, const void *data);

// ���������� ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameters(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentParameters(int engine_index, const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// ���������� ��������� ���������� �� �������������� � ����������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// ���������� ���������� ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid);

// ���������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentParameterValue(int engine_index, const char *stringid, const char *paramname);

// ������������� ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentParameters(int engine_index, const char *stringid, const char* buffer);

// ������������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentParameterValue(int engine_index, const char *stringid, const char *paramname, const char *buffer);

// ���������� ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentState(const char *stringid, unsigned int type_mask=0xFFFFFFFF);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentState(int engine_index, const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// ���������� ���������� ������ ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid);

// ���������� �������� ��������� ��������� ��������� �� �������������� ���������� � ����� ����������
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentStateValue(int engine_index, const char *stringid, const char *statename);

// ������������� ��������� ���������� �� ��������������
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentState(int engine_index, const char *stringid, const char* buffer);

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

// ��������� ����� �� ����� connector_index ���������� 'connectorid'
RDK_LIB_TYPE int RDK_CALL Model_BreakConnectorLink(const char* connectorname, int connector_index);

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
RDK_LIB_TYPE const char * RDK_CALL MModel_SaveComponent(int engine_index, const char *stringid, unsigned int params_type_mask=ptParameter | pgPublic);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponent(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_LoadComponent(int engine_index, const char *stringid, const char* buffer);

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentProperties(const char *stringid, unsigned int type_mask);

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentProperties(const char *stringid, char* buffer);

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentParameters(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentParameters(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_LoadComponentParameters(int engine_index, const char *stringid, const char* buffer);

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

// ������� ����� ������� ���������� ������ � �������������
RDK_LIB_TYPE long long RDK_CALL Model_GetSourceTime(void);
RDK_LIB_TYPE long long RDK_CALL MModel_GetSourceTime(int engine_index);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleSourceTime(void);
RDK_LIB_TYPE double RDK_CALL MModel_GetDoubleSourceTime(int engine_index);

// ������������� ����� ������� ���������� ������
RDK_LIB_TYPE bool RDK_CALL Model_SetSourceTime(long long value);
RDK_LIB_TYPE bool RDK_CALL MModel_SetSourceTime(int engine_index, long long value);
RDK_LIB_TYPE bool RDK_CALL Model_SetSourceTimeAll(long long value);

// ������������� ����� ������� ���������� ������ � ����
RDK_LIB_TYPE bool RDK_CALL Model_SetDoubleSourceTime(double value);
RDK_LIB_TYPE bool RDK_CALL MModel_SetDoubleSourceTime(int engine_index, double value);
RDK_LIB_TYPE bool RDK_CALL Model_SetDoubleSourceTimeAll(double value);

// ����������� ����� ������� ���������� ������ �� �������� ��������
RDK_LIB_TYPE bool RDK_CALL Model_IncreaseSourceTime(long long value);
RDK_LIB_TYPE bool RDK_CALL MModel_IncreaseSourceTime(int engine_index, long long value);

// ���������� ��� �� ������� ������� ���������� ������ � �������������
RDK_LIB_TYPE long long RDK_CALL Model_GetSourceTimeStep(void);
RDK_LIB_TYPE long long RDK_CALL MModel_GetSourceTimeStep(int engine_index);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleSourceTimeStep(void);
RDK_LIB_TYPE double RDK_CALL MModel_GetDoubleSourceTimeStep(int engine_index);

// ���������� ����� ������� ���������� ��� ������� ������� �������� ��������� (��)
RDK_LIB_TYPE long long RDK_CALL Model_GetStepDuration(const char *stringid);
RDK_LIB_TYPE long long RDK_CALL MModel_GetStepDuration(int engine_index, const char *stringid);

// ���������� �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ���������) (��)
RDK_LIB_TYPE long long RDK_CALL Model_GetFullStepDuration(const char *stringid);
RDK_LIB_TYPE long long RDK_CALL MModel_GetFullStepDuration(int engine_index, const char *stringid);

// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
RDK_LIB_TYPE double RDK_CALL Model_GetInstantPerformance(const char *stringid);
RDK_LIB_TYPE double RDK_CALL MModel_GetInstantPerformance(int engine_index, const char *stringid);

// �����, ��������� ����� ����� ���������� ���������� �����
RDK_LIB_TYPE long long RDK_CALL Model_GetInterstepsInterval(const char *stringid);
RDK_LIB_TYPE long long RDK_CALL MModel_GetInterstepsInterval(int engine_index, const char *stringid);
// --------------------------

// --------------------------
// ������� ���������� ������� ��������������
// --------------------------
// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
// ������������ �������� ����� ����������� ��� RDK::MDMatrix*
// ���� ����� �� �������� ������ ������ ����, �� ���������� 0
RDK_LIB_TYPE const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrixByIndex(const char *stringid, int index);

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
// ������������ �������� ����� ����������� ��� RDK::UBitmap*
RDK_LIB_TYPE const /* RDK::UBitmap* */void* RDK_CALL Model_GetComponentOutput(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /* RDK::UBitmap* */void* RDK_CALL Model_GetComponentOutputByIndex(const char *stringid, int index);

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutput(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutputByIndex(const char *stringid, int index);

// ���������� ��������� �� ���� � �������� 'index' ���������� 'id'
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInput(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInputByIndex(const char *stringid, int index);

// �������� ����������� ������ � �������� 'index' ���������� 'id'
RDK_LIB_TYPE void RDK_CALL Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE void RDK_CALL MModel_SetComponentBitmapOutput(int engine_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE void RDK_CALL Model_SetComponentBitmapOutputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE void RDK_CALL MModel_SetComponentBitmapOutputByIndex(int engine_index, const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);

// �������� ����������� ����� � �������� 'index' ���������� 'id'
RDK_LIB_TYPE void RDK_CALL Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE void RDK_CALL Model_SetComponentBitmapInputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
// --------------------------

// --------------------------
// ������� ���������� ������������
// ----------------------------
// ���������� ��������-������������ ����������
RDK_LIB_TYPE void* RDK_CALL Engine_GetExceptionHandler(void);
RDK_LIB_TYPE void* RDK_CALL MEngine_GetExceptionHandler(int engine_index);
RDK_LIB_TYPE bool RDK_CALL Engine_SetExceptionHandler(void* value);
RDK_LIB_TYPE bool RDK_CALL MEngine_SetExceptionHandler(int engine_index, void* value);

// ���������� ������ ����� ����
RDK_LIB_TYPE const char* RDK_CALL Engine_GetLog(int &error_level);
RDK_LIB_TYPE const char* RDK_CALL MEngine_GetLog(int engine_index, int &error_level);

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
RDK_LIB_TYPE const char* RDK_CALL Engine_GetUnreadLog(int &error_level);
RDK_LIB_TYPE const char* RDK_CALL MEngine_GetUnreadLog(int engine_index, int &error_level);

// ���������� � ��� ����� ���������
RDK_LIB_TYPE int RDK_CALL Engine_LogMessage(int log_level, const char *message);
RDK_LIB_TYPE int RDK_CALL MEngine_LogMessage(int engine_index, int log_level, const char *message);
// ----------------------------

#ifdef __cplusplus
}
#endif

#endif
