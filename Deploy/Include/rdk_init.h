#ifndef RDK_INIT_H
#define RDK_INIT_H

#include "rdk_error_codes.h"

#ifdef __cplusplus
extern "C"  {
#else
typedef int bool;
#endif

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

/// Идентификатор системного канала логгирования
#ifndef RDK_SYS_MESSAGE
#define RDK_SYS_MESSAGE -1
#endif


#ifndef RDK_PROPERTY_TYPES
#define RDK_PROPERTY_TYPES
// Варианты типа свойства (битовая маска) pt - Property Type
// 0x1 - Параметр
// 0x2 - Переменная состояния
// 0x4 - Временная переменная
// 0x8 - Вход
enum {ptParameter=1, ptState=2, ptTemp=4, ptInput=8, ptOutput=16, ptAny=255};

// Варианты групп свойства (битовая маска) pg - Property Group
// 0x100 - Общедоступный
// 0x200 - Системный
// 0x400 - Входные данные
// 0x800 - Выходные данные
// 0x1000 - Флаг смены режима работы компонента
enum {pgPublic=0x100, pgSystem=0x200, pgInput=0x400, pgOutput=0x800, pgMode=0x1000, pgAny=0xFFFFFF};

// Наиболее часто используемые сочетания типа и группы
enum {ptPubParameter=ptParameter|pgPublic, ptPubState=ptState|pgPublic, ptPubInput=ptInput|pgPublic, ptPubOutput=ptOutput|pgPublic};

enum { ipData=1, ipComp=2 };
enum { ipSingle=16, ipRange=32, ipList=64 };
enum { ipDataSingle=ipData|ipSingle, ipDataRange=ipData|ipRange,
       ipDataList=ipData|ipList, ipCompSingle=ipComp|ipSingle,
       ipCompRange=ipComp|ipRange, ipCompList=ipComp|ipList };
#endif

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
/// </RpcResponse>
RDK_LIB_TYPE const char* RDK_CALL RemoteCall(const char *request, int &return_value, int &channel_index);
// ----------------------------

// ----------------------------
// Функции логирования
// ----------------------------
// Возвращает состояние внутренего логгирования
RDK_LIB_TYPE bool RDK_CALL Log_GetEventsLogMode(void);
RDK_LIB_TYPE bool RDK_CALL MLog_GetEventsLogMode(int channel_index);

// Включает/выключает внутренне логгирование
RDK_LIB_TYPE int RDK_CALL Log_SetEventsLogMode(bool value);
RDK_LIB_TYPE int RDK_CALL MLog_SetEventsLogMode(int channel_index, bool value);

// !
// Устанавливает имя текущего каталога для хранения данных
RDK_LIB_TYPE int RDK_CALL Log_SetCurrentDataDir(const char *dir);
RDK_LIB_TYPE int RDK_CALL MLog_SetCurrentDataDir(int channel_index, const char *dir);

/// Возвращает состояние флага отладочного режима среды
RDK_LIB_TYPE bool RDK_CALL Log_GetDebugMode(void);
RDK_LIB_TYPE bool RDK_CALL MLog_GetDebugMode(int channel_index);

/// Устанавливает состояние флага отладочного режима среды
RDK_LIB_TYPE int RDK_CALL Log_SetDebugMode(bool value);
RDK_LIB_TYPE int RDK_CALL MLog_SetDebugMode(int channel_index, bool value);

/// Возвращает маску системных событий для логирования
RDK_LIB_TYPE unsigned int RDK_CALL Log_GetDebugSysEventsMask(void);
RDK_LIB_TYPE unsigned int RDK_CALL MLog_GetDebugSysEventsMask(int channel_index);

/// Устанавливает маску системных событий для логирования
RDK_LIB_TYPE int RDK_CALL Log_SetDebugSysEventsMask(unsigned int value);
RDK_LIB_TYPE int RDK_CALL MLog_SetDebugSysEventsMask(int channel_index, unsigned int value);

/// Возвращает флаг включения вывода лога в отладчик
RDK_LIB_TYPE bool RDK_CALL Log_GetDebuggerMessageFlag(void);
RDK_LIB_TYPE bool RDK_CALL MLog_GetDebuggerMessageFlag(int channel_index);

/// Устанавливает флаг включения вывода лога в отладчик
RDK_LIB_TYPE bool RDK_CALL Log_SetDebuggerMessageFlag(bool value);
RDK_LIB_TYPE bool RDK_CALL MLog_SetDebuggerMessageFlag(int channel_index, bool value);
// !

RDK_LIB_TYPE void* RDK_CALL Log_GetExceptionHandler(void);
RDK_LIB_TYPE void* RDK_CALL MLog_GetExceptionHandler(int channel_index);
RDK_LIB_TYPE int RDK_CALL Log_SetExceptionHandler(void* value);
RDK_LIB_TYPE int RDK_CALL MLog_SetExceptionHandler(int channel_index, void* value);

// Возвращает массив строк лога
RDK_LIB_TYPE const char* RDK_CALL Log_GetLog(int &error_level);
RDK_LIB_TYPE const char* RDK_CALL MLog_GetLog(int channel_index, int &error_level);

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
RDK_LIB_TYPE const char* RDK_CALL Log_GetUnreadLog(int &error_level, int &number, unsigned long long &time);
RDK_LIB_TYPE const char* RDK_CALL MLog_GetUnreadLog(int channel_index, int &error_level, int &number, unsigned long long &time);
RDK_LIB_TYPE const char* RDK_CALL Log_GetUnreadLogUnsafe(int &error_level, int &number, unsigned long long &time);
RDK_LIB_TYPE const char* RDK_CALL MLog_GetUnreadLogUnsafe(int channel_index, int &error_level, int &number, unsigned long long &time);

// Записывает в лог новое сообщение
RDK_LIB_TYPE int RDK_CALL Log_LogMessage(int log_level, const char *message);
RDK_LIB_TYPE int RDK_CALL MLog_LogMessage(int channel_index, int log_level, const char *message);

// Записывает в лог новое сообщение с кодом ошибки
RDK_LIB_TYPE int RDK_CALL Log_LogMessageEx(int log_level, const char *message, int error_event_number);
RDK_LIB_TYPE int RDK_CALL MLog_LogMessageEx(int channel_index, int log_level, const char *message, int error_event_number);

/// Возвращает число непрочитанных строк лога
RDK_LIB_TYPE int RDK_CALL Log_GetNumUnreadLogLines(void);
RDK_LIB_TYPE int RDK_CALL MLog_GetNumUnreadLogLines(int channel_index);

/// Возвращает число строк лога
RDK_LIB_TYPE int RDK_CALL Log_GetNumLogLines(void);
RDK_LIB_TYPE int RDK_CALL MLog_GetNumLogLines(int channel_index);

/// Очищает лог прочитанных сообщений
RDK_LIB_TYPE int RDK_CALL Log_ClearReadLog(void);
RDK_LIB_TYPE int RDK_CALL MLog_ClearReadLog(int channel_index);
// ----------------------------

// ----------------------------
// Функции управления ядром
// ----------------------------
// Возвращает имя каталога бинарных файлов
RDK_LIB_TYPE const char* RDK_CALL Core_GetSystemDir(void);
RDK_LIB_TYPE const char* RDK_CALL GetSystemDir(void); // deprecated

// Устанавливает имя каталога бинарных файлов
RDK_LIB_TYPE int RDK_CALL Core_SetSystemDir(const char *dir);
RDK_LIB_TYPE int RDK_CALL SetSystemDir(const char *dir); // deprecated

// Возвращает имя каталога бинарных файлов
RDK_LIB_TYPE const char* RDK_CALL Core_GetLogDir(void);

// Устанавливает имя каталога бинарных файлов
RDK_LIB_TYPE int RDK_CALL Core_SetLogDir(const char *dir);

// Возвращает глобальную настройку включения отладочного режима
RDK_LIB_TYPE bool RDK_CALL Core_GetDebugMode(void);

// Устанавливает глобальную настройку включения отладочного режима
RDK_LIB_TYPE int RDK_CALL Core_SetDebugMode(bool value);

// Загружает глобальные шрифты
RDK_LIB_TYPE int RDK_CALL Core_LoadFonts(void);
RDK_LIB_TYPE int RDK_CALL Engine_LoadFonts(void); // deprecated

// Возвращает число дивжков
RDK_LIB_TYPE int RDK_CALL Core_GetNumChannels(void);
RDK_LIB_TYPE int RDK_CALL GetNumEngines(void); // deprecated

// Создает требуемое число движков
// num > 0
RDK_LIB_TYPE int RDK_CALL Core_SetNumChannels(int num);
RDK_LIB_TYPE int RDK_CALL SetNumEngines(int num); // deprecated

// Добавляет движок в позицию заданного индекса
// Если позиция лежит вне пределов диапазона то
// добавляет в конец
RDK_LIB_TYPE int RDK_CALL Core_AddChannel(int index);
RDK_LIB_TYPE int RDK_CALL Engine_Add(int index); // deprecated

// Удаляет движок по индексу
RDK_LIB_TYPE int RDK_CALL Core_DelChannel(int index);
RDK_LIB_TYPE int RDK_CALL Engine_Del(int index); // deprecated

// Возвращает индекс текущего выбранного движка
RDK_LIB_TYPE int RDK_CALL Core_GetSelectedChannelIndex(void);
RDK_LIB_TYPE int RDK_CALL GetSelectedEngineIndex(void); // deprecated

// Настраивает обычный интерфейс на работу с заданным движком
// В случае удаления движка, интерфейс автоматически перенастраивается на 0 движок
RDK_LIB_TYPE int RDK_CALL Core_SelectChannel(int index);
RDK_LIB_TYPE int RDK_CALL SelectEngine(int index); // deprecated

/// Блокирует канал до вызова функции UnlockEngine, Core_UnlockChannel
RDK_LIB_TYPE int RDK_CALL Core_LockChannel(void);
RDK_LIB_TYPE int RDK_CALL LockEngine(void); // deprecated
RDK_LIB_TYPE int RDK_CALL MCore_LockChannel(int index);
RDK_LIB_TYPE int RDK_CALL MLockEngine(int index); // deprecated

/// Разблокирует канал
RDK_LIB_TYPE int RDK_CALL Core_UnLockChannel(void);
RDK_LIB_TYPE int RDK_CALL UnLockEngine(void); // deprecated
RDK_LIB_TYPE int RDK_CALL MCore_UnLockChannel(int index);
RDK_LIB_TYPE int RDK_CALL MUnLockEngine(int index); // deprecated

// Инициализирует движок (функция должна быть вызвана первой!)
// Upd: Функция может быть вызвана после SetNumEngines и SelectEngine
RDK_LIB_TYPE int RDK_CALL Core_ChannelInit(int predefined_structure, void* exception_handler=0);
RDK_LIB_TYPE int RDK_CALL EngineInit(int predefined_structure, void* exception_handler=0); // deprecated
RDK_LIB_TYPE int RDK_CALL MCore_ChannelInit(int channel_index, int predefined_structure, void* exception_handler=0);
RDK_LIB_TYPE int RDK_CALL MEngineInit(int channel_index, int predefined_structure, void* exception_handler=0); // deprecated

// Инициализирует графический движок (функция должна быть вызвана первой!)
// Upd: Функция может быть вызвана после SetNumEngines и SelectEngine
// deprecated
RDK_LIB_TYPE int RDK_CALL GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx=false,
		void* exception_handler=0); // deprecated
