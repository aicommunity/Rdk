//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsSharedMemoryFrameUnitH
#define TVideoCaptureOptionsSharedMemoryFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TVideoCaptureOptionsInterface.h"
#include "TUVisualControllerFormUnit.h"

//---------------------------------------------------------------------------
class TVideoCaptureOptionsSharedMemoryForm : public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TLabel *Label11;
	TLabel *Label12;
	TEdit *PipeIndexEdit;
	TEdit *PipeUidEdit;
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsSharedMemoryForm(TComponent* Owner);

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
virtual TVideoCaptureOptionsSharedMemoryForm* New(TComponent *owner);

/// Считывает параметры в поля интерфейса
virtual bool ReadParametersToGui(RDK::USerStorageXML &xml);

/// Записывает параметры из полей интерфейса в xml
virtual bool WriteParametersToXml(RDK::USerStorageXML &xml);
/// -------------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsSharedMemoryForm *VideoCaptureOptionsSharedMemoryForm;
//---------------------------------------------------------------------------
#endif
