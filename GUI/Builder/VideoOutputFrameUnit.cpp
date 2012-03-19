//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VideoOutputFrameUnit.h"
#include "rdk_builder.h"
#include "TUBitmap.h"
#include "TVideoGrabberControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VidGrab"
#pragma resource "*.dfm"
TVideoOutputFrame *VideoOutputFrame;

//---------------------------------------------------------------------------
// ������������� ������ avi-������
void TVideoOutputFrame::InitByAvi(const String &filename)
{
 VideoGrabber->PlayerFileName=filename;

   VideoGrabber->Display_AutoSize = false;
   VideoGrabber->PlayerRefreshPausedDisplay = false;
   VideoGrabber->AutoStartPlayer = false;
   VideoGrabber->BurstCount = 0;
   VideoGrabber->BurstInterval = 0;
   VideoGrabber->BurstMode = true;
   VideoGrabber->BurstType = fc_TBitmap;

   VideoGrabber->OpenPlayer();
   Mode=1;
}

// ������������� ������ bmp-������
void TVideoOutputFrame::InitByBmp(const String &filename)
{
 LoadBitmapFromFile(AnsiString(filename).c_str(),&BmpSource);
 BmpSource.SetColorModel(RDK::ubmRGB24);
 Mode=0;
 UpdateVideo();
}

// ������������� ��������� �����������
bool TVideoOutputFrame::InitByBmp(const RDK::UBitmap &bmp)
{
 BmpSource=bmp;
 BmpSource.SetColorModel(RDK::ubmRGB24);
 Mode=0;
 return UpdateVideo();
}


// ������������� ������ avi-������
void TVideoOutputFrame::InitByCamera(int camera_index)
{
 VideoGrabber->BurstType = fc_TBitmap;
 VideoGrabber->BurstMode = True;
 VideoGrabber->BurstCount = 0;


 Mode=2;
}


// ������������� �������� ����
bool TVideoOutputFrame::SetTitle(String title)
{
 if(title == "")
  GroupBox->Caption="";
 else
  GroupBox->Caption=String(" ")+title+String(" ");

 return true;
}

// ������������� ���������� ������������ ����
// ���� ��� ���������� == -1 �� ���� �� ������������
bool TVideoOutputFrame::SetFrameRect(int x,int y, int x_width, int y_height, TColor color)
{
 left=x; top=y; width=x_width; height=y_height;
 SelColor=color;
 UpdateVideo();
 return true;
}

// ��������� ��������� ����
bool TVideoOutputFrame::UpdateVideo(void)
{       /*
 if(!Capture)
 {
  UpdateFlag=true;
  StartButton->Enabled=false;
  StopButton->Enabled=false;
  ImageTrackBar->Enabled=false;
  UpdateFlag=false;
 }
 else
 if(dynamic_cast<VFCapture*>(Capture))
 {
  ImageTrackBar->Enabled=true;
  TimeEdit->ReadOnly=false;
  TimeEdit->Color=clWindow;
  ImageTrackBar->Max=static_cast<VFCapture*>(Capture)->GetNumFrames();
  ImageTrackBar->Position=static_cast<VFCapture*>(Capture)->GetCurrentFrame();
  UpdateFlag=false;
 }
 else
 {
  TimeEdit->ReadOnly=true;
  TimeEdit->Color=clBtnFace;
 }

 if(Capture)
 {
  string sstamp;
  VTimeStamp stamp(Capture->GetCurrentVideoTime(),Capture->GetFPS());
  stamp>>sstamp;
  TimeEdit->Text=sstamp.c_str();
  StartButton->Caption="����";
  StopButton->Caption="����";
  UpdateFlag=true;
  if(Capture->GetCaptureState())
  {
   StartButton->Enabled=false;
   StopButton->Enabled=true;
  }
  else
  {
   StartButton->Enabled=true;
   StopButton->Enabled=false;
  }

  UpdateFlag=false;
 }
			 */

// TrackBar->Max=VideoGrabber->G;
// TrackBar->Position=CurrentFrameNumber;

 DrawCapture(Image->Picture->Bitmap);
 Image->Repaint();
 if(left != -1 || top != -1 || width != -1 || height != -1)
  DrawFrameRect(Image, left, top, left+width, top+height, 2, SelColor);
 Image->Repaint();
 UpdateGeometryList(MyVideoOutputToolsForm->GeometryCheckListBox,
					MyVideoOutputToolsForm->PointsCheckListBox);

 return true;
}

