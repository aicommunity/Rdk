#ifndef rdkdll_loaderH
#define rdkdll_loaderH

#include "rdk.h"

namespace RDK {

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
