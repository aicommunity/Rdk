#ifndef UDLL_LOADER_GCC_CPP
#define UDLL_LOADER_GCC_CPP

#include "../UDllLoader.h"
#include <dlfcn.h>
#include <string>

class RDK_LIB_TYPE UDllLoaderGcc: public UDllLoader
{
private:
    void* handle;
    std::string fileName;

public:
    UDllLoaderGcc();
    UDllLoaderGcc(std::string dll_name);
    virtual ~UDllLoaderGcc();

    virtual bool Load(void);
    virtual bool Load(std::string dll_name);
    virtual bool isLoaded();

    /// Уменьшит счетчик подключения библиотеки, если счетчик станет нулём - выгрузит библиотеку,
    /// не выгрузит библиотеку, пока на все Load не вызовется UnLoad
    virtual bool UnLoad(void);
    virtual void * Resolve(std::string symbol);
    virtual std::string GetErrorString();
};


UDllLoaderGcc::UDllLoaderGcc()
{
    handle = NULL;
}

UDllLoaderGcc::UDllLoaderGcc(std::string dll_name)
{
    fileName = dll_name;
    handle = NULL;
}

UDllLoaderGcc::~UDllLoaderGcc()
{

}

bool UDllLoaderGcc::Load(void)
{
    handle = dlopen(fileName.c_str(), RTLD_LAZY);
    return handle != NULL;
}

bool UDllLoaderGcc::Load(std::string dll_name)
{
    fileName = dll_name;
    handle = dlopen(fileName.c_str(), RTLD_LAZY);
    return handle != NULL;
}

bool UDllLoaderGcc::isLoaded()
{
    return handle != NULL;
}

bool UDllLoaderGcc::UnLoad(void)
{
    int res = dlclose(handle);
    handle = NULL;
    return res == 0;
}

void *UDllLoaderGcc::Resolve(std::string symbol)
{
    return dlsym(handle, symbol.c_str());
}

std::string UDllLoaderGcc::GetErrorString()
{
    return dlerror();
}

#endif
