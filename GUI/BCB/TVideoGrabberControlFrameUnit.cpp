//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoGrabberControlFrameUnit.h"
#include "UGEngineControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoGrabberControlFrame *VideoGrabberControlFrame;
//---------------------------------------------------------------------------
__fastcall TVideoGrabberControlFrame::TVideoGrabberControlFrame(TComponent* Owner)
    : TUVisualControllerFrame(Owner)
{
 VideoGrabber=0;
 VideoOutputFrame=0;
}



// -----------------------------
// Методы управления устройством ввода видео
// -----------------------------
// Выбор управляемой компоненты видеозахвата
TVideoGrabber* TVideoGrabberControlFrame::GetVideoGrabber(void)
{
 return VideoGrabber;
}

TVideoOutputFrame* TVideoGrabberControlFrame::GetVideoOutputFrame(void)
{
 return VideoOutputFrame;
}

void TVideoGrabberControlFrame::Init(TVideoOutputFrame* video_output_frame, TVideoGrabber* grabber)
{
 VideoGrabber=grabber;
 VideoOutputFrame=video_output_frame;
 UpdateInterface();
}

// Выбор активного режима видеоввода
int TVideoGrabberControlFrame::GetMode(void) const
{
 if(VCapturePageControl->ActivePage == DeviceTabSheet)
  return 0;
 if(VCapturePageControl->ActivePage == VideoFileTabSheet)
  return 1;
 if(VCapturePageControl->ActivePage == PictureFileTabSheet)
  return 2;

 return -1;
}

void TVideoGrabberControlFrame::SelectMode(int mode)
{
 switch(mode)
 {
 case 0:
  VCapturePageControl->ActivePage = DeviceTabSheet;
 break;

 case 1:
  VCapturePageControl->ActivePage = VideoFileTabSheet;
 break;

 case 2:
  VCapturePageControl->ActivePage = PictureFileTabSheet;
 break;
 }

 VCapturePageControlChange(this);
}

void __fastcall TVideoGrabberControlFrame::AssignListToComboBox (TComboBox* ComboBox, String List, int Index)
{
   ComboBox->Items->Text = List;
   if (ComboBox->Items->Count > 0) {
	  ComboBox->ItemIndex = Index;
   }
}
// -----------------------------

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед шагом расчета
void TVideoGrabberControlFrame::ABeforeCalculate(void)
{

}

// Метод, вызываемый после шага расчета
void TVideoGrabberControlFrame::AAfterCalculate(void)
{

}

// Обновление интерфейса
void TVideoGrabberControlFrame::AUpdateInterface(void)
{
 if(VCapturePageControl->ActivePage != PictureFileTabSheet)
 {
  if(VideoGrabber->VideoSource == vs_VideoCaptureDevice)
   VCapturePageControl->ActivePage = DeviceTabSheet;
  else
  if(VideoGrabber->VideoSource == vs_VideoFileOrURL)
   VCapturePageControl->ActivePage = VideoFileTabSheet;

  AssignListToComboBox (DeviceComboBox, VideoGrabber->VideoDevices, VideoGrabber->VideoDevice);
  VideoGrabber->Update();
// DeviceComboBox->ItemIndex = VideoGrabber->VideoDevice;

  AssignListToComboBox (VideoSizeComboBox, VideoGrabber->VideoSizes, VideoGrabber->VideoSize);
  AssignListToComboBox (VideoSubTypeComboBox, VideoGrabber->VideoSubtypes, VideoGrabber->VideoSubtype);
  AssignListToComboBox (AnalogVideoStandardComboBox, VideoGrabber->AnalogVideoStandards, VideoGrabber->AnalogVideoStandard);
  AssignListToComboBox (InputComboBox, VideoGrabber->VideoInputs, VideoGrabber->VideoInput);

  VFNameEdit->Text=VideoGrabber->PlayerFileName;
 }
}

// Сохраняет параметры интерфейса в xml
void TVideoGrabberControlFrame::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{
 xml.WriteInteger("Mode",GetMode());
 if(ExtractFilePath(VFNameEdit->Text).Length() == 0)
 {
  xml.WriteString("VideoFileName",AnsiString(UGEngineControlForm->ProjectPath+VFNameEdit->Text).c_str());
 }
 else
 {
  xml.WriteString("VideoFileName",AnsiString(VFNameEdit->Text).c_str());
 }

 if(ExtractFilePath(ImageFileNameEdit->Text).Length() == 0)
 {
  xml.WriteString("PictureFileName",AnsiString(UGEngineControlForm->ProjectPath+ImageFileNameEdit->Text).c_str());
 }
 else
 {
  xml.WriteString("PictureFileName",AnsiString(ImageFileNameEdit->Text).c_str());
 }
}

// Загружает параметры интерфейса из xml
void TVideoGrabberControlFrame::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{
 VFNameEdit->Text=xml.ReadString("VideoFileName","").c_str();
 if(ExtractFilePath(VFNameEdit->Text) == UGEngineControlForm->ProjectPath)
  VFNameEdit->Text=ExtractFileName(VFNameEdit->Text);

 ImageFileNameEdit->Text=xml.ReadString("PictureFileName","").c_str();
 if(ExtractFilePath(ImageFileNameEdit->Text) == UGEngineControlForm->ProjectPath)
  ImageFileNameEdit->Text=ExtractFileName(ImageFileNameEdit->Text);

 SelectMode(xml.ReadInteger("Mode",1));
// VideoOutputFrame->InitByBmp(PicturesOpenDialog->FileName);
 UpdateInterface();
}
// -----------------------------

