//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoRegistratorFrameUnit.h"
#include "rdk_initdll.h"
#include "UGEngineControlFormUnit.h"
#include "TUBitmap.h"
#include "VideoOutputFormUnit.h"
#include "VideoOutputFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VidGrab"
#pragma resource "*.dfm"
TTVideoRegistratorFrame *TVideoRegistratorFrame;

// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoGetBitmapFrameThread::TVideoGetBitmapFrameThread(TTVideoRegistratorFrame *frame, bool CreateSuspended)
: Frame(frame), TThread(CreateSuspended)
{
 SourceMode=-1;
 //CaptureEnabled=CreateEvent(0,TRUE,0,0);
 SourceUnlock=CreateEvent(0,TRUE,TRUE,0);
 SourceWriteUnlock=CreateEvent(0,TRUE,TRUE,0);
 FrameNotInProgress=CreateEvent(0,TRUE,TRUE,0);
 CalcCompleteEvent=CreateEvent(0,TRUE,TRUE,0);
 ReadSource=&Source[0];
 WriteSource=&Source[1];
 FreeOnTerminate=false;
}

__fastcall TVideoGetBitmapFrameThread::~TVideoGetBitmapFrameThread(void)
{
 //CloseHandle(CaptureEnabled);
 CloseHandle(SourceUnlock);
 CloseHandle(SourceWriteUnlock);
 CloseHandle(FrameNotInProgress);
 CloseHandle(CalcCompleteEvent);
}
// --------------------------
// ���������� �����������
// --------------------------
/// �������� �����
int TVideoGetBitmapFrameThread::GetSourceMode(void) const
{
 return SourceMode;
}

/// ������ ������ � ���������� ���������, ����������� ������
int TVideoGetBitmapFrameThread::GetChannelIndex(void) const
{
 return ChannelIndex;
}

bool TVideoGetBitmapFrameThread::SetChannelIndex(int value)
{
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 ChannelIndex=value;
 return true;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ��������� �� ���������
TTVideoRegistratorFrame* TVideoGetBitmapFrameThread::GetFrame(void) const
{
 return Frame;
}

bool TVideoGetBitmapFrameThread::SetFrame(TTVideoRegistratorFrame * frame)
{
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 Frame=frame;
 return true;
}
// --------------------------
// ���������� ���������
// --------------------------
/// ������������ �� ���������� ������� ������ �����
HANDLE TVideoGetBitmapFrameThread::GetFrameNotInProgress(void) const
{
 return FrameNotInProgress;
}

/// ���������� ������. ������������ �� ����� ������� � �����������
HANDLE TVideoGetBitmapFrameThread::GetSourceUnlock(void) const
{
 return SourceUnlock;
}

/// ������������ �� ����� ������ ������������
HANDLE TVideoGetBitmapFrameThread::GetCaptureEnabled(void) const
{
 //return CaptureEnabled;
}

/// ������������ �� ����� �������� �������
HANDLE TVideoGetBitmapFrameThread::GetCalcCompleteEvent(void) const
{
 return CalcCompleteEvent;
}
// --------------------------
// ���������� �������
// --------------------------
void __fastcall TVideoGetBitmapFrameThread::Start(void)
{
 //SetEvent(CaptureEnabled);
}

void __fastcall TVideoGetBitmapFrameThread::Stop(void)
{
 //ResetEvent(CaptureEnabled);
}

void __fastcall TVideoGetBitmapFrameThread::BeforeCalculate(void)
{

}

void __fastcall TVideoGetBitmapFrameThread::AfterCalculate(void)
{

}



void __fastcall TVideoGetBitmapFrameThread::Execute(void)
{
 while(!Terminated)
 {
  ResetEvent(CalcCompleteEvent);
  ResetEvent(FrameNotInProgress);
  BeforeCalculate();
  Calculate();
  AfterCalculate();
  SetEvent(CalcCompleteEvent);
  SetEvent(FrameNotInProgress);
 }
}
/// ���������� ����� ����������� � �����������
bool TVideoGetBitmapFrameThread::ReadSourceSafe(RDK::UBitmap& dest, bool reflect)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;

 ResetEvent(SourceUnlock);
 RDK::UBitmap* source=ReadSource;

 if(reflect)
  source->ReflectionX(&dest);
 else
  dest=*source;

 SetEvent(SourceUnlock);
 return true;
}