RDK_LIB_TYPE int RDK_CALL MGraphicalEngineInit(int channel_index, int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx=false,
		void* exception_handler=0); // deprecated

// Деинициализирует движок (функция автоматически вызывается при вызове инициализации)
RDK_LIB_TYPE int RDK_CALL Core_ChannelUnInit(void);
RDK_LIB_TYPE int RDK_CALL EngineUnInit(void); // deprecated
RDK_LIB_TYPE int RDK_CALL MCore_ChannelUnInit(int channel_index);
RDK_LIB_TYPE int RDK_CALL MEngineUnInit(int channel_index); // deprecated

/// Проверяет инициализирован ли движок
RDK_LIB_TYPE bool RDK_CALL Core_IsChannelInit(void);
RDK_LIB_TYPE bool RDK_CALL IsEngineInit(void); // deprecated
RDK_LIB_TYPE bool RDK_CALL MCore_IsChannelInit(int channel_index);
RDK_LIB_TYPE bool RDK_CALL MIsEngineInit(int channel_index); // deprecated

/// Высвобождает буферную строку движка, по заданному указателю
RDK_LIB_TYPE int RDK_CALL Engine_FreeBufString(const char *pointer);
RDK_LIB_TYPE int RDK_CALL MEngine_FreeBufString(int channel_index,const char *pointer);
RDK_LIB_TYPE int RDK_CALL Engine_FreeBufStringUnsafe(const char *pointer);
RDK_LIB_TYPE int RDK_CALL MEngine_FreeBufStringUnsafe(int channel_index,const char *pointer);

/// Режим создания внутренних временных переменных для
/// возвращаемых значений
/// 0 - одна переменная для всех методов, возвращающих такой тип
/// 1 - уникальные переменные с необходимостью вызвова функции очистки
RDK_LIB_TYPE int RDK_CALL Engine_GetBufObjectsMode(void);
RDK_LIB_TYPE int RDK_CALL Engine_SetBufObjectsMode(int mode);

