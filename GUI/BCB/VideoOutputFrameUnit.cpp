//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VideoOutputFrameUnit.h"
#include "rdk.bcb.h"
#include "TUBitmap.h"
#include "TVideoGrabberControlFormUnit.h"
#include "myrdk.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VidGrab"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TVideoOutputFrame *VideoOutputFrame;


//---------------------------------------------------------------------------
__fastcall TVideoOutputFrame::TVideoOutputFrame(TComponent* Owner)
    : TUVisualControllerFrame(Owner)
{
// Capture=0;

 // ������ �������
 Graph.SetCanvas(&BmpCanvas);

 // ��������� ���������
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

 PointFlag=0;
// FigureFlag=false;

 MyVideoOutputToolsForm=new TVideoOutputToolsForm(this,
    this,
    GeometryGraphics,
//    SampleGeometryGraphics,
    Figure,
    FigureIndex,
//    FigureFlag,
    PointIndex,
    PointFlag);

 MyVideoGrabberControlForm=new TVideoGrabberControlForm(this);
 MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this,VideoGrabber);


 ConvertBitmap=new Graphics::TBitmap;

 MyComponentsListForm=new TUComponentsListForm(this);
}

__fastcall TVideoOutputFrame::~TVideoOutputFrame(void)
{
 CloseHandle(ZoneSelectEvent);
 delete MyVideoOutputToolsForm;
 MyVideoOutputToolsForm=0;

 delete MyVideoGrabberControlForm;
 MyVideoGrabberControlForm=0;

 delete ConvertBitmap;

 if(MyComponentsListForm)
  delete MyComponentsListForm;
}

//---------------------------------------------------------------------------
// ������������� ������ avi-������
void TVideoOutputFrame::InitByAvi(const String &filename)
{
 StopButtonClick(this);
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
 StopButtonClick(this);
 LoadBitmapFromFile(AnsiString(filename).c_str(),&BmpSource);
 BmpSource.SetColorModel(RDK::ubmRGB24);
 Mode=0;
 UpdateVideo();
}

// ������������� ��������� �����������
bool TVideoOutputFrame::InitByBmp(const RDK::UBitmap &bmp)
{
 StopButtonClick(this);
 BmpSource=bmp;
 BmpSource.SetColorModel(RDK::ubmRGB24);
 Mode=0;
 return UpdateVideo();
}


// ������������� ������ �������
void TVideoOutputFrame::InitByCamera(int camera_index, int input_index, int size_index, int subtype_index, int analog_index)
{
 StopButtonClick(this);
 VideoGrabber->BurstType = fc_TBitmap;
 VideoGrabber->BurstMode = True;
 VideoGrabber->BurstCount = 0;
 VideoGrabber->VideoDevice=camera_index;
 VideoGrabber->VideoInput=input_index;
 VideoGrabber->VideoSize=size_index;
 VideoGrabber->VideoSubtype=subtype_index;
 VideoGrabber->AnalogVideoStandard=analog_index;
 Mode=2;
 StartButtonClick(this);
 UpdateVideo();
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
{

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
/*
 if(FigureFlag)
 {
  GeometryCheckListBox->ItemIndex=FigureIndex;
  GeometryCheckListBox->Enabled=false;
 }
 else
  GeometryCheckListBox->Enabled=true;
  */
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
 /*
 if(PointFlag > 0)
 {
  PointsCheckListBox->Enabled=false;
 }
 else
 {
  PointsCheckListBox->Enabled=true;
 } */
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


// -------------------------
// ������ ����� ������ ����� ��������� �� ���������� � ���������� ��������� ���������
// -------------------------
// ���������� ����� ����� � �������� ����������
void TVideoOutputFrame::SendToComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index)
{
 const std::vector<RDK::MVector<double,2> > &points=GeometryGraphics.GetGeometry(figure_index).GetVertex().GetVertex();
 RDK::WriteParameterValue(stringid, parameter_name, points);
}

// ���������� ����� ����� � ���������� ��������� ����������
void TVideoOutputFrame::SendToComponentState(const std::string &stringid, const std::string &state_name, int figure_index)
{
 const std::vector<RDK::MVector<double,2> > &points=GeometryGraphics.GetGeometry(figure_index).GetVertex().GetVertex();
 RDK::WriteStateValue(stringid, state_name, points);
}

// ��������� ����� ����� �� ��������� ����������
void TVideoOutputFrame::ReceiveFromComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index)
{
 std::vector<RDK::MVector<double,2> > points;
 RDK::ReadParameterValue(stringid, parameter_name, points);
}

// ��������� ����� ����� �� ���������� ��������� ����������
void TVideoOutputFrame::ReceiveFromComponentState(const std::string &stringid, const std::string &state_name, int figure_index)
{
 std::vector<RDK::MVector<double,2> > points;
 RDK::ReadStateValue(stringid, state_name, points);
}
// -------------------------

