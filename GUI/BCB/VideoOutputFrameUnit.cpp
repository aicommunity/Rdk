//---------------------------------------------------------------------------
#include <vcl.h>
#include <jpeg.hpp>
#pragma hdrstop

#include "VideoOutputFrameUnit.h"
#include "TUBitmap.h"
#include "TVideoGrabberControlFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "myrdk.h"
#include "rdk_initdll.h"
//#include "USharedMemoryLoader.h"
//#include "TUFileSystem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VidGrab"
#pragma link "TUVisualControllerFrameUnit"
#pragma link "TUHttpServerUnit"
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
   VideoGrabber->Synchronized=true;

   VideoGrabber->OpenPlayer();
   Mode=1;
   UpdateInterfaceFlag=true;
   TrackBar->Position=0;
   UpdateInterfaceFlag=false;
}

// ������������� ������ bmp-������
void TVideoOutputFrame::InitByBmp(const String &filename)
{
 StopButtonClick(this);
 try {
  if(filename.Pos(".jpg") || filename.Pos(".jpeg") )
  {
   TJPEGImage* JpegIm=new TJPEGImage;
   JpegIm->LoadFromFile(filename);
   Image->Picture->Bitmap->Assign(JpegIm);
   BmpSource<<Image->Picture->Bitmap;
   delete JpegIm;
  }
  else
  if(filename.Pos(".bmp"))
  {
   LoadBitmapFromFile(AnsiString(filename).c_str(),&BmpSource);
  }
  else
   BmpSource.Fill(0);
 }
 catch (EFOpenError &exception) {
  BmpSource.SetRes(0,0);
 }
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

// ������������� ������ IP-�������
void TVideoOutputFrame::InitByIPCamera(const String camera_url, const String user_name, const String user_password)
{
 StopButtonClick(this);
 VideoGrabber->BurstType = fc_TBitmap;
 VideoGrabber->BurstMode = True;
 VideoGrabber->BurstCount = 0;
 VideoGrabber->IPCameraURL=camera_url;

 VideoGrabber->SetAuthentication(at_IPCamera,user_name,user_password);
 Mode=3;
// StartButtonClick(this);
 UpdateVideo();
}

// ������������� ������������������� �����������
bool TVideoOutputFrame::InitByImageSequence(const String &pathname)
{
 StopButtonClick(this);
 Mode=4;
// StartButtonClick(this);
 BmpSequencePathName=AnsiString(pathname+"\\").c_str();

 RDK::FindFilesList(BmpSequencePathName, "*.bmp", true, BmpSequenceNames);
 if(BmpSequenceNames.size() == 0)
  RDK::FindFilesList(BmpSequencePathName, "*.jpg", true, BmpSequenceNames);
 if(BmpSequenceNames.size() == 0)
  RDK::FindFilesList(BmpSequencePathName, "*.jpeg", true, BmpSequenceNames);

 CurrentBmpSequenceIndex=0;

 LastReadSequenceIndex=-1;
 if(BmpSequenceNames.size()>0)
 {
  LoadImageFromSequence(0,BmpSource);
  BmpSource.SetColorModel(RDK::ubmRGB24);
  LastReadSequenceIndex=0;
 }
 else
 {
  BmpSource.Clear();
  LastReadSequenceIndex=-1;
 }

 UpdateInterfaceFlag=true;
 TrackBar->Position=0;
 CurrentBmpSequenceIndex=0;
 UpdateInterfaceFlag=false;
 UpdateVideo();

 return true;
}

// ��������� ��������� �������� �� ������ � ������� ����

bool TVideoOutputFrame::LoadImageFromSequence(int index, RDK::UBitmap &bmp)
{
 if(LastReadSequenceIndex == index)
  return true;

 if(BmpSequenceNames[index].find(".bmp") != std::string::npos)
 {
  RDK::LoadBitmapFromFile((BmpSequencePathName+BmpSequenceNames[index]).c_str(),bmp);
 }
 else
 if(BmpSequenceNames[index].find(".jpg") != std::string::npos || BmpSequenceNames[index].find(".jpeg") != std::string::npos)
 {
  TJPEGImage* JpegIm=new TJPEGImage;
  JpegIm->LoadFromFile((BmpSequencePathName+BmpSequenceNames[index]).c_str());
  Image->Picture->Bitmap->Assign(JpegIm);
  bmp<<Image->Picture->Bitmap;
  delete JpegIm;
 }
 else
  bmp.Fill(0);
 LastReadSequenceIndex = index;
 return true;
}

// ������������� http-�������
bool TVideoOutputFrame::InitByHttpServer(int listen_port)
{
 StopButtonClick(this);
 Mode=5;
 UHttpServerFrame->IdHTTPServer->Bindings->DefaultPort=listen_port;
 TIdSocketHandle* bind=0;
 if(UHttpServerFrame->IdHTTPServer->Bindings->Count == 0)
  bind=UHttpServerFrame->IdHTTPServer->Bindings->Add();
 else
  bind=UHttpServerFrame->IdHTTPServer->Bindings->Items[0];

 bind->Port=listen_port;
 return true;
}

// ������������� ����� ������
bool TVideoOutputFrame::InitBySharedMemory(int pipe_index, const std::string &pipe_name)
{
 PipeIndex=pipe_index;
 PipeName=pipe_name;
 SharedMemoryPipeSize=0;
 Mode=6;
 return true;
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
 }

 return true;
}