/// Возвращает число буферных строк движка
RDK_LIB_TYPE int RDK_CALL Engine_GetNumBufStrings(void);
RDK_LIB_TYPE int RDK_CALL MEngine_GetNumBufStrings(int channel_index);

/// Доступ к мьютексу
RDK_LIB_TYPE void* RDK_CALL Engine_GetMutex(void);
RDK_LIB_TYPE void* RDK_CALL MEngine_GetMutex(int index);
// ----------------------------

// --------------------------
// Функции управления хранилищем
// ----------------------------
// Возвращает число классов в хранилище
RDK_LIB_TYPE int RDK_CALL Storage_GetNumClasses(void);

// Возвращает id классов в хранилище. Память должна быть выделена
RDK_LIB_TYPE int RDK_CALL Storage_GetClassesList(int *buffer);

// Возвращает имена классов в хранилище в виде строки разделенной запятыми
RDK_LIB_TYPE const char * RDK_CALL Storage_GetClassesNameList(void);

// Возвращает имя класса по его id.
RDK_LIB_TYPE const char * RDK_CALL Storage_GetClassName(int id);

// Возвращает Id класса по его имени
RDK_LIB_TYPE int RDK_CALL Storage_GetClassId(const char *name);

// Удаляет образец класса объекта из хранилища
// Возвращает ошибку если classid не найден,
// или присутствуют объекты этого класса
RDK_LIB_TYPE int RDK_CALL Storage_DelClass(int classid);

// Удаляет все свободные объекты из хранилища
RDK_LIB_TYPE int RDK_CALL Storage_FreeObjectsStorage(void);

// Удаляет все объекты из хранилища
RDK_LIB_TYPE int RDK_CALL Storage_ClearObjectsStorage(void);

// Вычисляет суммарное число объектов в хранилище
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjects(void);
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjectsById(int classid);
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjectsByName(const char* classname);

// Возвращает описание класса по его id в формате xml
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassDescription(const char* classname);

// Устанавливает описание класса по его id, считывая его из формата xml
RDK_LIB_TYPE int RDK_CALL Storage_SetClassDescription(const char* classname, const char* description);

// Сохраняет описание всех классов в xml
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveClassesDescription(void);

// Загружает описание всех классов из xml
RDK_LIB_TYPE int RDK_CALL Storage_LoadClassesDescription(const char* xmltext);
RDK_LIB_TYPE int RDK_CALL MStorage_LoadClassesDescription(int channel_index, const char* xmltext);

// Сохраняет общее описание всех классов в xml
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveCommonClassesDescription(void);

// Загружает общее описание всех классов из xml
RDK_LIB_TYPE int RDK_CALL Storage_LoadCommonClassesDescription(const char* xmltext);
RDK_LIB_TYPE int RDK_CALL MStorage_LoadCommonClassesDescription(int channel_index, const char* xmltext);

// Сохраняет описание всех классов в xml включая общее описание
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveAllClassesDescription(void);

// Загружает описание всех классов из xml включая общее описание
RDK_LIB_TYPE int RDK_CALL Storage_LoadAllClassesDescription(const char* xmltext);
// --------------------------

// ----------------------------
// Методы управления коллекциями компонент
// ----------------------------
// Возвращает число библиотек
RDK_LIB_TYPE int RDK_CALL Storage_GetNumClassLibraries(void);

// Возвращает список библиотек в виде строки, разделенной запятыми
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassLibrariesList(void);

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// library_name - имя библиотеки
RDK_LIB_TYPE const char* RDK_CALL Storage_GetLibraryClassNames(const char *library_name);

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// index - индекс библиотеки
RDK_LIB_TYPE const char* RDK_CALL Storage_GetLibraryClassNamesByIndex(int index);

// Возвращает имя библиотеки по индексу
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassLibraryNameByIndex(int index);

// Возвращает версию библиотеки по индексу
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassLibraryVersionByIndex(int index);

/// Создает новую runtime-библиотеку
RDK_LIB_TYPE int RDK_CALL Storage_CreateRuntimeCollection(const char *collection_name);

// Загружает коллекцию по имени dll-файла
RDK_LIB_TYPE int RDK_CALL Storage_LoadBinaryCollectionFromFile(const char *filename);

// Загружает runtime-коллекцию
RDK_LIB_TYPE int RDK_CALL Storage_LoadRuntimeCollectionFromFile(const char *filename);
RDK_LIB_TYPE int RDK_CALL Storage_LoadRuntimeCollectionFromString(const char *buffer);

// Сохраняет runtime-коллекцию
RDK_LIB_TYPE int RDK_CALL Storage_SaveRuntimeCollectionToFile(const char *filename);
RDK_LIB_TYPE int RDK_CALL Storage_SaveRuntimeCollectionToString(const char *buffer);

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
RDK_LIB_TYPE int RDK_CALL Storage_DelClassLibraryByIndex(int index);

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
RDK_LIB_TYPE int RDK_CALL Storage_DelClassLibraryByName(const char *name);

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
RDK_LIB_TYPE int RDK_CALL Storage_DelAllClassLibraries(void);

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
RDK_LIB_TYPE int RDK_CALL Storage_CreateClass(const char* stringid, const char *class_name, const char *collection_name);

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
RDK_LIB_TYPE int RDK_CALL Storage_BuildStorage(void);
// ----------------------------


// --------------------------
// Функции управления средой
// ----------------------------
// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
RDK_LIB_TYPE int RDK_CALL Env_Calculate(const char* stringid);
RDK_LIB_TYPE int RDK_CALL MEnv_Calculate(int channel_index, const char* stringid);
RDK_LIB_TYPE int RDK_CALL MEnv_CalculateUnsafe(int channel_index, const char* stringid);

// Расчет всей модели в реальном времени
RDK_LIB_TYPE int RDK_CALL Env_RTCalculate(void);
RDK_LIB_TYPE int RDK_CALL MEnv_RTCalculate(int channel_index);

// Метод сброса счета
// Если stringid == 0 то сбрасывает всю модель целиком,
// иначе - только указанный компонент модели
RDK_LIB_TYPE int RDK_CALL Env_Reset(const char* stringid);
RDK_LIB_TYPE int RDK_CALL MEnv_Reset(int channel_index, const char* stringid);

/// Метод сброса параметров на значения по умолчанию
/// Если stringid == 0 то сбрасывает всю модель целиком,
/// иначе - только указанный компонент модели
/// Если subcomps == true то также сбрасывает параметры всех дочерних компонент
RDK_LIB_TYPE int RDK_CALL Env_Default(const char* stringid, bool subcomps=false);
RDK_LIB_TYPE int RDK_CALL MEnv_Default(int channel_index, const char* stringid, bool subcomps=false);

// Производит увеличение времени модели на требуемую величину
RDK_LIB_TYPE int RDK_CALL Env_IncreaseModelTimeByStep(void);

