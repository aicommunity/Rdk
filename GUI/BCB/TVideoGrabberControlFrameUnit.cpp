//---------------------------------------------------------------------------

#include <vcl.h>
#include <Vcl.FileCtrl.hpp>
#pragma hdrstop

#include "TVideoGrabberControlFrameUnit.h"
#include "UGEngineControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TVideoGrabberControlFrame *VideoGrabberControlFrame;
//---------------------------------------------------------------------------
__fastcall TVideoGrabberControlFrame::TVideoGrabberControlFrame(TComponent* Owner)
    : TUVisualControllerFrame(Owner)
{
 VideoGrabber=0;
 VideoOutputFrame=0;
 IpPtzInfo.InitCanonVBM40();
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
 if(VCapturePageControl->ActivePage == IPCameraTabSheet)
  return 3;
 if(VCapturePageControl->ActivePage == ImageSequenceTabSheet)
  return 4;

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

 case 3:
  VCapturePageControl->ActivePage = IPCameraTabSheet;
 break;

 case 4:
  VCapturePageControl->ActivePage = ImageSequenceTabSheet;
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

void TVideoGrabberControlFrame::SendIpPtzCommand(void)
{
 std::string prefix, result;
 prefix=AnsiString(IPCameraControlPostfixEdit->Text).c_str();// "http://194.85.99.186/-wvhttp-01-/control.cgi?";
 IpPtzInfo.GenerateCanonVBM40Command(prefix,result);

 VideoGrabber->SendIPCameraCommand(result.c_str());
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
 if(!VideoGrabber)
  return;

 if(VCapturePageControl->ActivePage != PictureFileTabSheet && VCapturePageControl->ActivePage != ImageSequenceTabSheet)
 {
  if(VideoGrabber->VideoSource == vs_VideoCaptureDevice)
   VCapturePageControl->ActivePage = DeviceTabSheet;
  else
  if(VideoGrabber->VideoSource == vs_VideoFileOrURL)
   VCapturePageControl->ActivePage = VideoFileTabSheet;
  else
  if(VideoGrabber->VideoSource == vs_IPCamera)
   VCapturePageControl->ActivePage = IPCameraTabSheet;

  AssignListToComboBox (DeviceComboBox, VideoGrabber->VideoDevices, VideoGrabber->VideoDevice);
  VideoGrabber->Update();

  AssignListToComboBox (VideoSizeComboBox, VideoGrabber->VideoSizes, VideoGrabber->VideoSize);
  AssignListToComboBox (VideoSubTypeComboBox, VideoGrabber->VideoSubtypes, VideoGrabber->VideoSubtype);
  AssignListToComboBox (AnalogVideoStandardComboBox, VideoGrabber->AnalogVideoStandards, VideoGrabber->AnalogVideoStandard);
  AssignListToComboBox (InputComboBox, VideoGrabber->VideoInputs, VideoGrabber->VideoInput);

  VFNameEdit->Text=VideoGrabber->PlayerFileName;
 }
}

// Сохраняет параметры интерфейса в xml
void TVideoGrabberControlFrame::ASaveParameters(RDK::USerStorageXML &xml)
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

 xml.WriteString("ImageSequencePath",AnsiString(ImageSequencePathEdit->Text).c_str());

 xml.WriteString("IPCameraUrl",AnsiString(IPCameraUrlEdit->Text).c_str());
 xml.WriteString("IPCameraUserName",AnsiString(IPCameraUserNameEdit->Text).c_str());
 xml.WriteString("IPCameraUserPassword",AnsiString(IPCameraUserPasswordEdit->Text).c_str());

 xml.WriteString("IPCameraControlPostfix",AnsiString(IPCameraControlPostfixEdit->Text).c_str());


 xml.WriteInteger("DeviceId",DeviceComboBox->ItemIndex);
 xml.WriteInteger("InputId",InputComboBox->ItemIndex);
 xml.WriteInteger("VideoSizeId",VideoSizeComboBox->ItemIndex);
 xml.WriteInteger("VideoSubTypeId",VideoSubTypeComboBox->ItemIndex);
 xml.WriteInteger("AnalogVideoStandardId",AnalogVideoStandardComboBox->ItemIndex);
}

