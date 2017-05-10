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
    UDllLoaderQt(std::string dll_name);
    virtual ~UDllLoaderQt();

    virtual bool Load(void);
    virtual bool Load(std::string dll_name);
    virtual bool isLoaded();

    /// QLibrary �� �������� �����������, ���� ��� ��������� ������� Qlibrary �� ������ UnLoad
    virtual bool UnLoad(void);
    virtual void * Resolve(std::string symbol);
    virtual std::string GetErrorString();
};


UDllLoaderQt::UDllLoaderQt()
{

}

UDllLoaderQt::UDllLoaderQt(std::string dll_name)
{
    library.setFileName(QString::fromStdString(dll_name));
}

UDllLoaderQt::~UDllLoaderQt()
{

}

bool UDllLoaderQt::Load(void)
{
    return library.load();
}

bool UDllLoaderQt::Load(std::string dll_name)
{
    library.setFileName(QString::fromStdString(dll_name));
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

void *UDllLoaderQt::Resolve(std::string symbol)
{
    return (void *)library.resolve(symbol.c_str());
}

std::string UDllLoaderQt::GetErrorString()
{
    return library.errorString().toLocal8Bit().constData();
}

#endif