/// Устанавливает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
RDK_LIB_TYPE int RDK_CALL Env_SetMinInterstepsInterval(unsigned long long value);
RDK_LIB_TYPE int RDK_CALL MEnv_SetMinInterstepsInterval(int channel_index, unsigned long long value);

/// Возвращает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
RDK_LIB_TYPE unsigned long long RDK_CALL Env_GetMinInterstepsInterval(void);
RDK_LIB_TYPE unsigned long long RDK_CALL MEnv_GetMinInterstepsInterval(int channel_index);

// Время, потраченное на последний RT-расчет
RDK_LIB_TYPE double RDK_CALL Env_GetRTLastDuration(void);
RDK_LIB_TYPE double RDK_CALL MEnv_GetRTLastDuration(int channel_index);

/// Время, расчитанное в модели за один вызов RTCalculate;
RDK_LIB_TYPE double RDK_CALL Env_GetRTModelCalcTime(void);
RDK_LIB_TYPE double RDK_CALL MEnv_GetRTModelCalcTime(int channel_index);

/// Производительность RT расчета (отношение RTModelCalcTime/RTLastDuration)
RDK_LIB_TYPE double RDK_CALL Env_CalcRTPerformance(void);
RDK_LIB_TYPE double RDK_CALL MEnv_CalcRTPerformance(int channel_index);

// Возвращает имя текущего каталога для хранения данных
RDK_LIB_TYPE const char* RDK_CALL Env_GetCurrentDataDir(void);

// Устанавливает имя текущего каталога для хранения данных
RDK_LIB_TYPE int RDK_CALL Env_SetCurrentDataDir(const char *dir);
RDK_LIB_TYPE int RDK_CALL MEnv_SetCurrentDataDir(int channel_index, const char *dir);

