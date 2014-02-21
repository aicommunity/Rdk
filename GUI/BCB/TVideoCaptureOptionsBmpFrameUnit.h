//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsBmpFrameUnitH
#define TVideoCaptureOptionsBmpFrameUnitH
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
class TVideoCaptureOptionsBmpForm : public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TCheckBox *PictureTruncPathCheckBox;
	TLabeledEdit *PictureFileFpsLabeledEdit;
	TButton *OpenImageFileButton;
	TLabel *Label3;
	TEdit *ImageFileNameEdit;
	TOpenDialog *PicturesOpenDialog;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall OpenImageFileButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsBmpForm(TComponent* Owner);


/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
virtual TVideoCaptureOptionsBmpForm* New(TComponent *owner);

/// Считывает параметры в поля интерфейса
virtual bool ReadParametersToGui(RDK::USerStorageXML &xml);

/// Записывает параметры из полей интерфейса в xml
virtual bool WriteParametersToXml(RDK::USerStorageXML &xml);
/// -------------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsBmpForm *VideoCaptureOptionsBmpForm;
//---------------------------------------------------------------------------
#endif
