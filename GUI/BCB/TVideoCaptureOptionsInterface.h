//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsInterfaceH
#define TVideoCaptureOptionsInterfaceH

#include "myrdk.h"
#include "TUVisualControllerFormUnit.h"

class TVideoCaptureOptionsInterface: public TUVisualControllerForm
{
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsInterface(TComponent* Owner);

int VideoSourceType;

public:
/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Возвращает тип видеоисточника, соотвествующий этому фрейму
int GetVideoSourceType(void);

/// Считывает параметры в поля интерфейса
virtual bool ReadParametersToGui(RDK::USerStorageXML &xml)=0;

/// Записывает параметры из полей интерфейса в xml
virtual bool WriteParametersToXml(RDK::USerStorageXML &xml)=0;
/// -------------------------------------

};
#endif
