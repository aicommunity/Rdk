//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsInterfaceH
#define TVideoCaptureOptionsInterfaceH

#include "myrdk.h"

class TVideoCaptureOptionsInterface
{
public:
/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Считывает параметры в поля интерфейса
virtual bool LoadParamters(RDK::USerStorageXML &xml)=0;

/// Записывает параметры из полей интерфейса в xml
virtual bool SaveParamters(RDK::USerStorageXML &xml)=0;
/// -------------------------------------

};
#endif
