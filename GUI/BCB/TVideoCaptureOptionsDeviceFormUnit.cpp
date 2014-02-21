//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsDeviceFormUnit.h"
#include "TVideoCaptureOptionsFormUnit.h"
#include "TVideoSourceThread.h"
#include "VideoOutputFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsDeviceForm *VideoCaptureOptionsDeviceForm;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsDeviceForm::TVideoCaptureOptionsDeviceForm(TComponent* Owner)
	: TVideoCaptureOptionsInterface(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
TVideoCaptureOptionsDeviceForm* TVideoCaptureOptionsDeviceForm::New(TComponent *owner)
{
 return new TVideoCaptureOptionsDeviceForm(owner);
}

/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsDeviceForm::ReadParametersToGui(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("CameraIndex",DeviceComboBox->ItemIndex);
 xml.WriteInteger("InputIndex",InputComboBox->ItemIndex);
 xml.WriteInteger("SizeIndex",VideoSizeComboBox->ItemIndex);
 xml.WriteInteger("SubtypeIndex",VideoSubTypeComboBox->ItemIndex);
 xml.WriteInteger("AnalogIndex",AnalogVideoStandardComboBox->ItemIndex);

 return true;
}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsDeviceForm::WriteParametersToXml(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");
 DeviceComboBox->ItemIndex=xml.ReadInteger("CameraIndex",DeviceComboBox->ItemIndex);
 InputComboBox->ItemIndex=xml.ReadInteger("InputIndex",InputComboBox->ItemIndex);
 VideoSizeComboBox->ItemIndex=xml.ReadInteger("SizeIndex",VideoSizeComboBox->ItemIndex);
 VideoSubTypeComboBox->ItemIndex=xml.ReadInteger("SubtypeIndex",VideoSubTypeComboBox->ItemIndex);
 AnalogVideoStandardComboBox->ItemIndex=xml.ReadInteger("AnalogIndex",AnalogVideoStandardComboBox->ItemIndex);

 return true;
}
/// -------------------------------------
void __fastcall TVideoCaptureOptionsDeviceForm::FormCreate(TObject *Sender)
{
 VideoSourceType=2;
 TVideoCaptureOptionsDesciption descr;
 descr.Form=this;
 descr.Name="Device";
 descr.Position=0;
 VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,descr);
 VideoCaptureOptionsForm->AddVideoSourcePrototypes(VideoSourceType,new TVideoCaptureThreadVideoGrabberCamera(0,true));
}
//---------------------------------------------------------------------------

