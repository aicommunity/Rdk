//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsVideoFileFormUnitH
#define TVideoCaptureOptionsVideoFileFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TVideoCaptureOptionsInterface.h"
#include "TUVisualControllerFormUnit.h"
#include <Vcl.Dialogs.hpp>

//---------------------------------------------------------------------------
class TVideoCaptureOptionsVideoFileForm : public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TPanel *VFCapturePanel;
	TLabel *VideoFileNameLabel;
	TEdit *VFNameEdit;
	TButton *VFBrowseButton;
	TCheckBox *VideoTruncPathCheckBox;
	TCheckBox *RepeatVideoCheckBox;
	TCheckBox *ProcessAllFramesVideoCheckBox;
	TOpenDialog *VideoOpenDialog;
	TLabeledEdit *FpsLabeledEdit;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall VFBrowseButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsVideoFileForm(TComponent* Owner);


/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
virtual TVideoCaptureOptionsVideoFileForm* New(TComponent *owner);

/// Считывает параметры в поля интерфейса
virtual bool ReadParametersToGui(RDK::USerStorageXML &xml);

/// Записывает параметры из полей интерфейса в xml
virtual bool WriteParametersToXml(RDK::USerStorageXML &xml);
/// -------------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsVideoFileForm *VideoCaptureOptionsVideoFileForm;
//---------------------------------------------------------------------------
#endif
