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

//---------------------------------------------------------------------------
class TVideoCaptureOptionsBmpFrame : public TFrame, public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TCheckBox *PictureTruncPathCheckBox;
	TLabeledEdit *PictureFileFpsLabeledEdit;
	TButton *OpenImageFileButton;
	TLabel *Label3;
	TEdit *ImageFileNameEdit;
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsBmpFrame(TComponent* Owner);


/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Считывает параметры в поля интерфейса
virtual bool LoadParamters(RDK::USerStorageXML &xml);

/// Записывает параметры из полей интерфейса в xml
virtual bool SaveParamters(RDK::USerStorageXML &xml);
/// -------------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsBmpFrame *VideoCaptureOptionsBmpFrame;
//---------------------------------------------------------------------------
#endif
