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
 PauseInvisibleFlag=true;
}

// -----------------------------
// Методы управления параметрами
// -----------------------------
// Если флаг true - при смене активной вкладке - неактивные источники видео
// останавливаются командой Pause.
bool TVideoGrabberControlFrame::GetPauseInvisibleFlag(void) const
{
 return PauseInvisibleFlag;
}

bool TVideoGrabberControlFrame::SetPauseInvisibleFlag(bool value)
{
 if(PauseInvisibleFlag == value)
  return true;

 PauseInvisibleFlag=value;
 return true;
}
// -----------------------------


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
			/*
 if(DCapture)
 {
  DeviceTabSheet->TabVisible=true;

  if(VCapturePageControl->ActivePage == DeviceTabSheet)
  {
   std::vector<std::wstring> &devices=DCapture->GetCaptureDevicesList();
   DeviceComboBox->Clear();
   for(size_t i=0;i<devices.size();i++)
	DeviceComboBox->Items->Add(devices[i].c_str());

   std::vector<long> &crossbar=DCapture->GetCrossbarInputsList();
   InputComboBox->Clear();
   InputComboBox->Items->Add("Default");
   for(size_t i=0;i<crossbar.size();i++)
    InputComboBox->Items->Add(RDK::VCapture::VideoCrossbarsNameList[crossbar[i]].c_str());

   if(DCapture->GetActiveCrossbarInput() >= 0 && DCapture->GetActiveCrossbarInput()< int(crossbar.size()))
    InputComboBox->ItemIndex=DCapture->GetActiveCrossbarInput()+1;
   else
    InputComboBox->ItemIndex=0;


   ModeComboBox->Clear();
   std::map<int,VIDEOINFOHEADER>& mode=DCapture->GetCaptureModesList();
   std::map<int,AM_MEDIA_TYPE>& fullmode=DCapture->GetCaptureFullModesList();

   String textmode;

   std::map<int,VIDEOINFOHEADER>::iterator I,J;
   I=mode.begin();
   J=mode.end();
   while(I != J)
   {
    textmode=IntToStr(int(I->first))+"> ";
    textmode+=IntToStr(int(I->second.bmiHeader.biWidth));
    textmode+="x";
    textmode+=IntToStr(int(I->second.bmiHeader.biHeight));
    textmode+="x";
    textmode+=IntToStr(int(I->second.bmiHeader.biBitCount));
    textmode+=": ";
    textmode+=DCapture->GetTextVideoModeType(I->first).c_str();
    ModeComboBox->Items->Add(textmode);
    ++I;
   }

   if(DCapture->GetActiveMode() >= 0 && DCapture->GetActiveMode()< int(mode.size()))
    ModeComboBox->ItemIndex=DCapture->GetActiveMode();
   else
    ModeComboBox->ItemIndex=-1;

   if(DCapture->GetActiveDevice() >= 0 && DCapture->GetActiveDevice()< int(devices.size()))
    DeviceComboBox->ItemIndex=DCapture->GetActiveDevice();
   else
    DeviceComboBox->ItemIndex=-1;
  }
 }
 else
 {
  DeviceTabSheet->TabVisible=false;
  DeviceComboBox->Clear();
  InputComboBox->Clear();
  ModeComboBox->Clear();
 }


 if(FCapture)
 {
  VideoFileTabSheet->TabVisible=true;

  if(VCapturePageControl->ActivePage == VideoFileTabSheet)
  {
   std::string sstamp;
   double cfiletime=FCapture->GetCurrentFileTime();
   RDK::VCapture::VTimeStamp stamp(cfiletime,FCapture->GetFPS());
   stamp>>sstamp;

   TimeTrackBar->Max=FCapture->GetLength();
   TimeTrackBar->Position=cfiletime;

   TimeEdit->Text=sstamp.c_str();
   TimeEdit->Repaint();

   VFNameEdit->Text=FCapture->GetFileName().c_str();
   VFNameEdit->Repaint();
  }
 }
 else
 {
  VideoFileTabSheet->TabVisible=false;
 }
           */
 UpdateCaptureInterfaceFlag=false;
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
 OpenDialog->FilterIndex=0;
 if(!OpenDialog->Execute())
  return;

 VFNameEdit->Text=OpenDialog->FileName;
 VideoOutputFrame->InitByAvi(OpenDialog->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TVideoGrabberControlFrame::TimeEditChange(TObject *Sender)
{
/* if(UpdateCaptureInterfaceFlag || !FCapture)
  return;

 std::string sstamp;
 RDK::VCapture::VTimeStamp stamp(0.0,FCapture->GetFPS());
 sstamp=AnsiString(TimeEdit->Text).c_str();
 stamp<<sstamp;

 FCapture->SelectFrame(stamp());
 UpdateInterface();*/
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::TimeTrackBarChange(TObject *Sender)
{
/* if(UpdateCaptureInterfaceFlag || !FCapture)
  return;

 std::string sstamp;
 RDK::VCapture::VTimeStamp stamp(double(TimeTrackBar->Position),double(FCapture->GetFPS()));
 FCapture->SelectFrame(stamp());
 UpdateInterface();    */
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
 OpenDialog->FilterIndex=1;
 if(!OpenDialog->Execute())
  return;

 ImageFileNameEdit->Text=OpenDialog->FileName;
 VideoOutputFrame->InitByBmp(OpenDialog->FileName);
 UpdateInterface();
}
//---------------------------------------------------------------------------

