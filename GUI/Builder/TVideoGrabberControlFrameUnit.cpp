//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoGrabberControlFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoGrabberControlFrame *VideoGrabberControlFrame;
//---------------------------------------------------------------------------
__fastcall TVideoGrabberControlFrame::TVideoGrabberControlFrame(TComponent* Owner)
    : TFrame(Owner)
{
 UpdateCaptureInterfaceFlag=false;
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

// Обновляет интерфейс фрейма
void TVideoGrabberControlFrame::UpdateInterface(void)
{
 UpdateCaptureInterfaceFlag=true;

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
 UpdateCaptureInterfaceFlag=false;
}


// Сохраняет информацию об источниках данных в заданный ini файл
void TVideoGrabberControlFrame::SaveToIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return;

 ini->WriteInteger(section,"Mode",GetMode());
 ini->WriteString(section,"VideoFileName",VFNameEdit->Text);
 ini->WriteString(section,"PictureFileName",ImageFileNameEdit->Text);
}

// Загружает информацию об источниках данных из заданного ini файла
void TVideoGrabberControlFrame::LoadFromIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return;

 VFNameEdit->Text=ini->ReadString(section,"VideoFileName","");
 ImageFileNameEdit->Text=ini->ReadString(section,"PictureFileName","");
 SelectMode(ini->ReadInteger(section,"Mode",1));
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

 SelectMode(1);
 String FileName;
 if(ExtractFilePath(Application->ExeName) == ExtractFilePath(VideoOpenDialog->FileName) || VideoTruncPathCheckBox->Checked == true)
  FileName=ExtractFileName(VideoOpenDialog->FileName);
 else
  FileName=VideoOpenDialog->FileName;

 VFNameEdit->Text=FileName;
 VideoOutputFrame->InitByAvi(FileName);
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::VCapturePageControlChange(TObject *Sender)
{
 if(UpdateCaptureInterfaceFlag)
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
   VideoOutputFrame->InitByAvi(VFNameEdit->Text);
 }
 else
 if(VCapturePageControl->ActivePage == PictureFileTabSheet)
 {
  if(ImageFileNameEdit->Text != "")
   VideoOutputFrame->InitByBmp(ImageFileNameEdit->Text);
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

 SelectMode(2);

 String FileName;
// if(ExtractFilePath(Application->ExeName) == ExtractFilePath(PicturesOpenDialog->FileName))
 if(ExtractFilePath(Application->ExeName) == ExtractFilePath(PicturesOpenDialog->FileName) || PictureTruncPathCheckBox->Checked == true)
  FileName=ExtractFileName(PicturesOpenDialog->FileName);
 else
  FileName=PicturesOpenDialog->FileName;

 ImageFileNameEdit->Text=FileName;
 VideoOutputFrame->InitByBmp(FileName);
 UpdateInterface();
}
//---------------------------------------------------------------------------