// Загружает параметры интерфейса из xml
void TVideoGrabberControlFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 VFNameEdit->Text=xml.ReadString("VideoFileName","").c_str();
 if(ExtractFilePath(VFNameEdit->Text) == UGEngineControlForm->ProjectPath)
  VFNameEdit->Text=ExtractFileName(VFNameEdit->Text);

 ImageFileNameEdit->Text=xml.ReadString("PictureFileName","").c_str();
 if(ExtractFilePath(ImageFileNameEdit->Text) == UGEngineControlForm->ProjectPath)
  ImageFileNameEdit->Text=ExtractFileName(ImageFileNameEdit->Text);

 ImageSequencePathEdit->Text=xml.ReadString("ImageSequencePath","").c_str();

 IPCameraUrlEdit->Text=xml.ReadString("IPCameraUrl","").c_str();
 IPCameraUserNameEdit->Text=xml.ReadString("IPCameraUserName","").c_str();
 IPCameraUserPasswordEdit->Text=xml.ReadString("IPCameraUserPassword","").c_str();
 IPCameraControlPostfixEdit->Text=xml.ReadString("IPCameraControlPostfix","").c_str();

 SelectMode(xml.ReadInteger("Mode",1));
 UpdateInterface();

 int index=-1;
 index=xml.ReadInteger("DeviceId",-1);
 if(index<DeviceComboBox->Items->Count)
  DeviceComboBox->ItemIndex=index;

 index=xml.ReadInteger("InputId",-1);
 if(index<InputComboBox->Items->Count)
  InputComboBox->ItemIndex=index;

 index=xml.ReadInteger("VideoSizeId",-1);
 if(index<VideoSizeComboBox->Items->Count)
  VideoSizeComboBox->ItemIndex=index;

 index=xml.ReadInteger("VideoSubTypeId",-1);
 if(index<VideoSubTypeComboBox->Items->Count)
  VideoSubTypeComboBox->ItemIndex=index;

 index=xml.ReadInteger("AnalogVideoStandardId",-1);
 if(index<AnalogVideoStandardComboBox->Items->Count)
  AnalogVideoStandardComboBox->ItemIndex=index;
}
// -----------------------------

