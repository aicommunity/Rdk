//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsBmpSequenceFrameUnitH
#define TVideoCaptureOptionsBmpSequenceFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TVideoCaptureOptionsInterface.h"
#include "TUVisualControllerFormUnit.h"

//---------------------------------------------------------------------------
class TVideoCaptureOptionsBmpSequenceForm : public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TLabel *Label7;
	TEdit *ImageSequencePathEdit;
	TCheckBox *ImageSequencePathCheckBox;
	TButton *ImageSequencePathBrowseButton;
	TCheckBox *RepeatSequenceCheckBox;
	TLabeledEdit *ImageSequenceFpsLabeledEdit;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ImageSequencePathBrowseButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsBmpSequenceForm(TComponent* Owner);


/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
virtual TVideoCaptureOptionsBmpSequenceForm* New(TComponent *owner);

/// Считывает параметры в поля интерфейса
virtual bool ReadParametersToGui(RDK::USerStorageXML &xml);

/// Записывает параметры из полей интерфейса в xml
virtual bool WriteParametersToXml(RDK::USerStorageXML &xml);
/// -------------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsBmpSequenceForm *VideoCaptureOptionsBmpSequenceForm;
//---------------------------------------------------------------------------
#endif
