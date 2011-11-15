#ifndef RDK_INIT_H
#define RDK_INIT_H

#include "rdk.h"
#include "rdk_gengine.h"

extern "C"  {


// Экземпляр менеджера
class RDKDllManager;
extern RDKDllManager DllManager;

// Инициализация dll
RDK_LIB_TYPE bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine);

// Возвращает число хранилищ в библиотеке
RDK_LIB_TYPE size_t GetNumStorages(void);

// Возвращает число сред в библиотеке
RDK_LIB_TYPE size_t GetNumEnvironments(void);

// Возвращает число движков в библиотеке
RDK_LIB_TYPE size_t GetNumEngines(void);

// Возвращает хранилище по индексу
RDK_LIB_TYPE RDK::UAContainerStorage* GetStorage(size_t i);

// Возвращает среду по индексу
RDK_LIB_TYPE RDK::UAContainerEnvironment*  GetEnvironment(size_t i);

// Возвращает движок по индексу
RDK_LIB_TYPE RDK::UEngine*  GetEngine(size_t i);

// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
RDK_LIB_TYPE RDK::UAContainerStorage*  AddNewStorage(void);

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
RDK_LIB_TYPE RDK::UAContainerEnvironment*  AddNewEnvironment(RDK::UAContainerStorage *storage=0,bool isinit=true, std::list<RDK::UAContainer*>* external_classes=0, std::list<RDK::UALibrary*>* external_libs=0);

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
RDK_LIB_TYPE RDK::UEngine*  AddNewEngine(void);
}

// Менеджер DLL
class RDKDllManager
{
public:
// Массив хранилищ
std::vector<RDK::UAContainerStorage*> StorageList;

// Массив сред
std::vector<RDK::UAContainerEnvironment*> EnvironmentList;

// Массив движков
std::vector<RDK::UEngine*> EngineList;

// ----------------------------------------------------------
// Глобальные указатели на функции создания хранилища и среды
// ----------------------------------------------------------
// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
typedef RDK::UAContainerStorage* (*PCreateNewStorage)(void);
PCreateNewStorage FuncCreateNewStorage;

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
typedef RDK::UAContainerEnvironment* (*PCreateNewEnvironment)(void);
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
// --------------------------
};


#endif
