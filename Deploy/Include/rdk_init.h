#ifndef RDK_INIT_H
#define RDK_INIT_H

#include "rdk_error_codes.h"
#include "initdll_defs.h"

// Suppress redefinition warnings for macros that may be defined via -D in command line
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-macro-redefined"
#endif
#ifndef RDK_APP_NAME
#define RDK_APP_NAME "RDK"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __cplusplus
extern "C"  {
#else
typedef int bool;
#endif

// Функция для проверки режима инициализации (предотвращает фатальные краши)
RDK_LIB_TYPE bool RDK_CALL RDK_IsInitializationMode(void);

#ifndef RDK_EX_UNKNOWN
#define RDK_EX_UNKNOWN 0 // Unknown exception
#define RDK_EX_FATAL 1 // Fatal error (correction impossible)
#define RDK_EX_ERROR 2 // Correctable error
#define RDK_EX_WARNING 3 // Warning (performance, possible errors etc)
#define RDK_EX_INFO 4 // Information (port open, client connected etc)
#define RDK_EX_APP 5 // Application-defined event (high-level errors, etc)
#define RDK_EX_DEBUG 6 // Debug messages (can be switched off)

#define RDK_SYS_DEBUG_CALC 1
#define RDK_SYS_DEBUG_RESET 2
#define RDK_SYS_DEBUG_PROPERTIES 4
#define RDK_SYS_DEBUG_PARAMETERS 8
#define RDK_SYS_DEBUG_STATES 16
#define RDK_SYS_DEBUG_INPUTS 32
#define RDK_SYS_DEBUG_OUTPUTS 64

#define RDK_SYS_MESSAGE_ENTER 1
#define RDK_SYS_MESSAGE_EXIT_OK 2
#define RDK_SYS_MESSAGE_EXIT_ININIT_FAIL 4
#define RDK_SYS_MESSAGE_NEW_CALC_ITERATION 8

#endif

/// См. описание в rdk_init.cpp
#ifndef RDK_SYS_MESSAGE
#define RDK_SYS_MESSAGE -1
#endif

#ifndef RDK_GLOB_MESSAGE
#define RDK_GLOB_MESSAGE -2
#endif

#if defined(NDEBUG) || defined(DEBUG)
#define RDK_ASSERTD_DEBUGGER(FUNCTION_RESULT) (RDK::AssertDebug(FUNCTION_RESULT,__FUNCTION__,__FILE__,__LINE__),FUNCTION_RESULT)
#define RDK_ASSERTD_LOG(FUNCTION_RESULT) (RDK::AssertLog(FUNCTION_RESULT,__FUNCTION__,__FILE__,__LINE__),FUNCTION_RESULT)
#else
#define RDK_ASSERTD_DEBUGGER(FUNCTION_RESULT) FUNCTION_RESULT
#define RDK_ASSERTD_LOG(FUNCTION_RESULT) FUNCTION_RESULT
#endif

#define RDK_ASSERT_DEBUGGER(FUNCTION_RESULT) (RDK::AssertDebug(FUNCTION_RESULT,__FUNCTION__,__FILE__,__LINE__),FUNCTION_RESULT)
#define RDK_ASSERT_LOG(FUNCTION_RESULT) (RDK::AssertLog(FUNCTION_RESULT,__FUNCTION__,__FILE__,__LINE__),FUNCTION_RESULT)


#ifndef RDK_APP_NAME
#define RDK_APP_NAME "RDK"
#endif

#ifndef RDK_PROPERTY_TYPES
#define RDK_PROPERTY_TYPES
// Варианты типа свойства (битовая маска) pt - Property Type
// 0x1 - Параметр
// 0x2 - Переменная состояния
// 0x4 - Временная переменная
// 0x8 - Вход
enum : unsigned int {ptParameter=1, ptState=2, ptTemp=4, ptInput=8, ptOutput=16, ptAny=255};

// Варианты групп свойства (битовая маска) pg - Property Group
// 0x100 - Общедоступный
// 0x200 - Системный
// 0x400 - Входные данные
// 0x800 - Выходные данные
// 0x1000 - Флаг смены режима работы компонента
enum : unsigned int {pgPublic=0x100, pgSystem=0x200, pgInput=0x400, pgOutput=0x800, pgMode=0x1000, pgAny=0xFFFFFF};

// Наиболее часто используемые сочетания типа и группы
constexpr unsigned int ptPubParameter = static_cast<unsigned int>(ptParameter) | static_cast<unsigned int>(pgPublic);
constexpr unsigned int ptPubState = static_cast<unsigned int>(ptState) | static_cast<unsigned int>(pgPublic);
constexpr unsigned int ptPubInput = static_cast<unsigned int>(ptInput) | static_cast<unsigned int>(pgPublic);
constexpr unsigned int ptPubOutput = static_cast<unsigned int>(ptOutput) | static_cast<unsigned int>(pgPublic);
// Combined flags for system property types
constexpr unsigned int ptSysParameter = static_cast<unsigned int>(ptParameter) | static_cast<unsigned int>(pgSystem);
constexpr unsigned int ptPubSysParameter = static_cast<unsigned int>(ptParameter) | static_cast<unsigned int>(pgPublic) | static_cast<unsigned int>(pgSystem);
constexpr unsigned int ptPubSysState = static_cast<unsigned int>(ptState) | static_cast<unsigned int>(pgPublic) | static_cast<unsigned int>(pgSystem);
// Combined flags for any type with public group
constexpr unsigned int ptAnyPub = static_cast<unsigned int>(ptAny) | static_cast<unsigned int>(pgPublic);

enum : unsigned int { ipData=1 }; // ipComp removed as legacy (was used for component pointers, now unused)
enum : unsigned int { ipSingle=16, ipRange=32, ipList=64 };
constexpr unsigned int ipDataSingle = static_cast<unsigned int>(ipData) | static_cast<unsigned int>(ipSingle);
constexpr unsigned int ipDataRange = static_cast<unsigned int>(ipData) | static_cast<unsigned int>(ipRange);
constexpr unsigned int ipDataList = static_cast<unsigned int>(ipData) | static_cast<unsigned int>(ipList);
#endif

// ----------------------------
// ----------------------------
// Функции RPC
// С помощью этих функций возможно вызвать любую функицю библиотеки,
// возвращающую строковые данные
// ----------------------------
/// Выполняет запрос и возвращает xml-описание ответа
/// request - xml описание запроса
/// return_value - возвращаемое значение для тех функций, которые его имеют
/// для остальных возвращает 0
/// запрос request имеет следующий вид (часть полей может отсутстовать в зависимости
/// от вызываемой фукнции):
/// <RpcRequest>
///	    <Id>уникальный идентификатор запроса</Id>
///     <Channel>индекс движка</Channel>
///     <Cmd>имя вызываемой функции</Cmd>
///     <Component>имя компонента</Component>
///     <Class>имя класса</Class>
///     <Data>xml-описание данных функции, например xml с параметрами компонента</Data>
/// </RpcRequest>
/// Ответ представляет собой данные в следующем виде:
/// <RpcResponse>
///	    <Id>уникальный идентификатор запроса</Id>
///     <Data>xml-описание данных функции, например xml с параметрами компонента</Data>
///     <Res>идентификатор возвращаемой ошибки или 0 если вызов успешен</Res>
RDK_LIB_TYPE const char* RDK_CALL Core_RemoteCall(const char *request, int &return_value, int &channel_index);
// ----------------------------

// ----------------------------
// ----------------------------
// Функции определения версий
// ----------------------------
RDK_LIB_TYPE int RDK_CALL Ver_CoreMajor(void);

/// Возвращает номер минорной версии ядра
RDK_LIB_TYPE int RDK_CALL Ver_CoreMinor(void);

/// Возвращает номер ревизии ядра
RDK_LIB_TYPE int RDK_CALL Ver_CoreRevision(void);

/// Возвращает строку версии ядра в формате строки
RDK_LIB_TYPE const char* RDK_CALL Ver_Core(void);

/// Сравнивает версию ядра с переданной
/// Сравнивает версию ядра с переданной
/// возвращает >0 если версия ядра больше,
/// возвращает <0 если версия ядра меньше,
RDK_LIB_TYPE int RDK_CALL Ver_CoreCompare(int major, int minor, int revision);

/// Возвращает имя компилятора ядра
RDK_LIB_TYPE const char* RDK_CALL Ver_CompilerName(void);

/// Возвращает версию компилятора ядра
RDK_LIB_TYPE const char* RDK_CALL Ver_CompilerVersion(void);

/// Возвращает версию boost

/// См. описание в rdk_init.cpp
/// Возвращает версию opencv (если используется)
//RDK_LIB_TYPE const char* RDK_CALL Ver_OpenCvVersion(void);

/// Возвращает текущий режим отладки логгера ядра
RDK_LIB_TYPE bool RDK_CALL Log_GetDebugMode(void);
RDK_LIB_TYPE bool RDK_CALL MLog_GetDebugMode(int channel_index);

/// Устанавливает текущий режим отладки логгера ядра
RDK_LIB_TYPE int RDK_CALL Log_SetDebugMode(bool value);
RDK_LIB_TYPE int RDK_CALL MLog_SetDebugMode(int channel_index, bool value);

/// Возвращает маску системных событий для отладки
RDK_LIB_TYPE unsigned int RDK_CALL Log_GetDebugSysEventsMask(void);
RDK_LIB_TYPE unsigned int RDK_CALL MLog_GetDebugSysEventsMask(int channel_index);

/// Устанавливает маску системных событий для отладки
RDK_LIB_TYPE int RDK_CALL Log_SetDebugSysEventsMask(unsigned int value);
RDK_LIB_TYPE int RDK_CALL MLog_SetDebugSysEventsMask(int channel_index, unsigned int value);

/// Возвращает флаг вывода сообщений в отладчик
RDK_LIB_TYPE bool RDK_CALL Log_GetDebuggerMessageFlag(void);
RDK_LIB_TYPE bool RDK_CALL MLog_GetDebuggerMessageFlag(int channel_index);

/// Устанавливает флаг вывода сообщений в отладчик
RDK_LIB_TYPE int RDK_CALL Log_SetDebuggerMessageFlag(bool value);
RDK_LIB_TYPE int RDK_CALL MLog_SetDebuggerMessageFlag(int channel_index, bool value);

// !

RDK_LIB_TYPE void* RDK_CALL Log_GetExceptionHandler(void);
RDK_LIB_TYPE void* RDK_CALL MLog_GetExceptionHandler(int channel_index);
RDK_LIB_TYPE int RDK_CALL Log_SetExceptionHandler(void* value);
RDK_LIB_TYPE int RDK_CALL MLog_SetExceptionHandler(int channel_index, void* value);

/// Записывает строку в лог ядра
RDK_LIB_TYPE int RDK_CALL Log_LogMessage(int log_level, const char *message);
RDK_LIB_TYPE int RDK_CALL MLog_LogMessage(int channel_index, int log_level, const char *message);

/// Записывает в лог сообщение с номером события
RDK_LIB_TYPE int RDK_CALL Log_LogMessageEx(int log_level, const char *message, int error_event_number);
RDK_LIB_TYPE int RDK_CALL MLog_LogMessageEx(int channel_index, int log_level, const char *message, int error_event_number);

// ----------------------------
// ----------------------------
// Функции управления ядром
// ----------------------------
RDK_LIB_TYPE const char* RDK_CALL Core_GetSystemDir(void);
//RDK_LIB_TYPE const char* RDK_CALL GetSystemDir(void); // deprecated

/// Устанавливает путь к системной директории ядра
RDK_LIB_TYPE int RDK_CALL Core_SetSystemDir(const char *dir);
//RDK_LIB_TYPE int RDK_CALL SetSystemDir(const char *dir); // deprecated

/// Возвращает путь к директории логов ядра
RDK_LIB_TYPE const char* RDK_CALL Core_GetLogDir(void);

/// Устанавливает путь к директории логов ядра
RDK_LIB_TYPE int RDK_CALL Core_SetLogDir(const char *dir);

/// Возвращает текущий режим отладки системных сообщений логгера ядра
RDK_LIB_TYPE bool RDK_CALL Core_GetDebugMode(void);

/// Устанавливает текущий режим отладки системных сообщений логгера ядра
RDK_LIB_TYPE int RDK_CALL Core_SetDebugMode(bool value);

/// Возвращает флаг вывода сообщений в отладчик
RDK_LIB_TYPE bool RDK_CALL Core_GetDebuggerMessageFlag(void);

/// Устанавливает флаг вывода сообщений в отладчик
RDK_LIB_TYPE int RDK_CALL Core_SetDebuggerMessageFlag(bool value);

/// Очищает загруженные шрифты
RDK_LIB_TYPE int RDK_CALL Core_ClearFonts(void);

/// Загружает загруженные шрифты
RDK_LIB_TYPE int RDK_CALL Core_LoadFonts(void);

/// Возвращает число каналов
RDK_LIB_TYPE int RDK_CALL Core_GetNumChannels(void);
//RDK_LIB_TYPE int RDK_CALL GetNumEngines(void); // deprecated

/// Устанавливает число каналов ядра
// Создает требуемое число движков
RDK_LIB_TYPE int RDK_CALL Core_SetNumChannels(int num);
//RDK_LIB_TYPE int RDK_CALL SetNumEngines(int num); // deprecated

// См. описание в rdk_init.cpp
// Добавляет движок в позицию заданного индекса
// Если позиция лежит вне пределов диапазона то
RDK_LIB_TYPE int RDK_CALL Core_AddChannel(int index);
//RDK_LIB_TYPE int RDK_CALL Engine_Add(int index); // deprecated

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Core_DelChannel(int index);
//RDK_LIB_TYPE int RDK_CALL Engine_Del(int index); // deprecated

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Core_GetSelectedChannelIndex(void);

// См. описание в rdk_init.cpp
// Настраивает обычный интерфейс на работу с заданным движком
RDK_LIB_TYPE int RDK_CALL Core_SelectChannel(int index);
//RDK_LIB_TYPE int RDK_CALL SelectEngine(int index); // deprecated

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Core_LockChannel(void);
//RDK_LIB_TYPE int RDK_CALL LockEngine(void); // deprecated
RDK_LIB_TYPE int RDK_CALL MCore_LockChannel(int index);
//RDK_LIB_TYPE int RDK_CALL MLockEngine(int index); // deprecated

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Core_UnLockChannel(void);
//RDK_LIB_TYPE int RDK_CALL UnLockEngine(void); // deprecated
RDK_LIB_TYPE int RDK_CALL MCore_UnLockChannel(int index);
//RDK_LIB_TYPE int RDK_CALL MUnLockEngine(int index); // deprecated

// См. описание в rdk_init.cpp
// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Core_ChannelInit(int predefined_structure, void* exception_handler=0);
//RDK_LIB_TYPE int RDK_CALL EngineInit(int predefined_structure, void* exception_handler=0); // deprecated
RDK_LIB_TYPE int RDK_CALL MCore_ChannelInit(int channel_index, int predefined_structure, void* exception_handler=0);
//RDK_LIB_TYPE int RDK_CALL MEngineInit(int channel_index, int predefined_structure, void* exception_handler=0); // deprecated

// См. описание в rdk_init.cpp
// См. описание в rdk_init.cpp
// Upd: Функция может быть вызвана после SetNumEngines и SelectEngine
// deprecated
//RDK_LIB_TYPE int RDK_CALL GraphicalEngineInit(int predefined_structure, int num_inputs,
//		int num_outputs, int input_width, int input_height, bool reflectionx=false,
//		void* exception_handler=0); // deprecated
//RDK_LIB_TYPE int RDK_CALL MGraphicalEngineInit(int channel_index, int predefined_structure, int num_inputs,
//		int num_outputs, int input_width, int input_height, bool reflectionx=false,

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Core_ChannelUnInit(void);
//RDK_LIB_TYPE int RDK_CALL EngineUnInit(void); // deprecated
RDK_LIB_TYPE int RDK_CALL MCore_ChannelUnInit(int channel_index);
//RDK_LIB_TYPE int RDK_CALL MEngineUnInit(int channel_index); // deprecated

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE bool RDK_CALL Core_IsChannelInit(void);
//RDK_LIB_TYPE bool RDK_CALL IsEngineInit(void); // deprecated
RDK_LIB_TYPE bool RDK_CALL MCore_IsChannelInit(int channel_index);
//RDK_LIB_TYPE bool RDK_CALL MIsEngineInit(int channel_index); // deprecated

/// См. описание в rdk_init.cpp
/// Режим создания внутренних временных переменных для
/// возвращаемых значений
/// 0 - одна переменная для всех методов, возвращающих такой тип
RDK_LIB_TYPE int RDK_CALL Core_GetBufObjectsMode(void);
RDK_LIB_TYPE int RDK_CALL Core_SetBufObjectsMode(int mode);

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Engine_FreeBufString(const char *pointer);
RDK_LIB_TYPE int RDK_CALL MEngine_FreeBufString(int channel_index,const char *pointer);
RDK_LIB_TYPE int RDK_CALL Engine_FreeBufStringUnsafe(const char *pointer);
RDK_LIB_TYPE int RDK_CALL MEngine_FreeBufStringUnsafe(int channel_index,const char *pointer);

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Engine_GetNumBufStrings(void);
RDK_LIB_TYPE int RDK_CALL MEngine_GetNumBufStrings(int channel_index);

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE void* RDK_CALL Engine_GetMutex(void);
RDK_LIB_TYPE void* RDK_CALL MEngine_GetMutex(int index);
// ----------------------------

// --------------------------
// --------------------------
// Функции управления хранилищем
// ----------------------------
RDK_LIB_TYPE int RDK_CALL Storage_GetNumClasses(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_GetClassesList(int *buffer);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char * RDK_CALL Storage_GetClassesNameList(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char * RDK_CALL Storage_GetClassName(int id);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_GetClassId(const char *name);

// См. описание в rdk_init.cpp
// Удаляет образец класса объекта из хранилища
// Возвращает ошибку если classid не найден,
RDK_LIB_TYPE int RDK_CALL Storage_DelClass(int classid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_FreeObjectsStorage(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_ClearObjectsStorage(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjects(void);
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjectsById(int classid);
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjectsByName(const char* classname);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassDescription(const char* classname);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_SetClassDescription(const char* classname, const char* description);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveClassesDescription(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_LoadClassesDescription(const char* xmltext);
RDK_LIB_TYPE int RDK_CALL MStorage_LoadClassesDescription(int channel_index, const char* xmltext);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveCommonClassesDescription(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_LoadCommonClassesDescription(const char* xmltext);
RDK_LIB_TYPE int RDK_CALL MStorage_LoadCommonClassesDescription(int channel_index, const char* xmltext);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveAllClassesDescription(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_LoadAllClassesDescription(const char* xmltext);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassProperties(const char *stringid, unsigned int type_mask=0xFFFFFFFF);
RDK_LIB_TYPE const char* RDK_CALL MStorage_GetClassProperties(int channel_index, const char *stringid, unsigned int type_mask=0xFFFFFFFF);


// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassStructure(const char *stringid, unsigned int type_mask=0xFFFFFFFF);
RDK_LIB_TYPE const char* RDK_CALL MStorage_GetClassStructure(int channel_index, const char *stringid, unsigned int type_mask=0xFFFFFFFF);
// --------------------------

// ----------------------------
// ----------------------------
// Методы управления коллекциями компонент
// ----------------------------
RDK_LIB_TYPE int RDK_CALL Storage_GetNumClassLibraries(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassLibrariesList(void);

// См. описание в rdk_init.cpp
// Возвращает список классов библиотеки в виде строки, разделенной запятыми
RDK_LIB_TYPE const char* RDK_CALL Storage_GetLibraryClassNames(const char *library_name);

// См. описание в rdk_init.cpp
// Возвращает список классов библиотеки в виде строки, разделенной запятыми
RDK_LIB_TYPE const char* RDK_CALL Storage_GetLibraryClassNamesByIndex(int index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassLibraryNameByIndex(int index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassLibraryVersionByIndex(int index);

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_CreateRuntimeCollection(const char *collection_name);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_LoadBinaryCollectionFromFile(const char *filename);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_LoadRuntimeCollectionFromFile(const char *filename);
RDK_LIB_TYPE int RDK_CALL Storage_LoadRuntimeCollectionFromString(const char *buffer);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Storage_SaveRuntimeCollectionToFile(const char *filename);
RDK_LIB_TYPE int RDK_CALL Storage_SaveRuntimeCollectionToString(const char *buffer);

// См. описание в rdk_init.cpp
// Удаляет подключенную библиотеку из списка по индексу
RDK_LIB_TYPE int RDK_CALL Storage_DelClassLibraryByIndex(int index);

// См. описание в rdk_init.cpp
// Удаляет подключенную библиотеку из списка по имени
RDK_LIB_TYPE int RDK_CALL Storage_DelClassLibraryByName(const char *name);

// См. описание в rdk_init.cpp
// Удаляет из списка все библиотеки
RDK_LIB_TYPE int RDK_CALL Storage_DelAllClassLibraries(void);

// См. описание в rdk_init.cpp
// Перемещает объект в Storage как образец классов.
RDK_LIB_TYPE int RDK_CALL Storage_CreateClass(const char* stringid, const char *class_name, const char *collection_name);

// См. описание в rdk_init.cpp
// Заполняет хранилище данными библиотек
RDK_LIB_TYPE int RDK_CALL Storage_BuildStorage(void);
// ----------------------------


// --------------------------
// --------------------------
// Функции управления средой
// ----------------------------
// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
RDK_LIB_TYPE int RDK_CALL Env_Calculate(const char* stringid);
RDK_LIB_TYPE int RDK_CALL MEnv_Calculate(int channel_index, const char* stringid);
RDK_LIB_TYPE int RDK_CALL MEnv_CalculateUnsafe(int channel_index, const char* stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_RTCalculate(void);
RDK_LIB_TYPE int RDK_CALL MEnv_RTCalculate(int channel_index);

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_FastCalculate(double calc_interval);
RDK_LIB_TYPE int RDK_CALL MEnv_FastCalculate(int channel_index, double calc_interval);

// См. описание в rdk_init.cpp
// Метод сброса счета
// Если stringid == 0 то сбрасывает всю модель целиком,
RDK_LIB_TYPE int RDK_CALL Env_Reset(const char* stringid);
RDK_LIB_TYPE int RDK_CALL MEnv_Reset(int channel_index, const char* stringid);

/// См. описание в rdk_init.cpp
/// Метод сброса параметров на значения по умолчанию
/// Если stringid == 0 то сбрасывает всю модель целиком,
/// иначе - только указанный компонент модели
RDK_LIB_TYPE int RDK_CALL Env_Default(const char* stringid, bool subcomps=false);
RDK_LIB_TYPE int RDK_CALL MEnv_Default(int channel_index, const char* stringid, bool subcomps=false);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_IncreaseModelTimeByStep(void);

/// См. описание в rdk_init.cpp
/// Устанавливает минимальный интервал времени между шагами расчета (мс)
/// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_SetMinInterstepsInterval(unsigned long long value);
RDK_LIB_TYPE int RDK_CALL MEnv_SetMinInterstepsInterval(int channel_index, unsigned long long value);

/// См. описание в rdk_init.cpp
/// Возвращает минимальный интервал времени между шагами расчета (мс)
/// См. описание в rdk_init.cpp
RDK_LIB_TYPE unsigned long long RDK_CALL Env_GetMinInterstepsInterval(void);
RDK_LIB_TYPE unsigned long long RDK_CALL MEnv_GetMinInterstepsInterval(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE double RDK_CALL Env_GetRTLastDuration(void);
RDK_LIB_TYPE double RDK_CALL MEnv_GetRTLastDuration(int channel_index);

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE double RDK_CALL Env_GetRTModelCalcTime(void);
RDK_LIB_TYPE double RDK_CALL MEnv_GetRTModelCalcTime(int channel_index);

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE double RDK_CALL Env_CalcRTPerformance(void);
RDK_LIB_TYPE double RDK_CALL MEnv_CalcRTPerformance(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Env_GetCurrentDataDir(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_SetCurrentDataDir(const char *dir);
RDK_LIB_TYPE int RDK_CALL MEnv_SetCurrentDataDir(int channel_index, const char *dir);

/// См. описание в rdk_init.cpp
/// Возвращает состояние флага отладочного режима среды
RDK_LIB_TYPE bool RDK_CALL Env_GetDebugMode(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_GetDebugMode(int channel_index);

/// См. описание в rdk_init.cpp
/// Устанавливает состояние флага отладочного режима среды
RDK_LIB_TYPE int RDK_CALL Env_SetDebugMode(bool value);
RDK_LIB_TYPE int RDK_CALL MEnv_SetDebugMode(int channel_index, bool value);

/// См. описание в rdk_init.cpp
/// Возвращает маску системных событий для логирования
RDK_LIB_TYPE unsigned int RDK_CALL Env_GetDebugSysEventsMask(void);
RDK_LIB_TYPE unsigned int RDK_CALL MEnv_GetDebugSysEventsMask(int channel_index);

/// См. описание в rdk_init.cpp
/// Устанавливает маску системных событий для логирования
RDK_LIB_TYPE int RDK_CALL Env_SetDebugSysEventsMask(unsigned int value);
RDK_LIB_TYPE int RDK_CALL MEnv_SetDebugSysEventsMask(int channel_index, unsigned int value);

/// См. описание в rdk_init.cpp
/// Возвращает флаг включения вывода лога в отладчик
RDK_LIB_TYPE bool RDK_CALL Env_GetDebuggerMessageFlag(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_GetDebuggerMessageFlag(int channel_index);

/// См. описание в rdk_init.cpp
/// Устанавливает флаг включения вывода лога в отладчик
RDK_LIB_TYPE bool RDK_CALL Env_SetDebuggerMessageFlag(bool value);
RDK_LIB_TYPE bool RDK_CALL MEnv_SetDebuggerMessageFlag(int channel_index, bool value);

// ***********************************************
// Методы управления текущим компонентом
// !!! Следующие методы влияют на все
// методы, обращающиеся к компонентам по строковому id !!!
// ***********************************************
// Устанавливает текущий компонент (адресация относительно корня - модели)
// Методы управления текущим компонентом
// !!! Следующие методы влияют на все
// методы, обращающиеся к компонентам по строковому id !!!
RDK_LIB_TYPE int RDK_CALL Env_SelectCurrentComponent(const char *stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_ResetCurrentComponent(const char *stringid);

// См. описание в rdk_init.cpp
// Меняет текущий компонент на его родителя (подъем на уровень вверх)
RDK_LIB_TYPE int RDK_CALL Env_UpCurrentComponent(void);

// См. описание в rdk_init.cpp
// Меняет текущий компонент на его дочерний на произвольном уровне вложенности
RDK_LIB_TYPE int RDK_CALL Env_DownCurrentComponent(const char *stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Env_GetCurrentComponentName(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Env_GetCurrentComponentId(void);
// ***********************************************
// Вспомогательные функции управления средой. обычно вызов не требуется
// ***********************************************
// Индекс предварительно заданной модели обработки

// Вспомогательные функции управления средой. обычно вызов не требуется
RDK_LIB_TYPE int RDK_CALL Env_GetPredefinedStructure(void);
RDK_LIB_TYPE int RDK_CALL MEnv_GetPredefinedStructure(int channel_index);
RDK_LIB_TYPE int RDK_CALL Env_SetPredefinedStructure(int value);
RDK_LIB_TYPE int RDK_CALL MEnv_SetPredefinedStructure(int channel_index, int value);

// См. описание в rdk_init.cpp
// Флаг состояния инициализации
// true - хранилище готово к использованию
RDK_LIB_TYPE bool RDK_CALL Env_IsStoragePresent(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_IsStoragePresent(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE bool RDK_CALL Env_IsInit(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_IsInit(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE bool RDK_CALL Env_IsStructured(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_IsStructured(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_Init(void);
RDK_LIB_TYPE int RDK_CALL MEnv_Init(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_UnInit(void);
RDK_LIB_TYPE int RDK_CALL MEnv_UnInit(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_CreateStructure(void);
RDK_LIB_TYPE int RDK_CALL MEnv_CreateStructure(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_DestroyStructure(void);
RDK_LIB_TYPE int RDK_CALL MEnv_DestroyStructure(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_Destroy(void);
RDK_LIB_TYPE int RDK_CALL MEnv_Destroy(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_ModelInit(const char *stringid);
RDK_LIB_TYPE int RDK_CALL MEnv_ModelInit(int channel_index, const char *stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_ModelUnInit(const char *stringid);
RDK_LIB_TYPE int RDK_CALL MEnv_ModelUnInit(int channel_index, const char *stringid);
// --------------------------
// Функции управления средой видеообработки
// --------------------------
// Задает число входов среды


// --------------------------
// --------------------------
// Функции управления средой видеообработки
// --------------------------
RDK_LIB_TYPE void RDK_CALL Env_SetNumInputImages(int number);
RDK_LIB_TYPE void RDK_CALL MEnv_SetNumInputImages(int channel_index, int number);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE void RDK_CALL Env_SetNumOutputImages(int number);
RDK_LIB_TYPE void RDK_CALL MEnv_SetNumOutputImages(int channel_index, int number);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_GetNumInputImages(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_GetNumOutputImages(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE void RDK_CALL Env_SetInputRes(int number, int width, int height);
RDK_LIB_TYPE void RDK_CALL MEnv_SetInputRes(int channel_index, int number, int width, int height);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE void Env_SetReflectionXFlag(bool value);
RDK_LIB_TYPE void MEnv_SetReflectionXFlag(int channel_index, bool value);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageWidth(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageHeight(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageColorModel(int number);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageWidth(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageHeight(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageColorModel(int number);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetInputImage(int index);
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetOutputImage(int index);
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetOutputImageY8(int index);

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Env_CallSourceController(void);
RDK_LIB_TYPE int RDK_CALL MEnv_CallSourceController(int channel_index);
// --------------------------

// ----------------------------
// ----------------------------
// Методы управления моделью
// ----------------------------
RDK_LIB_TYPE int RDK_CALL Model_Destroy(void);
RDK_LIB_TYPE int RDK_CALL MModel_Destroy(int channel_index);

// См. описание в rdk_init.cpp
// Создает новую модель по имени класса в хранилище
RDK_LIB_TYPE int RDK_CALL Model_Create(const char *classname);
RDK_LIB_TYPE int RDK_CALL MModel_Create(int channel_index, const char *classname);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_Clear(void);
RDK_LIB_TYPE int RDK_CALL MModel_Clear(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE bool RDK_CALL Model_Check(void);
RDK_LIB_TYPE bool RDK_CALL MModel_Check(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE bool RDK_CALL Model_CheckComponent(const char* stringid);
RDK_LIB_TYPE bool RDK_CALL MModel_CheckComponent(int channel_index, const char* stringid);

// См. описание в rdk_init.cpp
// Добавляет в выбранный компонент модели с идентификатором 'stringid' экземпляр
// компонента с заданным 'classname'
// если stringid - пустая строка, то добавляет в саму модель
RDK_LIB_TYPE const char* RDK_CALL Model_AddComponent(const char* stringid, const char *classname);
RDK_LIB_TYPE const char* RDK_CALL MModel_AddComponent(int channel_index, const char* stringid, const char *classname);

// См. описание в rdk_init.cpp
// Удаляет из выбранного компонента модели с идентификатором 'stringid' экземпляр
// компонента с заданным 'name'
// если stringid - пустая строка, то удаляет из самой модели
RDK_LIB_TYPE int RDK_CALL Model_DelComponent(const char* stringid, const char *name);
RDK_LIB_TYPE int RDK_CALL MModel_DelComponent(int channel_index, const char* stringid, const char *name);

/// См. описание в rdk_init.cpp
/// Клонирует компонент со всеми содержимым и внутренними связями
RDK_LIB_TYPE int RDK_CALL Model_CloneComponent(const char* component_name, const char* new_name);
RDK_LIB_TYPE int RDK_CALL MModel_CloneComponent(int channel_index, const char* component_name, const char* new_name);

/// См. описание в rdk_init.cpp
/// Перемещает компоненту в другой компонент
/// Если comp не принадлежит этому компоненту, или target имеет отличный от
/// этого компонента storage, или target не может принять в себя компонент
RDK_LIB_TYPE int RDK_CALL Model_MoveComponent(const char* component, const char* target);
RDK_LIB_TYPE int RDK_CALL MModel_MoveComponent(int channel_index, const char* component, const char* target);

// См. описание в rdk_init.cpp
// Возвращает число всех компонент в заданном компоненте 'stringid'
RDK_LIB_TYPE int RDK_CALL Model_GetNumComponents(const char* stringid);

// См. описание в rdk_init.cpp
// Возвращает массив id всех компонент заданного компонента 'stringid'
RDK_LIB_TYPE int RDK_CALL Model_GetComponentsList(const char* stringid, int *buffer);

// См. описание в rdk_init.cpp
// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentsNameList(const char* stringid);
RDK_LIB_TYPE const char* RDK_CALL MModel_GetComponentsNameList(int channel_index, const char* stringid);

// См. описание в rdk_init.cpp
// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ',' и имеющих имя класса 'class_name'
RDK_LIB_TYPE const char* RDK_CALL Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all);

// См. описание в rdk_init.cpp
// Перемещает компонент с текущим индексом index или именем 'name' вверх или
// вниз по списку на заданное число элементов
// Применяется для изменения порядка расчета компонент
// Если значение 'step' выводит за границы массива, то компонент устанавливается
RDK_LIB_TYPE int RDK_CALL Model_ChangeComponentPosition(const char* stringid, int step);
RDK_LIB_TYPE int RDK_CALL MModel_ChangeComponentPosition(int channel_index, const char* stringid, int step);

// См. описание в rdk_init.cpp
// Возвращает xml-список длинных идентификаторов всех коннекторов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
RDK_LIB_TYPE const char* RDK_CALL Model_GetConnectorsList(const char* stringid,
						  int sublevel=-1, const char* owner_level_stringid=0);

// См. описание в rdk_init.cpp
// Возвращает xml-список длинных идентификаторов всех элементов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
RDK_LIB_TYPE const char* RDK_CALL Model_GetItemsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// См. описание в rdk_init.cpp
// Возвращает xml-список длинных идентификаторов всех подсетей сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
RDK_LIB_TYPE const char* RDK_CALL Model_GetNetsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// См. описание в rdk_init.cpp
// Возвращает короткое имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentName(const char* stringid);

// См. описание в rdk_init.cpp
// Возвращает длинное имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid=0);
RDK_LIB_TYPE const char* RDK_CALL MModel_GetComponentLongName(int channel_index, const char* stringid, const char* owner_level_stringid=0);

// См. описание в rdk_init.cpp
// Возвращает длинный id компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid=0);

// См. описание в rdk_init.cpp
// Возвращает имя класса компонента в хранилище по длинному 'stringid'
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentClassName(const char* stringid);
RDK_LIB_TYPE const char* RDK_CALL MModel_GetComponentClassName(int channel_index, const char* stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask);

// См. описание в rdk_init.cpp
// Возвращает список имен и индексов свойств компонента разделенный запятыми
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentProperties(const char *stringid, unsigned int type_mask);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedProperties(const char *stringid, unsigned int type_mask);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentPropertyValue(const char *stringid, const char *paramname);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentPropertyValue(int channel_index, const char *stringid, const char *paramname);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SetComponentProperties(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentProperties(int engine_index, const char *stringid, const char* buffer);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentPropertyValue(int channel_index, const char *stringid, const char *paramname, const char *buffer);

// См. описание в rdk_init.cpp
// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
RDK_LIB_TYPE int RDK_CALL Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer);

// См. описание в rdk_init.cpp
// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
RDK_LIB_TYPE int RDK_CALL Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const void* RDK_CALL Model_GetComponentPropertyData(const char *stringid, const char *property_name);
RDK_LIB_TYPE const void* RDK_CALL MModel_GetComponentPropertyData(int channel_index, const char *stringid, const char *property_name);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentPropertyData(int channel_index, const char *stringid, const char *property_name, const void *data);

// См. описание в rdk_init.cpp
// Возвращает параметры компонента по идентификатору
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameters(const char *stringid, unsigned int type_mask=ptPubParameter);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentParameters(int channel_index, const char *stringid, unsigned int type_mask=ptPubParameter);

// См. описание в rdk_init.cpp
// Возвращает параметры компонента по идентификатору с описаниями
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid, unsigned int type_mask=ptPubParameter);

// См. описание в rdk_init.cpp
// Возвращает выборочные параметры компонента по идентификатору
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid);

// См. описание в rdk_init.cpp
// Возвращает значение параметра компонента по идентификатору компонента и имени параметра
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentParameterValue(int channel_index, const char *stringid, const char *paramname);

// См. описание в rdk_init.cpp
// Устанавливает параметры компонента по идентификатору
RDK_LIB_TYPE int RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentParameters(int channel_index, const char *stringid, const char* buffer);

// См. описание в rdk_init.cpp
// Устанавливает значение параметра компонента по идентификатору компонента и имени параметра
RDK_LIB_TYPE int RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentParameterValue(int channel_index, const char *stringid, const char *paramname, const char *buffer);

// См. описание в rdk_init.cpp
// Возвращает состояние компонента по идентификатору
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentState(const char *stringid, unsigned int type_mask=0xFFFFFFFF);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentState(int channel_index, const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// См. описание в rdk_init.cpp
// Возвращает выборочные данные состояния компонента по идентификатору
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid);

// См. описание в rdk_init.cpp
// Возвращает значение параметра перменной состояния по идентификатору компонента и имени переменной
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentStateValue(int channel_index, const char *stringid, const char *statename);

// См. описание в rdk_init.cpp
// Устанавливает состояние компонента по идентификатору
RDK_LIB_TYPE int RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentState(int channel_index, const char *stringid, const char* buffer);

// См. описание в rdk_init.cpp
// Устанавливает значение переменной состояния компонента по идентификатору компонента и имени переменной
RDK_LIB_TYPE int RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_CreateLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);
RDK_LIB_TYPE int RDK_CALL MModel_CreateLinkByName(int channel_index, const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);
RDK_LIB_TYPE int RDK_CALL Model_CreateLinkByNameEx(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name, int connector_c_index);

// См. описание в rdk_init.cpp
// Разрывает выбранную связь
RDK_LIB_TYPE int RDK_CALL Model_BreakLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_BreakAllLinks(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentLinks(const char* stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentInputLinks(const char* stringid);
// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL MModel_BreakAllComponentInputLinks(int channel_index, const char* stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentOutputLinks(const char* stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE bool RDK_CALL Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number);
RDK_LIB_TYPE bool RDK_CALL Model_CheckLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);

/// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SwitchOutputLinks(const char* item_name_1, const char* item_property_name1, const char* item_name_2, const char* item_property_name2);
RDK_LIB_TYPE int RDK_CALL MModel_SwitchOutputLinks(int channel_index, const char* item_name_1, const char* item_property_name1, const char* item_name_2, const char* item_property_name2);

// --------------------------
// Методы управления алиасами свойств
// --------------------------
/// Добавляет алиас свойства вложенного компонента
RDK_LIB_TYPE int RDK_CALL Model_AddPropertyAlias(const char* stringid, const char* alias, const char* component_path,
                                                  const char* property_name, unsigned int property_type);
RDK_LIB_TYPE int RDK_CALL MModel_AddPropertyAlias(int channel_index, const char* stringid, const char* alias,
                                                   const char* component_path, const char* property_name, unsigned int property_type);

/// Удаляет алиас свойства
RDK_LIB_TYPE int RDK_CALL Model_DelPropertyAlias(const char* stringid, const char* alias);
RDK_LIB_TYPE int RDK_CALL MModel_DelPropertyAlias(int channel_index, const char* stringid, const char* alias);

/// Удаляет все алиасы свойств
RDK_LIB_TYPE int RDK_CALL Model_ClearPropertyAliases(const char* stringid);
RDK_LIB_TYPE int RDK_CALL MModel_ClearPropertyAliases(int channel_index, const char* stringid);

/// Проверяет наличие алиаса
RDK_LIB_TYPE bool RDK_CALL Model_CheckPropertyAlias(const char* stringid, const char* alias);
RDK_LIB_TYPE bool RDK_CALL MModel_CheckPropertyAlias(int channel_index, const char* stringid, const char* alias);

/// Возвращает информацию об алиасе в формате XML
RDK_LIB_TYPE const char* RDK_CALL Model_GetPropertyAlias(const char* stringid, const char* alias);
RDK_LIB_TYPE const char* RDK_CALL MModel_GetPropertyAlias(int channel_index, const char* stringid, const char* alias);

/// Возвращает все алиасы компонента в формате XML
RDK_LIB_TYPE const char* RDK_CALL Model_GetPropertyAliases(const char* stringid);
RDK_LIB_TYPE const char* RDK_CALL MModel_GetPropertyAliases(int channel_index, const char* stringid);

/// Возвращает все алиасы определенного типа (входы или выходы) в формате XML
RDK_LIB_TYPE const char* RDK_CALL Model_GetPropertyAliasesByType(const char* stringid, unsigned int type_mask);
RDK_LIB_TYPE const char* RDK_CALL MModel_GetPropertyAliasesByType(int channel_index, const char* stringid, unsigned int type_mask);

/// Создает связь с использованием алиасов
RDK_LIB_TYPE int RDK_CALL Model_CreateLinkByAlias(const char* stringid, const char* item_alias, const char* connector_alias);
RDK_LIB_TYPE int RDK_CALL MModel_CreateLinkByAlias(int channel_index, const char* stringid, const char* item_alias, const char* connector_alias);

/// Разрывает связь с использованием алиасов
RDK_LIB_TYPE int RDK_CALL Model_BreakLinkByAlias(const char* stringid, const char* item_alias, const char* connector_alias);
RDK_LIB_TYPE int RDK_CALL MModel_BreakLinkByAlias(int channel_index, const char* stringid, const char* item_alias, const char* connector_alias);
// --------------------------

// См. описание в rdk_init.cpp
// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid=0);

// См. описание в rdk_init.cpp
// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid=0);

// См. описание в rdk_init.cpp
// Возвращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid=0, int sublevel=-1);

// См. описание в rdk_init.cpp
// Возвращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid=0, int sublevel=-1);

// См. описание в rdk_init.cpp
// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// См. описание в rdk_init.cpp
// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid=0);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_GetComponentNumInputs(const char *stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputDataSize(const char *stringid, int index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputElementSize(const char *stringid, int index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputByteSize(const char *stringid, int index);

// См. описание в rdk_init.cpp
// Возвращает указатель на данные входа как на массив байт
RDK_LIB_TYPE unsigned char* RDK_CALL Model_GetComponentInputData(const char *stringid, int index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_GetComponentNumOutputs(const char *stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputDataSize(const char *stringid, int index);

// См. описание в rdk_init.cpp
// Возвращает размер элемента выхода в байтах

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputByteSize(const char *stringid, int index);

// См. описание в rdk_init.cpp
// Возвращает указатель на данные выхода как на массив байт
RDK_LIB_TYPE unsigned char* RDK_CALL Model_GetComponentOutputData(const char *stringid, int index);

// См. описание в rdk_init.cpp
// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponent(const char *stringid, unsigned int params_type_mask=ptPubParameter);
RDK_LIB_TYPE const char * RDK_CALL MModel_SaveComponent(int channel_index, const char *stringid, unsigned int params_type_mask=ptPubParameter);

// См. описание в rdk_init.cpp
// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
RDK_LIB_TYPE int RDK_CALL Model_SaveComponentToFile(const char *stringid, const char* file_name, unsigned int params_type_mask=0xFFFFFFFF);
RDK_LIB_TYPE int RDK_CALL MModel_SaveComponentToFile(int channel_index, const char *stringid, const char* file_name, unsigned int params_type_mask=0xFFFFFFFF);

// См. описание в rdk_init.cpp
// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
RDK_LIB_TYPE int RDK_CALL Model_LoadComponent(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_LoadComponent(int channel_index, const char *stringid, const char* buffer);

// См. описание в rdk_init.cpp
// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentFromFile(const char *stringid, const char* file_name);
RDK_LIB_TYPE int RDK_CALL MModel_LoadComponentFromFile(int channel_index, const char *stringid, const char* file_name);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentProperties(const char *stringid, unsigned int type_mask);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SaveComponentPropertiesToFile(const char *stringid, const char* file_name, unsigned int type_mask=0xFFFFFFFF);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentProperties(const char *stringid, char* buffer);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentPropertiesFromFile(const char *stringid, const char* file_name);

// См. описание в rdk_init.cpp
// Сохраняет все параметры компонента и его дочерних компонент в xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentParameters(const char *stringid, unsigned int type_mask=ptPubParameter);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const char * RDK_CALL MModel_SaveComponentParameters(int channel_index, const char *stringid, unsigned int type_mask=ptPubParameter);
// См. описание в rdk_init.cpp
// Загружает все параметры компонента и его дочерних компонент из xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentParameters(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_LoadComponentParameters(int channel_index, const char *stringid, const char* buffer);

// См. описание в rdk_init.cpp
// Сохраняет состояние компонента и его дочерних компонент в xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentState(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// См. описание в rdk_init.cpp
// Загружает состояние компонента и его дочерних компонент из xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentState(const char *stringid, char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_LoadComponentState(int channel_index, const char *stringid, char* buffer);

// См. описание в rdk_init.cpp
// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
RDK_LIB_TYPE const char* RDK_CALL Model_SaveComponentDrawInfo(const char *stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE unsigned int RDK_CALL Model_GetDefaultTimeStep(void);
RDK_LIB_TYPE int RDK_CALL Model_SetDefaultTimeStep(unsigned int value);
RDK_LIB_TYPE int RDK_CALL MModel_SetDefaultTimeStep(int channel_index, unsigned int value);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE unsigned int RDK_CALL Model_GetTimeStep(const char *stringid);
RDK_LIB_TYPE int RDK_CALL Model_SetTimeStep(const char *stringid, unsigned int value);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SetGlobalTimeStep(const char *stringid, unsigned int value);
RDK_LIB_TYPE int RDK_CALL MModel_SetGlobalTimeStep(int channel_index, const char *stringid, unsigned int value);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetTime(void);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetTime(int channel_index);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleTime(void);
RDK_LIB_TYPE double RDK_CALL MModel_GetDoubleTime(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SetTime(unsigned long long value);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetRealTime(void);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleRealTime(void);
RDK_LIB_TYPE double RDK_CALL MModel_GetDoubleRealTime(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SetRealTime(unsigned long long value);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_IncreaseRealTime(unsigned long long value);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetRealTimeStep(void);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleRealTimeStep(void);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetSourceTime(void);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetSourceTime(int channel_index);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleSourceTime(void);
RDK_LIB_TYPE double RDK_CALL MModel_GetDoubleSourceTime(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SetSourceTime(unsigned long long value);
RDK_LIB_TYPE int RDK_CALL MModel_SetSourceTime(int channel_index, unsigned long long value);
RDK_LIB_TYPE int RDK_CALL Model_SetSourceTimeAll(unsigned long long value);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SetDoubleSourceTime(double value);
RDK_LIB_TYPE int RDK_CALL MModel_SetDoubleSourceTime(int channel_index, double value);
RDK_LIB_TYPE int RDK_CALL Model_SetDoubleSourceTimeAll(double value);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_IncreaseSourceTime(unsigned long long value);
RDK_LIB_TYPE int RDK_CALL MModel_IncreaseSourceTime(int channel_index, unsigned long long value);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetSourceTimeStep(void);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetSourceTimeStep(int channel_index);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleSourceTimeStep(void);
RDK_LIB_TYPE double RDK_CALL MModel_GetDoubleSourceTimeStep(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetStepDuration(const char *stringid);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetStepDuration(int channel_index, const char *stringid);

// См. описание в rdk_init.cpp
// Возвращает время, затраченное на обработку объекта
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetFullStepDuration(const char *stringid);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetFullStepDuration(int channel_index, const char *stringid);

// См. описание в rdk_init.cpp
// Возвращает мгновенное быстродействие, равное отношению
RDK_LIB_TYPE double RDK_CALL Model_GetInstantPerformance(const char *stringid);
RDK_LIB_TYPE double RDK_CALL MModel_GetInstantPerformance(int channel_index, const char *stringid);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetInterstepsInterval(const char *stringid);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetInterstepsInterval(int channel_index, const char *stringid);
// --------------------------

// --------------------------
// --------------------------
// Функции управления моделью видеообработки
// --------------------------
// Возвращает указатель на выход с индексом 'index' компонента 'id'
// возвращаемое значение имеет фактический тип RDK::MDMatrix*
RDK_LIB_TYPE const // РµСЃР»Рё РІС‹С…РѕРґ РЅРµ СЃРѕРґРµСЂР¶РёС‚ РґР°РЅРЅС‹С… С‚Р°РєРѕРіРѕ С‚РёРїР°, С‚Рѕ РІРѕР·РІСЂР°С‰Р°РµС‚ 0
void* RDK_CALL Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrixByIndex(const char *stringid, int index);

// См. описание в rdk_init.cpp
// Возвращает указатель на выход с индексом 'index' компонента 'id'
RDK_LIB_TYPE const // РІРѕР·РІСЂР°С‰Р°РµРјРѕРµ Р·РЅР°С‡РµРЅРёРµ РёРјРµРµС‚ С„Р°РєС‚РёС‡РµСЃРєРёР№ С‚РёРї RDK::UBitmap*
void* RDK_CALL Model_GetComponentOutput(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /* RDK::UBitmap* */void* RDK_CALL Model_GetComponentOutputByIndex(const char *stringid, int index);

RDK_LIB_TYPE const /* RDK::UBitmap* */void* RDK_CALL MModel_GetComponentOutput(int channel_index, const char *stringid, const char *property_name);
RDK_LIB_TYPE const /* RDK::UBitmap* */void* RDK_CALL MModel_GetComponentOutputByIndex(int channel_index, const char *stringid, int index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const // Р’РѕР·РІСЂР°С‰Р°РµС‚ СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РІС‹С…РѕРґ СЃ РёРЅРґРµРєСЃРѕРј 'index' РєРѕРјРїРѕРЅРµРЅС‚Р° 'id'
 void* RDK_CALL Model_GetComponentBitmapOutput(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutputByIndex(const char *stringid, int index);

RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentBitmapOutput(int channel_index, const char *stringid, const char *property_name);
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentBitmapOutputByIndex(int channel_index, const char *stringid, int index);

/// См. описание в rdk_init.cpp
/// Копирует данные о разрешении изображения выхода с индексом 'index' компонента 'id'
RDK_LIB_TYPE int RDK_CALL Model_CopyComponentBitmapOutputHeader(const char *stringid, const char *property_name, /// РІ СЃС‚СЂРєС‚СѓСЂСѓ bmp_param
 void* bmp_param);
RDK_LIB_TYPE int RDK_CALL MModel_CopyComponentBitmapOutputHeader(int channel_index, const char *stringid, const char *property_name, /*RDK::UBitmapParam* */ void* bmp_param);
RDK_LIB_TYPE int RDK_CALL Model_CopyComponentBitmapOutputHeaderByIndex(const char *stringid, int index, /*RDK::UBitmapParam* */ void* bmp_param);
RDK_LIB_TYPE int RDK_CALL MModel_CopyComponentBitmapOutputHeaderByIndex(int channel_index, const char *stringid, int index, /*RDK::UBitmapParam* */ void* bmp_param);

/// См. описание в rdk_init.cpp
/// Копирует изображение выхода с индексом 'index' компонента 'id'
RDK_LIB_TYPE int RDK_CALL Model_CopyComponentBitmapOutput(const char *stringid, const char *property_name, /// РјРµС‚РѕРґ РїСЂРµРґРїРѕР»Р°РіР°РµС‚, С‡С‚Рѕ bmp СѓР¶Рµ РёРјРµРµС‚ РІС‹РґРµР»РµРЅРЅСѓСЋ РїР°РјСЏС‚СЊ РїРѕРґ РёР·РѕР±СЂР¶РµРЅРёРµ С‚СЂРµР±СѓРµРјРѕРіРѕ СЂР°Р·РјРµСЂР°
void* bmp);
RDK_LIB_TYPE int RDK_CALL MModel_CopyComponentBitmapOutput(int channel_index, const char *stringid, const char *property_name, /*RDK::UBitmap**/void* bmp);
RDK_LIB_TYPE int RDK_CALL Model_CopyComponentBitmapOutputByIndex(const char *stringid, int index, /*RDK::UBitmap**/void* bmp);
RDK_LIB_TYPE int RDK_CALL MModel_CopyComponentBitmapOutputByIndex(int channel_index, const char *stringid, int index, /*RDK::UBitmap**/void* bmp);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE const // Р’РѕР·РІСЂР°С‰Р°РµС‚ СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РІС…РѕРґ СЃ РёРЅРґРµРєСЃРѕРј 'index' РєРѕРјРїРѕРЅРµРЅС‚Р° 'id'
 void* RDK_CALL Model_GetComponentBitmapInput(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInputByIndex(const char *stringid, int index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const // Р—Р°РјРµС‰Р°РµС‚ РёР·РѕР±СЂР°Р¶РµРЅРёРµ РІС‹С…РѕРґР° СЃ РёРЅРґРµРєСЃРѕРј 'index' РєРѕРјРїРѕРЅРµРЅС‚Р° 'id'
 void* const bmp, bool reflect=false);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentBitmapOutput(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentBitmapOutputUnsafe(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);

RDK_LIB_TYPE int RDK_CALL Model_SetComponentBitmapOutputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentBitmapOutputByIndex(int channel_index, const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE int RDK_CALL Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const // Р—Р°РјРµС‰Р°РµС‚ РёР·РѕР±СЂР°Р¶РµРЅРёРµ РІС…РѕРґР° СЃ РёРЅРґРµРєСЃРѕРј 'index' РєРѕРјРїРѕРЅРµРЅС‚Р° 'id'
 void* const bmp, bool reflect=false);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentBitmapInput(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE int RDK_CALL Model_SetComponentBitmapInputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
/*RDK::UBitmap* */

// --------------------------
// --------------------------
// Функции управления исключениями
// ----------------------------
// Управление функцией-обработчиком исключений
// deprecated. See Log_ functions
//RDK_LIB_TYPE void* RDK_CALL Engine_GetExceptionHandler(void);
//RDK_LIB_TYPE void* RDK_CALL MEngine_GetExceptionHandler(int channel_index);
//RDK_LIB_TYPE int RDK_CALL Engine_SetExceptionHandler(void* value);

// См. описание в rdk_init.cpp
// Возвращает массив строк лога
// deprecated. See Log_ functions
//RDK_LIB_TYPE const char* RDK_CALL Engine_GetLog(int &error_level);

// См. описание в rdk_init.cpp
// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
// deprecated. See Log_ functions
//RDK_LIB_TYPE const char* RDK_CALL Engine_GetUnreadLog(int &error_level, int &number, unsigned long long &time);
//RDK_LIB_TYPE const char* RDK_CALL MEngine_GetUnreadLog(int channel_index, int &error_level, int &number, unsigned long long &time);
//RDK_LIB_TYPE const char* RDK_CALL Engine_GetUnreadLogUnsafe(int &error_level, int &number, unsigned long long &time);

// См. описание в rdk_init.cpp
// Записывает в лог новое сообщение
// deprecated
//RDK_LIB_TYPE int RDK_CALL Engine_LogMessage(int log_level, const char *message);

// См. описание в rdk_init.cpp
// Записывает в лог новое сообщение с кодом ошибки
// deprecated
//RDK_LIB_TYPE int RDK_CALL Engine_LogMessageEx(int log_level, const char *message, int error_event_number);

/// См. описание в rdk_init.cpp
/// Возвращает число непрочитанных строк лога
// deprecated
//RDK_LIB_TYPE int RDK_CALL Engine_GetNumUnreadLogLines(void);

/// См. описание в rdk_init.cpp
/// Возвращает число строк лога
// deprecated
//RDK_LIB_TYPE int RDK_CALL Engine_GetNumLogLines(void);

/// См. описание в rdk_init.cpp
/// Очищает лог прочитанных сообщений
// deprecated
//RDK_LIB_TYPE int RDK_CALL Engine_ClearReadLog(void);
//RDK_LIB_TYPE int RDK_CALL MEngine_ClearReadLog(int channel_index);

// См. описание в rdk_init.cpp
RDK_LIB_TYPE void tss_cleanup_implemented(void);

#ifdef __cplusplus
}
#endif

#endif