void __fastcall TVideoGrabberControlFrame::DeviceComboBoxSelect(TObject *Sender)
{
 if(!VideoOutputFrame || !VideoGrabber)
  return;

 VideoGrabber->VideoDevice = DeviceComboBox->ItemIndex;
 VideoOutputFrame->InitByCamera(VideoGrabber->VideoDevice, VideoGrabber->VideoInput, VideoGrabber->VideoSize, VideoGrabber->VideoSubtype, VideoGrabber->AnalogVideoStandard);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::InputComboBoxSelect(TObject *Sender)
{
 if(!VideoOutputFrame || !VideoGrabber)
  return;

 VideoGrabber->VideoInput = InputComboBox->ItemIndex;
 VideoOutputFrame->InitByCamera(VideoGrabber->VideoDevice, VideoGrabber->VideoInput, VideoGrabber->VideoSize, VideoGrabber->VideoSubtype, VideoGrabber->AnalogVideoStandard);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::VideoSizeComboBoxSelect(TObject *Sender)
{
 if(!VideoOutputFrame || !VideoGrabber)
  return;

 VideoGrabber->VideoSize = VideoSizeComboBox->ItemIndex;
 VideoOutputFrame->InitByCamera(VideoGrabber->VideoDevice, VideoGrabber->VideoInput, VideoGrabber->VideoSize, VideoGrabber->VideoSubtype, VideoGrabber->AnalogVideoStandard);
 UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TVideoGrabberControlFrame::VFBrowseButtonClick(TObject *Sender)
{
 if(!VideoOutputFrame || !VideoGrabber)
  return;

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

 if(!VideoOutputFrame || !VideoGrabber)
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
 else
 if(VCapturePageControl->ActivePage == IPCameraTabSheet)
 {
  VideoGrabber->VideoSource=vs_IPCamera;
  VideoOutputFrame->InitByIPCamera(IPCameraUrlEdit->Text, IPCameraUserNameEdit->Text, IPCameraUserPasswordEdit->Text);
 }
 else
 if(VCapturePageControl->ActivePage == ImageSequenceTabSheet)
 {
  VideoOutputFrame->InitByImageSequence(ImageSequencePathEdit->Text);
 }

 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::VideoSubTypeComboBoxSelect(TObject *Sender)
{
 if(!VideoOutputFrame || !VideoGrabber)
  return;

 VideoGrabber->VideoSubtype = VideoSubTypeComboBox->ItemIndex;
 VideoOutputFrame->InitByCamera(VideoGrabber->VideoDevice, VideoGrabber->VideoInput, VideoGrabber->VideoSize, VideoGrabber->VideoSubtype, VideoGrabber->AnalogVideoStandard);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::AnalogVideoStandardComboBoxSelect(TObject *Sender)
{
 if(!VideoOutputFrame || !VideoGrabber)
  return;

 VideoGrabber->AnalogVideoStandard = AnalogVideoStandardComboBox->ItemIndex;
 VideoOutputFrame->InitByCamera(VideoGrabber->VideoDevice, VideoGrabber->VideoInput, VideoGrabber->VideoSize, VideoGrabber->VideoSubtype, VideoGrabber->AnalogVideoStandard);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::OpenImageFileButtonClick(TObject *Sender)
{
 if(!PicturesOpenDialog->Execute())
  return;

 if(!VideoOutputFrame || !VideoGrabber)
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

void __fastcall TVideoGrabberControlFrame::ImageSequencePathBrowseButtonClick(TObject *Sender)
{
 String chosenDir=ExtractFilePath(Application->ExeName);

 if(SelectDirectory("Select image sequence directory", ExtractFilePath(Application->ExeName), chosenDir,TSelectDirExtOpts() << sdNewFolder << sdNewUI))
 {
  ImageSequencePathEdit->Text=chosenDir;
 }

 if(!VideoOutputFrame || !VideoGrabber)
  return;

 SelectMode(4);
 VideoOutputFrame->InitByImageSequence(ImageSequencePathEdit->Text);

 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::StreamButtonClick(TObject *Sender)
{
 VideoGrabber->ShowDialog(dlg_StreamConfig);
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::IpMoveUpButtonClick(TObject *Sender)
{
 IpPtzInfo.MoveUp(100);
 SendIpPtzCommand();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::IpMoveLeftButtonClick(TObject *Sender)

{
 IpPtzInfo.MoveLeft(100);
 SendIpPtzCommand();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::IpMoveDownButtonClick(TObject *Sender)
{
 IpPtzInfo.MoveDown(100);
 SendIpPtzCommand();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::IpMoveRightButtonClick(TObject *Sender)

{
 IpPtzInfo.MoveRight(100);
 SendIpPtzCommand();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::ZoomInButtonClick(TObject *Sender)
{
 IpPtzInfo.ZoomOut(100);
 SendIpPtzCommand();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::ZoomOutButtonClick(TObject *Sender)
{
 IpPtzInfo.ZoomIn(100);
 SendIpPtzCommand();
}
//---------------------------------------------------------------------------

void __fastcall TVideoGrabberControlFrame::ResetButtonClick(TObject *Sender)
{
 IpPtzInfo.ZeroPosition();
 SendIpPtzCommand();
}
//---------------------------------------------------------------------------

