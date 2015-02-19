//---------------------------------------------------------------------------
#include <vcl.h>
#include <jpeg.hpp>
#pragma hdrstop

#include "VideoOutputFrameUnit.h"
#include "TUBitmap.h"
//#include "TVideoGrabberControlFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "myrdk.h"
#include "rdk_initdll.h"
#include "UGEngineControlFormUnit.h"
#include "TVideoCaptureOptionsFormUnit.h"
#include "TBcbSupport.h"
//#include "USharedMemoryLoader.h"
//#include "TUFileSystem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VidGrab"
#pragma link "TUVisualControllerFrameUnit"
#pragma link "TUHttpServerUnit"
#pragma link "TVideoRegistratorFrameUnit"
#pragma resource "*.dfm"
TVideoOutputFrame *VideoOutputFrame;

String TVGrabberLicenseString;

//---------------------------------------------------------------------------
__fastcall TVideoOutputFrame::TVideoOutputFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 CaptureThread=0;
 FrameIndex=0;
// Capture=0;

 // Модуль графики
 Graph.SetCanvas(&BmpCanvas);

 // Отрисовка геометрии
 GeometryGraphics.SetGraphics(&Graph);

 x1b=x2b=y1b=y2b=-1;
 corrx1b=corry1b=corrx2b=corry2b=-1;
 left=-1; top=-1; width=-1; height=-1;
 PointIndex=-1;
 FigureIndex=-1;

 CorrSelectFlag=false;
 ZoneSelectEnable=false;

 ZoneSelectEvent=CreateEvent(0,0,0,0);
 UpdateFlag=false;

 PointFlag=1;
// FigureFlag=false;

 MyVideoOutputToolsForm=new TVideoOutputToolsForm(this,
    this,
    GeometryGraphics,
//    SampleGeometryGraphics,
//    Figure,
    FigureIndex,
//    FigureFlag,
	PointIndex,
	PointFlag);

// MyVideoGrabberControlForm=new TVideoGrabberControlForm(this);
// MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this,0);//VideoGrabber);

// ConvertBitmap=new Graphics::TBitmap;

 MyComponentsListForm=new TUComponentsListForm(this);

 IsStarted=false;

 VideoCaptureOptionsForm=new TVideoCaptureOptionsForm(this);
 VideoCaptureOptionsForm->VideoOutputFrame=this;

 std::map<int, RDK::UEPtr<TVideoCaptureThread> >::const_iterator I=TVideoCaptureOptionsForm::GetVideoSourcePrototypes().begin(),
													J=TVideoCaptureOptionsForm::GetVideoSourcePrototypes().end();
 for(;I != J;++I)
 {
  VideoSourceOptions[I->first].Create("VideoSourceThread");
 }
}

__fastcall TVideoOutputFrame::~TVideoOutputFrame(void)
{
 IsStarted=false;
 CloseHandle(ZoneSelectEvent);

 delete VideoCaptureOptionsForm;
 VideoCaptureOptionsForm=0;

 delete MyVideoOutputToolsForm;
 MyVideoOutputToolsForm=0;

// delete MyVideoGrabberControlForm;
// MyVideoGrabberControlForm=0;

// delete ConvertBitmap;

 if(MyComponentsListForm)
  delete MyComponentsListForm;

 if(CaptureThread)
 {
  CaptureThread->Terminate();
  WaitForSingleObject(CaptureThread->GetFrameNotInProgress(),INFINITE);
  CaptureThread->WaitFor();
  delete CaptureThread;
  CaptureThread=0;
 }
}

// ---------------------------
// Методы управления поддерживаемыми источниками видео
// ---------------------------
/// Создает копию требуемого треда по индексу видеорежима
RDK::UEPtr<TVideoCaptureThread> TVideoOutputFrame::TakeVideoCapureThread(int mode, TVideoOutputFrame *frame, bool create_suspended)
{
 const std::map<int, RDK::UEPtr<TVideoCaptureThread> >::iterator I=VideoCaptureOptionsForm->VideoSourcePrototypes.find(mode);
 if(I == VideoCaptureOptionsForm->VideoSourcePrototypes.end())
  return 0;
 return I->second->New(frame, create_suspended);
}

/// Уничтожает заданный тред
void TVideoOutputFrame::ReturnVideoCapureThread(RDK::UEPtr<TVideoCaptureThread> thread)
{
 delete thread;
}
// ---------------------------

// -----------------------------
// Методы управления видеозахватом
// -----------------------------
/// Инициализация захвата в заданном режиме
/// Если mode == -1 то осуществляет переиницализацию в текущем режиме
void TVideoOutputFrame::Init(int mode)
{
 Pause();
 if(mode == -1)
 {
  if(CaptureThread)
   CaptureThread->LoadParameters(VideoSourceOptions[mode]);
 }
 else
 {
  if(mode < 0 || !TVideoCaptureOptionsForm::CheckVideoSourcePrototypes(mode))
   return;

  UnInit();
  CaptureThread=TakeVideoCapureThread(mode,this,true);
  if(!CaptureThread)
   return;
  CaptureThread->Priority=RDK_DEFAULT_THREAD_PRIORITY;
  CaptureThread->SetChannelIndex(FrameIndex);

  #ifdef RDK_MUTEX_DEADLOCK_DEBUG
  TUThreadInfo info;
  info.Pid=CaptureThread->ThreadID;
  info.Name=string("CaptureThread ")+RDK::sntoa(FrameIndex);
  GlobalThreadInfoMap[info.Pid]=info;
  #endif

//  TVideoCaptureThreadVideoGrabber *thread=dynamic_cast<TVideoCaptureThreadVideoGrabber*>(CaptureThread);
//  if(thread && thread->GetVideoGrabber())
//   thread->GetVideoGrabber()->LicenseString=TVGrabberLicenseString;

  CaptureThread->Resume();
  CaptureThread->LoadParameters(VideoSourceOptions[mode]);
 }
 UpdateInterface();
}

void TVideoOutputFrame::Init(int mode, RDK::USerStorageXML &raw_xml_data)
{
 Pause();
 if(mode == -1)
 {
  if(CaptureThread)
  {
   CaptureThread->LoadParametersEx(raw_xml_data);
   CaptureThread->SaveParameters(VideoSourceOptions[mode]);
  }
 }
 else
 {
  if(mode < 0 || !TVideoCaptureOptionsForm::CheckVideoSourcePrototypes(mode))
   return;

  UnInit();
  CaptureThread=TakeVideoCapureThread(mode,this,true);
  if(!CaptureThread)
   return;
  CaptureThread->Priority=RDK_DEFAULT_THREAD_PRIORITY;
  CaptureThread->SetChannelIndex(FrameIndex);

  #ifdef RDK_MUTEX_DEADLOCK_DEBUG
  TUThreadInfo info;
  info.Pid=CaptureThread->ThreadID;
  info.Name=string("CaptureThread ")+RDK::sntoa(FrameIndex);
  GlobalThreadInfoMap[info.Pid]=info;
  #endif
//  TVideoCaptureThreadVideoGrabber *thread=dynamic_cast<TVideoCaptureThreadVideoGrabber*>(CaptureThread);
//  if(thread)
//   thread->GetVideoGrabber()->LicenseString=TVGrabberLicenseString;

  CaptureThread->Resume();
  CaptureThread->LoadParametersEx(raw_xml_data);
  CaptureThread->SaveParameters(VideoSourceOptions[mode]);
 }
 UpdateInterface();
}

void TVideoOutputFrame::Init(RDK::USerStorageXML &raw_xml_data)
{
 int mode=raw_xml_data.ReadInteger("SourceMode",-1);
 if(mode == -1)
  return; // TODO тут запись в лог

 Init(mode, raw_xml_data);
}

void TVideoOutputFrame::Init(std::string raw_xml_data)
{
	RDK::USerStorageXML xml;
	xml.Load(raw_xml_data, "VideoSourceTest");
	Init(xml);
}

/// Деинициализация захвата
void TVideoOutputFrame::UnInit(void)
{
 Pause();
 DestroyCaptureThread();
}

/// Запуск захвата
void TVideoOutputFrame::Start(double time)
{
 if(CaptureThread)
 {
  CaptureThread->Start(time);
  WaitForSingleObject(CaptureThread->GetFrameNotInProgress(),30);
 }
 Timer->Enabled=true;

 IsStarted=true;
}

/// Останов захвата
void TVideoOutputFrame::Pause(double time)
{
 IsStarted=false;
 Timer->Enabled=false;

 if(CaptureThread)
 {
  CaptureThread->Stop(time);
  WaitForSingleObject(CaptureThread->GetFrameNotInProgress(),30);
 }
}

