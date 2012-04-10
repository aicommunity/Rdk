//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include "UGEngineControlFormUnit.h"
#include "TVideoGrabberControlFormUnit.h"
#include "rdk_initdll.h"
#include "UImagesFormUnit.h"
#include "UComponentsControlFormUnit.h"
#include "UComponentLinksFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "VideoOutputFormUnit.h"
#include "UComponentsPerformanceFormUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUGEngineControlForm *UGEngineControlForm;
//---------------------------------------------------------------------------
__fastcall TUGEngineControlForm::TUGEngineControlForm(TComponent* Owner)
	: TForm(Owner)
{
 UpdateInterfaceFlag=false;
}
//---------------------------------------------------------------------------
void TUGEngineControlForm::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

 // Update video
 if(UImagesForm->Visible)
 {
  UImagesForm->ImagesFrame->UpdateInterface();
 }

 if(UComponentsPerformanceForm->Visible)
  UComponentsPerformanceForm->UComponentsPerformanceFrame->UpdateInterface();

 UpdateInterfaceFlag=false;
}

void __fastcall TUGEngineControlForm::FormShow(TObject *Sender)
{
 GraphicalEngineInit(0,4,4,360,240,1,ExceptionHandler);
 for(int i=0;i<4;i++)
  VideoOutputForm->AddSource();
 UImagesForm->ImagesFrame->SetReflectionXFlag(true);
 UpdateInterface();

 ProjectIni=new TMemIniFile("project.ini");

 String modelfilename=ProjectIni->ReadString("General","ModelFileName","");
 if(modelfilename.Length() != 0)
 {
  UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
 }

 UImagesForm->ImagesFrame->LoadFromIni(ProjectIni,"ImagesFrame");
 UComponentsPerformanceForm->UComponentsPerformanceFrame->LoadFromIni(ProjectIni,"PerformanceFrame");
 VideoOutputForm->LoadFromIni(ProjectIni,"VideoOutputForm");
// UComponentsPerformanceForm->UComponentsPerformanceFrame->AddAllComponents("Pipeline1");
}
//---------------------------------------------------------------------------
void __fastcall TUGEngineControlForm::TimerTimer(TObject *Sender)
{
 Timer->Enabled=false;

 Step1Click(Sender);
 StatusBar->SimpleText=UEngineMonitorForm->EngineMonitorFrame->StatusBar->SimpleText;

 Timer->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TUGEngineControlForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 if(ProjectIni)
 {
  UImagesForm->ImagesFrame->SaveToIni(ProjectIni,"ImagesFrame");
  UComponentsPerformanceForm->UComponentsPerformanceFrame->SaveToIni(ProjectIni,"PerformanceFrame");
  VideoOutputForm->SaveToIni(ProjectIni,"VideoOutputForm");
  ProjectIni->UpdateFile();
  delete ProjectIni;
  ProjectIni=0;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Start1Click(TObject *Sender)
{
// UImagesForm->ImagesFrame->SetNumCells(2,2);
// UImagesForm->ImagesFrame->LinkToComponent(0,0,"Pipeline1.ShowTZoneExtVector2",0);
// UImagesForm->ImagesFrame->LinkToComponent(0,1,"Pipeline1.TrackingSimple",1);
// UImagesForm->ImagesFrame->LinkToComponent(1,0,"Pipeline1.DifferenceFrameSimple",0);
// UImagesForm->ImagesFrame->LinkToComponent(1,1,"Pipeline1.BackgroundSimple",2);

 Timer->Enabled=true;
 VideoOutputForm->GetVideoOutputFrame(0)->StartButtonClick(Sender);
 UEngineMonitorForm->EngineMonitorFrame->Start1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Pause1Click(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Pause1Click(Sender);
 VideoOutputForm->GetVideoOutputFrame(0)->StopButtonClick(Sender);
 Timer->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::EngineMonitor1Click(TObject *Sender)
{
 UEngineMonitorForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::Images1Click(TObject *Sender)
{
 UImagesForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::VideoSource1Click(TObject *Sender)
{
 VideoOutputForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::Step1Click(TObject *Sender)
{
 for(int i=0;i<Env_GetNumInputImages();i++)
 {
  RDK::UBitmap &source=VideoOutputForm->GetVideoOutputFrame(i)->BmpSource;

  if(source.GetByteLength()>0)
  {
   Env_SetInputRes(i, source.GetWidth(), source.GetHeight());
   Env_SetInputImage(i,(unsigned char*)source.GetData(),source.GetWidth(), source.GetHeight(),source.GetColorModel());
  }
 }
 Env_Calculate(0);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::ComponentsControl1Click(TObject *Sender)
{
 UComponentsControlForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::ComponentsLinks1Click(TObject *Sender)
{
 UComponentLinksForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::LoadModel1Click(TObject *Sender)
{
 UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile("");
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::SaveModel1Click(TObject *Sender)
{
 UComponentsControlForm->ComponentsControlFrame->SaveModelToFile("");
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::OpenImage1Click(TObject *Sender)
{
 VideoOutputForm->GetVideoOutputFrame(0)->MyVideoGrabberControlForm->VideoGrabberControlFrame->OpenImageFileButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::OpenVideo1Click(TObject *Sender)
{
 VideoOutputForm->GetVideoOutputFrame(0)->MyVideoGrabberControlForm->VideoGrabberControlFrame->VFBrowseButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::CaptureVideo1Click(TObject *Sender)
{
 VideoOutputForm->GetVideoOutputFrame(0)->MyVideoGrabberControlForm->VideoGrabberControlFrame->SelectMode(0);
 VideoOutputForm->GetVideoOutputFrame(0)->MyVideoGrabberControlForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Reset1Click(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Reset1Click(Sender);
 VideoOutputForm->GetVideoOutputFrame(0)->StopButtonClick(Sender);
 Timer->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Performance1Click(TObject *Sender)
{
 UComponentsPerformanceForm->Show();
}
//---------------------------------------------------------------------------

