#ifndef RDK_ENGINE_SUPPORT_H
#define RDK_ENGINE_SUPPORT_H

#include <vector>
#include "../../Core/Engine/UELockPtr.h"
#include "../../Core/Engine/UELockVar.h"
#include "../../Core/Engine/UStorage.h"
#include "../../Core/Engine/UEngine.h"
#include "../../Core/Engine/UEnvironment.h"

// Менеджер DLL
class RDK_LIB_TYPE RDKDllManager
{
public:
// Массив хранилищ
std::vector<RDK::UStorage*> StorageList;

// Массив сред
std::vector<RDK::UEnvironment*> EnvironmentList;

// Массив движков
std::vector<RDK::UEngine*> EngineList;

// Массив мьютексов
std::vector<UGenericMutex*> MutexList;


// Массив локеров
std::vector<UGenericMutexExclusiveLocker*> LockerList;

// Массив логгеров
std::vector<RDK::ULoggerEnv*> LoggerList;

// Системный (глобальный) логгер
RDK::ULoggerEnv GlobalLogger;

UGenericMutex* GlobalMutex;

/// Текущий выбраный канал
RDK::UELockVar<int> SelectedChannelIndex;

/// Текущее число каналов
RDK::UELockVar<int> NumEngines;

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

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
RDKDllManager(void);
virtual ~RDKDllManager(void);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Возвращает имя каталога бинарных файлов
std::string GetSystemDir(void);

// Устанавливает имя каталога бинарных файлов
int SetSystemDir(const char *dir);

// Возвращает имя каталога бинарных файлов
const char* GetLogDir(void);

// Устанавливает имя каталога бинарных файлов
int SetLogDir(const char *dir);

/// Флаг режима отладки
bool GetDebugMode(void);
int SetDebugMode(bool value);
// --------------------------

// --------------------------
// Методы управления созданием каналов
// --------------------------
/// Устанавливает указатели на глобальные функции, создающие экземпляр движка, среды и хранилища
bool SetCoreElementsCreationFunctions(PCreateNewStorage fCreateNewStorage,
            PCreateNewEnvironment fCreateNewEnvironment,
			PCreateNewEngine fCreateNewEngine);

/// Возвращает число движков
int GetNumEngines(void) const;

/// Создает требуемое число пустых движков
int SetNumEngines(int num);

/// Добавляет новый движок в позицию index
/// Если index <0 или >= NumEngines то добавляет в конец
int Add(int index);

/// Удаляет движок из позиции index
int Del(int index);

/// Создаает требуемый движок
/// (если движок уже инициализирован, то не делает ничего
int EngineCreate(int index);

/// Уничтожает требуемый движок
/// (если движок уже уничтожен, то не делает ничего
int EngineDestroy(int index);
// --------------------------

// --------------------------
// Методы доступа к каналам
// --------------------------
/// Текущий выбраный канал
int GetSelectedChannelIndex(void) const;
bool SetSelectedChannelIndex(int channel_index);

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
/// Метод доступа к глобальному лучше
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

// Возвращает указатель на логгер выбранного канала, или GlobalLogger
RDK::UEPtr<RDK::ULoggerEnv> GetLogger(int channel_index);

/// Возвращает ссылку на глобальный логгер
RDK::UEPtr<RDK::ULoggerEnv> GetGlobalLogger(void);

// Записывает в лог новое сообщение с кодом ошибки
//int RDK_CALL LogMessage(int channel_index, int log_level, const std::string &message, int error_event_number);

// Обрабатывает возникшее исключение
//protected:
//void ProcessException(RDK::UException &exception);
// --------------------------

};

//extern RDK::UEPtr<RDK::UEngine> PEngine;
//extern RDK::UEPtr<RDK::UEnvironment> PEnvironment;
//extern RDK::UEPtr<RDK::UStorage> PStorage;

//extern int SelectedEngineIndex;

//extern RDK_LIB_TYPE std::string RdkSystemDir;

// Экземпляр менеджера
extern RDK_LIB_TYPE RDKDllManager DllManager;

extern RDK_LIB_TYPE int BufObjectsMode;

#endif
