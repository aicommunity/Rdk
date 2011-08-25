//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VideoOutputFrameUnit.h"
#include "rdk_builder.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoOutputFrame *VideoOutputFrame;

//---------------------------------------------------------------------------
// Устанавливает новый источник видео
// Если capture == 0, то отменяет текущий источник
bool TVideoOutputFrame::SetCapture(VACapture *capture)
{
 Capture=capture;

 return UpdateVideo();
}

// Устанавливает отдельное изображение
bool TVideoOutputFrame::SetBitmap(const UBitmap &bmp)
{
 BmpSource=bmp;
 BmpSource.SetColorModel(ubmRGB24);
 Capture=0;
 return UpdateVideo();
}


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
//  ImageTrackBar->Enabled=true;
//  TimeEdit->ReadOnly=false;
//  TimeEdit->Color=clWindow;
//  ImageTrackBar->Max=static_cast<VFCapture*>(Capture)->GetNumFrames();
//  ImageTrackBar->Position=static_cast<VFCapture*>(Capture)->GetCurrentFrame();
  UpdateFlag=false;
 }
 else
 {
//  TimeEdit->ReadOnly=true;
//  TimeEdit->Color=clBtnFace;
 }

 if(Capture)
 {
/*  string sstamp;
  VTimeStamp stamp(Capture->GetCurrentVideoTime(),Capture->GetFPS());
  stamp>>sstamp;
  TimeEdit->Text=sstamp.c_str();
  StartButton->Caption="Пуск";
  StopButton->Caption="Стоп";  */
  UpdateFlag=true;
  if(Capture->GetCaptureState())
  {
//   StartButton->Enabled=false;
//   StopButton->Enabled=true;
  }
  else
  {
//   StartButton->Enabled=true;
//   StopButton->Enabled=false;
  }

  UpdateFlag=false;
 }
 DrawCapture(Capture,Image->Picture->Bitmap);
 Image->Repaint();
 if(left != -1 || top != -1 || width != -1 || height != -1)
  DrawFrameRect(Image, left, top, left+width, top+height, 2, SelColor);
 Image->Repaint();

 UpdateGeometryList(MyVideoOutputToolsForm->GeometryCheckListBox,
                    MyVideoOutputToolsForm->PointsCheckListBox);

 return true;
}

// Отрисовываем текущее состояние видеопотока
void TVideoOutputFrame::DrawCapture(VACapture *capture, Graphics::TBitmap *bmp)
{
 if(!bmp)
  return;

 if(capture && capture->GetReadyState())
 {
  bmp->Width=capture->GetWidth();
  bmp->Height=capture->GetHeight();
  bmp->HandleType=bmDIB;
  switch(capture->GetBitsPerPixel())
  {
  case 8:
   bmp->PixelFormat=pf8bit;
  break;

  case 16:
   bmp->PixelFormat=pf16bit;
  break;

  case 15:
   bmp->PixelFormat=pf15bit;
  break;

  case 24:
   bmp->PixelFormat=pf24bit;
  break;

  case 32:
   bmp->PixelFormat=pf32bit;
  break;
  }

  BmpCanvas<<capture;
 }
 else
 {
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
 }

 GeometryGraphics.Repaint();

 BmpCanvas>>bmp;

 if(capture)
 {
  UpdateFlag=true;
//  string sstamp;
//  VTimeStamp stamp(capture->GetCurrentVideoTime(),capture->GetFPS());
//  stamp>>sstamp;
//  TimeEdit->Text=sstamp.c_str();
  UpdateFlag=false;
 }
}


// Обновляем список точек
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
  stringstream stream;
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
   realwidth=max(image->Picture->Bitmap->Width/image->Width,
                 image->Picture->Bitmap->Height/image->Height);
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


// Устанавливает образец графики
void TVideoOutputFrame::SetSampleGeometryGraphics(MGraphics<double>& samplegraphics)
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
 Capture=0;

 // Модуль графики
 Graph.SetCanvas(&BmpCanvas);

 // Отрисовка геометрии
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

 SelColor=0x000000FF;
}

__fastcall TVideoOutputFrame::~TVideoOutputFrame(void)
{
 CloseHandle(ZoneSelectEvent);
 delete MyVideoOutputToolsForm;
 MyVideoOutputToolsForm=0;
}

//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::ImageTrackBarChange(TObject *Sender)
{
/* if(UpdateFlag)
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
 Image->Update();    */
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::TimerTimer(TObject *Sender)
{
 if(!Capture->GetCaptureState())
  {
//   StartButton->Enabled=false;
//   StopButton->Enabled=true;
//   Timer->Enabled=false;
   return;
  }

 DrawCapture(Capture,Image->Picture->Bitmap);
 UpdateFlag=true;
/*
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
 if(!Capture)
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
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::StopButtonClick(TObject *Sender)
{
 if(!Capture)
  return;

 if(UpdateFlag)
  return;

  StartButton->Enabled=true;
  StopButton->Enabled=false;
  Capture->Pause();
  Timer->Enabled=false;
//  if(ImageTrackBar->Position<ImageTrackBar->Max)
//   ImageTrackBar->Position=ImageTrackBar->Position+1;
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

//     DrawFrameRect(Image, k1x, k1y, k2x, k2y, 3, (TColor)(255<<16));

     left=k1x;
     top=k1y;
     width=(k2x-k1x);
     height=(k2y-k1y);
     DrawCapture(Capture,Image->Picture->Bitmap);
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

//     correctionwidth=(k2x-k1x);
//     correctionheight=(k2y-k1y);
     DrawCapture(Capture,Image->Picture->Bitmap);
     DrawFrameRect(Image, k1x, k1y, k2x, k2y, 2, SelColor);
//     ImageTrackBarChange(Sender);
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

 // Если добавляем геометрический объект, то...
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

 if(!Capture || !dynamic_cast<VFCapture*>(Capture))
  return;

 string sstamp;
 VTimeStamp stamp(0.0,Capture->GetFPS());
 sstamp=TimeEdit->Text.t_str();
 stamp<<sstamp;

 static_cast<VFCapture*>(Capture)->SelectFrame(stamp());
 UpdateFlag=true;
 ImageTrackBar->Position=stamp();
 UpdateFlag=false;

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