/// Чтение текущего изображения в bmp
bool TVideoOutputFrame::ReadSourceSafe(RDK::UBitmap &bmp, double &time_stamp, bool reflect)
{
 if(CaptureThread)
  return CaptureThread->ReadSourceSafe(bmp,time_stamp,reflect);
 return false;
}
//---------------------------------------------------------------------------
/// Проверяет состояние завхата по id канала
/// 0 - не активен
/// 1 - активен
int TVideoOutputFrame::CheckCaptureState(void) const
{
 if(!CaptureThread)
  return 0;

 return CaptureThread->CheckCaptureThreadState();
}
// -----------------------------
/// Инициализация первичных настроек
void TVideoOutputFrame::InitPrimarySettings(void)
{
 NetworkStreamingFrame->RecordingTabSheet->TabVisible=false;
 RecordingFrame->NetworkStreamingTabSheet->TabVisible=false;

 NetworkStreamingFrame->FrameIndexLabeledEdit->Text=IntToStr(FrameIndex);
 RecordingFrame->FrameIndexLabeledEdit->Text=IntToStr(FrameIndex);

 RecordingFrame->Init();

 // Получаем указатели на два динамических пункта меню
 TMenuItem *parentItem1=0;
 TMenuItem *parentItem2=0;
 for(int i=0; i<RecordingPopupMenu->Items->Count; i++)
 {
  if(RecordingPopupMenu->Items->Items[i]->Name == "VideoCodec")
  {
   parentItem1=RecordingPopupMenu->Items->Items[i];
  }

  if(RecordingPopupMenu->Items->Items[i]->Name == "RecordingMethod")
  {
   parentItem2=RecordingPopupMenu->Items->Items[i];
  }
 }

 // Наполняем веткор имен для динамического меню
 std::vector<std::string> itemList1;
 std::vector<std::string> itemList2;
 itemList1.clear();
 itemList2.clear();

 int size=RecordingFrame->VideoCompressorComboBox->Items->Capacity;
 for(int i=0; i<size; i++)
 {
  itemList1.push_back(AnsiString(RecordingFrame->VideoCompressorComboBox->Items->Strings[i]).c_str());
 }

 size=RecordingFrame->RecordingMethodComboBox->Items->Capacity;
 for(int i=0; i<size; i++)
 {
  itemList2.push_back(AnsiString(RecordingFrame->RecordingMethodComboBox->Items->Strings[i]).c_str());
 }

 // Наполняем меню из полученного вектора имен
 DynamicMenuFilling(parentItem1, itemList1);
 DynamicMenuFilling(parentItem2, itemList2);

 // Назначаем событие onClick для динамических пунктов меню
 for(int i=0; i<parentItem1->Count; i++)
 {
  parentItem1->Items[i]->OnClick=OnClickVideoCodec;
 }

 for(int i=0; i<parentItem2->Count; i++)
 {
  parentItem2->Items[i]->OnClick=OnClickRecordingMethod;
 }

 String currentItem=RecordingFrame->VideoCompressorComboBox->Items->Strings[RecordingFrame->VideoCompressorComboBox->ItemIndex];
 TMenuItem *currentCodec=parentItem1->Find(currentItem);
 if(currentCodec)
  currentCodec->Checked=true;

 currentItem=RecordingFrame->RecordingMethodComboBox->Items->Strings[RecordingFrame->RecordingMethodComboBox->ItemIndex];
 TMenuItem *currentRMethod=parentItem2->Find(currentItem);
 if(currentRMethod)
  currentRMethod->Checked=true;

 if(currentRMethod && (currentRMethod->MenuIndex==0 || currentRMethod->MenuIndex==8))
  parentItem1->Enabled=false;
}

//---------------------------------------------------------------------------
// Возвращает форму управления инициализацией видео
/*TVideoGrabberControlForm* TVideoOutputFrame::GetMyVideoGrabberControlForm(void)
{
 return MyVideoGrabberControlForm;
}
  */
// Уничтожает созданный поток
bool TVideoOutputFrame::DestroyCaptureThread(void)
{
 if(CaptureThread)
 {
  CaptureThread->Stop(0);
  CaptureThread->Terminate();
  CaptureThread->WaitFor();
  if(WaitForSingleObject(CaptureThread->GetFrameNotInProgress(),1000) != WAIT_TIMEOUT)
  {
   CaptureThread=0;
  }
  else
   return false;
 }

 return true;
}

	 /*
// Инициализация фрейма avi-файлом
void TVideoOutputFrame::InitByAvi(const String &filename)
{
 StopButtonClick(this);

   Mode=1;
//   UpdateInterfaceFlag=true;
//   TrackBar->Position=0;
//   UpdateInterfaceFlag=false;

 if(CaptureThread && dynamic_cast<TVideoCaptureThreadVideoGrabberAvi*>(CaptureThread))
 {
  TVideoCaptureThreadVideoGrabberAvi* thread=dynamic_cast<TVideoCaptureThreadVideoGrabberAvi*>(CaptureThread);
  thread->SetFileName(AnsiString(filename).c_str());
  thread->SetRepeatFlag(RepeatVideoFlag);
//  MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, thread->GetVideoGrabber());
 }
 else
 {
  if(!DestroyCaptureThread())
   return;
  if(!CaptureThread)
  {
   CaptureThread=new TVideoCaptureThreadVideoGrabberAvi(this,false);
   CaptureThread->Priority=RDK_DEFAULT_THREAD_PRIORITY;
   CaptureThread->SetChannelIndex(FrameIndex);
  }
  TVideoCaptureThreadVideoGrabberAvi* thread=dynamic_cast<TVideoCaptureThreadVideoGrabberAvi*>(CaptureThread);
  if(thread)
  {
   thread->SetFileName(AnsiString(filename).c_str());
   thread->SetRepeatFlag(RepeatVideoFlag);
//   MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, thread->GetVideoGrabber());
  }
 }

 UpdateInterface(true);
}

// Инициализация фрейма bmp-файлом
void TVideoOutputFrame::InitByBmp(const String &filename, double fps)
{
 if(CaptureThread && dynamic_cast<TVideoCaptureThreadBmp*>(CaptureThread))
 {
  dynamic_cast<TVideoCaptureThreadBmp*>(CaptureThread)->SetFileName(AnsiString(filename).c_str());
//  MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, 0);
 }
 else
 {
  StopButtonClick(this);
  if(!DestroyCaptureThread())
   return;

  if(!CaptureThread)
  {
   CaptureThread=new TVideoCaptureThreadBmp(this,false);
   CaptureThread->Priority=RDK_DEFAULT_THREAD_PRIORITY;
   CaptureThread->SetChannelIndex(FrameIndex);
   TVideoCaptureThreadBmp* thread=dynamic_cast<TVideoCaptureThreadBmp*>(CaptureThread);
   if(thread)
   {
	thread->SetFileName(AnsiString(filename).c_str());
//	MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, 0);
    thread->SetFps(fps);
   }
  }
 }
 Mode=0;
 UpdateInterface(true);
}          */

