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
	UDllLoaderWin(const std::string &dll_name);
    virtual ~UDllLoaderWin();
    
    virtual bool Load(void);
    virtual bool Load(const std::string &dll_name);
    virtual bool isLoaded();

    /// WINAPI принудительно выгрузит бибилиотеку, даже если она ещё где-то использщуется
    virtual bool UnLoad(void);
    virtual void * Resolve(const std::string &symbol);
    virtual std::string GetErrorString();
};


UDllLoaderWin::UDllLoaderWin()
{
    library = NULL;
}

UDllLoaderWin::UDllLoaderWin(const std::string &dll_name)
{
	library = NULL;
    dllName = dll_name;
}

UDllLoaderWin::~UDllLoaderWin()
{

}

bool UDllLoaderWin::Load(void)
{
 if(library)
 {
  if(!UnLoad())
   return false;
  library=0;
 }

    library = LoadLibraryA(dllName.c_str());
    return library != NULL;
}

bool UDllLoaderWin::Load(const std::string &dll_name)
{
 if(library)
 {
  if(!UnLoad())
   return false;
  library =0;
 }

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

void *UDllLoaderWin::Resolve(const std::string &symbol)
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
