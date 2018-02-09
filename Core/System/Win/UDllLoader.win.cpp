#ifndef UDLL_LOADER_WIN_CPP
#define UDLL_LOADER_WIN_CPP

#include "windows.h"
#include "../UDllLoader.h"

#include <string>

class RDK_LIB_TYPE UDllLoaderWin: public UDllLoader
{
private:
    HMODULE library;
    std::string dllName;
    
public:
    UDllLoaderWin();
    UDllLoaderWin(std::string dll_name);
    virtual ~UDllLoaderWin();
    
    virtual bool Load(void);
    virtual bool Load(std::string dll_name);
    virtual bool isLoaded();

    /// WINAPI принудительно выгрузит бибилиотеку, даже если она ещё где-то использщуется
    virtual bool UnLoad(void);
    virtual void * Resolve(std::string symbol);
    virtual std::string GetErrorString();
};


UDllLoaderWin::UDllLoaderWin()
{
    library = NULL;
}

UDllLoaderWin::UDllLoaderWin(std::string dll_name)
{
    dllName = dll_name;
}

UDllLoaderWin::~UDllLoaderWin()
{

}

bool UDllLoaderWin::Load(void)
{
    library = LoadLibraryA(dllName.c_str());
    return library != NULL;
}

bool UDllLoaderWin::Load(std::string dll_name)
{
    dllName = dll_name;
	library = LoadLibraryA(dllName.c_str());
    return library != NULL;
}

bool UDllLoaderWin::isLoaded()
{
    return library != NULL;
}

bool UDllLoaderWin::UnLoad(void)
{
    return FreeLibrary(library);
}

void *UDllLoaderWin::Resolve(std::string symbol)
{
    return (void *)GetProcAddress(library, symbol.c_str());
}

std::string UDllLoaderWin::GetErrorString()
{
  DWORD error = GetLastError();
  if (error)
  {
    LPVOID lpMsgBuf;
    DWORD bufLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
    if (bufLen)
    {
      LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
      std::string result(lpMsgStr, lpMsgStr+bufLen);

      LocalFree(lpMsgBuf);

      return result;
    }
  }
  return std::string();
}

#endif