// Устанавливает отдельное изображение
bool TVideoOutputFrame::InitByBmp(const RDK::UBitmap &bmp, double fps)
{
// BmpSource=bmp;
// BmpSource.SetColorModel(RDK::ubmRGB24);

 if(CaptureThread && dynamic_cast<TVideoCaptureThreadBmp*>(CaptureThread))
 {
	RDK::ULongTime time_stamp=TDateTime::CurrentDateTime().operator double();//GetTickCount();
	CaptureThread->WriteSourceSafe(bmp,time_stamp,false);
//	MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, 0);
   dynamic_cast<TVideoCaptureThreadBmp*>(CaptureThread)->SetFps(fps);
 }
 else
 {
  StopButtonClick(this);
  if(!DestroyCaptureThread())
   return false;

  if(!CaptureThread)
  {
   CaptureThread=new TVideoCaptureThreadBmp(this,false);
   CaptureThread->Priority=RDK_DEFAULT_THREAD_PRIORITY;
   CaptureThread->SetChannelIndex(FrameIndex);
   TVideoCaptureThreadBmp* thread=dynamic_cast<TVideoCaptureThreadBmp*>(CaptureThread);
   if(thread)
   {
	RDK::ULongTime time_stamp=TDateTime::CurrentDateTime().operator double();//GetTickCount();
	thread->WriteSourceSafe(bmp,time_stamp,false);
//	MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, 0);
	thread->SetFps(fps);
   }
  }
 }
 Mode=0;

 UpdateInterface(true);
 return true;//UpdateVideo();
}

        /*
// Инициализация фрейма камерой
void TVideoOutputFrame::InitByCamera(int camera_index, int input_index, int size_index, int subtype_index, int analog_index)
{
 StopButtonClick(this);

// StartButtonClick(this);
// UpdateVideo();

 if(CaptureThread && dynamic_cast<TVideoCaptureThreadVideoGrabberCamera*>(CaptureThread))
 {
  TVideoCaptureThreadVideoGrabberCamera* thread=dynamic_cast<TVideoCaptureThreadVideoGrabberCamera*>(CaptureThread);
  thread->Init(camera_index, input_index, size_index, subtype_index, analog_index);
//  MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, thread->GetVideoGrabber());
 }
 else
 {
  if(!DestroyCaptureThread())
   return;
  if(!CaptureThread)
  {
   CaptureThread=new TVideoCaptureThreadVideoGrabberCamera(this,false);
   CaptureThread->Priority=RDK_DEFAULT_THREAD_PRIORITY;
   CaptureThread->SetChannelIndex(FrameIndex);
   TVideoCaptureThreadVideoGrabberCamera* thread=dynamic_cast<TVideoCaptureThreadVideoGrabberCamera*>(CaptureThread);
   if(thread)
   {
	thread->Init(camera_index, input_index, size_index, subtype_index, analog_index);
//	MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, thread->GetVideoGrabber());
   }
  }
 }
 UpdateInterface(true);
}

// Инициализация фрейма IP-камерой
void TVideoOutputFrame::InitByIPCamera(const String camera_url, const String user_name, const String user_password)
{
 StopButtonClick(this);
 if(CaptureThread && dynamic_cast<TVideoCaptureThreadVideoGrabberIpCamera*>(CaptureThread))
 {
  TVideoCaptureThreadVideoGrabberIpCamera* thread=dynamic_cast<TVideoCaptureThreadVideoGrabberIpCamera*>(CaptureThread);
  thread->Init(camera_url, user_name, user_password);
  thread->GetVideoGrabber()->LicenseString=TVGrabberLicenseString;//UGEngineControlForm->VideoGrabberLicenseString;
//  MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, thread->GetVideoGrabber());
 }
 else
 {
  if(!DestroyCaptureThread())
   return;
  if(!CaptureThread)
  {
   CaptureThread=new TVideoCaptureThreadVideoGrabberIpCamera(this,false);
   CaptureThread->Priority=RDK_DEFAULT_THREAD_PRIORITY;
   CaptureThread->SetChannelIndex(FrameIndex);
   TVideoCaptureThreadVideoGrabberIpCamera* thread=dynamic_cast<TVideoCaptureThreadVideoGrabberIpCamera*>(CaptureThread);
   if(thread)
   {
	thread->Init(camera_url, user_name, user_password);
	thread->GetVideoGrabber()->LicenseString=TVGrabberLicenseString;//UGEngineControlForm->VideoGrabberLicenseString;
//	MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, thread->GetVideoGrabber());
   }
  }
 }

 Mode=3;
// StartButtonClick(this);
// UpdateVideo();
 UpdateInterface(true);
}

// Инициализация последовательностью изображений
bool TVideoOutputFrame::InitByImageSequence(const String &pathname, double fps)
{
 StopButtonClick(this);
 Mode=4;

 if(!DestroyCaptureThread())
  return false;
 if(!CaptureThread)
 {
  CaptureThread=new TVideoCaptureThreadBmpSequence(this,false);
  CaptureThread->Priority=RDK_DEFAULT_THREAD_PRIORITY;
  CaptureThread->SetChannelIndex(FrameIndex);
 }

 TVideoCaptureThreadBmpSequence* thread=dynamic_cast<TVideoCaptureThreadBmpSequence*>(CaptureThread);
 if(thread)
 {
  thread->SetPathName(AnsiString(pathname).c_str());
  thread->SetRepeatFlag(RepeatSequenceFlag);
  thread->SetFps(fps);
//  MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, 0);
 }


// CurrentBmpSequenceIndex=0;
 TrackBar->Position=0;
 UpdateInterface(true);

 return true;
}

// Инициализация http-сервера
bool TVideoOutputFrame::InitByHttpServer(int listen_port)
{
 StopButtonClick(this);
 Mode=5;
 if(!DestroyCaptureThread())
  return false;
 if(!CaptureThread)
 {
  CaptureThread=new TVideoCaptureThreadHttpServer(this,false);
  CaptureThread->Priority=RDK_DEFAULT_THREAD_PRIORITY;
  CaptureThread->SetChannelIndex(FrameIndex);
  TVideoCaptureThreadHttpServer* thread=dynamic_cast<TVideoCaptureThreadHttpServer*>(CaptureThread);
  if(thread)
  {
   thread->SetListenPort(listen_port);
//   MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, 0);
  }
 }

 return true;
}

// Инициализация общей памяти
bool TVideoOutputFrame::InitBySharedMemory(int pipe_index, const std::string &pipe_name)
{
 StopButtonClick(this);

 Mode=6;

 if(!Usm_GetNumPipes)
  return false;

 if(Usm_GetNumPipes() <= pipe_index)
  Usm_SetNumPipes(pipe_index+1);

 if(CaptureThread)
 {
  CaptureThread->Stop();
  CaptureThread->Terminate();
  if(WaitForSingleObject(CaptureThread->GetFrameNotInProgress(),1000) != WAIT_TIMEOUT)
  {
   delete CaptureThread;
   CaptureThread=0;
  }
  else
   return false;
 }

 if(!CaptureThread)
 {
  CaptureThread=new TVideoCaptureThreadSharedMemory(this,false);
  CaptureThread->Priority=RDK_DEFAULT_THREAD_PRIORITY;
  CaptureThread->SetChannelIndex(FrameIndex);
  TVideoCaptureThreadSharedMemory* thread=dynamic_cast<TVideoCaptureThreadSharedMemory*>(CaptureThread);
  if(thread)
  {
   thread->SetPipeIndex(pipe_index);
   thread->SetPipeName(pipe_name);
  }
 }

 UpdateInterface(true);
 return true;
}        */

// Устанавливает название окна
bool TVideoOutputFrame::SetTitle(String title)
{
 if(title == "")
  GroupBox->Caption="";
 else
  GroupBox->Caption=String(" ")+title+String(" ");

 return true;
}

// Устанавливает координаты отображаемой зоны
// Если все координаты == -1 то зона не отображается
bool TVideoOutputFrame::SetFrameRect(int x,int y, int x_width, int y_height, TColor color)
{
 left=x; top=y; width=x_width; height=y_height;
 SelColor=color;
 UpdateVideo();
 return true;
}

// Обновляет отрисовку окна
bool TVideoOutputFrame::UpdateVideo(void)
{
 Graphics::TBitmap *bmp=Image->Picture->Bitmap;
 if(!bmp)
  return false;

  bmp->Width=BmpSource.GetWidth();
  bmp->Height=BmpSource.GetHeight();
  bmp->HandleType=bmDIB;
  switch(BmpSource.GetPixelByteLength())
  {
  case 1:
   bmp->PixelFormat=pf8bit;
  break;

  case 2:
   bmp->PixelFormat=pf16bit;
  break;

  case 3:
   bmp->PixelFormat=pf24bit;
  break;

  case 4:
   bmp->PixelFormat=pf32bit;
  break;
  }
 BmpSource.ReflectionX(&BmpCanvas);
// BmpCanvas=BmpSource;

 GeometryGraphics.Repaint();
 if(ShowCentralPointCheckBox->Checked)
 {
  RDK::UGraphics gr(&BmpCanvas);
  gr.SetPenColor(0x000000FF);
  gr.SetPenWidth(1);
  int x=StrToInt(PointXEdit->Text);
  int y=StrToInt(PointYEdit->Text);
  gr.Line(x,0,x,BmpCanvas.GetHeight());
  gr.Line(0,y,BmpCanvas.GetWidth(),y);
//  Image->Canvas->Pen->Color=clRed;
//  Image->Canvas->Pen->Width=1;
//  Image->Canvas->MoveTo(Image->Width/2,0);
//  Image->Canvas->LineTo(Image->Width/2,Image->Height-1);
//  Image->Canvas->MoveTo(0,Image->Height/2);
//  Image->Canvas->LineTo(Image->Width-1,Image->Height/2);
 }

 BmpCanvas.ReflectionX();
 BmpCanvas>>bmp;

// DrawCapture(Image->Picture->Bitmap);
 Image->Repaint();
 if(left != -1 || top != -1 || width != -1 || height != -1)
  DrawFrameRect(Image, left, top, left+width, top+height, 2, SelColor);

 Image->Repaint();
 UpdateGeometryList(MyVideoOutputToolsForm->GeometryCheckListBox,
					MyVideoOutputToolsForm->PointsCheckListBox);
  /*
 if(Mode == 4)
 {
  UpdateFlag=true;
  std::string sstamp;
  RDK::UTimeStamp stamp(long(CurrentBmpSequenceIndex),1);
  stamp>>sstamp;
  TimeEdit->Text=sstamp.c_str();
  TrackBar->Max=int(BmpSequenceNames.size())-1;
  TrackBar->Enabled=true;
  TrackBar->Position=CurrentBmpSequenceIndex;
  UpdateFlag=false;
 }  */

 return true;
}

// Отрисовываем текущее состояние видеопотока
void TVideoOutputFrame::DrawCapture(Graphics::TBitmap *bmp)
{
/* if(!bmp)
  return;

  bmp->Width=BmpSource.GetWidth();
  bmp->Height=BmpSource.GetHeight();
  bmp->HandleType=bmDIB;
  switch(BmpSource.GetPixelByteLength())
  {
  case 1:
   bmp->PixelFormat=pf8bit;
  break;

  case 2:
   bmp->PixelFormat=pf16bit;
  break;

  case 3:
   bmp->PixelFormat=pf24bit;
  break;

  case 4:
   bmp->PixelFormat=pf32bit;
  break;
  }
 BmpSource.ReflectionX(&BmpCanvas);
// BmpCanvas=BmpSource;

 GeometryGraphics.Repaint();
 BmpCanvas.ReflectionX();
 BmpCanvas>>bmp;
 */
}

// Обновляем список точек
void TVideoOutputFrame::UpdateGeometryList(TCheckListBox *GeometryCheckListBox, TCheckListBox *PointsCheckListBox)
{
 int ix=GeometryCheckListBox->ItemIndex;

 GeometryCheckListBox->Clear();
 for(size_t i=0;i<GeometryGraphics.GetNumGeometries();i++)
  GeometryCheckListBox->Items->Add(GeometryGraphics.Description(i).Name.c_str());
 for(size_t i=0;i<GeometryGraphics.GetNumGeometries();i++)
  GeometryCheckListBox->Checked[i]=GeometryGraphics.Description(i).Visible;

 if(ix>=0)
  GeometryCheckListBox->ItemIndex=ix;
 else
  GeometryCheckListBox->ItemIndex=0;

 if(GeometryCheckListBox->ItemIndex<0)
  return;
/*
 if(FigureFlag)
 {
  GeometryCheckListBox->ItemIndex=FigureIndex;
  GeometryCheckListBox->Enabled=false;
 }
 else
  GeometryCheckListBox->Enabled=true;
  */
 if(int(GeometryGraphics.GetNumGeometries())<=FigureIndex)
  return;

 ix=PointsCheckListBox->ItemIndex;

// PointsCheckListBox->Clear();
 if(GeometryCheckListBox->ItemIndex>=0 && int(CurrentGeometryGraphics.GetNumGeometries())>GeometryCheckListBox->ItemIndex)
 {
//  if(GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex) == CurrentGeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex))
//   return;
 }
 else
  CurrentGeometryGraphics=GeometryGraphics;

 CurrentGeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex)=GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex);
 int new_value=GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex).GetNumVertices();
 int diff=abs(PointsCheckListBox->Count-new_value);
 if(PointsCheckListBox->Count<new_value)
 {
  for(int i=0;i<diff;i++)
   PointsCheckListBox->Items->Add("");
 }
 else
 if(PointsCheckListBox->Count>new_value)
 {
  for(int i=0;i<diff;i++)
   PointsCheckListBox->Items->Delete(PointsCheckListBox->Count-1);
 }
 //PointsCheckListBox->Count=new_value;
 for(int i=0;i<new_value;i++)
 {
  std::stringstream stream;
  stream<<GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex).VertexName(i)<<" ";
  stream<<GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex).Vertex(i);
  PointsCheckListBox->Items->Strings[i]=stream.str().c_str();// Add(stream.str().c_str());