// ������������ ������� ��������� �����������
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

// ��������� ������ �����
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
/*
 if(FigureFlag)
 {
  GeometryCheckListBox->ItemIndex=FigureIndex;
  GeometryCheckListBox->Enabled=false;
 }
 else
  GeometryCheckListBox->Enabled=true;
  */
 if(GeometryGraphics.GetNumGeometries()<=FigureIndex)
  return;

 ix=PointsCheckListBox->ItemIndex;

// PointsCheckListBox->Clear();
 if(CurrentGeometryGraphics.GetNumGeometries()>GeometryCheckListBox->ItemIndex)
 {
  if(GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex) == CurrentGeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex))
   return;
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
 for(size_t i=0;i<new_value;i++)
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

 if(FigureIndex<0)
  return;

 GeometryGraphics.Geometry(FigureIndex).Vertex(PointIndex).x=l+w/2;
 GeometryGraphics.Geometry(FigureIndex).Vertex(PointIndex).y=/*BmpCanvas.GetHeight()-*/(t+h/2);

// GeometryGraphics.Geometry(FigureIndex)=Figure;

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

// -------------------------
// ������ ����� ������ ����� ��������� �� ���������� � ���������� ��������� ���������
// -------------------------
// ���������� ����� ����� � �������� ����������
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

// ���������� ����� ����� � �������� ����������
void TVideoOutputFrame::SendToComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index)
{
 if(figure_index<0)
  return;

 const std::vector<RDK::MVector<double,2> > &points=GeometryGraphics.Geometry(figure_index).GetVertices();
 Model_SetComponentPropertyData(stringid.c_str(), parameter_name.c_str(), &points);
// RDK::WriteParameterValue(stringid, parameter_name, points);
}

// ���������� ����� ����� � ���������� ��������� ����������
void TVideoOutputFrame::SendToComponentState(const std::string &stringid, const std::string &state_name, int figure_index)
{
 if(figure_index<0)
  return;

 const std::vector<RDK::MVector<double,2> > &points=GeometryGraphics.Geometry(figure_index).GetVertices();
 Model_SetComponentPropertyData(stringid.c_str(), state_name.c_str(), &points);
// RDK::WriteStateValue(stringid, state_name, points);
}

// ��������� ����� ����� �� ��������� ����������
void TVideoOutputFrame::ReceiveFromComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index)
{
 std::vector<RDK::MVector<double,2> > points;
 points=*(const std::vector<RDK::MVector<double,2> > *)Model_GetComponentPropertyData(stringid.c_str(), parameter_name.c_str());
// RDK::ReadParameterValue(stringid, parameter_name, points);
}