/// Возвращает состояние флага отладочного режима среды
/// Deprecated
RDK_LIB_TYPE bool RDK_CALL Env_GetDebugMode(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_GetDebugMode(int channel_index);

/// Устанавливает состояние флага отладочного режима среды
/// Deprecated
RDK_LIB_TYPE int RDK_CALL Env_SetDebugMode(bool value);
RDK_LIB_TYPE int RDK_CALL MEnv_SetDebugMode(int channel_index, bool value);

/// Возвращает маску системных событий для логирования
/// Deprecated
RDK_LIB_TYPE unsigned int RDK_CALL Env_GetDebugSysEventsMask(void);
RDK_LIB_TYPE unsigned int RDK_CALL MEnv_GetDebugSysEventsMask(int channel_index);

/// Устанавливает маску системных событий для логирования
/// Deprecated
RDK_LIB_TYPE int RDK_CALL Env_SetDebugSysEventsMask(unsigned int value);
RDK_LIB_TYPE int RDK_CALL MEnv_SetDebugSysEventsMask(int channel_index, unsigned int value);

/// Возвращает флаг включения вывода лога в отладчик
/// Deprecated
RDK_LIB_TYPE bool RDK_CALL Env_GetDebuggerMessageFlag(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_GetDebuggerMessageFlag(int channel_index);

/// Устанавливает флаг включения вывода лога в отладчик
/// Deprecated
RDK_LIB_TYPE bool RDK_CALL Env_SetDebuggerMessageFlag(bool value);
RDK_LIB_TYPE bool RDK_CALL MEnv_SetDebuggerMessageFlag(int channel_index, bool value);

// ***********************************************
// Методы управления текущим компонентом
// !!! Следующие методы влияют на все
// методы, обращающиеся к компонентам по строковому id !!!
// ***********************************************
// Устанавливает текущий компонент (адресация относительно корня - модели)
RDK_LIB_TYPE int RDK_CALL Env_SelectCurrentComponent(const char *stringid);

// Сбрасывает текущий компонент в состояние по умолчению (модель)
RDK_LIB_TYPE int RDK_CALL Env_ResetCurrentComponent(const char *stringid);

// Меняет текущий компонент на его родителя (подъем на уровень вверх)
// Если уже на верхнем уровне, то не делает ничего
RDK_LIB_TYPE int RDK_CALL Env_UpCurrentComponent(void);

// Меняет текущий компонент на его дочерний на произвольном уровне вложенности
// (спуск на N уровней вниз относительно текущего компонента)
RDK_LIB_TYPE int RDK_CALL Env_DownCurrentComponent(const char *stringid);

// Возвращает длинное имя текущего компонента
RDK_LIB_TYPE const char* RDK_CALL Env_GetCurrentComponentName(void);

// Возвращает длинный строковой id текущего компонента
RDK_LIB_TYPE const char* RDK_CALL Env_GetCurrentComponentId(void);
// ***********************************************

// ***********************************************
// Вспомогательные функции управления средой. обычно вызов не требуется
// ***********************************************
// Индекс предварительно заданной модели обработки
RDK_LIB_TYPE int RDK_CALL Env_GetPredefinedStructure(void);
RDK_LIB_TYPE int RDK_CALL MEnv_GetPredefinedStructure(int channel_index);
RDK_LIB_TYPE int RDK_CALL Env_SetPredefinedStructure(int value);
RDK_LIB_TYPE int RDK_CALL MEnv_SetPredefinedStructure(int channel_index, int value);

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
RDK_LIB_TYPE bool RDK_CALL Env_IsStoragePresent(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_IsStoragePresent(int channel_index);

// Возвращает состояние инициализации
RDK_LIB_TYPE bool RDK_CALL Env_IsInit(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_IsInit(int channel_index);

// Признак наличия сформированной структуры
RDK_LIB_TYPE bool RDK_CALL Env_IsStructured(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_IsStructured(int channel_index);

// Инициализация среды
RDK_LIB_TYPE int RDK_CALL Env_Init(void);
RDK_LIB_TYPE int RDK_CALL MEnv_Init(int channel_index);

// Возвращает состояние внутренего логгирования
// deprecated. use Log_GetEventsLogMode
RDK_LIB_TYPE bool RDK_CALL Env_GetEventsLogMode(void);
RDK_LIB_TYPE bool RDK_CALL MEnv_GetEventsLogMode(int channel_index);

// Включает/выключает внутренне логгирование
// deprecated. Use Log_SetEventsLogMode
RDK_LIB_TYPE int RDK_CALL Env_SetEventsLogMode(bool value);
RDK_LIB_TYPE int RDK_CALL MEnv_SetEventsLogMode(int channel_index, bool value);

// Деинициализация среды
RDK_LIB_TYPE int RDK_CALL Env_UnInit(void);
RDK_LIB_TYPE int RDK_CALL MEnv_UnInit(int channel_index);

// Формирует предварительно заданную модель обработки
RDK_LIB_TYPE int RDK_CALL Env_CreateStructure(void);
RDK_LIB_TYPE int RDK_CALL MEnv_CreateStructure(int channel_index);

// Уничтожает текущую модель обработки
RDK_LIB_TYPE int RDK_CALL Env_DestroyStructure(void);
RDK_LIB_TYPE int RDK_CALL MEnv_DestroyStructure(int channel_index);

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
RDK_LIB_TYPE int RDK_CALL Env_Destroy(void);
RDK_LIB_TYPE int RDK_CALL MEnv_Destroy(int channel_index);
// ***********************************************
// ----------------------------


// --------------------------
// Функции управления средой видеообработки
// --------------------------
// Задает число входов среды
RDK_LIB_TYPE void RDK_CALL Env_SetNumInputImages(int number);
RDK_LIB_TYPE void RDK_CALL MEnv_SetNumInputImages(int channel_index, int number);

// Задает число выходов среды
RDK_LIB_TYPE void RDK_CALL Env_SetNumOutputImages(int number);
RDK_LIB_TYPE void RDK_CALL MEnv_SetNumOutputImages(int channel_index, int number);

// Возвращает число входов среды
RDK_LIB_TYPE int RDK_CALL Env_GetNumInputImages(void);

// Возвращает число выходов среды
RDK_LIB_TYPE int RDK_CALL Env_GetNumOutputImages(void);

// Задает разрешение по умолчанию (рабочее разрешение)
RDK_LIB_TYPE void RDK_CALL Env_SetInputRes(int number, int width, int height);
RDK_LIB_TYPE void RDK_CALL MEnv_SetInputRes(int channel_index, int number, int width, int height);

// Задает данные изображения
RDK_LIB_TYPE void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

// Задает флаг отражения входного изображения вокруг горизонтальной оси
RDK_LIB_TYPE void Env_SetReflectionXFlag(bool value);
RDK_LIB_TYPE void MEnv_SetReflectionXFlag(int channel_index, bool value);

// Возвращает разрешение по умолчанию (рабочее разрешение)
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageWidth(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageHeight(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageColorModel(int number);

// Возвращает текущее выходное разрешение
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageWidth(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageHeight(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageColorModel(int number);

// Возвращает данные выходного изображения
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetInputImage(int index);
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetOutputImage(int index);
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetOutputImageY8(int index);

/// Инициирует извещение о сбое в работе источника данных
RDK_LIB_TYPE int RDK_CALL Env_CallSourceController(void);
RDK_LIB_TYPE int RDK_CALL MEnv_CallSourceController(int channel_index);
// --------------------------

// ----------------------------
// Методы управления моделью
// ----------------------------
// Удаляет модель
RDK_LIB_TYPE int RDK_CALL Model_Destroy(void);
RDK_LIB_TYPE int RDK_CALL MModel_Destroy(int channel_index);

// Создает новую модель по имени класса в хранилище
// Предварительно удаляет существующую модель
RDK_LIB_TYPE int RDK_CALL Model_Create(const char *classname);
RDK_LIB_TYPE int RDK_CALL MModel_Create(int channel_index, const char *classname);

// Очищает модель
RDK_LIB_TYPE int RDK_CALL Model_Clear(void);
RDK_LIB_TYPE int RDK_CALL MModel_Clear(int channel_index);

// Проверяет, существует ли модель
RDK_LIB_TYPE bool RDK_CALL Model_Check(void);
RDK_LIB_TYPE bool RDK_CALL MModel_Check(int channel_index);

// Проверяет, существует ли в модели компонент с именем stringid)
RDK_LIB_TYPE bool RDK_CALL Model_CheckComponent(const char* stringid);
RDK_LIB_TYPE bool RDK_CALL MModel_CheckComponent(int channel_index, const char* stringid);

// Добавляет в выбранный компонент модели с идентификатором 'stringid' экземпляр
// компонента с заданным 'classname'
// если stringid - пустая строка, то добавляет в саму модель
// Возвращает имя компонента в случае успеха
RDK_LIB_TYPE const char* RDK_CALL Model_AddComponent(const char* stringid, const char *classname);
RDK_LIB_TYPE const char* RDK_CALL MModel_AddComponent(int channel_index, const char* stringid, const char *classname);

// Удаляет из выбранного компонента модели с идентификатором 'stringid' экземпляр
// компонента с заданным 'name'
// если stringid - пустая строка, то удаляет из самой модели
RDK_LIB_TYPE int RDK_CALL Model_DelComponent(const char* stringid, const char *name);
RDK_LIB_TYPE int RDK_CALL MModel_DelComponent(int channel_index, const char* stringid, const char *name);

/// Перемещает компоненту в другой компонент
/// Если comp не принадлежит этому компоненту, или target имеет отличный от
/// этого компонента storage, или target не может принять в себя компонент
/// то возвращает false и не делает ничего
RDK_LIB_TYPE int RDK_CALL Model_MoveComponent(const char* component, const char* target);
RDK_LIB_TYPE int RDK_CALL MModel_MoveComponent(int channel_index, const char* component, const char* target);

// Возвращает число всех компонент в заданном компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
RDK_LIB_TYPE int RDK_CALL Model_GetNumComponents(const char* stringid);

// Возвращает массив id всех компонент заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
RDK_LIB_TYPE int RDK_CALL Model_GetComponentsList(const char* stringid, int *buffer);

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ','
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentsNameList(const char* stringid);
RDK_LIB_TYPE const char* RDK_CALL MModel_GetComponentsNameList(int channel_index, const char* stringid);

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ',' и имеющих имя класса 'class_name'
// Если find_all == true то поиск ведется и во всех сабкомпонентах
RDK_LIB_TYPE const char* RDK_CALL Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all);

// Перемещает компонент с текущим индексом index или именем 'name' вверх или
// вниз по списку на заданное число элементов
// Применяется для изменения порядка расчета компонент
// Если значение 'step' выводит за границы массива, то компонент устанавливается
// на эту границу
RDK_LIB_TYPE int RDK_CALL Model_ChangeComponentPosition(const char* stringid, int step);

// Возвращает xml-список длинных идентификаторов всех коннекторов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
// Предварительная очистка буфера не производится.
RDK_LIB_TYPE const char* RDK_CALL Model_GetConnectorsList(const char* stringid,
						  int sublevel=-1, const char* owner_level_stringid=0);

// Возвращает xml-список длинных идентификаторов всех элементов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
// Предварительная очистка буфера не производится.
RDK_LIB_TYPE const char* RDK_CALL Model_GetItemsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// Возвращает xml-список длинных идентификаторов всех подсетей сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
// Предварительная очистка буфера не производится.
RDK_LIB_TYPE const char* RDK_CALL Model_GetNetsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// Возвращает короткое имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentName(const char* stringid);

// Возвращает длинное имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid=0);
RDK_LIB_TYPE const char* RDK_CALL MModel_GetComponentLongName(int channel_index, const char* stringid, const char* owner_level_stringid=0);

// Возвращает длинный id компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid=0);

// Возвращает имя класса компонента в хранилище по длинному 'stringid'
// если stringid - пустая строка, то возвращает имя класса модели
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentClassName(const char* stringid);
RDK_LIB_TYPE const char* RDK_CALL MModel_GetComponentClassName(int channel_index, const char* stringid);

// Возвращает список свойств компонента разделенный запятыми
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask);

// Возвращает список имен и индексов свойств компонента разделенный запятыми
// каждый элемент имеет вид имя_свойства:индекс_входа(выхода)
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask);

// Возвращает свойства компонента по идентификатору
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentProperties(const char *stringid, unsigned int type_mask);

// Возвращает свойства компонента по идентификатору с описаниями
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask);

// Возвращает выборочные свойства компонента по идентификатору
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedProperties(const char *stringid, unsigned int type_mask);

// Возвращает значение свойства компонента по идентификатору компонента и имени свойства
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentPropertyValue(const char *stringid, const char *paramname);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentPropertyValue(int channel_index, const char *stringid, const char *paramname);

// Устанавливает свойства компонента по идентификатору
RDK_LIB_TYPE int RDK_CALL Model_SetComponentProperties(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentProperties(int engine_index, const char *stringid, const char* buffer);

// Устанавливает значение свойства компонента по идентификатору компонента и имени свойства
RDK_LIB_TYPE int RDK_CALL Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentPropertyValue(int channel_index, const char *stringid, const char *paramname, const char *buffer);

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
RDK_LIB_TYPE int RDK_CALL Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer);

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
RDK_LIB_TYPE int RDK_CALL Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer);

// Возвращает указатель void* на данные свойства компонента
RDK_LIB_TYPE const void* RDK_CALL Model_GetComponentPropertyData(const char *stringid, const char *property_name);
RDK_LIB_TYPE const void* RDK_CALL MModel_GetComponentPropertyData(int channel_index, const char *stringid, const char *property_name);

// Копирует данные 'data' в заданное свойство компонента
RDK_LIB_TYPE int RDK_CALL Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentPropertyData(int channel_index, const char *stringid, const char *property_name, const void *data);

// Возвращает параметры компонента по идентификатору
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameters(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentParameters(int channel_index, const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// Возвращает параметры компонента по идентификатору с описаниями
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// Возвращает выборочные параметры компонента по идентификатору
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid);

// Возвращает значение параметра компонента по идентификатору компонента и имени параметра
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentParameterValue(int channel_index, const char *stringid, const char *paramname);

// Устанавливает параметры компонента по идентификатору
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentParameters(int channel_index, const char *stringid, const char* buffer);

// Устанавливает значение параметра компонента по идентификатору компонента и имени параметра
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentParameterValue(int channel_index, const char *stringid, const char *paramname, const char *buffer);

// Возвращает состояние компонента по идентификатору
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentState(const char *stringid, unsigned int type_mask=0xFFFFFFFF);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentState(int channel_index, const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// Возвращает выборочные данные состояния компонента по идентификатору
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid);

// Возвращает значение параметра перменной состояния по идентификатору компонента и имени переменной
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename);
RDK_LIB_TYPE const char * RDK_CALL MModel_GetComponentStateValue(int channel_index, const char *stringid, const char *statename);

// Устанавливает состояние компонента по идентификатору
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentState(int channel_index, const char *stringid, const char* buffer);

// Устанавливает значение переменной состояния компонента по идентификатору компонента и имени переменной
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer);

// Связывает выбранные компоненты друг с другом
RDK_LIB_TYPE int RDK_CALL Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number);
RDK_LIB_TYPE int RDK_CALL Model_CreateLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);
RDK_LIB_TYPE int RDK_CALL Model_CreateLinkByNameEx(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name, int connector_c_index);

// Связывает все компоненты выбранного компонента по возрастанию id в формате: 0 выход к 0 входу
RDK_LIB_TYPE int RDK_CALL Model_ChainLinking(const char* stringid);

// Связывает все компоненты выбранного компонента параллельно, подключая их к необходимому числу выходов модели
// Используется для тестирования производительности
RDK_LIB_TYPE int RDK_CALL Model_ParallelLinking(const char* stringid);

// Разрывает выбранную связь
RDK_LIB_TYPE int RDK_CALL Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number);
RDK_LIB_TYPE int RDK_CALL Model_BreakLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);

