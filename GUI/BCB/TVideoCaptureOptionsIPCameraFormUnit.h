//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsIPCameraFormUnitH
#define TVideoCaptureOptionsIPCameraFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TVideoCaptureOptionsInterface.h"
#include "TUVisualControllerFormUnit.h"

//---------------------------------------------------------------------------
class TVideoCaptureOptionsIPCameraForm : public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label8;
	TEdit *IPCameraUrlEdit;
	TEdit *IPCameraUserNameEdit;
	TEdit *IPCameraUserPasswordEdit;
	TButton *IpMoveUpButton;
	TButton *IpMoveLeftButton;
	TButton *IpMoveRightButton;
	TButton *IpMoveDownButton;
	TButton *ZoomInButton;
	TButton *ZoomOutButton;
	TButton *ResetButton;
	TEdit *IPCameraControlPostfixEdit;
	TLabeledEdit *FpsLabeledEdit;
	TCheckBox *DesiredResFlagCheckBox;
	TLabeledEdit *DesiredWidthLabeledEdit;
	TLabeledEdit *DesiredHeightLabeledEdit;
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsIPCameraForm(TComponent* Owner);


/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
virtual TVideoCaptureOptionsIPCameraForm* New(TComponent *owner);

/// Считывает параметры в поля интерфейса
virtual bool ReadParametersToGui(RDK::USerStorageXML &xml);

/// Записывает параметры из полей интерфейса в xml
virtual bool WriteParametersToXml(RDK::USerStorageXML &xml);
/// -------------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsIPCameraForm *VideoCaptureOptionsIPCameraForm;
//---------------------------------------------------------------------------
#endif