// ��������� ����� ����� �� ���������� ��������� ����������
void TVideoOutputFrame::ReceiveFromComponentState(const std::string &stringid, const std::string &state_name, int figure_index)
{
 std::vector<RDK::MVector<double,2> > points;
 points=*(const std::vector<RDK::MVector<double,2> > *)Model_GetComponentPropertyData(stringid.c_str(), state_name.c_str());
// RDK::ReadStateValue(stringid, state_name, points);
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
 BmpSource.ReflectionX(&ReflectedBmpSource);
 switch(LinkedMode)
 {
 case 0:
  Model_SetComponentBitmapInput(LinkedComponentName.c_str(), LinkedIndex, &ReflectedBmpSource);
 break;

 case 1:
  Model_SetComponentBitmapOutput(LinkedComponentName.c_str(), LinkedIndex, &ReflectedBmpSource);
 break;

 case 2:
  Model_SetComponentPropertyData(LinkedComponentName.c_str(), LinkedComponentPropertyName.c_str(), &ReflectedBmpSource);
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
 if(Mode == 4)
 {
  CurrentBmpSequenceIndex++;
  UpdateVideo();
  Sleep(0);
 }
 else
 if(Mode == 6)
 {
  if(Usm_IsPipeInit)
  {
   int real_size=0;
   if(Usm_IsPipeInit(PipeIndex)<0)
   {
	StartButtonClick(this);
	if(Usm_IsPipeInit(PipeIndex)<0)
	 return;
   }

   SharedMemoryPipeSize=Usm_GetPipeSize(PipeIndex);
   if(SharedMemoryPipeSize<0)
	SharedMemoryPipeSize=0;
   PipeBuffer.resize(SharedMemoryPipeSize);
   if(!SharedMemoryPipeSize || SharedMemoryPipeSize<16)
	return;

   real_size=Usm_ReadData(PipeIndex,&PipeBuffer[0],PipeBuffer.size());
   if(real_size>0)
   {
	int shift=0;
	long long time_stamp;
	memcpy(&time_stamp,&PipeBuffer[0],sizeof(ServerTimeStamp));

	if(ServerTimeStamp == time_stamp)
	{
	 // ����� ��������� ������ � ���, ��� �������� ������� ������� ����������
	 return;
	}
	shift+=sizeof(ServerTimeStamp);

	UEngineMonitorForm->EngineMonitorFrame->ServerTimeStamp=ServerTimeStamp;
	std::string sstamp;
	RDK::UTimeStamp stamp(double(ServerTimeStamp/1000),25);
	stamp>>sstamp;
	TimeEdit->Text=sstamp.c_str();

	int width=0;
	int height=0;
	memcpy(&width,&PipeBuffer[shift],sizeof(width));
	shift+=sizeof(width);
	memcpy(&height,&PipeBuffer[shift],sizeof(height));
	shift+=sizeof(height);
	BmpSource.SetRes(width,height,RDK::ubmRGB24);
	if(shift<SharedMemoryPipeSize)
	{
	 int image_size=width*height*3;
	 if(image_size>SharedMemoryPipeSize-16)
	  image_size=SharedMemoryPipeSize-16;
	 memcpy(BmpSource.GetData(),&PipeBuffer[shift],image_size);
	}
	UpdateVideo();
   }
  }
 }


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
}

// ���������� ����������
void TVideoOutputFrame::AUpdateInterface(void)
{
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
   SendImageToComponentInput1->Caption=String("Send Image To Input: ")+String(LinkedComponentName.c_str())+String("[")+IntToStr(LinkedIndex)+String("]");
   SendImageToComponentOutput1->Caption="Send Image To Component Output...";
   SendImageToComponentProperty1->Caption="Send Image To Component Property...";
  break;

  case 1:
   SendImageToComponentInput1->Caption="Send Image To Component Input...";
   SendImageToComponentProperty1->Caption="Send Image To Component Property...";
   SendImageToComponentOutput1->Caption=String("Send Image To Output: ")+String(LinkedComponentName.c_str())+String("[")+IntToStr(LinkedIndex)+String("]");
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

// ��������� ��������� ���������� � xml
void TVideoOutputFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("LinkedComponentName",LinkedComponentName);
 xml.WriteString("LinkedComponentPropertyName",LinkedComponentPropertyName);
 xml.WriteInteger("LinkedMode",LinkedMode);
 xml.WriteInteger("LinkedIndex",LinkedIndex);

 xml.WriteString("SelectedComponentSName",SelectedComponentSName);
 xml.WriteString("SelectedComponentStateName",SelectedComponentStateName);
 xml.WriteString("SelectedComponentPName",SelectedComponentPName);
 xml.WriteString("SelectedComponentParameterName",SelectedComponentParameterName);
 xml.WriteString("SelectedComponentMatrixName",SelectedComponentMatrixName);
 xml.WriteString("SelectedComponentPropertyMatrixName",SelectedComponentPropertyMatrixName);
 xml.WriteInteger("SendPointsByStep",SendPointsByStepCheckBox->Checked);
 xml.WriteInteger("DeletePointsAfterSendCheckBox",DeletePointsAfterSendCheckBox->Checked);
}

// ��������� ��������� ���������� �� xml
void TVideoOutputFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 LinkedComponentName=xml.ReadString("LinkedComponentName","");
 LinkedComponentPropertyName=xml.ReadString("LinkedComponentPropertyName","");
 LinkedMode=xml.ReadInteger("LinkedMode",1);
 LinkedIndex=xml.ReadInteger("LinkedIndex",0);

 SelectedComponentSName=xml.ReadString("SelectedComponentSName","");
 SelectedComponentStateName=xml.ReadString("SelectedComponentStateName","");
 SelectedComponentPName=xml.ReadString("SelectedComponentPName","");
 SelectedComponentParameterName=xml.ReadString("SelectedComponentParameterName","");
 SelectedComponentMatrixName=xml.ReadString("SelectedComponentMatrixName","");
 SelectedComponentPropertyMatrixName=xml.ReadString("SelectedComponentPropertyMatrixName","");
 SendPointsByStepCheckBox->Checked=xml.ReadInteger("SendPointsByStep",0);
 DeletePointsAfterSendCheckBox->Checked=xml.ReadInteger("DeletePointsAfterSendCheckBox",0);

 UpdateInterface();
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
  VideoGrabber->StartSynchronized();
  VideoGrabber->RunPlayer();
 break;

 case 2:
   VideoGrabber->StartPreview();
   VideoGrabber->StartSynchronized();
 break;

 case 3:
   VideoGrabber->StartPreview();
   VideoGrabber->StartSynchronized();
 break;

 case 4:
  if(CurrentBmpSequenceIndex<int(BmpSequenceNames.size())-1)
  {
   LoadImageFromSequence(CurrentBmpSequenceIndex,BmpSource);
   BmpSource.SetColorModel(RDK::ubmRGB24);
   UpdateVideo();
  }

//  VideoGrabber->StartPreview();
//  VideoGrabber->StartSynchronized();
//  VideoGrabber->PlayerFrameStep(1);
 break;

 case 5:
  UHttpServerFrame->IdHTTPServer->Active=true;
 break;

 case 6:
 {
  if(Usm_SetNumPipes)
   Usm_SetNumPipes(PipeIndex+1);

  int res=Usm_InitPipe(PipeIndex,SharedMemoryPipeSize,0,PipeName.c_str());
 }
 break;

 default:
     ;
 }  
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::StopButtonClick(TObject *Sender)
{
 int res=0;
 switch (Mode) {
 case 0:
 break;

 case 1:
    VideoGrabber->PausePlayer();
 break;

 case 2:
   VideoGrabber->PausePreview();
 break;

 case 3:
   VideoGrabber->PausePreview();
 break;

 case 4:
	VideoGrabber->PausePlayer();
 break;

 case 5:
  UHttpServerFrame->IdHTTPServer->Active=false;
 break;

 case 6:
 {
  if(Usm_UnInitPipe)
   res=Usm_UnInitPipe(PipeIndex);
 }
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
 if(PointFlag == 1)
 {
  if(FigureIndex<0)
   MyVideoOutputToolsForm->AddFigureButtonClick(Sender);
  GeometryGraphics.Geometry(FigureIndex).SetNumVertices(GeometryGraphics.Geometry(FigureIndex).GetNumVertices()+1);
  PointIndex=GeometryGraphics.Geometry(FigureIndex).GetNumVertices()-1;
  GeometryGraphics.Geometry(FigureIndex).VertexName(PointIndex)=std::string(AnsiString(MyVideoOutputToolsForm->PointNameEdit->Text).c_str())+RDK::sntoa(PointIndex+1,3);
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

 // ���� ��������� �������������� ������, ��...
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
 }
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
//         DrawCapture(Image->Picture->Bitmap);
		 UpdateVideo();

	  break;
   }
 UEngineMonitorForm->EngineMonitorFrame->CalculateSignal=true;
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::TrackBarChange(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

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
 }
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

void __fastcall TVideoOutputFrame::UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext,
		  TIdHTTPRequestInfo *ARequestInfo, TIdHTTPResponseInfo *AResponseInfo)

{
 std::vector<char> &time_stamp=UHttpServerFrame->ParsedRequestArgs["TimeStamp"];
 if(!time_stamp.empty())
 {
  std::string temp_stamp;
  temp_stamp.assign(&time_stamp[0],time_stamp.size());
  ServerTimeStamp=RDK::atoi(temp_stamp);
  UEngineMonitorForm->EngineMonitorFrame->ServerTimeStamp=ServerTimeStamp;

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

 std::vector<char> &step=UHttpServerFrame->ParsedRequestArgs["StepEnable"];
 if(!step.empty() && step[0]=='1')
 {
  UEngineMonitorForm->EngineMonitorFrame->CalculateSignal=true;
 }
}
//---------------------------------------------------------------------------