// Разрывает все связи
RDK_LIB_TYPE int RDK_CALL Model_BreakAllLinks(void);

// Разрывает связь ко входу connector_index коннектора 'connectorid'
RDK_LIB_TYPE int RDK_CALL Model_BreakConnectorLink(const char* connectorname, int connector_index);

// Разрывает все входные и выходные связи выбранного контейнера
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentLinks(const char* stringid);

// Разрывает все входные связи выбранного контейнера
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentInputLinks(const char* stringid);

// Разрывает все выходные связи выбранного контейнера
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentOutputLinks(const char* stringid);

// Проверяет, существует ли заданна связь
RDK_LIB_TYPE bool RDK_CALL Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number);
RDK_LIB_TYPE bool RDK_CALL Model_CheckLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid=0);

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются с уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то применяется уровень текущего компонента
RDK_LIB_TYPE int RDK_CALL Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid=0);

// Возвращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid=0, int sublevel=-1);

// Возвращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid=0, int sublevel=-1);

// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid=0);

// Возвращает число входов у компонента
RDK_LIB_TYPE int RDK_CALL Model_GetComponentNumInputs(const char *stringid);

// Возвращает размер входа компонента в числе элементов
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputDataSize(const char *stringid, int index);

// Возвращает размер элемента входа в байтах
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputElementSize(const char *stringid, int index);

// Возвращает размер входа компонента в байтах
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputByteSize(const char *stringid, int index);

// Возвращает указатель на данные входа как на массив байт
// Только для чтения!
RDK_LIB_TYPE unsigned char* RDK_CALL Model_GetComponentInputData(const char *stringid, int index);

// Возвращает число выходов у компонента
RDK_LIB_TYPE int RDK_CALL Model_GetComponentNumOutputs(const char *stringid);

// Возвращает размер выхода компонента в числе элементов
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputDataSize(const char *stringid, int index);

// Возвращает размер элемента выхода в байтах
//RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputElementSize(const char *stringid, int index);