bool TVideoGetBitmapFrameThread::ReadSourceSafe(Graphics::TBitmap *dest, bool reflect)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;

 ResetEvent(SourceUnlock);
 RDK::UBitmap* source=ReadSource;
 UBitmapToTBitmap(*source, dest, reflect);

 SetEvent(SourceUnlock);
 return true;
}

/// ���������� ����������� � ���� � �����������
bool TVideoGetBitmapFrameThread::WriteSourceSafe(const RDK::UBitmap& src, bool reflect)
{
 if(WaitForSingleObject(SourceWriteUnlock,100) == WAIT_TIMEOUT)
  return false;

 ResetEvent(SourceWriteUnlock);
 if(reflect)
  const_cast<RDK::UBitmap&>(src).ReflectionX(WriteSource);
 else
  *WriteSource=src;
 SetEvent(SourceWriteUnlock);

 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;
 ResetEvent(SourceUnlock);

 RDK::UBitmap* old_read_source=ReadSource;
 ReadSource=WriteSource;
 WriteSource=old_read_source;
 SetEvent(SourceUnlock);
 return true;
}

bool TVideoGetBitmapFrameThread::WriteSourceSafe(Graphics::TBitmap *src, bool reflect)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;

 ResetEvent(SourceUnlock);
 TBitmapToUBitmap(*WriteSource, src, reflect);

 RDK::UBitmap* old_read_source=ReadSource;
 ReadSource=WriteSource;
 WriteSource=old_read_source;
 SetEvent(SourceUnlock);
 return true;
}
//---------------------------------------------------------------------------
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoGetBitmapFrameFromVideoThread::TVideoGetBitmapFrameFromVideoThread(TTVideoRegistratorFrame *frame, bool CreateSuspended)
: TVideoGetBitmapFrameThread(frame, CreateSuspended)
{
 //TempBitmap=new RDK::UBitmap();
}

__fastcall TVideoGetBitmapFrameFromVideoThread::~TVideoGetBitmapFrameFromVideoThread(void)
{
 //if(TempBitmap)
 //{
  //delete TempBitmap;
  //TempBitmap=0;
 //}
}
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
bool TVideoGetBitmapFrameFromVideoThread::SetFrameIndex(const int &value)
{
 if(FrameIndex == value)
  return true;

 FrameIndex=value;
 return true;
}

const int& TVideoGetBitmapFrameFromVideoThread::GetFrameIndex(void) const
{
 return FrameIndex;
}
// --------------------------
// ���������� �������
// --------------------------
void __fastcall TVideoGetBitmapFrameFromVideoThread::Start(void)
{
 TVideoGetBitmapFrameThread::Start();
}

void __fastcall TVideoGetBitmapFrameFromVideoThread::Stop(void)
{
 TVideoGetBitmapFrameThread::Stop();
}


void __fastcall TVideoGetBitmapFrameFromVideoThread::BeforeCalculate(void)
{

}

void __fastcall TVideoGetBitmapFrameFromVideoThread::AfterCalculate(void)
{
  TVideoGetBitmapFrameThread::AfterCalculate();
  //Sleep(30);
}

void __fastcall TVideoGetBitmapFrameFromVideoThread::Calculate(void)
{
 TVideoOutputFrame* VideoOutputFrame=VideoOutputForm->GetVideoOutputFrame(FrameIndex);

 if(VideoOutputFrame)
 {
  double time_stamp;
  VideoOutputFrame->CaptureThread->ReadSourceSafe(TempBitmap, time_stamp, false);
  if(TempBitmap.GetData())
   WriteSourceSafe(TempBitmap, false);

  else
  {
   TempBitmap.SetRes(640, 480, 3);
   TempBitmap.Fill(0);
   WriteSourceSafe(TempBitmap, true);
  }
 }
}
//---------------------------------------------------------------------------
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoGetBitmapFrameFromComponentThread::TVideoGetBitmapFrameFromComponentThread(TTVideoRegistratorFrame *frame, bool CreateSuspended)
: TVideoGetBitmapFrameThread(frame, CreateSuspended)
{
 //TempBitmap=new Graphics::TBitmap;
}

