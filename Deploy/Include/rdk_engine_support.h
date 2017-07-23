#ifndef RDK_ENGINE_SUPPORT_H
#define RDK_ENGINE_SUPPORT_H

#include <vector>
#include "../../Core/Engine/UELockPtr.h"
#include "../../Core/Engine/UELockVar.h"
#include "../../Core/Engine/UStorage.h"
#include "../../Core/Engine/UEngine.h"
#include "../../Core/Engine/UEnvironment.h"

// Менеджер DLL
class RDK_LIB_TYPE URdkCoreManager
{
public:
/// Массив хранилищ
std::vector<RDK::UStorage*> StorageList;

/// Массив сред
std::vector<RDK::UEnvironment*> EnvironmentList;

/// Массив движков
std::vector<RDK::UEngine*> EngineList;

/// Массив мьютексов
std::vector<UGenericMutex*> MutexList;

/// Массив локеров
std::vector<UGenericMutexExclusiveLocker*> LockerList;

/// Массив логгеров
std::vector<RDK::ULoggerEnv*> LoggerList;

/// Системный логгер
RDK::ULoggerEnv SystemLogger;

/// Глобальный логгер (интегрирует информацию со всех логгеров)
RDK::ULoggerEnv GlobalLogger;

UGenericMutex* GlobalMutex;

/// Текущий выбраный канал
RDK::UELockVar<int> SelectedChannelIndex;

/// Текущее число каналов
RDK::UELockVar<int> NumChannels;

/// Данные текущего выбранного канала
RDK::UEPtr<RDK::ULoggerEnv> Logger;
RDK::UEPtr<RDK::UEngine> Engine;
RDK::UEPtr<RDK::UEnvironment> Environment;
RDK::UEPtr<RDK::UStorage> Storage;

/// Путь до директории с бинарными файлами ядра (приложения)
std::string SystemDir;

/// Путь до директории с логами
std::string LogDir;

/// Флаг режима отладки
bool DebugMode;

/// Флаг включения вывода лога в отладчик
bool DebuggerMessageFlag;

int BufObjectsMode;


// ----------------------------------------------------------
// Глобальные указатели на функции создания хранилища и среды
// ----------------------------------------------------------
// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
typedef RDK::UStorage* (*PCreateNewStorage)(void);
PCreateNewStorage FuncCreateNewStorage;

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
typedef RDK::UEnvironment* (*PCreateNewEnvironment)(void);
PCreateNewEnvironment FuncCreateNewEnvironment;

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
typedef RDK::UEngine* (*PCreateNewEngine)(void);
PCreateNewEngine FuncCreateNewEngine;
// ----------------------------------------------------------

// Глобальная коллекция шрифтов
RDK::UBitmapFontCollection Fonts;


public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URdkCoreManager(void);
virtual ~URdkCoreManager(void);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Возвращает имя каталога бинарных файлов
const char* GetSystemDir(void);

// Устанавливает имя каталога бинарных файлов
int SetSystemDir(const char *dir);

// Возвращает имя каталога логов
const char* GetLogDir(void);

// Устанавливает имя каталога логов
int SetLogDir(const char *dir);

/// Флаг режима отладки
bool GetDebugMode(void) const;
int SetDebugMode(bool value);

/// Флаг включения вывода лога в отладчик
bool GetDebuggerMessageFlag(void) const;
int SetDebuggerMessageFlag(bool value);

int GetBufObjectsMode(void);
int SetBufObjectsMode(int value);

// Очищает коллекцию глобальных шрифтов
int ClearFonts(void);

// Загружает глобальные шрифты
int LoadFonts(void);

// Загружает новый глобальный шрифт
bool AddFont(const std::string &font_file_name);

// Возвращает ссылку на коллекцию шрифтов
RDK::UBitmapFontCollection& GetFonts(void);
// --------------------------

// --------------------------
// Методы управления созданием каналов
// --------------------------
/// Устанавливает указатели на глобальные функции, создающие экземпляр движка, среды и хранилища
bool SetCoreElementsCreationFunctions(PCreateNewStorage fCreateNewStorage,
            PCreateNewEnvironment fCreateNewEnvironment,
			PCreateNewEngine fCreateNewEngine);

/// Возвращает число движков
int GetNumChannels(void) const;

/// Создает требуемое число пустых движков
int SetNumChannels(int num);

/// Делает текущим канала с заданным индексом
int SelectChannel(int index);

/// Возвращает индекс текущего выбраного канала
int GetSelectedChannelIndex(void) const;

/// Добавляет новый движок в позицию index
/// Если index <0 или >= NumChannels то добавляет в конец
int Add(int index);

/// Удаляет движок из позиции index
int Del(int index);

/// Инициализирует канал (функция должна быть вызвана первой!)
/// Upd: Функция может быть вызвана после SetNumChannels и SelectChannel
int ChannelInit(int channel_index, int predefined_structure, void* exception_handler);

/// Деинициализирует канал (функция автоматически вызывается при вызове инициализации)
int ChannelUnInit(int channel_index);

protected:
/// Создаает требуемый канал
/// (если канал уже инициализирован, то не делает ничего
int ChannelCreate(int index);

/// Уничтожает требуемый канал
/// (если канал уже уничтожен, то не делает ничего
int ChannelDestroy(int index);
// --------------------------

// --------------------------
// Методы доступа к каналам
// --------------------------
public:
// Возвращает ссылку на указатель управляющего ядра
RDK::UEPtr<RDK::UEngine>& GetEngine(void);
RDK::UEPtr<RDK::UEngine> GetEngine(int channel_index);

// Возвращает ссылку на указатель среды выполнения
RDK::UEPtr<RDK::UEnvironment>& GetEnvironment(void);
RDK::UEPtr<RDK::UEnvironment> GetEnvironment(int channel_index);

// Возвращает ссылку на указатель хранилища
RDK::UEPtr<RDK::UStorage>& GetStorage(void);
RDK::UEPtr<RDK::UStorage> GetStorage(int channel_index);

// Возвращает указатель на текущую модель
RDK::UEPtr<RDK::UContainer> GetModel(void);
RDK::UEPtr<RDK::UContainer> GetModel(int channel_index);
// --------------------------

// --------------------------
// Методы доступа к каналам с блокировкой
// --------------------------
/// Метод доступа к глобальному мьютексу
UGenericMutex* GetGlobalMutex(void);

/// Метод доступ к мьютексу
UGenericMutex* GetEngineMutex(void);
UGenericMutex* GetEngineMutex(int index);

// Возвращает ссылку на указатель управляющего ядра
RDK::UELockPtr<RDK::UEngine> GetEngineLock(void);
RDK::UELockPtr<RDK::UEngine> GetEngineLock(int channel_index);

// Возвращает ссылку на указатель среды выполнения
RDK::UELockPtr<RDK::UEnvironment> GetEnvironmentLock(void);
RDK::UELockPtr<RDK::UEnvironment> GetEnvironmentLock(int channel_index);

// Возвращает ссылку на указатель хранилища
RDK::UELockPtr<RDK::UStorage> GetStorageLock(void);
RDK::UELockPtr<RDK::UStorage> GetStorageLock(int channel_index);

// Возвращает указатель на текущую модель
RDK::UELockPtr<RDK::UContainer> GetModelLock(void);
RDK::UELockPtr<RDK::UContainer> GetModelLock(int channel_index);
// --------------------------

// --------------------------
/// Средства логгирования
// --------------------------
// Возвращает ссылку на указатель на логгер текущего канала
RDK::UEPtr<RDK::ULoggerEnv>& GetLogger(void);

// Возвращает указатель на логгер выбранного канала, или SystemLogger
RDK::UEPtr<RDK::ULoggerEnv> GetLogger(int channel_index);

/// Возвращает указатель на системный логгер
RDK::UEPtr<RDK::ULoggerEnv> GetSystemLogger(void);

/// Возвращает указатель  на глобальный логгер (интегрирует информацию со всех логгеров)
RDK::UEPtr<RDK::ULoggerEnv> GetGlobalLogger(void);
// --------------------------


// --------------------------
// Вспомогательные методы управления
// --------------------------
protected:
/// Меняет текущий выбраный канал
bool SetSelectedChannelIndex(int channel_index);
// --------------------------



};

//extern RDK::UEPtr<RDK::UEngine> PEngine;
//extern RDK::UEPtr<RDK::UEnvironment> PEnvironment;
//extern RDK::UEPtr<RDK::UStorage> PStorage;

//extern int SelectedEngineIndex;

//extern RDK_LIB_TYPE std::string RdkSystemDir;

// Экземпляр менеджера
extern RDK_LIB_TYPE URdkCoreManager RdkCoreManager;

#endif
