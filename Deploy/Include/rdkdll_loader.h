#ifndef rdkdll_loaderH
#define rdkdll_loaderH

#include "rdk.h"

namespace RDK {

// ��������������� �������, ��������� ��������� ��� ����������� �� �������� '_'
//extern FARPROC WINAPI LoadDllFunctionAddr(HMODULE hModule, LPCSTR lpProcName);

// ������������� dll
typedef bool(*DLLPDllInit)(void* pfstorage,void* pfenvironment,void* pfengine);
extern DLLPDllInit DLLDllInit;

// ��������� �� ������� ������������ ����� �������� � ����������
typedef std::size_t(*DLLPGetNumStorages)(void);
extern DLLPGetNumStorages DLLGetNumStorages;

// ��������� �� ������� ������������ ����� ���� � ����������
typedef std::size_t(*DLLPGetNumEnvironments)(void);
extern DLLPGetNumEnvironments DLLGetNumEnvironments;

// ��������� �� ������� ������������ ����� ������� � ����������
typedef std::size_t(*DLLPGetNumEngines)(void);
extern DLLPGetNumEngines DLLGetNumEngines;

// ���������� ��������� �� �������
typedef UAContainerStorage* (*DLLPGetStorage)(size_t i);
extern DLLPGetStorage DLLGetStorage;

// ���������� ����� �� �������
typedef UAContainerEnvironment* (*DLLPGetEnvironment)(size_t i);
extern DLLPGetEnvironment DLLGetEnvironment;

// ���������� ������ �� �������
typedef UEngine* (*DLLPGetEngine)(size_t i);
extern DLLPGetEngine DLLGetEngine;

// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
typedef UAContainerStorage* (*DLLPAddNewStorage)(void);
extern DLLPAddNewStorage DLLAddNewStorage;

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
typedef UAContainerEnvironment* (*DLLPAddNewEnvironment)(UAContainerStorage *storage,bool isinit,list<UAContainer*>* external_classes, list<UALibrary*>* external_libs);
extern DLLPAddNewEnvironment DLLAddNewEnvironment;

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
typedef UEngine* (*DLLPAddNewEngine)(void);
extern DLLPAddNewEngine DLLAddNewEngine;

// ��������� ���������� � �������������� ��� ��������� �� �������
// ���������� 0 � ������ ������
int LoadDll(const char *dllname);

// ��������� ���������� � ���������� ��������� �� ���
UALibrary* LoadULibrary(const string &dllname, const string &libname);

// ��������� ����� � ���������� ��������� �� ����
UAContainer* LoadUClass(const string &dllname, const string &classname);

// ��������� ������ ������������� � ���������� ��������� �� Storage � Environment
// �������� ��, ���� ������� � dll �� �������
// ���������� 0 � ������ ������
//int DllInit(RDK::UIniFile<char> &inifile, const string &libsection,
}
#endif