__fastcall TVideoGetBitmapFrameFromComponentThread::~TVideoGetBitmapFrameFromComponentThread(void)
{
 //if(TempBitmap)
 //{
 // delete TempBitmap;
 // TempBitmap=0;
 //}
}
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
bool TVideoGetBitmapFrameFromComponentThread::SetComponentName(const std::string &comp_name)
{
 if(ComponentName == comp_name)
  return true;

 ComponentName = comp_name;
 return true;
}

const std::string TVideoGetBitmapFrameFromComponentThread::GetComponentName(void)
{
 return ComponentName;
}

bool TVideoGetBitmapFrameFromComponentThread::SetPropertyName(const std::string &property_name)
{
 if(PropertyName == property_name)
  return true;

 PropertyName = property_name;
 return true;
}

const std::string TVideoGetBitmapFrameFromComponentThread::GetPropertyName(void)
{
 return PropertyName;
}

// --------------------------
// ���������� �������
// --------------------------
void __fastcall TVideoGetBitmapFrameFromComponentThread::Start(void)
{
 TVideoGetBitmapFrameThread::Start();
}

void __fastcall TVideoGetBitmapFrameFromComponentThread::Stop(void)
{
 TVideoGetBitmapFrameThread::Stop();
}


void __fastcall TVideoGetBitmapFrameFromComponentThread::BeforeCalculate(void)
{

}

void __fastcall TVideoGetBitmapFrameFromComponentThread::AfterCalculate(void)
{
  TVideoGetBitmapFrameThread::AfterCalculate();
  //Sleep(30);
}

