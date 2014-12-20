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

void __fastcall TVideoCaptureOptionsDeviceForm::AssignListToComboBox (TComboBox* ComboBox, String List, int Index)
{
   ComboBox->Items->Text = List;
   if (ComboBox->Items->Count > 0) {
	  ComboBox->ItemIndex = Index;
   }
}


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
 if(&Xml != &xml)
  Xml=xml;
 RDK::UEPtr<TVideoCaptureThreadVideoGrabberCamera> thread=RDK::dynamic_pointer_cast<TVideoCaptureThreadVideoGrabberCamera>(TVideoCaptureOptionsForm::VideoSourcePrototypes[VideoSourceType]);
 if(thread)
 {
  TVideoGrabber* VideoGrabber=thread->GetVideoGrabber();
  if(!VideoGrabber)
  {
   thread->ExecuteCaptureInit();
   VideoGrabber=thread->GetVideoGrabber();
   if(!VideoGrabber)
    return false;
  }
  AssignListToComboBox (DeviceComboBox, VideoGrabber->VideoDevices, VideoGrabber->VideoDevice);

  DeviceComboBox->ItemIndex=xml.ReadInteger("CameraIndex",DeviceComboBox->ItemIndex);
  InputComboBox->ItemIndex=xml.ReadInteger("InputIndex",InputComboBox->ItemIndex);
  VideoSizeComboBox->ItemIndex=xml.ReadInteger("SizeIndex",VideoSizeComboBox->ItemIndex);
  VideoSubTypeComboBox->ItemIndex=xml.ReadInteger("SubtypeIndex",VideoSubTypeComboBox->ItemIndex);
  AnalogVideoStandardComboBox->ItemIndex=xml.ReadInteger("AnalogIndex",AnalogVideoStandardComboBox->ItemIndex);
  FpsLabeledEdit->Text=xml.ReadString("Fps", "0").c_str();

  VideoGrabber->VideoDevice=DeviceComboBox->ItemIndex;
//  thread->Init(DeviceComboBox->ItemIndex, InputComboBox->ItemIndex, VideoSizeComboBox->ItemIndex, VideoSubTypeComboBox->ItemIndex, AnalogVideoStandardComboBox->ItemIndex);
  VideoGrabber->Update();

  AssignListToComboBox (VideoSizeComboBox, VideoGrabber->VideoSizes, VideoGrabber->VideoSize);
  AssignListToComboBox (VideoSubTypeComboBox, VideoGrabber->VideoSubtypes, VideoGrabber->VideoSubtype);
  AssignListToComboBox (AnalogVideoStandardComboBox, VideoGrabber->AnalogVideoStandards, VideoGrabber->AnalogVideoStandard);
  AssignListToComboBox (InputComboBox, VideoGrabber->VideoInputs, VideoGrabber->VideoInput);
 }

 return true;
}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsDeviceForm::WriteParametersToXml(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");

 xml.WriteInteger("CameraIndex",DeviceComboBox->ItemIndex);
 xml.WriteInteger("InputIndex",InputComboBox->ItemIndex);
 xml.WriteInteger("SizeIndex",VideoSizeComboBox->ItemIndex);
 xml.WriteInteger("SubtypeIndex",VideoSubTypeComboBox->ItemIndex);
 xml.WriteInteger("AnalogIndex",AnalogVideoStandardComboBox->ItemIndex);
 xml.WriteString("Fps", AnsiString(FpsLabeledEdit->Text).c_str());

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

void __fastcall TVideoCaptureOptionsDeviceForm::DeviceComboBoxSelect(TObject *Sender)
{
 Xml.WriteInteger("CameraIndex",DeviceComboBox->ItemIndex);
 ReadParametersToGui(Xml);
}
//---------------------------------------------------------------------------