//  PointsCheckListBox->Items->Add(stream.str().c_str());
   if(GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex).Vertex(i).z>=0)
   {
    PointsCheckListBox->Checked[i]=true;
   }
   else
    PointsCheckListBox->Checked[i]=false;
 }

 if(ix>=0)
  PointsCheckListBox->ItemIndex=ix;
 else
  PointsCheckListBox->ItemIndex=0;
}

// Метод отрисовки прямоугольной зоны
void __fastcall TVideoOutputFrame::DrawFrameRect(TImage *image, int x1, int y1, int x2,
                                                int y2, int framewidth, TColor color)
{
 TRect rect;
 int realwidth;

 image->Picture->Bitmap->Canvas->Brush->Color = color;

 if(framewidth > 0)
  realwidth=framewidth;
 else
  {
   int w1=image->Picture->Bitmap->Width/image->Width;
   int w2=image->Picture->Bitmap->Height/image->Height;
   realwidth=(w1>w2)?w1:w2;
   if(realwidth <= 0)
    realwidth=1;

   realwidth++;
  }

 for(int i=0;i<realwidth;i++)
  {
   rect.left=x1+i; rect.top=y1+i;
   rect.right=x2-i; rect.bottom=y2-i;
   image->Picture->Bitmap->Canvas->FrameRect(rect);
  }
}

// Добавляет очередной элемент фигуры
void TVideoOutputFrame::AddFigureRect(double l,double t,double w,double h)
{
 if(l<0 || t<0)
  return;

 if(FigureIndex<0)
  return;

 RDK::MVector<double,2> point;
 point.x=l+w/2;
 point.y=(t+h/2);
 EditPoint(FigureIndex,PointIndex,point);

 UpdateVideo();
}

/// Возвращает число фигур
int TVideoOutputFrame::GetNumFigures(void)
{
 return GeometryGraphics.GetNumGeometries();
}


/// Добавляет новую фигуру
/// Функция возвращает индекс фигуры
int TVideoOutputFrame::AddFigure(TColor color)
{
 GeometryGraphics.SetNumGeometries(GeometryGraphics.GetNumGeometries()+1);
 int index=GeometryGraphics.GetNumGeometries()-1;
 GeometryGraphics.Description(index).PenWidth=3;
 GeometryGraphics.Description(index).Color.rgb.r=color & 0x000000FF;
 GeometryGraphics.Description(index).Color.rgb.g=(color & 0x0000FF00) >> 8;
 GeometryGraphics.Description(index).Color.rgb.b=(color & 0x00FF0000) >> 16;
 GeometryGraphics.Description(index).Name=string("Figure")+RDK::sntoa(index+1,3);
 GeometryGraphics.Description(index).Description=string("Геометрия №")+RDK::sntoa(index+1);
 GeometryGraphics.Description(index).TargetPoints=true;
 return index;
}

/// Удаляет фигуру
void TVideoOutputFrame::DelFigure(int figure_index)
{
 if(figure_index < 0 || figure_index >= int(GeometryGraphics.GetNumGeometries()))
  return;

 GeometryGraphics.DelGeometry(figure_index);
}

/// Удаляет все фигуры
void TVideoOutputFrame::DelAllFigures(void)
{
 GeometryGraphics.Clear();
}

/// Модифицирует имя фигуры
void TVideoOutputFrame::EditFigureName(int figure_index, const std::string &figure_name)
{
 if(figure_index < 0 || figure_index >= int(GeometryGraphics.GetNumGeometries()))
  return;

 GeometryGraphics.Description(FigureIndex).Name=figure_name;
}

/// Возвращает число точек в фигуре
int TVideoOutputFrame::GetNumPoints(int figure_index)
{
 if(figure_index < 0 || figure_index >= int(GeometryGraphics.GetNumGeometries()))
  return 0;

 return GeometryGraphics.Geometry(figure_index).GetNumVertices();
}

/// Возвращает точку
const RDK::MVector<double,2> TVideoOutputFrame::GetPoint(int figure_index, int point_index)
{
 if(figure_index < 0 || figure_index >= int(GeometryGraphics.GetNumGeometries()))
  return RDK::MVector<double,2>(0);

 if(point_index < 0 || point_index >= int(GeometryGraphics.Geometry(figure_index).GetNumVertices()))
  return RDK::MVector<double,2>(0);

 return GeometryGraphics.Geometry(figure_index).Vertex(point_index);
}

/// Добавляет новую точку
/// Функция возвращает индекс точки
int TVideoOutputFrame::AddPoint(int figure_index, const RDK::MVector<double,2> &point_data)
{
 if(figure_index < 0 || figure_index >= int(GeometryGraphics.GetNumGeometries()))
  return -1;

 GeometryGraphics.Geometry(figure_index).SetNumVertices(GeometryGraphics.Geometry(figure_index).GetNumVertices()+1);
 int index=GeometryGraphics.Geometry(figure_index).GetNumVertices()-1;
 GeometryGraphics.Geometry(figure_index).Vertex(index).x=point_data.x;
 GeometryGraphics.Geometry(figure_index).Vertex(index).y=point_data.y;
 return index;
}

/// Добавляет новую точку
/// Имя точки задается вручную
/// Функция возвращает индекс точки
int TVideoOutputFrame::AddPoint(int figure_index, const std::string &point_name, const RDK::MVector<double,2> &point_data)
{
 if(figure_index < 0 || figure_index >= int(GeometryGraphics.GetNumGeometries()))
  return -1;

 GeometryGraphics.Geometry(figure_index).SetNumVertices(GeometryGraphics.Geometry(figure_index).GetNumVertices()+1);
 int index=GeometryGraphics.Geometry(figure_index).GetNumVertices()-1;
 GeometryGraphics.Geometry(figure_index).VertexName(index)=point_name;
 GeometryGraphics.Geometry(figure_index).Vertex(index).x=point_data.x;
 GeometryGraphics.Geometry(figure_index).Vertex(index).y=point_data.y;
 return index;
}

/// Удаляет точку
void TVideoOutputFrame::DelPoint(int figure_index, int point_index)
{
 if(figure_index < 0 || figure_index >= int(GeometryGraphics.GetNumGeometries()))
  return;

 if(point_index < 0 || point_index >= int(GeometryGraphics.Geometry(figure_index).GetNumVertices()))
  return;

 GeometryGraphics.Geometry(figure_index).DelVertex(point_index);
}

/// Удаляет все точки
void TVideoOutputFrame::DelAllPoints(int figure_index)
{
 if(figure_index < 0 || figure_index >= int(GeometryGraphics.GetNumGeometries()))
  return;

 GeometryGraphics.Geometry(figure_index).Clear();
}

/// Модифицирует точку
void TVideoOutputFrame::EditPoint(int figure_index, int point_index, const RDK::MVector<double,2> &point_data)
{
 if(figure_index < 0 || figure_index >= int(GeometryGraphics.GetNumGeometries()))
  return;

 if(point_index < 0 || point_index >= int(GeometryGraphics.Geometry(figure_index).GetNumVertices()))
  return;

 GeometryGraphics.Geometry(figure_index).Vertex(point_index).x=point_data.x;
 GeometryGraphics.Geometry(figure_index).Vertex(point_index).y=point_data.y;
}

/// Модифицирует имя точки
void TVideoOutputFrame::EditPointName(int figure_index, int point_index, const std::string &point_name)
{
 if(figure_index < 0 || figure_index >= int(GeometryGraphics.GetNumGeometries()))
  return;

 if(point_index < 0 || point_index >= int(GeometryGraphics.Geometry(figure_index).GetNumVertices()))
  return;

 GeometryGraphics.Geometry(figure_index).VertexName(point_index)=point_name;
}

// Устанавливает образец графики
void TVideoOutputFrame::SetSampleGeometryGraphics(RDK::MGraphics<double,2>& samplegraphics)
{
// SampleGeometryGraphics=samplegraphics;
// GeometryGraphics=SampleGeometryGraphics;
 UpdateVideo();
 FigureIndex=0;
 PointIndex=0;
 MyVideoOutputToolsForm->PointsCheckListBox->ItemIndex=PointIndex;
 MyVideoOutputToolsForm->GeometryCheckListBox->ItemIndex=FigureIndex;
}

/// Флаг повтора воспроизведения сначала после завершения
bool TVideoOutputFrame::SetRepeatVideoFlag(bool value)
{
 RepeatVideoFlag=value;
 TVideoCaptureThreadVideoGrabberAvi* thread=dynamic_cast<TVideoCaptureThreadVideoGrabberAvi*>(CaptureThread);
 if(thread)
 {
  thread->SetRepeatFlag(RepeatVideoFlag);
 }

 return true;
}

