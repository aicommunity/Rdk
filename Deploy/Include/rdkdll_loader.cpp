#include <windows.h>
#include "rdkdll_loader.h"

namespace RDK {

// ���������� dll
HMODULE RDKDllHandler=0;

// ������������� dll
DLLPDllInit DLLDllInit=0;

// ��������� �� ������� ������������ ����� �������� � ����������
DLLPGetNumStorages DLLGetNumStorages=0;

// ��������� �� ������� ������������ ����� ���� � ����������
DLLPGetNumEnvironments DLLGetNumEnvironments=0;

// ��������� �� ������� ������������ ����� ������� � ����������
DLLPGetNumEngines DLLGetNumEngines=0;

// ���������� ��������� �� �������
DLLPGetStorage DLLGetStorage=0;

// ���������� ����� �� �������
DLLPGetEnvironment DLLGetEnvironment=0;

// ���������� ������ �� �������
DLLPGetEngine DLLGetEngine=0;

// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
DLLPAddNewStorage DLLAddNewStorage=0;

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
DLLPAddNewEnvironment DLLAddNewEnvironment=0;

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
DLLPAddNewEngine DLLAddNewEngine=0;

// ��������������� �������, ��������� ��������� ��� ����������� �� �������� '_'
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



// ��������� ���������� � �������������� ��� ��������� �� �������
// ���������� 0 � ������ ������
int LoadDll(const char *dllname)
{
 RDKDllHandler=LoadLibrary(dllname);
 if(!RDKDllHandler)
  return -1;

 // ������������� dll
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

// ��������� ���������� � ���������� ��������� �� ���
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

// ��������� ����� � ���������� ��������� �� ����
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
