#include <windows.h>
#include "rdkdll_loader.h"

namespace RDK {

// Дескриптор dll
HMODULE RDKDllHandler=0;

// Инициализация dll
DLLPDllInit DLLDllInit=0;

// Указатель на функцию возвращающую число хранилищ в библиотеке
DLLPGetNumStorages DLLGetNumStorages=0;

// Указатель на функцию возвращающую число сред в библиотеке
DLLPGetNumEnvironments DLLGetNumEnvironments=0;

// Указатель на функцию возвращающую число движков в библиотеке
DLLPGetNumEngines DLLGetNumEngines=0;

// Возвращает хранилище по индексу
DLLPGetStorage DLLGetStorage=0;

// Возвращает среду по индексу
DLLPGetEnvironment DLLGetEnvironment=0;

// Возвращает движок по индексу
DLLPGetEngine DLLGetEngine=0;

// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
DLLPAddNewStorage DLLAddNewStorage=0;

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
DLLPAddNewEnvironment DLLAddNewEnvironment=0;

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
DLLPAddNewEngine DLLAddNewEngine=0;

// Вспомогательная функция, загружает указатель вне зависимости от префикса '_'
FARPROC WINAPI LoadDllFunctionAddr(HMODULE hModule, LPCSTR lpProcName)
{
 static char buffer[1024];

 FARPROC WINAPI result=GetProcAddress(RDKDllHandler,lpProcName);
 if(!result)
 {
  buffer[0]='_';
  buffer[1]='\0';
  result=GetProcAddress(RDKDllHandler,strcat(buffer,lpProcName));
 }
 return result;
}



// Загружает библиотеку и инициализирует все указатели на функции
// возвращает 0 в случае успеха
int LoadDll(const char *dllname)
{
 RDKDllHandler=LoadLibrary(dllname);
 if(!RDKDllHandler)
  return -1;

 // Инициализация dll
 DLLDllInit=(DLLPDllInit)LoadDllFunctionAddr(RDKDllHandler,"DllInit");
 if(!DLLDllInit)
  return -8;

 DLLGetNumStorages=(DLLPGetNumStorages)LoadDllFunctionAddr(RDKDllHandler,"GetNumStorages");
 if(!DLLGetNumStorages)
  return -2;

 DLLGetNumEnvironments=(DLLPGetNumEnvironments)LoadDllFunctionAddr(RDKDllHandler,"GetNumEnvironments");
 if(!DLLGetNumEnvironments)
  return -3;

 DLLGetStorage=(DLLPGetStorage)LoadDllFunctionAddr(RDKDllHandler,"GetStorage");
 if(!DLLGetStorage)
  return -4;

 DLLGetEnvironment=(DLLPGetEnvironment)LoadDllFunctionAddr(RDKDllHandler,"GetEnvironment");
 if(!DLLGetEnvironment)
  return -5;

 DLLAddNewStorage=(DLLPAddNewStorage)LoadDllFunctionAddr(RDKDllHandler,"AddNewStorage");
 if(!DLLAddNewStorage)
  return -6;

 DLLAddNewEnvironment=(DLLPAddNewEnvironment)LoadDllFunctionAddr(RDKDllHandler,"AddNewEnvironment");
 if(!DLLAddNewEnvironment)
  return -7;

 DLLAddNewEngine=(DLLPAddNewEngine)LoadDllFunctionAddr(RDKDllHandler,"AddNewEngine");
 if(!DLLAddNewEngine)
  return -9;

 return 0;
}

// Загружает библиотеку и возвращает указатель на нее
UALibrary* LoadULibrary(const string &dllname, const string &libname)
{
 HMODULE nlibraryhandler=LoadLibrary(dllname.c_str());
 if(!nlibraryhandler)
  return 0;

 UALibrary* lib=(UALibrary*)GetProcAddress(nlibraryhandler,libname.c_str());
 if(!lib)
 {
  UALibrary* lib=(UALibrary*)GetProcAddress(nlibraryhandler,(string("_")+libname).c_str());
  if(!lib)
   return 0;
 }

 return lib;
}

// Загружает класс и возвращает указатель на него
UAContainer* LoadUClass(const string &dllname, const string &classname)
{
 HMODULE nclasshandler=LoadLibrary(dllname.c_str());
 if(!nclasshandler)
  return 0;

 UAContainer* dllclass=(UAContainer*)GetProcAddress(nclasshandler,classname.c_str());
 if(!dllclass)
 {
  UAContainer* dllclass=(UAContainer*)GetProcAddress(nclasshandler,(string("_")+classname).c_str());
  if(!dllclass)
   return 0;
 }

 return dllclass;
}


}