bool TVideoOutputFrame::SetRepeatSequenceFlag(bool value)
{
 RepeatSequenceFlag=value;
 TVideoCaptureThreadBmpSequence* thread=dynamic_cast<TVideoCaptureThreadBmpSequence*>(CaptureThread);
 if(thread)
 {
  thread->SetRepeatFlag(RepeatSequenceFlag);
 }

 return true;
}

bool TVideoOutputFrame::SetProcessAllFramesFlag(bool value)
{
 RepeatVideoFlag=value;
 TVideoCaptureThreadVideoGrabberAvi* thread=dynamic_cast<TVideoCaptureThreadVideoGrabberAvi*>(CaptureThread);
 if(thread)
 {
  thread->SetProcessAllFramesFlag(ProcessAllFramesFlag);
 }

 return true;
}

// -------------------------
// Методы ввода вывода точек геометрии из параметров и переменных состояния компонент
// -------------------------
// Отправляет набор точек в свойство компонента
void TVideoOutputFrame::SendToComponentPropertyMatrix(const std::string &stringid, const std::string &parameter_name, int figure_index)
{
 if(figure_index<0)
  return;

 RDK::MDMatrix<double> matrix;
 const std::vector<RDK::MVector<double,2> > &points=GeometryGraphics.Geometry(figure_index).GetVertices();
 int num_points=int(points.size());
 matrix.Resize(num_points,2);
 for(int i=0;i<num_points;i++)
 {
  matrix(i,0)=points[i][0];
  matrix(i,1)=points[i][1];
 }
 Model_SetComponentPropertyData(stringid.c_str(), parameter_name.c_str(), &matrix);
}

// Отправляет набор точек в параметр компонента
void TVideoOutputFrame::SendToComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index)
{
 if(figure_index<0)
  return;

 const std::vector<RDK::MVector<double,2> > &points=GeometryGraphics.Geometry(figure_index).GetVertices();
 Model_SetComponentPropertyData(stringid.c_str(), parameter_name.c_str(), &points);
// RDK::WriteParameterValue(stringid, parameter_name, points);
}

// Отправляет набор точек в переменную состояния компонента
void TVideoOutputFrame::SendToComponentState(const std::string &stringid, const std::string &state_name, int figure_index)
{
 if(figure_index<0)
  return;

 const std::vector<RDK::MVector<double,2> > &points=GeometryGraphics.Geometry(figure_index).GetVertices();
 Model_SetComponentPropertyData(stringid.c_str(), state_name.c_str(), &points);
// RDK::WriteStateValue(stringid, state_name, points);
}

// Считывает набор точек из параметра компонента
void TVideoOutputFrame::ReceiveFromComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index)
{
 std::vector<RDK::MVector<double,2> > points;
 points=*(const std::vector<RDK::MVector<double,2> > *)Model_GetComponentPropertyData(stringid.c_str(), parameter_name.c_str());
// RDK::ReadParameterValue(stringid, parameter_name, points);
}

// Считывает набор точек из переменной состояния компонента
void TVideoOutputFrame::ReceiveFromComponentState(const std::string &stringid, const std::string &state_name, int figure_index)
{
 std::vector<RDK::MVector<double,2> > points;
 points=*(const std::vector<RDK::MVector<double,2> > *)Model_GetComponentPropertyData(stringid.c_str(), state_name.c_str());
// RDK::ReadStateValue(stringid, state_name, points);
}
// -------------------------

// -------------------------
// Методы вывода изображений во входы-выходы компонент
// -------------------------
// Отправляет изображение в выбранный компонент
void TVideoOutputFrame::SendToComponentIO(void)
{
 if(LinkedComponentName.empty())
  return;

 SendBmpSource.ReflectionX(&SendReflectedBmpSource);
 int num_channels=GetNumEngines();
 switch(LinkedMode)
 {
 case 0:
  if(num_channels>FrameIndex)
   MModel_SetComponentBitmapInput(FrameIndex,LinkedComponentName.c_str(), LinkedIndex.c_str(), &SendReflectedBmpSource);
  else
   Model_SetComponentBitmapInput(LinkedComponentName.c_str(), LinkedIndex.c_str(), &SendReflectedBmpSource);
 break;

 case 1:
  if(num_channels>FrameIndex)
   MModel_SetComponentBitmapOutput(FrameIndex,LinkedComponentName.c_str(), LinkedIndex.c_str(), &SendReflectedBmpSource);
  else
   Model_SetComponentBitmapOutput(LinkedComponentName.c_str(), LinkedIndex.c_str(), &SendReflectedBmpSource);
 break;

 case 2:
  if(num_channels>FrameIndex)
   MModel_SetComponentPropertyData(FrameIndex,LinkedComponentName.c_str(), LinkedComponentPropertyName.c_str(), &SendReflectedBmpSource);
  else
   Model_SetComponentPropertyData(LinkedComponentName.c_str(), LinkedComponentPropertyName.c_str(), &SendReflectedBmpSource);
 break;
 }
}
// -------------------------

// Динамически наполняет меню
void TVideoOutputFrame::DynamicMenuFilling(TMenuItem* target, std::vector<std::string> &itemsList)
{
 if(!target)
  return;

 target->Clear();

 std::string itemName;

 for(size_t i=0; i<itemsList.size(); i++)
 {
  TMenuItem *newItem=new TMenuItem(target);
  newItem->Caption=itemsList[i].c_str();
  itemName=AnsiString(target->Name).c_str();
  itemName+=AnsiString(IntToStr(int(i))).c_str();
  newItem->Name=String(itemName.c_str());
  target->Add(newItem);
  //newItem->OnClick=OnClickVideoCodec;
 }
}

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед шагом расчета
void TVideoOutputFrame::ABeforeCalculate(void)
{
 int num_channels=GetNumEngines();
 if(CaptureThread)
 {
  double time_stamp=0;
  bool res=CaptureThread->ReadSourceSafe(SendBmpSource,time_stamp,false);

  if(!res)
   return;
  if(SendBmpSource.GetLength() == 0 && CaptureThread->CheckCaptureThreadState() == 1)
  {
   if(FrameIndex<num_channels)
	MEngine_LogMessage(FrameIndex, RDK_EX_INFO, std::string("TVideoOutputFrame::ABeforeCalculate: Frame have zero size!").c_str());
   else
	Engine_LogMessage(RDK_EX_INFO, std::string("TVideoOutputFrame::ABeforeCalculate: Frame have zero size!").c_str());
  }
 }

 {
  SendToComponentIO();
  if(SendPointsByStepCheckBox->Checked)
  {
   SendAsMatrixButtonClick(this);
   Button1Click(this);
  }
  if(DeletePointsAfterSendCheckBox->Checked)
  {
   MyVideoOutputToolsForm->DelAllPointsButtonClick(this);
  }

  if(num_channels == 1)
  {
   if(Model_Check() && SendBmpSource.GetByteLength()>0)
	 Model_SetComponentBitmapOutput("", "Output", &SendBmpSource,true); // Заглушка!!
  }
  else
  if(FrameIndex<num_channels && MModel_Check(FrameIndex))
  {
   if(SendBmpSource.GetByteLength()>0)
   {
	if(num_channels>FrameIndex)
	 MModel_SetComponentBitmapOutput(FrameIndex, "", "Output", &SendBmpSource,true);
   }
  }
 }
}

// Метод, вызываемый перед сбросом
void TVideoOutputFrame::ABeforeReset(void)
{
 int num_channels=GetNumEngines();
 if(num_channels == 1)
 {
  if(Model_Check() && SendBmpSource.GetByteLength()>0)
	 Model_SetComponentBitmapOutput("", "Output", &SendBmpSource,true); // Заглушка!!
 }
 else
 {
  if(FrameIndex<num_channels && MModel_Check(FrameIndex))
  {
   if(BmpSource.GetByteLength()>0)
   {
	if(num_channels>FrameIndex)
	 MModel_SetComponentBitmapOutput(FrameIndex, "", "Output", &BmpSource,true);
   }
  }
 }
}

// Метод, вызываемый после шага расчета
void TVideoOutputFrame::AAfterCalculate(void)
{
 if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() == 0)
 {
  if(CaptureThread)
   SetEvent(CaptureThread->GetCalcCompleteEvent());
 }
 else
 {
  if(CaptureThread)
   if(CaptureThread->GetSourceMode() == 4)
    SetEvent(CaptureThread->GetCalcCompleteEvent());
 }
}


