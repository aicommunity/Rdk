#ifndef RDK_ENGINE_SUPPORT_H
#define RDK_ENGINE_SUPPORT_H



// Менеджер DLL
class RDKDllManager
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
std::vector<UGenericMutexLocker*> LockerList;

UGenericMutex* GlobalMutex;

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
~RDKDllManager(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
bool Init(PCreateNewStorage fCreateNewStorage,
            PCreateNewEnvironment fCreateNewEnvironment,
			PCreateNewEngine fCreateNewEngine);

/// Возвращает число движков
int GetNumEngines(void) const;

/// Создает требуемое число пустых движков
int SetNumEngines(int num);

/// Создаает требуемый движок
/// (если движок уже инициализирован, то не делает ничего
int EngineCreate(int index);

/// Уничтожает требуемый движок
/// (если движок уже уничтожен, то не делает ничего
int EngineDestroy(int index);
// --------------------------
};

extern RDK::UEPtr<RDK::UEngine> PEngine;
extern RDK::UEPtr<RDK::UEnvironment> PEnvironment;
extern RDK::UEPtr<RDK::UStorage> PStorage;

extern int SelectedEngineIndex;

extern std::string RdkSystemDir;

// Экземпляр менеджера
extern RDKDllManager DllManager;

extern int BufObjectsMode;

#endif
