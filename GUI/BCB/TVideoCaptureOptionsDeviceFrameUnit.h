//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsDeviceFrameUnitH
#define TVideoCaptureOptionsDeviceFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TVideoCaptureOptionsInterface.h"
//---------------------------------------------------------------------------
class TVideoCaptureOptionsDeviceFrame : public TFrame, public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TPanel *VDCapturePanel;
	TLabel *ModeLabel;
	TLabel *InputLabel;
	TLabel *DeviceLabel;
	TLabel *Label1;
	TLabel *Label2;
	TComboBox *VideoSizeComboBox;
	TComboBox *InputComboBox;
	TComboBox *DeviceComboBox;
	TComboBox *VideoSubTypeComboBox;
	TComboBox *AnalogVideoStandardComboBox;
	TButton *StreamButton;
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsDeviceFrame(TComponent* Owner);


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
extern PACKAGE TVideoCaptureOptionsDeviceFrame *VideoCaptureOptionsDeviceFrame;
//---------------------------------------------------------------------------
#endif