void __fastcall TVideoGrabberControlFrame::DeviceComboBoxSelect(TObject *Sender)
{
 VideoGrabber->VideoDevice = DeviceComboBox->ItemIndex;
 VideoOutputFrame->InitByCamera(VideoGrabber->VideoDevice, VideoGrabber->VideoInput, VideoGrabber->VideoSize, VideoGrabber->VideoSubtype, VideoGrabber->AnalogVideoStandard);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::InputComboBoxSelect(TObject *Sender)
{
 VideoGrabber->VideoInput = InputComboBox->ItemIndex;
 VideoOutputFrame->InitByCamera(VideoGrabber->VideoDevice, VideoGrabber->VideoInput, VideoGrabber->VideoSize, VideoGrabber->VideoSubtype, VideoGrabber->AnalogVideoStandard);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::VideoSizeComboBoxSelect(TObject *Sender)
{
 VideoGrabber->VideoSize = VideoSizeComboBox->ItemIndex;
 VideoOutputFrame->InitByCamera(VideoGrabber->VideoDevice, VideoGrabber->VideoInput, VideoGrabber->VideoSize, VideoGrabber->VideoSubtype, VideoGrabber->AnalogVideoStandard);
 UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TVideoGrabberControlFrame::VFBrowseButtonClick(TObject *Sender)
{
 if(!VideoOpenDialog->Execute())
  return;

 String FileName;
 if(UGEngineControlForm->ProjectPath == ExtractFilePath(VideoOpenDialog->FileName) || VideoTruncPathCheckBox->Checked == true)
 {
  FileName=ExtractFileName(VideoOpenDialog->FileName);
  VFNameEdit->Text=FileName;
  SelectMode(1);
  VideoOutputFrame->InitByAvi(UGEngineControlForm->ProjectPath+FileName);
 }
 else
 {
  FileName=VideoOpenDialog->FileName;
  VFNameEdit->Text=FileName;
  SelectMode(1);
  VideoOutputFrame->InitByAvi(FileName);
 }
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::VCapturePageControlChange(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 VideoOutputFrame->StopButtonClick(Sender);

 if(VCapturePageControl->ActivePage == DeviceTabSheet)
 {
  VideoGrabber->VideoSource=vs_VideoCaptureDevice;
  VideoOutputFrame->InitByCamera(VideoGrabber->VideoDevice, VideoGrabber->VideoInput, VideoGrabber->VideoSize, VideoGrabber->VideoSubtype, VideoGrabber->AnalogVideoStandard);
 }
 else
 if(VCapturePageControl->ActivePage == VideoFileTabSheet)
 {
  VideoGrabber->VideoSource=vs_VideoFileOrURL;
  if(VFNameEdit->Text != "")
  {
   if(ExtractFilePath(VFNameEdit->Text).Length() == 0)
	VideoOutputFrame->InitByAvi(UGEngineControlForm->ProjectPath+VFNameEdit->Text);
   else
	VideoOutputFrame->InitByAvi(VFNameEdit->Text);
  }
 }
 else
 if(VCapturePageControl->ActivePage == PictureFileTabSheet)
 {
  if(ImageFileNameEdit->Text != "")
  {
   if(ExtractFilePath(ImageFileNameEdit->Text).Length() == 0)
	VideoOutputFrame->InitByBmp(UGEngineControlForm->ProjectPath+ImageFileNameEdit->Text);
   else
	VideoOutputFrame->InitByBmp(ImageFileNameEdit->Text);
  }
 }

 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::VideoSubTypeComboBoxSelect(TObject *Sender)
{
 VideoGrabber->VideoSubtype = VideoSubTypeComboBox->ItemIndex;
 VideoOutputFrame->InitByCamera(VideoGrabber->VideoDevice, VideoGrabber->VideoInput, VideoGrabber->VideoSize, VideoGrabber->VideoSubtype, VideoGrabber->AnalogVideoStandard);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::AnalogVideoStandardComboBoxSelect(TObject *Sender)
{
 VideoGrabber->AnalogVideoStandard = AnalogVideoStandardComboBox->ItemIndex;
 VideoOutputFrame->InitByCamera(VideoGrabber->VideoDevice, VideoGrabber->VideoInput, VideoGrabber->VideoSize, VideoGrabber->VideoSubtype, VideoGrabber->AnalogVideoStandard);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::OpenImageFileButtonClick(TObject *Sender)
{
 if(!PicturesOpenDialog->Execute())
  return;


 String FileName;
// if(ExtractFilePath(Application->ExeName) == ExtractFilePath(PicturesOpenDialog->FileName))
 if(UGEngineControlForm->ProjectPath == ExtractFilePath(PicturesOpenDialog->FileName) || PictureTruncPathCheckBox->Checked == true)
 {
  FileName=ExtractFileName(PicturesOpenDialog->FileName);
  ImageFileNameEdit->Text=FileName;
  SelectMode(2);
  VideoOutputFrame->InitByBmp(UGEngineControlForm->ProjectPath+FileName);
 }
 else
 {
  FileName=PicturesOpenDialog->FileName;
  ImageFileNameEdit->Text=FileName;
  SelectMode(2);
  VideoOutputFrame->InitByBmp(FileName);
 }

 UpdateInterface();
}
//---------------------------------------------------------------------------

