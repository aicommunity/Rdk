#ifndef rdkdll_loaderH
#define rdkdll_loaderH

#include "rdk.h"

namespace RDK {

// Вспомогательная функция, загружает указатель вне зависимости от префикса '_'
//extern FARPROC WINAPI LoadDllFunctionAddr(HMODULE hModule, LPCSTR lpProcName);

// Инициализация dll
typedef bool(*DLLPDllInit)(void* pfstorage,void* pfenvironment,void* pfengine);
extern DLLPDllInit DLLDllInit;

// Указатель на функцию возвращающую число хранилищ в библиотеке
typedef std::size_t(*DLLPGetNumStorages)(void);
extern DLLPGetNumStorages DLLGetNumStorages;

// Указатель на функцию возвращающую число сред в библиотеке
typedef std::size_t(*DLLPGetNumEnvironments)(void);
extern DLLPGetNumEnvironments DLLGetNumEnvironments;

// Указатель на функцию возвращающую число движков в библиотеке
typedef std::size_t(*DLLPGetNumEngines)(void);
extern DLLPGetNumEngines DLLGetNumEngines;

// Возвращает хранилище по индексу
typedef UAContainerStorage* (*DLLPGetStorage)(size_t i);
extern DLLPGetStorage DLLGetStorage;

// Возвращает среду по индексу
typedef UAContainerEnvironment* (*DLLPGetEnvironment)(size_t i);
extern DLLPGetEnvironment DLLGetEnvironment;

// Возвращает движок по индексу
typedef UEngine* (*DLLPGetEngine)(size_t i);
extern DLLPGetEngine DLLGetEngine;

// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
typedef UAContainerStorage* (*DLLPAddNewStorage)(void);
extern DLLPAddNewStorage DLLAddNewStorage;

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
typedef UAContainerEnvironment* (*DLLPAddNewEnvironment)(UAContainerStorage *storage,bool isinit,list<UAContainer*>* external_classes, list<UALibrary*>* external_libs);
extern DLLPAddNewEnvironment DLLAddNewEnvironment;

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
typedef UEngine* (*DLLPAddNewEngine)(void);
extern DLLPAddNewEngine DLLAddNewEngine;

// Загружает библиотеку и инициализирует все указатели на функции
// возвращает 0 в случае успеха
int LoadDll(const char *dllname);

// Загружает библиотеку и возвращает указатель на нее
UALibrary* LoadULibrary(const string &dllname, const string &libname);

// Загружает класс и возвращает указатель на него
UAContainer* LoadUClass(const string &dllname, const string &classname);

// Выполняет полную инициализацию и возвращает указатели на Storage и Environment
// создавая их, если индексы в dll не указаны
// возвращает 0 в случае успеха
//int DllInit(RDK::UIniFile<char> &inifile, const string &libsection,
}
#endif
