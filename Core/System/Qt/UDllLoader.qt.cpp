#ifndef UDLL_LOADER_QT_CPP
#define UDLL_LOADER_QT_CPP

#include "../UDllLoader.h"
#include <QLibrary>
#include <QString>
#include <string>

class RDK_LIB_TYPE UDllLoaderQt: public UDllLoader
{
private:
    QLibrary library;

public:
    UDllLoaderQt();
	UDllLoaderQt(const std::string &dll_name);
    virtual ~UDllLoaderQt();

    virtual bool Load(void);
	virtual bool Load(const std::string &dll_name);
    virtual bool isLoaded();

    /// QLibrary НЕ выгрузит бибилиотеку, пока все созданные объекты Qlibrary не сделаю UnLoad
    virtual bool UnLoad(void);
	virtual void * Resolve(const std::string &symbol);
    virtual std::string GetErrorString();
};


UDllLoaderQt::UDllLoaderQt()
{

}

UDllLoaderQt::UDllLoaderQt(const std::string &dll_name)
{
    library.setFileName(QString::fromLocal8Bit(dll_name.c_str()));
}

UDllLoaderQt::~UDllLoaderQt()
{

}

bool UDllLoaderQt::Load(void)
{
    return library.load();
}

bool UDllLoaderQt::Load(const std::string &dll_name)
{
    library.setFileName(QString::fromLocal8Bit(dll_name.c_str()));
    return library.load();
}

bool UDllLoaderQt::isLoaded()
{
    return library.isLoaded();
}

bool UDllLoaderQt::UnLoad(void)
{
    return library.unload();
}

void *UDllLoaderQt::Resolve(const std::string &symbol)
{
    return (void *)library.resolve(symbol.c_str());
}

std::string UDllLoaderQt::GetErrorString()
{
    return library.errorString().toLocal8Bit().constData();
}

#endif