// Обновление интерфейса
void TVideoOutputFrame::AUpdateInterface(void)
{
// if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() == 1)
// if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() != 0 /*&& !IsStarted*/)
  if(CaptureThread)
  {
   double time_stamp=0;
	CaptureThread->ReadSourceSafe(BmpSource,time_stamp,false);

/*   if(Mode == 4)
   {
//	std::string sstamp;
//	RDK::UTimeStamp stamp(double(time_stamp),1);
//	stamp>>sstamp;
	TimeEdit->EditMask="";
	TimeEdit->Text=FloatToStr(int(time_stamp*86400.0));
   }
   else*/
   {
	std::string sstamp;
	RDK::UTimeStamp stamp(double((time_stamp*86400.0)),25);
	stamp>>sstamp;
	TimeEdit->EditMask="000\:00\:00\:00;1;_";
	TimeEdit->Text=sstamp.c_str();
   }

	TrackBar->Max=CaptureThread->GetNumBitmaps();
	TrackBar->Position=CaptureThread->GetPosition();
	TrackBar->UpdateControlState();
	UpdateVideo();
  }

 if(LinkedComponentName.size() == 0)
 {
  SendImageToComponentInput1->Caption="Send Image To Component Input...";
  SendImageToComponentOutput1->Caption="Send Image To Component Output...";
  SendImageToComponentProperty1->Caption="Send Image To Component Property...";
 }
 else
 {
  switch(LinkedMode)
  {
  case 0:
   SendImageToComponentInput1->Caption=String("Send Image To Input: ")+String(LinkedComponentName.c_str())+String("[")+String(LinkedIndex.c_str())+String("]");
   SendImageToComponentOutput1->Caption="Send Image To Component Output...";
   SendImageToComponentProperty1->Caption="Send Image To Component Property...";
  break;

  case 1:
   SendImageToComponentInput1->Caption="Send Image To Component Input...";
   SendImageToComponentProperty1->Caption="Send Image To Component Property...";
   SendImageToComponentOutput1->Caption=String("Send Image To Output: ")+String(LinkedComponentName.c_str())+String("[")+String(LinkedIndex.c_str())+String("]");
  break;

  case 2:
   SendImageToComponentInput1->Caption="Send Image To Component Input...";
   SendImageToComponentProperty1->Caption="Send Image To Component Output...";
   SendImageToComponentOutput1->Caption=String("Send Image To Property: ")+String(LinkedComponentName.c_str())+String("[")+String(LinkedComponentPropertyName.c_str())+String("]");
  break;
  }
 }

 if(!SelectedComponentSName.empty())
 {
  SendToEdit->Text=String("State ")+SelectedComponentSName.c_str()+String(":")+SelectedComponentStateName.c_str();
 }
 else
 if(!SelectedComponentPName.empty())
 {
  SendToEdit->Text=String("Parameter ")+SelectedComponentPName.c_str()+String(":")+SelectedComponentParameterName.c_str();
 }
 else
 if(!SelectedComponentMatrixName.empty())
 {
  SendToEdit->Text=String("Mat-Property ")+SelectedComponentMatrixName.c_str()+String(":")+SelectedComponentPropertyMatrixName.c_str();
 }
 else
  SendToEdit->Text="";
}

// Сохраняет параметры интерфейса в xml
void TVideoOutputFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("LinkedComponentName",LinkedComponentName);
 xml.WriteString("LinkedComponentPropertyName",LinkedComponentPropertyName);
 xml.WriteInteger("LinkedMode",LinkedMode);
 xml.WriteString("LinkedIndex",LinkedIndex);

 xml.WriteString("SelectedComponentSName",SelectedComponentSName);
 xml.WriteString("SelectedComponentStateName",SelectedComponentStateName);
 xml.WriteString("SelectedComponentPName",SelectedComponentPName);
 xml.WriteString("SelectedComponentParameterName",SelectedComponentParameterName);
 xml.WriteString("SelectedComponentMatrixName",SelectedComponentMatrixName);
 xml.WriteString("SelectedComponentPropertyMatrixName",SelectedComponentPropertyMatrixName);
 xml.WriteInteger("SendPointsByStep",SendPointsByStepCheckBox->Checked);
 xml.WriteInteger("DeletePointsAfterSendCheckBox",DeletePointsAfterSendCheckBox->Checked);
 if(CaptureThread)
  xml.WriteInteger("ActiveSource",CaptureThread->GetSourceMode());

 xml.SelectNodeForce("VideoSourceOptions");

  if(CaptureThread)
  {
   CaptureThread->SaveParameters(VideoSourceOptions[CaptureThread->GetSourceMode()]);
  }
  std::map<int, RDK::USerStorageXML>::iterator I=VideoSourceOptions.begin();
  for(;I != VideoSourceOptions.end();++I)
  {
   xml.SelectNodeForce(RDK::sntoa(I->first));
   try
   {
	std::string str;
	I->second.Save(str);
//	xml.LoadToNode(I->second,"VideoSourceThread");
	xml.LoadToNode(str,"VideoSourceThread",true);
   }
   catch(RDK::UException &exception)
   {
	xml.SelectUp();
	xml.SelectUp();
	throw;
   }
   xml.SelectUp();
  }

 xml.SelectUp();
}

// Загружает параметры интерфейса из xml
void TVideoOutputFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 LinkedComponentName=xml.ReadString("LinkedComponentName","");
 LinkedComponentPropertyName=xml.ReadString("LinkedComponentPropertyName","");
 LinkedMode=xml.ReadInteger("LinkedMode",1);
 LinkedIndex=xml.ReadString("LinkedIndex","");

 SelectedComponentSName=xml.ReadString("SelectedComponentSName","");
 SelectedComponentStateName=xml.ReadString("SelectedComponentStateName","");
 SelectedComponentPName=xml.ReadString("SelectedComponentPName","");
 SelectedComponentParameterName=xml.ReadString("SelectedComponentParameterName","");
 SelectedComponentMatrixName=xml.ReadString("SelectedComponentMatrixName","");
 SelectedComponentPropertyMatrixName=xml.ReadString("SelectedComponentPropertyMatrixName","");
 SendPointsByStepCheckBox->Checked=xml.ReadInteger("SendPointsByStep",0);
 DeletePointsAfterSendCheckBox->Checked=xml.ReadInteger("DeletePointsAfterSendCheckBox",0);

 xml.SelectNodeForce("VideoSourceOptions");
 int num_nodes=xml.GetNumNodes();
 std::string str;
//  std::map<int, RDK::USerStorageXML>::iterator I=VideoSourceOptions.begin();
//  for(;I != VideoSourceOptions.end();++I)
  for(int i=0;i<num_nodes;i++)
  {
   xml.SelectNode(i);
   try
   {
	xml.SaveFromNode(str);
	if(!VideoSourceOptions[RDK::atoi(xml.GetNodeName())].Load(str,"VideoSourceThread"))
	{
	 VideoSourceOptions[RDK::atoi(xml.GetNodeName())].Create("VideoSourceThread");
    }
   }
   catch(RDK::UException &exception)
   {
	xml.SelectUp();
	xml.SelectUp();
	throw;
   }
   xml.SelectUp();
  }

 xml.SelectUp();

 int mode=xml.ReadInteger("ActiveSource",0);
 Init(mode);

 UpdateInterface();
}
// -----------------------------