void __fastcall TVideoGetBitmapFrameFromComponentThread::Calculate(void)
{
 if(ComponentName != "" && PropertyName != "")
 {
  const RDK::UBitmap* bmp=(const RDK::UBitmap*)Model_GetComponentBitmapOutput(ComponentName.c_str(), PropertyName.c_str());
  if(bmp && bmp->GetData())
  {
   TempBitmap=*bmp;
   WriteSourceSafe(TempBitmap, true);
  }
  else
  {
   TempBitmap.SetRes(640, 480, 3);
   TempBitmap.Fill(0);
   WriteSourceSafe(TempBitmap, true);
  }
 }
}
//---------------------------------------------------------------------------
__fastcall TTVideoRegistratorFrame::TTVideoRegistratorFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 // ���������� ������� ������
 FillErrorsArray();

 // ������ ������ ������
 RecordingMethodComboBox->Items->Add("rm_AVI");
 RecordingMethodComboBox->Items->Add("rm_ASF");
 RecordingMethodComboBox->Items->Add("rm_FLV");
 RecordingMethodComboBox->Items->Add("rm_MKV");
 RecordingMethodComboBox->Items->Add("rm_MOV");
 RecordingMethodComboBox->Items->Add("rm_MP4");
 RecordingMethodComboBox->Items->Add("rm_MPG");
 RecordingMethodComboBox->Items->Add("rm_Multiplexer");
 RecordingMethodComboBox->Items->Add("rm_SendToDV");
 RecordingMethodComboBox->Items->Add("rm_WebM");
 RecordingMethodComboBox->ItemIndex=0;

 // ������ ������
 VideoCompressorComboBox->Items->Clear();

 std::string strings=AnsiString(VideoGrabber->VideoCompressors).c_str();
 std::vector<std::string> temp;
 RDK::separatestring(strings, temp, '\n');
 for(int i=0; i<temp.size(); i++)
 {
  VideoCompressorComboBox->Items->Add(temp[i].c_str());
 }
 VideoCompressorComboBox->ItemIndex=0;

 // �������� ������ ���������
 MyComponentsListForm=new TUComponentsListForm(this);
}
//---------------------------------------------------------------------------
__fastcall TTVideoRegistratorFrame::~TTVideoRegistratorFrame(void)
{
 if(BitmapFrameThread)
 {
  BitmapFrameThread->Terminate();
  WaitForSingleObject(BitmapFrameThread->GetFrameNotInProgress(),1000);
  BitmapFrameThread->WaitFor();
  delete BitmapFrameThread;
  BitmapFrameThread=0;
 }

 if(MyComponentsListForm)
  delete MyComponentsListForm;
}
//---------------------------------------------------------------------------
// ������
// ���������� ������� ������
void TTVideoRegistratorFrame::FillErrorsArray(void)
{
 Errors[XCAMSNOTFOUND]="Cameras not found";
 Errors[XREINITSYSREM]="Dll already init";
 Errors[XCAMALREADYINIT]="Camera already init";
 Errors[XCAMALREADYSTART]="Camera already started";
 Errors[XNOIP]="IP not found";
 Errors[XINITERROR]="Init error";
 Errors[XSTARTERROR]="Camera start error";
 Errors[XCAMNOTINIT]="Camera not init";
 Errors[XFRAMEREADERROR]="Frame read error";
 Errors[XCAMNOTSTARTED]="Camera not started";
 Errors[XCAMFRAMETYPEERROR]="Frame type error";
 Errors[XCAMCOULDNOTSTOP]="Camera could not stopped";
}
//---------------------------------------------------------------------------
// ������������ ������
bool TTVideoRegistratorFrame::WriteLogMessage(const int &err)
{
 if(Errors[err] != "")
 {
  //LogMemo->Lines->Add(Now().DateString()+" "+Now().TimeString()+" "+Errors[err].c_str());
  Engine_LogMessage(RDK_EX_DEBUG, Errors[err].c_str());
  return false;
 }

 return true;
}
//---------------------------------------------------------------------------
// ��������� ����� ������ preview
bool TTVideoRegistratorFrame::SetPreviewFlag(const bool &value)
{
 if(PreviewFlag == value)
  return true;

 PreviewFlag = value;
 return true;
}
//---------------------------------------------------------------------------
// �������������� DLL
int TTVideoRegistratorFrame::Init(void)
{
 return 0;
}
//---------------------------------------------------------------------------
// ���������������� DLL
int TTVideoRegistratorFrame::UnInit(void)
{
 //
 return 0;
}
//---------------------------------------------------------------------------
// �������������� ��������� direct network streaming TVideoGrabber
int TTVideoRegistratorFrame::InitStreamingSettings(void)
{
 VideoGrabber->ASFVideoMaxKeyFrameSpacing = StrToIntDef(MaxKeyFrameSpacingLabeledEdit->Text, 100);
 VideoGrabber->ASFBufferWindow = StrToIntDef(BufferWindowLabeledEdit->Text, 100);
 VideoGrabber->ASFVideoWidth = StrToIntDef(VideoWidthLabeledEdit->Text, 320);
 VideoGrabber->ASFVideoHeight = StrToIntDef(VideoHeightLabeledEdit->Text, 240);
 VideoGrabber->ASFVideoBitRate = StrToIntDef(BitRateLabeledEdit->Text, 2000000);
 VideoGrabber->NetworkStreaming = ns_ASFDirectNetworkStreaming;
 VideoGrabber->NetworkStreamingType = nst_VideoStreaming;
 VideoGrabber->ASFNetworkPort = StrToInt(PortLabeledEdit->Text);
 VideoGrabber->FrameRate = StrToIntDef(FrameRateLabeledEdit->Text, 30);
 VideoGrabber->ASFVideoFrameRate = StrToIntDef(FrameRateLabeledEdit->Text, 30);
 VideoGrabber->ASFNetworkMaxUsers = StrToIntDef(MaxUsersLabeledEdit->Text, 4);
}
//---------------------------------------------------------------------------
// �������������� ��������� ������ � ���� TVideoGrabber
int TTVideoRegistratorFrame::InitRecordingSettings(void)
{
 VideoGrabber->ASFVideoWidth = StrToIntDef(RecordWidthLabeledEdit->Text, 320);
 VideoGrabber->ASFVideoHeight = StrToIntDef(RecordHeightLabeledEdit->Text, 240);
 VideoGrabber->ASFVideoFrameRate = StrToIntDef(RecordingFrameRateLabeledEdit->Text, 30);
 VideoGrabber->FrameRate = StrToIntDef(RecordingFrameRateLabeledEdit->Text, 30);
 VideoGrabber->RecordingFileName = RecordingFileNameLabeledEdit->Text;
 //VideoGrabber->HoldRecording = true;
 VideoGrabber->RecordingMethod = RecordingMethodComboBox->ItemIndex;
 VideoGrabber->VideoCompressor = VideoCompressorComboBox->ItemIndex;
}
//---------------------------------------------------------------------------
int TTVideoRegistratorFrame::GetBitmapFrame(void)
{
 BitmapFrameThread->ReadSourceSafe(InputFrameBitmap, false);
}