// ������������ ������� ��������� �����������
void TVideoOutputFrame::DrawCapture(Graphics::TBitmap *bmp)
{
 if(!bmp)
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
  BmpCanvas=BmpSource;

 GeometryGraphics.Repaint();

 BmpCanvas>>bmp;
}

// ��������� ������ �����
void TVideoOutputFrame::UpdateGeometryList(TCheckListBox *GeometryCheckListBox, TCheckListBox *PointsCheckListBox)
{
 int ix=GeometryCheckListBox->ItemIndex;

 GeometryCheckListBox->Clear();
 for(size_t i=0;i<GeometryGraphics.GetNumGeometry();i++)
  GeometryCheckListBox->Items->Add(GeometryGraphics.GetDescription(i).Name.c_str());
 for(size_t i=0;i<GeometryGraphics.GetNumGeometry();i++)
  GeometryCheckListBox->Checked[i]=GeometryGraphics.GetDescription(i).Visible;

 if(ix>=0)
  GeometryCheckListBox->ItemIndex=ix;
 else
  GeometryCheckListBox->ItemIndex=0;

 if(FigureFlag)
 {
  GeometryCheckListBox->ItemIndex=FigureIndex;
  GeometryCheckListBox->Enabled=false;
 }
 else
  GeometryCheckListBox->Enabled=true;

 if(GeometryGraphics.GetNumGeometry()<=FigureIndex)
  return;

 ix=PointsCheckListBox->ItemIndex;

 PointsCheckListBox->Clear();
 for(size_t i=0;i<GeometryGraphics.GetGeometry(GeometryCheckListBox->ItemIndex)().GetNumVertex();i++)
 {
  std::stringstream stream;
  stream<<GeometryGraphics.GetGeometry(GeometryCheckListBox->ItemIndex)().UseName(i)<<" ";
  stream<<GeometryGraphics.GetGeometry(GeometryCheckListBox->ItemIndex)()[i];
  PointsCheckListBox->Items->Add(stream.str().c_str());
   if(GeometryGraphics.GetGeometry(GeometryCheckListBox->ItemIndex)()[i].z>=0)
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

 if(PointFlag > 0)
 {
  PointsCheckListBox->Enabled=false;
 }
 else
 {
  PointsCheckListBox->Enabled=true;
 }
}

// ����� ��������� ������������� ����
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

// ��������� ��������� ������� ������
void TVideoOutputFrame::AddFigureRect(double l,double t,double w,double h)
{
 if(l<0 || t<0)
  return;

// Figure().SetNumVertex(Figure().GetNumVertex()+1);
 Figure()[PointIndex].x=l+w/2;
 Figure()[PointIndex].y=BmpCanvas.GetHeight()-(t+h/2);

/*
 if(PointFlag == 1)
 {
  Figure().SetNumVertex(Figure().GetNumVertex()+1);
  Figure()[Figure().GetNumVertex()-1].x=l+w/2;
  Figure()[Figure().GetNumVertex()-1].y=BmpCanvas.GetHeight()-(t+h/2);
 }
 else
 if(PointFlag == 2)
 {
  Figure()[PointIndex].x=l+w/2;
  Figure()[PointIndex].y=BmpCanvas.GetHeight()-(t+h/2);
  if(PointIndex < MyVideoOutputToolsForm->PointsCheckListBox->Items->Count)
  MyVideoOutputToolsForm->PointsCheckListBox->ItemIndex = PointIndex+1;
 }
      */

 GeometryGraphics.GetGeometry(FigureIndex)=Figure;

 UpdateVideo();
}


// ������������� ������� �������
void TVideoOutputFrame::SetSampleGeometryGraphics(RDK::MGraphics<double>& samplegraphics)
{
 SampleGeometryGraphics=samplegraphics;
 GeometryGraphics=SampleGeometryGraphics;
 UpdateVideo();
 FigureIndex=0;
 PointIndex=0;
 MyVideoOutputToolsForm->PointsCheckListBox->ItemIndex=PointIndex;
 MyVideoOutputToolsForm->GeometryCheckListBox->ItemIndex=FigureIndex;
}

//---------------------------------------------------------------------------
__fastcall TVideoOutputFrame::TVideoOutputFrame(TComponent* Owner)
	: TFrame(Owner)
{
// Capture=0;

 // ������ �������
 Graph.SetCanvas(&BmpCanvas);

 // ��������� ���������
 GeometryGraphics.SetGraphics(&Graph);

 x1b=x2b=y1b=y2b=-1;
 corrx1b=corry1b=corrx2b=corry2b=-1;
 left=-1; top=-1; width=-1; height=-1;

 CorrSelectFlag=false;
 ZoneSelectEnable=false;

 ZoneSelectEvent=CreateEvent(0,0,0,0);
 UpdateFlag=false;

 PointFlag=0;
 FigureFlag=false;

 MyVideoOutputToolsForm=new TVideoOutputToolsForm(this,
	this,
	GeometryGraphics,
	SampleGeometryGraphics,
	Figure,
	FigureIndex,
	FigureFlag,
	PointIndex,
	PointFlag);

 MyVideoGrabberControlForm=new TVideoGrabberControlForm(this);


 ConvertBitmap=new Graphics::TBitmap;

}

__fastcall TVideoOutputFrame::~TVideoOutputFrame(void)
{
 CloseHandle(ZoneSelectEvent);
 delete MyVideoOutputToolsForm;
 MyVideoOutputToolsForm=0;

 delete MyVideoGrabberControlForm;
 MyVideoGrabberControlForm=0;

 delete ConvertBitmap;
}

/*
 if(UpdateFlag)
  return;

 if(!Capture || !dynamic_cast<VFCapture*>(Capture))
  return;

 static_cast<VFCapture*>(Capture)->SelectFrame((long long)(ImageTrackBar->Position));
 DrawCapture(Capture,Image->Picture->Bitmap);
 Image->Repaint();
 if(left != -1 || top != -1 || width != -1 || height != -1)
 {
  DrawFrameRect(Image, left, top, left+width, top+height, 2, SelColor);
 }
 string sstamp;
 VTimeStamp stamp(ImageTrackBar->Position/Capture->GetFPS(),Capture->GetFPS());
 stamp>>sstamp;

 UpdateFlag=true;
 TimeEdit->Text=sstamp.c_str();
 TimeEdit->Repaint();
 UpdateFlag=false;
 TimeEdit->Repaint();
 Image->Repaint();
 Image->Update();   */
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::TimerTimer(TObject *Sender)
{
/* if(!Capture->GetCaptureState())
  {
   StartButton->Enabled=false;
   StopButton->Enabled=true;
   Timer->Enabled=false;
   return;
  }

 DrawCapture(Capture,Image->Picture->Bitmap);
 UpdateFlag=true;

 if(dynamic_cast<VFCapture*>(Capture))
 {
  ImageTrackBar->Position=static_cast<VFCapture*>(Capture)->GetCurrentFrame();
  string sstamp;
  VTimeStamp stamp(ImageTrackBar->Position/Capture->GetFPS(),Capture->GetFPS());
  stamp>>sstamp;
  TimeEdit->Text=sstamp.c_str();
 }
 else
 {
  string sstamp;
  VTimeStamp stamp(Capture->GetCurrentVideoTime(),Capture->GetFPS());
  stamp>>sstamp;
  TimeEdit->Text=sstamp.c_str();
 }

                     */
 UpdateFlag=false;
 Image->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::StartButtonClick(TObject *Sender)
{
/*
 switch (Mode) {
 case 0:
 break;

 case 1:
	VideoGrabber->RunPlayer();
 break;

 case 2:
   VideoGrabber->StartPreview();
 break;

 default:
	 ;
 }*/
 if(VideoGrabber->VideoSource == vs_VideoFileOrURL)
  VideoGrabber->RunPlayer();
 else
 if(VideoGrabber->VideoSource == vs_VideoCaptureDevice)
   VideoGrabber->StartPreview();

/* if(!Capture)
  return;

 if(UpdateFlag)
  return;

 if(dynamic_cast<VFCapture*>(Capture))
 {
  static_cast<VFCapture*>(Capture)->SelectFrame((long long)(ImageTrackBar->Position));
 }

  StartButton->Enabled=false;
  StopButton->Enabled=true;
  Capture->Start();
  Timer->Enabled=true;
//  if(ImageTrackBar->Position>ImageTrackBar->Min)
//   ImageTrackBar->Position=ImageTrackBar->Position-1;
*/
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::StopButtonClick(TObject *Sender)
{
 switch (Mode) {
 case 0:
 break;

 case 1:
	VideoGrabber->PausePlayer();
 break;

 case 2:
   VideoGrabber->PausePreview();
 break;

 default:
	 ;
 }

 // VideoGrabber->PausePreview();
// VideoGrabber->PausePlayer();
/* if(!Capture)
  return;

 if(UpdateFlag)
  return;

  StartButton->Enabled=true;
  StopButton->Enabled=false;
  Capture->Pause();
  Timer->Enabled=false;
//  if(ImageTrackBar->Position<ImageTrackBar->Max)
//   ImageTrackBar->Position=ImageTrackBar->Position+1;
*/
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
 if(FigureFlag && !PointFlag)
  MyVideoOutputToolsForm->AddPointButtonClick(Sender);

 if(SampleGeometryGraphics.GetNumGeometry())
  MyVideoOutputToolsForm->EditPointButtonClick(Sender);
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

 if(!CorrSelectFlag)
  {
   if(x1b!=-1)
	{
	 int i;
	 int k1x,k1y,k2x,k2y;

	 iwidth=Image->Width;
	 iheight=Image->Height;
	 bwidth=Image->Picture->Bitmap->Width;
	 bheight=Image->Picture->Bitmap->Height;

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

//	 DrawFrameRect(Image, k1x, k1y, k2x, k2y, 3, (TColor)(255<<16));

	 left=k1x;
	 top=k1y;
	 width=(k2x-k1x);
	 height=(k2y-k1y);
	 DrawCapture(Image->Picture->Bitmap);
	 DrawFrameRect(Image, k1x, k1y, k2x, k2y, 2, SelColor);
//	 ImageTrackBarChange(Sender);
	}
  }
 else
  {
   if(corrx1b!=-1)
	{
	 int i;
	 int k1x,k1y,k2x,k2y;

	 iwidth=Image->Width;
	 iheight=Image->Height;
	 bwidth=Image->Picture->Bitmap->Width;
	 bheight=Image->Picture->Bitmap->Height;

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

	 DrawCapture(Image->Picture->Bitmap);
	 DrawFrameRect(Image, k1x, k1y, k2x, k2y, 2, SelColor);
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::ImageMouseUp(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(!ZoneSelectEnable)
  return;

 String str;
 int cardwidth,cardheight;

 if(!CorrSelectFlag)
  {
   if((X==x1b || Y==y1b) && !FigureFlag)
	{
	 x1b=x2b=y1b=y2b=-1;
	 return;
	}
   x2b=X; y2b=Y;

   x1b=x2b=y1b=y2b=-1;
  }
 else
  {
   if((X==corrx1b || Y==corry1b) && !FigureFlag)
	{
	 corrx1b=corrx2b=corry1b=corry2b=-1;
	 return;
	}
   corrx2b=X; corry2b=Y;

   corrx1b=corrx2b=corry1b=corry2b=-1;
  }

 // ���� ��������� �������������� ������, ��...
 if(PointFlag)
 {
  AddFigureRect(left,top,width,height);
  if(SampleGeometryGraphics.GetNumGeometry())
   MyVideoOutputToolsForm->EditPointButtonClick(Sender);

 }
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::TimeEditChange(TObject *Sender)
{
 if(UpdateFlag)
  return;

 int selstart=TimeEdit->SelStart;
 int sellength=TimeEdit->SelLength;

 std::string sstamp;
 RDK::UTimeStamp stamp(0.0,VideoGrabber->CurrentFrameRate);
 sstamp=AnsiString(TimeEdit->Text).c_str();
 stamp<<sstamp;

// TrackBar->Position=stamp()*VideoGrabber->CurrentFrameRate;

 UpdateVideo();
 TimeEdit->SelStart=selstart;
 TimeEdit->SelLength=sellength;
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::N1Click(TObject *Sender)
{
 MyVideoOutputToolsForm->Show();
}
//---------------------------------------------------------------------------

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
		 DrawCapture(Image->Picture->Bitmap);
		 UpdateVideo();

	  break;
   }
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::TrackBarChange(TObject *Sender)
{
 if(!VideoGrabber->InFrameProgressEvent)
  VideoGrabber->PlayerFramePosition = TrackBar->Position;
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SourceControl1Click(TObject *Sender)
{
 MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this,VideoGrabber);
 MyVideoGrabberControlForm->Show();
}
//---------------------------------------------------------------------------

