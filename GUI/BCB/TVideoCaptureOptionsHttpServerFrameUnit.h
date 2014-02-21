//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsHttpServerFrameUnitH
#define TVideoCaptureOptionsHttpServerFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TVideoCaptureOptionsInterface.h"
#include "TUVisualControllerFormUnit.h"

//---------------------------------------------------------------------------
class TVideoCaptureOptionsHttpServerForm : public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TEdit *ListerPortEdit;
	TLabel *Label10;
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsHttpServerForm(TComponent* Owner);


/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
virtual TVideoCaptureOptionsHttpServerForm* New(TComponent *owner);

/// Считывает параметры в поля интерфейса
virtual bool ReadParametersToGui(RDK::USerStorageXML &xml);

/// Записывает параметры из полей интерфейса в xml
virtual bool WriteParametersToXml(RDK::USerStorageXML &xml);
/// -------------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsHttpServerForm *VideoCaptureOptionsHttpServerForm;
//---------------------------------------------------------------------------
#endif