// �������� � ���������� TBitmap ��� �������� ����� � ������
int TTVideoRegistratorFrame::PrepareBitmapFrame(void)
{
 if(InputFrameBitmap)
  return 0;

 InputFrameBitmap = new TBitmap;
 //frame = new unsigned char [frameheight_*framewidth_];

 struct
 {
  TLogPalette lpal;
  TPaletteEntry dummy[256];
 } SysPal;

 InputFrameBitmap->HandleType=bmDIB;

 InputFrameBitmap->PixelFormat=pf8bit;

 SysPal.lpal.palVersion=0x300;
 SysPal.lpal.palNumEntries=256;
 for(int i=0;i<256;i++)
 {
  SysPal.lpal.palPalEntry[i].peRed=SysPal.lpal.palPalEntry[i].peGreen=SysPal.lpal.palPalEntry[i].peBlue=i;
  SysPal.lpal.palPalEntry[i].peFlags=0;
 }

 if(InputFrameBitmap->Palette)
  DeleteObject(InputFrameBitmap->Palette);

 InputFrameBitmap->Palette=CreatePalette((const tagLOGPALETTE *)&SysPal.lpal);
 return 0;
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::NetworkStreamingButtonClick(TObject *Sender)
{
 StopNetworkStreamingButtonClick(this);

 PrepareBitmapFrame();
 InitStreamingSettings();
 PreviewFlag=true;
 VideoGrabber->VideoSource=vs_JPEGsOrBitmaps;
 Mode=SourcePageControl->ActivePageIndex;

 switch(Mode)
 {
  case 0:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromComponentThread(this, true);
   TVideoGetBitmapFrameFromComponentThread* thread=dynamic_cast<TVideoGetBitmapFrameFromComponentThread*>(BitmapFrameThread);
   thread->SetComponentName(AnsiString(ComponentNameLabeledEdit->Text).c_str());
   thread->SetPropertyName(AnsiString(ComponentPropertyNameLabeledEdit->Text).c_str());
   thread->Resume();
   break;
  }
  case 1:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromVideoThread(this, true);
   TVideoGetBitmapFrameFromVideoThread* thread=dynamic_cast<TVideoGetBitmapFrameFromVideoThread*>(BitmapFrameThread);
   thread->SetFrameIndex(StrToIntDef(FrameIndexLabeledEdit->Text, 0));
   thread->Resume();
   break;
  }
 }
 //UGEngineControlForm->Start1Click(this);

 VideoGrabber->StartPreview();
 if(VideoGrabber->StreamingURL!="")
 {
  //RecordingTabSheet->TabVisible=false;
  Engine_LogMessage(RDK_EX_INFO, (std::string("Network streaming started: ")+AnsiString(VideoGrabber->StreamingURL).c_str()).c_str());
  //LogMemo->Lines->Add(VideoGrabber->StreamingURL);
  //LogMemo->Lines->Add(Now().DateString()+" "+Now().TimeString()+ " Network streaming started");
  //StreamingFlag=true;
 }

 else
  //LogMemo->Lines->Add("Network streaming not running");
  Engine_LogMessage(RDK_EX_INFO, (std::string("Network streaming not running")).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StopNetworkStreamingButtonClick(TObject *Sender)
{
 if(VideoGrabber->NetworkStreaming == ns_Disabled)
  return;

 VideoGrabber->NetworkStreaming=ns_Disabled;
 VideoGrabber->StopPreview();
  //LogMemo->Lines->Add(Now().DateString()+" "+Now().TimeString()+ " Network streaming stopped");
  Engine_LogMessage(RDK_EX_INFO, (std::string("Network streaming stopped")).c_str());

 if(BitmapFrameThread)
 {
  BitmapFrameThread->Terminate();
  WaitForSingleObject(BitmapFrameThread->GetFrameNotInProgress(),1000);
  BitmapFrameThread->WaitFor();
  delete BitmapFrameThread;
  BitmapFrameThread=0;
 }

  //InputFrameBitmap->Free();
  //InputFrameBitmap=NULL;
  //delete InputFrameBitmap;

  //RecordingTabSheet->TabVisible=true;
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::VideoGrabberVideoFromBitmapsNextFrameNeeded(TObject *Sender,
		  bool FirstSample)
{
 HBITMAP BitmapHandle;

 GetBitmapFrame();

 if(InputFrameBitmap)
 {
  BitmapHandle = InputFrameBitmap->Handle;
  VideoGrabber->SendImageToVideoFromBitmaps ("", BitmapHandle, false, false);
 }
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::GetStreamingHostButtonClick(TObject *Sender)
{
 if(VideoGrabber->StreamingURL != "")
  //LogMemo->Lines->Add(VideoGrabber->StreamingURL);
  Engine_LogMessage(RDK_EX_INFO, (AnsiString(VideoGrabber->StreamingURL)).c_str());

 else
  //LogMemo->Lines->Add(Now().DateString()+" "+Now().TimeString()+ " Network streaming not running");
  Engine_LogMessage(RDK_EX_INFO, (std::string("Network streaming not running").c_str()));
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::InitButtonClick(TObject *Sender)
{
 //
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::VideoGrabberFrameCaptureCompleted(TObject *Sender,
          Pointer FrameBitmap, int BitmapWidth, int BitmapHeight, DWORD FrameNumber,
          __int64 FrameTime, TFrameCaptureDest DestType, UnicodeString FileName,
          bool Success, int FrameId)
{
 if(PreviewFlag)
 {
  PreviewImage->Picture->Bitmap->Assign(InputFrameBitmap);
  PreviewImage->Update();
  PreviewImage->Repaint();
 }
}
//---------------------------------------------------------------------------




void __fastcall TTVideoRegistratorFrame::StartRecordingButtonClick(TObject *Sender)
{
 StopRecordingButtonClick(this);

 PrepareBitmapFrame();

 // recording settings
 InitRecordingSettings();
 VideoGrabber->VideoSource = vs_JPEGsOrBitmaps;
 PreviewFlag = true;
 Mode=SourcePageControl->ActivePageIndex;

 switch(Mode)
 {
  case 0:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromComponentThread(this, true);
   TVideoGetBitmapFrameFromComponentThread* thread=dynamic_cast<TVideoGetBitmapFrameFromComponentThread*>(BitmapFrameThread);
   thread->SetComponentName(AnsiString(ComponentNameLabeledEdit->Text).c_str());
   thread->SetPropertyName(AnsiString(ComponentPropertyNameLabeledEdit->Text).c_str());
   thread->Resume();
   break;
  }
  case 1:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromVideoThread(this, true);
   TVideoGetBitmapFrameFromVideoThread* thread=dynamic_cast<TVideoGetBitmapFrameFromVideoThread*>(BitmapFrameThread);
   thread->SetFrameIndex(StrToIntDef(FrameIndexLabeledEdit->Text, 0));
   thread->Resume();
   break;
  }
 }
 //UGEngineControlForm->Start1Click(this);

 if(VideoGrabber->StartRecording())
 {
  //LogMemo->Lines->Add("Recording started");
  Engine_LogMessage(RDK_EX_INFO, (std::string("Recording started").c_str()));
  //NetworkStreamingTabSheet->TabVisible=false;
 }

 //RecordingFlag=true;
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StopRecordingButtonClick(TObject *Sender)
{
 VideoGrabber->StopRecording();
 //LogMemo->Lines->Add("Recording stopped");
 Engine_LogMessage(RDK_EX_INFO, (std::string("Recording stopped").c_str()));

 if(BitmapFrameThread)
 {
  //WaitForSingleObject(BitmapFrameThread->GetCalcCompleteEvent(),1000);
  BitmapFrameThread->Terminate();
  WaitForSingleObject(BitmapFrameThread->GetFrameNotInProgress(),1000);
  BitmapFrameThread->WaitFor();
  delete BitmapFrameThread;
  BitmapFrameThread=0;
 }

 //InputFrameBitmap->Free();
 //InputFrameBitmap=NULL;
 //delete InputFrameBitmap;

 //NetworkStreamingTabSheet->TabVisible=true;
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::VideoGrabberAVIDurationUpdated(TObject *Sender,
          UnicodeString FileName, DWORD FrameCount, double &FrameRate, __int64 &Duration)

{
 FrameRate = StrToIntDef(RecordingFrameRateLabeledEdit->Text, 30);
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StartPreviewButtonClick(TObject *Sender)

{
 PrepareBitmapFrame();

 VideoGrabber->VideoSource = vs_JPEGsOrBitmaps;
 PreviewFlag = true;
 Mode=SourcePageControl->ActivePageIndex;

 switch(Mode)
 {
  case 0:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromComponentThread(this, true);
   TVideoGetBitmapFrameFromComponentThread* thread=dynamic_cast<TVideoGetBitmapFrameFromComponentThread*>(BitmapFrameThread);
   thread->SetComponentName(AnsiString(ComponentNameLabeledEdit->Text).c_str());
   thread->SetPropertyName(AnsiString(ComponentPropertyNameLabeledEdit->Text).c_str());
   thread->Resume();
   break;
  }
  case 1:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromVideoThread(this, true);
   TVideoGetBitmapFrameFromVideoThread* thread=dynamic_cast<TVideoGetBitmapFrameFromVideoThread*>(BitmapFrameThread);
   thread->SetFrameIndex(StrToIntDef(FrameIndexLabeledEdit->Text, 0));
   thread->Resume();
   break;
  }
 }
 //UGEngineControlForm->Start1Click(this);

 VideoGrabber->FrameRate = StrToIntDef(FrameRateLabeledEdit->Text, 30);
 if(VideoGrabber->StartPreview());
  //LogMemo->Lines->Add("Preview started");
  Engine_LogMessage(RDK_EX_INFO, (std::string("Preview started").c_str()));
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StopButtonClick(TObject *Sender)
{
 VideoGrabber->StopPreview();
 //LogMemo->Lines->Add(Now().DateString()+" "+Now().TimeString()+ " Preview stopped");
 Engine_LogMessage(RDK_EX_INFO, (std::string("Preview stopped").c_str()));

 if(BitmapFrameThread)
 {
  //WaitForSingleObject(BitmapFrameThread->GetCalcCompleteEvent(),1000);
  BitmapFrameThread->Terminate();
  WaitForSingleObject(BitmapFrameThread->GetFrameNotInProgress(),1000);
  BitmapFrameThread->WaitFor();
  delete BitmapFrameThread;
  BitmapFrameThread=0;
 }

 //UGEngineControlForm->Pause1Click(this);
}
//---------------------------------------------------------------------------
// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ����� �������
void TTVideoRegistratorFrame::ABeforeCalculate(void)
{

}

// �����, ���������� ����� �������
void TTVideoRegistratorFrame::ABeforeReset(void)
{

}

// �����, ���������� ����� ���� �������
void TTVideoRegistratorFrame::AAfterCalculate(void)
{

}


// ���������� ����������
void TTVideoRegistratorFrame::AUpdateInterface(void)
{

}

// ��������� ��������� ���������� � xml
void TTVideoRegistratorFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 // Network Streaming
 xml.WriteString("ComponentName", AnsiString(ComponentNameLabeledEdit->Text).c_str());
 xml.WriteString("ComponentProperty", AnsiString(ComponentPropertyNameLabeledEdit->Text).c_str());
 xml.WriteString("FrameIndex", AnsiString(FrameIndexLabeledEdit->Text).c_str());
 xml.WriteString("Port", AnsiString(PortLabeledEdit->Text).c_str());
 xml.WriteString("BufferWindow", AnsiString(BufferWindowLabeledEdit->Text).c_str());
 xml.WriteString("BitRate", AnsiString(BitRateLabeledEdit->Text).c_str());
 xml.WriteString("MaxKeyFrameSpacing", AnsiString(MaxKeyFrameSpacingLabeledEdit->Text).c_str());
 xml.WriteString("FrameRate", AnsiString(FrameRateLabeledEdit->Text).c_str());
 xml.WriteString("MaxUsers", AnsiString(MaxUsersLabeledEdit->Text).c_str());
 xml.WriteString("VideoHeight", AnsiString(VideoHeightLabeledEdit->Text).c_str());
 xml.WriteString("VideoWidth", AnsiString(VideoWidthLabeledEdit->Text).c_str());

 // Recording
 xml.WriteString("RecordingFileName", AnsiString(RecordingFileNameLabeledEdit->Text).c_str());
 xml.WriteString("RecordingFrameRate", AnsiString(RecordingFrameRateLabeledEdit->Text).c_str());
}

// ��������� ��������� ���������� �� xml
void TTVideoRegistratorFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
  // Network Streaming
 ComponentNameLabeledEdit->Text=(xml.ReadString("ComponentName", "")).c_str();
 ComponentPropertyNameLabeledEdit->Text=(xml.ReadString("ComponentProperty", "")).c_str();
 FrameIndexLabeledEdit->Text=(xml.ReadString("FrameIndex", "")).c_str();
 PortLabeledEdit->Text=(xml.ReadString("Port", "")).c_str();
 BufferWindowLabeledEdit->Text=(xml.ReadString("BufferWindow", "")).c_str();
 BitRateLabeledEdit->Text=(xml.ReadString("BitRate", "").c_str());
 MaxKeyFrameSpacingLabeledEdit->Text=(xml.ReadString("MaxKeyFrameSpacing", "")).c_str();
 FrameRateLabeledEdit->Text=(xml.ReadString("FrameRate", "")).c_str();
 MaxUsersLabeledEdit->Text=(xml.ReadString("MaxUsers", "")).c_str();
 VideoHeightLabeledEdit->Text=(xml.ReadString("VideoHeight", "")).c_str();
 VideoWidthLabeledEdit->Text=(xml.ReadString("VideoWidth", "")).c_str();

 // Recording
 RecordingFileNameLabeledEdit->Text=(xml.ReadString("RecordingFileName", "")).c_str();
 RecordingFrameRateLabeledEdit->Text=(xml.ReadString("RecordingFrameRate", "")).c_str();

 UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::BrowseComponentButtonClick(TObject *Sender)
{
 MyComponentsListForm->UpdateInterface(true);
 if(MyComponentsListForm->ShowIOSelect() != mrOk)
  return;

 ComponentNameLabeledEdit->Text=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str();
 ComponentPropertyNameLabeledEdit->Text=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput().c_str();
}
//---------------------------------------------------------------------------


void __fastcall TTVideoRegistratorFrame::BrowseFileNameButtonClick(TObject *Sender)
{
 RecordingFileOpen->Execute();
 RecordingFileNameLabeledEdit->Text=RecordingFileOpen->FileName;
}
//---------------------------------------------------------------------------