// -------------------------
// ������ ������ ����������� �� �����-������ ���������
// -------------------------
// ���������� ����������� � ��������� ���������
void TVideoOutputFrame::SendToComponentIO(void)
{
 if(LinkedComponentName.empty())
  return;
 switch(LinkedMode)
 {
 case 0:
  Model_SetComponentBitmapInput(LinkedComponentName.c_str(), LinkedIndex, &BmpSource);
 break;
 case 1:
  Model_SetComponentBitmapOutput(LinkedComponentName.c_str(), LinkedIndex, &BmpSource);
 break;
 }
}
// -------------------------


// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ����� �������
void TVideoOutputFrame::ABeforeCalculate(void)
{
 SendToComponentIO();
}

// ���������� ����������
void TVideoOutputFrame::AUpdateInterface(void)
{
 if(LinkedComponentName.size() == 0)
 {
  SendImageToComponentInput1->Caption="Send Image To Component Input...";
  SendImageToComponentOutput1->Caption="Send Image To Component Output...";
 }
 else
 {
  switch(LinkedMode)
  {
  case 0:
   SendImageToComponentInput1->Caption=String("Send Image To Input ")+String(LinkedComponentName.c_str())+String("[")+IntToStr(LinkedIndex)+String("]");
   SendImageToComponentOutput1->Caption="Send Image To Component Output...";
  break;

  case 1:
   SendImageToComponentInput1->Caption="Send Image To Component Input...";
   SendImageToComponentOutput1->Caption=String("Send Image To Output ")+String(LinkedComponentName.c_str())+String("[")+IntToStr(LinkedIndex)+String("]");
  break;
  }
 }
}

// ��������� ��������� ���������� � xml
void TVideoOutputFrame::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{
 xml.WriteString("LinkedComponentName",LinkedComponentName);
 xml.WriteInteger("LinkedMode",LinkedMode);
 xml.WriteInteger("LinkedIndex",LinkedIndex);
}

// ��������� ��������� ���������� �� xml
void TVideoOutputFrame::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{
 LinkedComponentName=xml.ReadString("LinkedComponentName","");
 LinkedMode=xml.ReadInteger("LinkedMode",1);
 LinkedIndex=xml.ReadInteger("LinkedIndex",0);
}
// -----------------------------



//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::TimerTimer(TObject *Sender)
{
 UpdateFlag=false;
 Image->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::StartButtonClick(TObject *Sender)
{
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
 }  
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
  MyVideoOutputToolsForm->AddPointButtonClick(Sender);

 //if(SampleGeometryGraphics.GetNumGeometry())
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

 iwidth=Image->Width;
 iheight=Image->Height;
 bwidth=Image->Picture->Bitmap->Width;
 bheight=Image->Picture->Bitmap->Height;


 DrawCapture(Image->Picture->Bitmap);
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

 // ���� ��������� �������������� ������, ��...
 if(PointFlag)
 {
  AddFigureRect(left,top,width,height);
//  if(SampleGeometryGraphics.GetNumGeometry())
//   MyVideoOutputToolsForm->EditPointButtonClick(Sender);

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
 MyVideoGrabberControlForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::SendToComponentClick(TObject *Sender)
{
 if(MyComponentsListForm->ShowParameterSelect() != mrOk)
  return;

 SelectedComponentPName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 SelectedComponentParameterName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentParameterName();
 SendToComponentParameter(SelectedComponentPName, SelectedComponentParameterName, FigureIndex);
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::PopupMenuPopup(TObject *Sender)
{
 if(SelectedComponentPName.size())
 {
  SendTo->Caption=(std::string("Send To ")+SelectedComponentPName+std::string(":")+SelectedComponentParameterName).c_str();
  SendTo->Enabled=true;
 }
 else
 {
  SendTo->Caption="Send To ";
  SendTo->Enabled=false;
 }

 if(SelectedComponentSName.size())
 {
  SendToState->Caption=(std::string("Send To ")+SelectedComponentSName+std::string(":")+SelectedComponentStateName).c_str();
  SendToState->Enabled=true;
 }
 else
 {
  SendToState->Caption="Send To ";
  SendToState->Enabled=false;
 }
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SendToClick(TObject *Sender)
{
 SendToComponentParameter(SelectedComponentPName, SelectedComponentParameterName, FigureIndex);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SendToComponentState1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowStateSelect() != mrOk)
  return;

 SelectedComponentSName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 SelectedComponentStateName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentStateName();
 SendToComponentParameter(SelectedComponentSName, SelectedComponentStateName, FigureIndex);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SendToStateClick(TObject *Sender)
{
 SendToComponentState(SelectedComponentSName, SelectedComponentStateName, FigureIndex);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SendImageToComponentOutput1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 LinkedMode=1;
 LinkedComponentName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongId();
 LinkedIndex=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SendImageToComponentInput1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 LinkedMode=0;
 LinkedComponentName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongId();
 LinkedIndex=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
}
//---------------------------------------------------------------------------
