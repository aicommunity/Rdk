//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsIPCameraFrameUnitH
#define TVideoCaptureOptionsIPCameraFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TVideoCaptureOptionsInterface.h"

//---------------------------------------------------------------------------
class TVideoCaptureOptionsIPCameraFrame : public TFrame, public TVideoCaptureOptionsInterface
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
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsIPCameraFrame(TComponent* Owner);


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
extern PACKAGE TVideoCaptureOptionsIPCameraFrame *VideoCaptureOptionsIPCameraFrame;
//---------------------------------------------------------------------------
#endif