// Возвращает размер выхода компонента в байтах
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputByteSize(const char *stringid, int index);

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
RDK_LIB_TYPE unsigned char* RDK_CALL Model_GetComponentOutputData(const char *stringid, int index);

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponent(const char *stringid, unsigned int params_type_mask=ptParameter | pgPublic);
RDK_LIB_TYPE const char * RDK_CALL MModel_SaveComponent(int channel_index, const char *stringid, unsigned int params_type_mask=ptParameter | pgPublic);

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
RDK_LIB_TYPE int RDK_CALL Model_SaveComponentToFile(const char *stringid, const char* file_name, unsigned int params_type_mask=0xFFFFFFFF);
RDK_LIB_TYPE int RDK_CALL MModel_SaveComponentToFile(int channel_index, const char *stringid, const char* file_name, unsigned int params_type_mask=0xFFFFFFFF);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponent(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_LoadComponent(int channel_index, const char *stringid, const char* buffer);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentFromFile(const char *stringid, const char* file_name);
RDK_LIB_TYPE int RDK_CALL MModel_LoadComponentFromFile(int channel_index, const char *stringid, const char* file_name);

// Сохраняет все свойства компонента и его дочерних компонент в xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentProperties(const char *stringid, unsigned int type_mask);

// Сохраняет все свойства компонента и его дочерних компонент в xml
RDK_LIB_TYPE int RDK_CALL Model_SaveComponentPropertiesToFile(const char *stringid, const char* file_name, unsigned int type_mask=0xFFFFFFFF);

// Загружает все свойства компонента и его дочерних компонент из xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentProperties(const char *stringid, char* buffer);

// Загружает все свойства компонента и его дочерних компонент из xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentPropertiesFromFile(const char *stringid, const char* file_name);

// Сохраняет все параметры компонента и его дочерних компонент в xml
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentParameters(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// Сохраняет все параметры компонента и его дочерних компонент в xml
RDK_LIB_TYPE const char * RDK_CALL MModel_SaveComponentParameters(int channel_index, const char *stringid, unsigned int type_mask=ptParameter | pgPublic);
// Загружает все параметры компонента и его дочерних компонент из xml
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentParameters(const char *stringid, const char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_LoadComponentParameters(int channel_index, const char *stringid, const char* buffer);

// Сохраняет состояние компонента и его дочерних компонент в xml
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentState(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// Загружает состояние компонента и его дочерних компонент из xml
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentState(const char *stringid, char* buffer);
RDK_LIB_TYPE int RDK_CALL MModel_LoadComponentState(int channel_index, const char *stringid, char* buffer);

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
RDK_LIB_TYPE const char* RDK_CALL Model_SaveComponentDrawInfo(const char *stringid);

// Управляет шагом счета модели по умолчанию
RDK_LIB_TYPE unsigned int RDK_CALL Model_GetDefaultTimeStep(void);
RDK_LIB_TYPE int RDK_CALL Model_SetDefaultTimeStep(unsigned int value);

// Управляет шагом счета компонента
RDK_LIB_TYPE unsigned int RDK_CALL Model_GetTimeStep(const char *stringid);
RDK_LIB_TYPE int RDK_CALL Model_SetTimeStep(const char *stringid, unsigned int value);

// Устанавливает шаг счета компонента и всех его дочерних компонент
RDK_LIB_TYPE int RDK_CALL Model_SetGlobalTimeStep(const char *stringid, unsigned int value);

// Возвращает текущее время модели
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetTime(void);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetTime(int channel_index);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleTime(void);
RDK_LIB_TYPE double RDK_CALL MModel_GetDoubleTime(int channel_index);

// Устанавливает текущее время модели
RDK_LIB_TYPE int RDK_CALL Model_SetTime(unsigned long long value);

// Возвращает реальное время
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetRealTime(void);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleRealTime(void);
RDK_LIB_TYPE double RDK_CALL MModel_GetDoubleRealTime(int channel_index);

// Устанавливает реальное время
RDK_LIB_TYPE int RDK_CALL Model_SetRealTime(unsigned long long value);

// Увеличивает реальное время на заданную величину
RDK_LIB_TYPE int RDK_CALL Model_IncreaseRealTime(unsigned long long value);

// Возвращает мгновенный шаг в реальном времени
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetRealTimeStep(void);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleRealTimeStep(void);

// Текущее время внешних источников данных в микросекундах
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetSourceTime(void);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetSourceTime(int channel_index);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleSourceTime(void);
RDK_LIB_TYPE double RDK_CALL MModel_GetDoubleSourceTime(int channel_index);

// Устанавливает время внешних источников данных
RDK_LIB_TYPE int RDK_CALL Model_SetSourceTime(unsigned long long value);
RDK_LIB_TYPE int RDK_CALL MModel_SetSourceTime(int channel_index, unsigned long long value);
RDK_LIB_TYPE int RDK_CALL Model_SetSourceTimeAll(unsigned long long value);

// Устанавливает время внешних источников данных в днях
RDK_LIB_TYPE int RDK_CALL Model_SetDoubleSourceTime(double value);
RDK_LIB_TYPE int RDK_CALL MModel_SetDoubleSourceTime(int channel_index, double value);
RDK_LIB_TYPE int RDK_CALL Model_SetDoubleSourceTimeAll(double value);

// Увеличивает время внешних источников данных на заданную величину
RDK_LIB_TYPE int RDK_CALL Model_IncreaseSourceTime(unsigned long long value);
RDK_LIB_TYPE int RDK_CALL MModel_IncreaseSourceTime(int channel_index, unsigned long long value);

// Мгновенный шаг во времени внешних источников данных в микросекундах
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetSourceTimeStep(void);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetSourceTimeStep(int channel_index);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleSourceTimeStep(void);
RDK_LIB_TYPE double RDK_CALL MModel_GetDoubleSourceTimeStep(int channel_index);

// Возвращает время расчета компонента без времени расчета дочерних компонент (мс)
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetStepDuration(const char *stringid);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetStepDuration(int channel_index, const char *stringid);

// Возвращает время, затраченное на обработку объекта
// (вместе со времени расчета дочерних компонент) (мс)
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetFullStepDuration(const char *stringid);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetFullStepDuration(int channel_index, const char *stringid);

// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
RDK_LIB_TYPE double RDK_CALL Model_GetInstantPerformance(const char *stringid);
RDK_LIB_TYPE double RDK_CALL MModel_GetInstantPerformance(int channel_index, const char *stringid);

// Время, прошедшее между двумя последними итерациями счета
RDK_LIB_TYPE unsigned long long RDK_CALL Model_GetInterstepsInterval(const char *stringid);
RDK_LIB_TYPE unsigned long long RDK_CALL MModel_GetInterstepsInterval(int channel_index, const char *stringid);
// --------------------------

// --------------------------
// Функции управления моделью видеообработки
// --------------------------
// Возвращает указатель на выход с индексом 'index' компонента 'id'
// возвращаемое значение имеет фактический тип RDK::MDMatrix*
// если выход не содержит данных такого типа, то возвращает 0
RDK_LIB_TYPE const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrixByIndex(const char *stringid, int index);

// Возвращает указатель на выход с индексом 'index' компонента 'id'
// возвращаемое значение имеет фактический тип RDK::UBitmap*
RDK_LIB_TYPE const /* RDK::UBitmap* */void* RDK_CALL Model_GetComponentOutput(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /* RDK::UBitmap* */void* RDK_CALL Model_GetComponentOutputByIndex(const char *stringid, int index);

RDK_LIB_TYPE const /* RDK::UBitmap* */void* RDK_CALL MModel_GetComponentOutput(int channel_index, const char *stringid, const char *property_name);
RDK_LIB_TYPE const /* RDK::UBitmap* */void* RDK_CALL MModel_GetComponentOutputByIndex(int channel_index, const char *stringid, int index);

// Возвращает указатель на выход с индексом 'index' компонента 'id'
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutput(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutputByIndex(const char *stringid, int index);

RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentBitmapOutput(int channel_index, const char *stringid, const char *property_name);
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentBitmapOutputByIndex(int channel_index, const char *stringid, int index);

/// Копирует данные о разрешении изображения выхода с индексом 'index' компонента 'id'
/// в стрктуру bmp_param
RDK_LIB_TYPE int RDK_CALL Model_CopyComponentBitmapOutputHeader(const char *stringid, const char *property_name, /*RDK::UBitmapParam* */ void* bmp_param);
RDK_LIB_TYPE int RDK_CALL MModel_CopyComponentBitmapOutputHeader(int channel_index, const char *stringid, const char *property_name, /*RDK::UBitmapParam* */ void* bmp_param);
RDK_LIB_TYPE int RDK_CALL Model_CopyComponentBitmapOutputHeaderByIndex(const char *stringid, int index, /*RDK::UBitmapParam* */ void* bmp_param);
RDK_LIB_TYPE int RDK_CALL MModel_CopyComponentBitmapOutputHeaderByIndex(int channel_index, const char *stringid, int index, /*RDK::UBitmapParam* */ void* bmp_param);

/// Копирует изображение выхода с индексом 'index' компонента 'id'
/// метод предполагает, что bmp уже имеет выделенную память под изобржение требуемого размера
RDK_LIB_TYPE int RDK_CALL Model_CopyComponentBitmapOutput(const char *stringid, const char *property_name, /*RDK::UBitmap**/void* bmp);
RDK_LIB_TYPE int RDK_CALL MModel_CopyComponentBitmapOutput(int channel_index, const char *stringid, const char *property_name, /*RDK::UBitmap**/void* bmp);
RDK_LIB_TYPE int RDK_CALL Model_CopyComponentBitmapOutputByIndex(const char *stringid, int index, /*RDK::UBitmap**/void* bmp);
RDK_LIB_TYPE int RDK_CALL MModel_CopyComponentBitmapOutputByIndex(int channel_index, const char *stringid, int index, /*RDK::UBitmap**/void* bmp);

// Возвращает указатель на вход с индексом 'index' компонента 'id'
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInput(const char *stringid, const char *property_name);
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInputByIndex(const char *stringid, int index);

// Замещает изображение выхода с индексом 'index' компонента 'id'
RDK_LIB_TYPE int RDK_CALL Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentBitmapOutput(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentBitmapOutputUnsafe(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);

RDK_LIB_TYPE int RDK_CALL Model_SetComponentBitmapOutputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentBitmapOutputByIndex(int channel_index, const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);

// Замещает изображение входа с индексом 'index' компонента 'id'
RDK_LIB_TYPE int RDK_CALL Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE int RDK_CALL MModel_SetComponentBitmapInput(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
RDK_LIB_TYPE int RDK_CALL Model_SetComponentBitmapInputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect=false);
// --------------------------

// --------------------------
// Функции управления исключениями
// ----------------------------
// Управление функцией-обработчиком исключений
// deprecated. See Log_ functions
RDK_LIB_TYPE void* RDK_CALL Engine_GetExceptionHandler(void);
RDK_LIB_TYPE void* RDK_CALL MEngine_GetExceptionHandler(int channel_index);
RDK_LIB_TYPE int RDK_CALL Engine_SetExceptionHandler(void* value);
RDK_LIB_TYPE int RDK_CALL MEngine_SetExceptionHandler(int channel_index, void* value);

// Возвращает массив строк лога
// deprecated. See Log_ functions
RDK_LIB_TYPE const char* RDK_CALL Engine_GetLog(int &error_level);
RDK_LIB_TYPE const char* RDK_CALL MEngine_GetLog(int channel_index, int &error_level);

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
// deprecated. See Log_ functions
RDK_LIB_TYPE const char* RDK_CALL Engine_GetUnreadLog(int &error_level, int &number, unsigned long long &time);
RDK_LIB_TYPE const char* RDK_CALL MEngine_GetUnreadLog(int channel_index, int &error_level, int &number, unsigned long long &time);
RDK_LIB_TYPE const char* RDK_CALL Engine_GetUnreadLogUnsafe(int &error_level, int &number, unsigned long long &time);
RDK_LIB_TYPE const char* RDK_CALL MEngine_GetUnreadLogUnsafe(int channel_index, int &error_level, int &number, unsigned long long &time);

// Записывает в лог новое сообщение
// deprecated
RDK_LIB_TYPE int RDK_CALL Engine_LogMessage(int log_level, const char *message);
RDK_LIB_TYPE int RDK_CALL MEngine_LogMessage(int channel_index, int log_level, const char *message);

// Записывает в лог новое сообщение с кодом ошибки
// deprecated
RDK_LIB_TYPE int RDK_CALL Engine_LogMessageEx(int log_level, const char *message, int error_event_number);
RDK_LIB_TYPE int RDK_CALL MEngine_LogMessageEx(int channel_index, int log_level, const char *message, int error_event_number);

/// Возвращает число непрочитанных строк лога
// deprecated
RDK_LIB_TYPE int RDK_CALL Engine_GetNumUnreadLogLines(void);
RDK_LIB_TYPE int RDK_CALL MEngine_GetNumUnreadLogLines(int channel_index);

/// Возвращает число строк лога
// deprecated
RDK_LIB_TYPE int RDK_CALL Engine_GetNumLogLines(void);
RDK_LIB_TYPE int RDK_CALL MEngine_GetNumLogLines(int channel_index);

/// Очищает лог прочитанных сообщений
// deprecated
RDK_LIB_TYPE int RDK_CALL Engine_ClearReadLog(void);
RDK_LIB_TYPE int RDK_CALL MEngine_ClearReadLog(int channel_index);
// ----------------------------

// Здесь RDK_CALL не нужен!
RDK_LIB_TYPE void tss_cleanup_implemented(void);

#ifdef __cplusplus
}
#endif

#endif
