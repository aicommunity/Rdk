//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsDeviceFormUnitH
#define TVideoCaptureOptionsDeviceFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TVideoCaptureOptionsInterface.h"
#include "TUVisualControllerFormUnit.h"
//---------------------------------------------------------------------------
class TVideoCaptureOptionsDeviceForm : public TVideoCaptureOptionsInterface
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
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall DeviceComboBoxSelect(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsDeviceForm(TComponent* Owner);

void __fastcall AssignListToComboBox (TComboBox* ComboBox, String List, int Index);

RDK::USerStorageXML Xml;

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
virtual TVideoCaptureOptionsDeviceForm* New(TComponent *owner);

/// Считывает параметры в поля интерфейса
virtual bool ReadParametersToGui(RDK::USerStorageXML &xml);

/// Записывает параметры из полей интерфейса в xml
virtual bool WriteParametersToXml(RDK::USerStorageXML &xml);
/// -------------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsDeviceForm *VideoCaptureOptionsDeviceForm;
//---------------------------------------------------------------------------
#endif