//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::TimerTimer(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::StartButtonClick(TObject *Sender)
{
 Start();
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::StopButtonClick(TObject *Sender)
{
 Pause();
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::ImageMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(!ZoneSelectEnable)
  return;

 if(Button != mbLeft)
  return;

 //ImageTrackBarChange(Sender);
 DrawFrameRect(Image, left, top, left+width, top+height,2, SelColor);

 double iwidth,iheight,bwidth,bheight;
 int k1x,k1y,k2x,k2y;

 iwidth=Image->Width;
 iheight=Image->Height;
 bwidth=Image->Picture->Bitmap->Width;
 bheight=Image->Picture->Bitmap->Height;

 if(Shift.Contains(ssShift))
  {
   corrx1b=X, corry1b=Y;
   CorrSelectFlag=true;
  }
 else
  {
   x1b=X; y1b=Y;

   k1x=X*bwidth/iwidth;
   k2x=x1b*bwidth/iwidth;
   k1y=Y*bheight/iheight;
   k2y=y1b*bheight/iheight;
   left=k1x;
   top=k1y;
   width=(k2x-k1x);
   height=(k2y-k1y);
   CorrSelectFlag=false;
  }
// if(FigureFlag && !PointFlag)
 if(PointFlag == 1)
 {
  if(FigureIndex<0)
   MyVideoOutputToolsForm->AddFigureButtonClick(Sender);
  RDK::MVector<double,2> point_data;
  PointIndex=AddPoint(FigureIndex, std::string(AnsiString(MyVideoOutputToolsForm->PointNameEdit->Text).c_str())+RDK::sntoa(PointIndex+1,3),
   point_data);
 }
 else
 if(PointFlag == 2)
 {
//  if(FigureIndex<0 || PointIndex<0)
//  {
//   PointFlag=0;
//   return;
//  }

//  MyVideoOutputToolsForm->EditPointButtonClick(Sender);
 }
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::ImageMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
 if(!ZoneSelectEnable)
  return;

 String str;
 int cardwidth,cardheight;
 double iwidth,iheight,bwidth,bheight;

 iwidth=Image->Width;
 iheight=Image->Height;
 bwidth=Image->Picture->Bitmap->Width;
 bheight=Image->Picture->Bitmap->Height;


 //DrawCapture(Image->Picture->Bitmap);
 UpdateVideo();
 Image->Canvas->Pen->Color=clLime;
 Image->Canvas->PenPos=TPoint(X*bwidth/iwidth,0);
 Image->Canvas->LineTo(X*bwidth/iwidth,Image->Height*bheight/iheight);
 Image->Canvas->PenPos=TPoint(0,Y*bheight/iheight);
 Image->Canvas->LineTo(Image->Width*bwidth/iwidth,Y*bheight/iheight);

 if(!CorrSelectFlag)
  {
   if(x1b!=-1)
    {
     int i;
     int k1x,k1y,k2x,k2y;


     if(X>x1b)
      {
       k1x=x1b*bwidth/iwidth;
       k2x=X*bwidth/iwidth;
      }
     else
      {
       k1x=X*bwidth/iwidth;
       k2x=x1b*bwidth/iwidth;
      }
     if(Y>y1b)
      {
       k1y=y1b*bheight/iheight;
       k2y=Y*bheight/iheight;
      }
     else
      {
       k1y=Y*bheight/iheight;
       k2y=y1b*bheight/iheight;
      }

//     DrawFrameRect(Image, k1x, k1y, k2x, k2y, 3, (TColor)(255<<16));

     left=k1x;
     top=k1y;
     width=(k2x-k1x);
     height=(k2y-k1y);
//     DrawCapture(Image->Picture->Bitmap);
     DrawFrameRect(Image, k1x, k1y, k2x, k2y, 2, SelColor);
//     ImageTrackBarChange(Sender);
    }
  }
 else
  {
   if(corrx1b!=-1)
    {
     int i;
     int k1x,k1y,k2x,k2y;

     if(X>corrx1b)
      {
       k1x=corrx1b*bwidth/iwidth;
       k2x=X*bwidth/iwidth;
      }
     else
      {
       k1x=X*bwidth/iwidth;
       k2x=corrx1b*bwidth/iwidth;
      }
     if(Y>corry1b)
      {
       k1y=corry1b*bheight/iheight;
       k2y=Y*bheight/iheight;
      }
     else
      {
       k1y=Y*bheight/iheight;
       k2y=corry1b*bheight/iheight;
      }

 //     DrawCapture(Image->Picture->Bitmap);
     DrawFrameRect(Image, k1x, k1y, k2x, k2y, 2, SelColor);
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::ImageMouseUp(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(Button == mbRight)
 {
  if(PointIndex<0)
   return;

//  Figure.SetNumVertices(Figure.GetNumVertices()-1);
//  if(PointIndex>=Figure.GetNumVertices())
//   --PointIndex;

//  GeometryGraphics.Geometry(FigureIndex)=Figure;

  UpdateVideo();
  return;
 }

 if(!ZoneSelectEnable)
  return;

 String str;
 int cardwidth,cardheight;

 if(!CorrSelectFlag)
  {
 /*  if((X==x1b || Y==y1b))
    {
     x1b=x2b=y1b=y2b=-1;
     return;
    }*/
   x2b=X; y2b=Y;

   x1b=x2b=y1b=y2b=-1;
  }
 else
  {
/*   if((X==corrx1b || Y==corry1b))
    {
     corrx1b=corrx2b=corry1b=corry2b=-1;
     return;
    }*/
   corrx2b=X; corry2b=Y;

   corrx1b=corrx2b=corry1b=corry2b=-1;
  }

 // Если добавляем геометрический объект, то...
 if(PointFlag)
 {
  AddFigureRect(left,top,width,height);
/*
 if(Shift.Contains(ssShift))
  MyVideoOutputToolsForm->PointsCheckListBox->Checked[MyVideoOutputToolsForm->PointsCheckListBox->ItemIndex] = false;
 else
  MyVideoOutputToolsForm->PointsCheckListBox->Checked[MyVideoOutputToolsForm->PointsCheckListBox->ItemIndex] = true;
 */
 }
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::TimeEditChange(TObject *Sender)
{
 if(UpdateFlag)
  return;

 /*
 if(Mode != 4)
 {
  int selstart=TimeEdit->SelStart;
  int sellength=TimeEdit->SelLength;

  std::string sstamp;
  RDK::UTimeStamp stamp(0.0,VideoGrabber->CurrentFrameRate);
  sstamp=AnsiString(TimeEdit->Text).c_str();
  stamp<<sstamp;

  UpdateVideo();
  TimeEdit->SelStart=selstart;
  TimeEdit->SelLength=sellength;
 }
 else
 if (Mode == 4)
 {
  std::string sstamp;
  RDK::UTimeStamp stamp(0.0,1);
  sstamp=AnsiString(TimeEdit->Text).c_str();
  stamp<<sstamp;
  CurrentBmpSequenceIndex=stamp.Frames;
  LoadImageFromSequence(CurrentBmpSequenceIndex,BmpSource);
  BmpSource.SetColorModel(RDK::ubmRGB24);
  UpdateVideo();
 }   */
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::N1Click(TObject *Sender)
{
 MyVideoOutputToolsForm->Show();
}
//---------------------------------------------------------------------------
								  /*
void __fastcall TVideoOutputFrame::VideoGrabberFrameCaptureCompleted(TObject *Sender,
          Pointer FrameBitmap, int BitmapWidth, int BitmapHeight, DWORD FrameNumber,
          __int64 FrameTime, TFrameCaptureDest DestType, UnicodeString FileName,
          bool Success, int FrameId)
{
Graphics::TBitmap *Frame_Bitmap;

   Frame_Bitmap = (Graphics::TBitmap*) FrameBitmap;

   switch (DestType) {
      case fc_TBitmap:

         if(Frame_Bitmap->PixelFormat == pf24bit)
          BmpSource<<Frame_Bitmap;
         else
         {
          ConvertBitmap->Assign(Frame_Bitmap);
          ConvertBitmap->PixelFormat=pf24bit;
          BmpSource<<ConvertBitmap;
         }

         UpdateFlag=true;
         std::string sstamp;
		 RDK::UTimeStamp stamp(CurrentFrameNumber,VideoGrabber->CurrentFrameRate);
		 stamp>>sstamp;
         TimeEdit->Text=sstamp.c_str();
         UpdateFlag=false;

         CurrentFrameNumber=FrameNumber;
//         DrawCapture(Image->Picture->Bitmap);
		 UpdateVideo();

	  break;
   }

 if(GetNumEngines() == 1)
  UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(0,FrameTime);
 else
 if(GetNumEngines() > FrameIndex)
  UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(FrameIndex,FrameTime);
}          */
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::TrackBarChange(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 if(CaptureThread)
 {
  CaptureThread->SetPosition(TrackBar->Position);
  CaptureThread->Calculate();
  UpdateInterface();
/*
  TVideoCaptureThreadVideoGrabber *vg_thread=dynamic_cast<TVideoCaptureThreadVideoGrabber*>(CaptureThread);
  if(vg_thread)
  {
   if(!vg_thread->GetVideoGrabber()->InFrameProgressEvent)
	vg_thread->GetVideoGrabber()->PlayerFramePosition = TrackBar->Position;
  }

  TVideoCaptureThreadBmpSequence *bmpseq_thread=dynamic_cast<TVideoCaptureThreadBmpSequence*>(CaptureThread);
  if(bmpseq_thread)
  {
   if(TrackBar->Position < bmpseq_thread->GetNumBitmaps())
   {
	bmpseq_thread->SetPosition(TrackBar->Position);
	UpdateInterface();
   }
  }*/
 }
	/*
 if(Mode != 4)
 {
  if(!VideoGrabber->InFrameProgressEvent)
   VideoGrabber->PlayerFramePosition = TrackBar->Position;
 }
 else
 {
  if(TrackBar->Position < BmpSequenceNames.size())
  {
   CurrentBmpSequenceIndex=TrackBar->Position;
   LoadImageFromSequence(CurrentBmpSequenceIndex, BmpSource);
   BmpSource.SetColorModel(RDK::ubmRGB24);
   UpdateVideo();
  }
 }     */
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SourceControl1Click(TObject *Sender)
{
// MyVideoGrabberControlForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::SendToComponentClick(TObject *Sender)
{
 if(MyComponentsListForm->ShowParameterSelect() != mrOk)
  return;

 SelectedComponentPName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 SelectedComponentParameterName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentParameterName();
 SelectedComponentSName="";
 SelectedComponentStateName="";
 SendToComponentParameter(SelectedComponentPName, SelectedComponentParameterName, FigureIndex);
 UpdateInterface();
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::SendToComponentState1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowStateSelect() != mrOk)
  return;

 SelectedComponentSName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 SelectedComponentStateName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentStateName();
 SelectedComponentPName="";
 SelectedComponentParameterName="";
 SendToComponentState(SelectedComponentSName, SelectedComponentStateName, FigureIndex);
 UpdateInterface();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SendImageToComponentOutput1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 LinkedMode=1;
 LinkedComponentName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 LinkedIndex=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SendImageToComponentInput1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 LinkedMode=0;
 LinkedComponentName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 LinkedIndex=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentInput();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::Parameter1Click(TObject *Sender)
{
 SendToComponentClick(Sender);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::State1Click(TObject *Sender)
{
 SendToComponentState1Click(Sender);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::Button1Click(TObject *Sender)
{
 if(!SelectedComponentSName.empty())
 {
  SendToComponentState(SelectedComponentSName, SelectedComponentStateName, FigureIndex);
 }
 else
 if(!SelectedComponentPName.empty())
 {
  SendToComponentParameter(SelectedComponentPName, SelectedComponentParameterName, FigureIndex);
 }
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::SendImageToComponentProperty1Click(TObject *Sender)

{
 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 LinkedMode=2;
 LinkedComponentName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 LinkedComponentPropertyName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentStateName();
// LinkedIndex=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SendAsMatrixButtonClick(TObject *Sender)
{
 if(!SelectedComponentMatrixName.empty())
 {
  SendToComponentPropertyMatrix(SelectedComponentMatrixName, SelectedComponentPropertyMatrixName, FigureIndex);
 }
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::PropertyMatrix1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowParameterSelect() != mrOk)
  return;

 SelectedComponentPropertyMatrixName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentStateName();
 if(SelectedComponentPropertyMatrixName=="")
  SelectedComponentPropertyMatrixName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentParameterName();

 SelectedComponentMatrixName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 SelectedComponentSName="";
 SelectedComponentStateName="";
 SelectedComponentPName="";
 SelectedComponentParameterName="";
 SendToComponentPropertyMatrix(SelectedComponentMatrixName, SelectedComponentPropertyMatrixName, FigureIndex);
 UpdateInterface();
}
//---------------------------------------------------------------------------
					   /*
void __fastcall TVideoOutputFrame::UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext,
		  TIdHTTPRequestInfo *ARequestInfo, TIdHTTPResponseInfo *AResponseInfo)

{
 std::vector<char> &time_stamp=UHttpServerFrame->ParsedRequestArgs["TimeStamp"];
 if(!time_stamp.empty())
 {
  std::string temp_stamp;
  temp_stamp.assign(&time_stamp[0],time_stamp.size());
  ServerTimeStamp=RDK::atoi(temp_stamp);

  if(GetNumEngines() == 1)
   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(0,ServerTimeStamp);
  else
  if(GetNumEngines() > FrameIndex)
   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(FrameIndex,ServerTimeStamp);

  std::string sstamp;
  RDK::UTimeStamp stamp(double(ServerTimeStamp/1000),25);
  stamp>>sstamp;
  TimeEdit->Text=sstamp.c_str();
 }

 UHttpServerFrame->IdHTTPServerCommandGet(AContext, ARequestInfo, AResponseInfo);
 if(ARequestInfo->Document == "/images.cgi")
 {
  std::vector<char> &image_data=UHttpServerFrame->ParsedRequestArgs["Image"];
  RDK::UBitmap &bmp=UHttpServerFrame->DecodeParamAsBitmap(image_data);
  BmpSource=bmp;
//  UpdateVideo();
  AResponseInfo->ContentText="OK";
 }
 else
 {
  AResponseInfo->ResponseNo=404;
  AResponseInfo->ResponseText="404 Not Found";
  AResponseInfo->ContentText="404 Not Found";
 }
}*/
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::SaveImage1Click(TObject *Sender)
{
 if(!SavePictureDialog->Execute())
  return;
 Image->Picture->SaveToFile(SavePictureDialog->FileName);
}
//---------------------------------------------------------------------------



void __fastcall TVideoOutputFrame::SourceControl21Click(TObject *Sender)
{
 if(CaptureThread)
 {
  CaptureThread->SaveParameters(VideoSourceOptions[CaptureThread->GetSourceMode()]);
  VideoCaptureOptionsForm->SelectVideoSourcePage(CaptureThread->GetSourceMode());
 }
 VideoCaptureOptionsForm->Show();
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::SourceControlModal()
{
 if(CaptureThread)
 {
  CaptureThread->SaveParameters(VideoSourceOptions[CaptureThread->GetSourceMode()]);
  VideoCaptureOptionsForm->SelectVideoSourcePage(CaptureThread->GetSourceMode());
 }
 VideoCaptureOptionsForm->ShowModal();
}

void __fastcall TVideoOutputFrame::StartRecordingToolButtonClick(TObject *Sender)
{
 RecordingFrame->FrameIndexLabeledEdit->Text=IntToStr(FrameIndex);
 RecordingFrame->StartRecordingButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SelectFile1Click(TObject *Sender)
{
 RecordingFrame->BrowseFileNameButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::CaptureStartToolButtonClick(TObject *Sender)
{
 Start();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::CaptureStopToolButtonClick(TObject *Sender)
{
 Pause();
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::RecordingFrameStartRecordingButtonClick(TObject *Sender)

{
 if(!IsStarted)
  Start();

 RecordingFrame->FrameIndexLabeledEdit->Text=IntToStr(FrameIndex);
 RecordingFrame->StartRecordingButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::StopRecordingToolButtonClick(TObject *Sender)
{
 RecordingFrame->StopRecordingButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::NetworkStreamingFrameNetworkStreamingButtonClick(TObject *Sender)
{
 if(!IsStarted)
  Start();

 NetworkStreamingFrame->FrameIndexLabeledEdit->Text=IntToStr(FrameIndex);
 NetworkStreamingFrame->NetworkStreamingButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::StopStreamingToolButtonClick(TObject *Sender)
{
 NetworkStreamingFrame->StopNetworkStreamingButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SelectSourceToolButtonClick(TObject *Sender)
{
 SourceControl21Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SavePictureActionExecute(TObject *Sender)
{
 String dir;
 if(UGEngineControlForm->ProjectOpenFlag)
  dir=UGEngineControlForm->ProjectPath;
 else
  dir=ExtractFilePath(Application->ExeName);

 dir+="SavedPictures\\";
 if(Owner)
 {
  dir+=Owner->Name;
  dir+="\\";
 }
 dir+=IntToStr(FrameIndex);
 if(!DirectoryExists(dir))
  ForceDirectories(dir);
 dir+="\\";

 string file_name=RDK::sntoa(SavePictureIndex++,3)+" ";

 time_t time_data;
 time(&time_data);
 file_name+=RDK::get_text_time(time_data,'.','-')+".jpg";

 UpdateInterface(true);

 TJPEGImage* jpg=new TJPEGImage;
 jpg->Assign(Image->Picture->Graphic);
 jpg->CompressionQuality=100;
 jpg->Compress();
 jpg->SaveToFile(dir+file_name.c_str());
 delete jpg;
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SavePicture(String directory, String filename)
{
 /*String dir;
 if(UGEngineControlForm->ProjectOpenFlag)
  dir=UGEngineControlForm->ProjectPath;
 else
  dir=ExtractFilePath(Application->ExeName);

 dir+="SavedPictures\\";
 if(Owner)
 {
  dir+=Owner->Name;
  dir+="\\";
 }
 dir+=IntToStr(FrameIndex);
 if(!DirectoryExists(dir))
  ForceDirectories(dir);
 dir+="\\";

 string file_name=RDK::sntoa(SavePictureIndex++,3)+" ";

 time_t time_data;
 time(&time_data);
 file_name+=RDK::get_text_time(time_data,'.','-')+".jpg"; */

 if(!DirectoryExists(directory))
  ForceDirectories(directory);

 UpdateInterface(true);

 TJPEGImage* jpg=new TJPEGImage;
 jpg->Assign(Image->Picture->Graphic);
 jpg->CompressionQuality=100;
 jpg->Compress();
 jpg->SaveToFile(directory+"\\"+filename);
 delete jpg;
}
//---------------------------------------------------------------------------
// Реакция на клик для динамического меню выбора кодека записи
void __fastcall TVideoOutputFrame::OnClickVideoCodec(TObject *Sender)
{
 TMenuItem *currItem=dynamic_cast<TMenuItem*>(Sender);
 TMenuItem *parentMenu=currItem->Parent;

 for(int i=0; i<parentMenu->Count; i++)
 {
  if(parentMenu->Items[i]->Checked)
   parentMenu->Items[i]->Checked=false;
 }

 RecordingFrame->VideoCompressorComboBox->ItemIndex=currItem->MenuIndex;
 currItem->Checked=true;
 return;
}
//---------------------------------------------------------------------------
// Реакция на клик динамического меню выбора видео кодека записи
void __fastcall TVideoOutputFrame::OnClickRecordingMethod(TObject *Sender)
{
 TMenuItem *currItem=dynamic_cast<TMenuItem*>(Sender);
 TMenuItem *parentMenu=currItem->Parent;

 for(int i=0; i<parentMenu->Count; i++)
 {
  if(parentMenu->Items[i]->Checked)
   parentMenu->Items[i]->Checked=false;
 }

 RecordingFrame->RecordingMethodComboBox->ItemIndex=currItem->MenuIndex;
 RecordingFrameRecordingMethodComboBoxChange(Sender);
 currItem->Checked=true;
 return;
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::RecordingFrameVideoCompressorComboBoxChange(TObject *Sender)
{
 RecordingFrame->VideoCompressorComboBoxChange(Sender);

 int indexCodec=RecordingFrame->VideoCompressorComboBox->ItemIndex;
 TMenuItem *videoCodec=0;
 for(int i=0; i<RecordingPopupMenu->Items->Count; i++)
 {
  if(RecordingPopupMenu->Items->Items[i]->Name == "VideoCodec")
  {
   videoCodec=RecordingPopupMenu->Items->Items[i];
   break;
  }
 }

 if(!videoCodec)
  return;

 for(int i=0; i<videoCodec->Count; i++)
 {
  if(videoCodec->Items[i]->Checked)
   videoCodec->Items[i]->Checked=false;
 }

 videoCodec->Items[indexCodec]->Checked=true;
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::StartStreamingToolButtonClick(TObject *Sender)
{
 NetworkStreamingFrame->FrameIndexLabeledEdit->Text=IntToStr(FrameIndex);
 NetworkStreamingFrame->NetworkStreamingButtonClick(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::RecordingFrameRecordingMethodComboBoxChange(TObject *Sender)

{
 RecordingFrame->RecordingMethodComboBoxChange(Sender);

 TMenuItem *recMethod=0, *vidCodec=0;
 int indexRecMethod=RecordingFrame->RecordingMethodComboBox->ItemIndex;
 for(int i=0; i<RecordingPopupMenu->Items->Count; i++)
 {
  if(RecordingPopupMenu->Items->Items[i]->Name == "RecordingMethod")
  {
   recMethod=RecordingPopupMenu->Items->Items[i];
  }

  if(RecordingPopupMenu->Items->Items[i]->Name == "VideoCodec")
  {
   vidCodec=RecordingPopupMenu->Items->Items[i];
  }
 }

 if(!recMethod)
  return;

 if(!vidCodec)
  return;

 for(int i=0; i<recMethod->Count; i++)
 {
  if(recMethod->Items[i]->Checked)
   recMethod->Items[i]->Checked=false;
 }

 recMethod->Items[indexRecMethod]->Checked=true;

 if( indexRecMethod==0 || indexRecMethod==8)
 {
  vidCodec->Enabled=false;
 }
 else
 {
  vidCodec->Enabled=true;
 }
}
//---------------------------------------------------------------------------


