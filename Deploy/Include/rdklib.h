#ifndef RDKLIB_H
#define RDKLIB_H

#include "rdk.h"

extern "C"  {


// Экземпляр менеджера
class RDKDllManager;
extern RDKDllManager DllManager;

// Инициализация dll
extern bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine);

// Возвращает число хранилищ в библиотеке
extern size_t GetNumStorages(void);

// Возвращает число сред в библиотеке
extern size_t GetNumEnvironments(void);

// Возвращает число движков в библиотеке
extern size_t GetNumEngines(void);

// Возвращает хранилище по индексу
extern RDK::UAContainerStorage* GetStorage(size_t i);

// Возвращает среду по индексу
extern RDK::UAContainerEnvironment*  GetEnvironment(size_t i);

// Возвращает движок по индексу
extern RDK::UEngine*  GetEngine(size_t i);

// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
extern RDK::UAContainerStorage*  AddNewStorage(void);

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
extern RDK::UAContainerEnvironment*  AddNewEnvironment(RDK::UAContainerStorage *storage=0,bool isinit=true, std::list<RDK::UAContainer*>* external_classes=0, std::list<RDK::UALibrary*>* external_libs=0);

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
extern RDK::UEngine*  AddNewEngine(void);

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
