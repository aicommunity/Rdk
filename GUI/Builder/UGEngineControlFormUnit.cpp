//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include "UGEngineControlFormUnit.h"
#include "TVideoGrabberControlFormUnit.h"
#include "UImagesFormUnit.h"
#include "UComponentsControlFormUnit.h"
#include "UComponentLinksFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "VideoOutputFormUnit.h"
#include "UComponentsPerformanceFormUnit.h"
#include "rdk_initdll.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUGEngineControlForm *UGEngineControlForm;
//---------------------------------------------------------------------------
__fastcall TUGEngineControlForm::TUGEngineControlForm(TComponent* Owner)
	: TForm(Owner)
{
 UpdateInterfaceFlag=false;
 ProjectAutoSaveFlag=true;
}
//---------------------------------------------------------------------------
// Метод, вызываемый перед шагом расчета
void TUGEngineControlForm::BeforeCalculate(void)
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

}

// Метод, вызываемый после шага расчета
void TUGEngineControlForm::AfterCalculate(void)
{
 StatusBar->SimpleText=UEngineMonitorForm->EngineMonitorFrame->StatusBar->SimpleText;
}

void TUGEngineControlForm::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

 CaptureVideo1->Caption=String("Capture Video (")+IntToStr(VideoOutputForm->GetActiveSource())+")";
 OpenVideo1->Caption=String("Open Video File (")+IntToStr(VideoOutputForm->GetActiveSource())+")";
 OpenImage1->Caption=String("Open Image (")+IntToStr(VideoOutputForm->GetActiveSource())+")";
 ToolButton6->Caption=CaptureVideo1->Caption;
 ToolButton8->Caption=OpenVideo1->Caption;
 ToolButton9->Caption=OpenImage1->Caption;

 Caption="Engine Control";
 if(ProjectIni)
 {
  Caption=Caption+String(" [")+ProjectName+"]";
 }

 UpdateInterfaceFlag=false;
}

void __fastcall TUGEngineControlForm::FormShow(TObject *Sender)
{
 if(UEngineMonitorForm)
  UEngineMonitorForm->AddInterface(this);
 UImagesForm->ImagesFrame->SetReflectionXFlag(true);
 UpdateInterface();
}

// Создает новый проект
void TUGEngineControlForm::CreateProject(const String &FileName)
{
 CloseProject();

 OpenProject(FileName);
}

// Закрывает существущий проект
void TUGEngineControlForm::CloseProject(void)
{
 if(ProjectAutoSaveFlag)
  SaveProject();

 if(ProjectIni)
 {
  delete ProjectIni;
  ProjectName="";
  ProjectPath="";
 }
 UpdateInterface();
}

// Открывает проект
void TUGEngineControlForm::OpenProject(const String &FileName)
{
 CloseProject();
 ProjectIni=new TMemIniFile(OpenDialog->FileName);
 ProjectPath=ExtractFilePath(OpenDialog->FileName);
 ProjectName=ExtractFileName(OpenDialog->FileName);

 // Число входов среды
 NumEnvInputs=ProjectIni->ReadInteger("General","NumEnvInputs",1);

 // Число выходов среды
 NumEnvOutputs=ProjectIni->ReadInteger("General","NumEnvOutputs",1);

 InputEnvImageWidth=ProjectIni->ReadInteger("General","InputEnvImageWidth",360);
 InputEnvImageHeight=ProjectIni->ReadInteger("General","InputEnvImageHeight",240);

 PredefinedStructure=ProjectIni->ReadInteger("General","PredefinedStructure",0);

 // Флаг автоматического сохранения проекта
 ProjectAutoSaveFlag=ProjectIni->ReadInteger("General","ProjectAutoSaveFlag",1);

 // Шаг счета по умолчанию
 DefaultTimeStep=ProjectIni->ReadInteger("General","DefaultTimeStep",30);

 // Глобальный шаг счета модели
 GlobalTimeStep=ProjectIni->ReadInteger("General","GlobalTimeStep",30);

 GraphicalEngineInit(PredefinedStructure,NumEnvInputs,NumEnvOutputs,InputEnvImageWidth, InputEnvImageHeight ,1,ExceptionHandler);
 Model_SetDefaultTimeStep(DefaultTimeStep);

 for(int i=0;i<NumEnvInputs;i++)
  VideoOutputForm->AddSource();

 String modelfilename=ProjectIni->ReadString("General","ModelFileName","");
 if(modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
 }

 Model_SetGlobalTimeStep("",GlobalTimeStep);

 UImagesForm->ImagesFrame->LoadFromIni(ProjectIni,"ImagesFrame");
 UComponentsPerformanceForm->UComponentsPerformanceFrame->LoadFromIni(ProjectIni,"PerformanceFrame");
 VideoOutputForm->LoadFromIni(ProjectIni,"VideoOutputForm");
 UEngineMonitorForm->EngineMonitorFrame->LoadFromIni(ProjectIni,"EngineMonitorForm");
// UComponentsPerformanceForm->UComponentsPerformanceFrame->AddAllComponents("Pipeline1");
 UpdateInterface();
}

// Сохраняет проект
void TUGEngineControlForm::SaveProject(void)
{
 if(!ProjectIni)
  return;

 UEngineMonitorForm->EngineMonitorFrame->SaveToIni(ProjectIni,"EngineMonitorForm");
 UImagesForm->ImagesFrame->SaveToIni(ProjectIni,"ImagesFrame");
 UComponentsPerformanceForm->UComponentsPerformanceFrame->SaveToIni(ProjectIni,"PerformanceFrame");
 VideoOutputForm->SaveToIni(ProjectIni,"VideoOutputForm");

 String modelfilename=ProjectIni->ReadString("General","ModelFileName","");
 if(modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(modelfilename);
 }

 ProjectIni->WriteInteger("General","PredefinedStructure",PredefinedStructure);

 // Флаг автоматического сохранения проекта
 ProjectIni->WriteInteger("General","ProjectAutoSaveFlag",ProjectAutoSaveFlag);

 // Число входов среды
 ProjectIni->WriteInteger("General","NumEnvInputs",NumEnvInputs);

 // Число выходов среды
 ProjectIni->WriteInteger("General","NumEnvOutputs",NumEnvOutputs);

 ProjectIni->WriteInteger("General","InputEnvImageWidth",InputEnvImageWidth);
 ProjectIni->WriteInteger("General","InputEnvImageHeight",InputEnvImageHeight);

 // Шаг счета по умолчанию
 ProjectIni->WriteInteger("General","DefaultTimeStep",DefaultTimeStep);

 // Глобальный шаг счета модели
 ProjectIni->WriteInteger("General","GlobalTimeStep",GlobalTimeStep);

 ProjectIni->UpdateFile();
}
//---------------------------------------------------------------------------
void __fastcall TUGEngineControlForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 SaveProjectItemClick(Sender);
 if(ProjectIni)
 {
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

// Timer->Enabled=true;
 VideoOutputForm->Start();
 UEngineMonitorForm->EngineMonitorFrame->Start1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Pause1Click(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Pause1Click(Sender);
 VideoOutputForm->Stop();
// Timer->Enabled=false;
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
 UEngineMonitorForm->EngineMonitorFrame->Step1Click(Sender);
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
 VideoOutputForm->GetActiveVideoOutputFrame()->MyVideoGrabberControlForm->VideoGrabberControlFrame->OpenImageFileButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::OpenVideo1Click(TObject *Sender)
{
 VideoOutputForm->GetActiveVideoOutputFrame()->MyVideoGrabberControlForm->VideoGrabberControlFrame->VFBrowseButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::CaptureVideo1Click(TObject *Sender)
{
 VideoOutputForm->GetActiveVideoOutputFrame()->MyVideoGrabberControlForm->VideoGrabberControlFrame->SelectMode(0);
 VideoOutputForm->GetActiveVideoOutputFrame()->MyVideoGrabberControlForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Reset1Click(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Reset1Click(Sender);
 VideoOutputForm->Stop();
// Timer->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Performance1Click(TObject *Sender)
{
 UComponentsPerformanceForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::LoadProjectItemClick(TObject *Sender)
{
 if(!OpenDialog->Execute())
  return;

 OpenProject(OpenDialog->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::SaveProjectItemClick(TObject *Sender)
{
 SaveProject();
}
//---------------------------------------------------------------------------




void __fastcall TUGEngineControlForm::FormHide(TObject *Sender)
{
 if(UEngineMonitorForm)
  UEngineMonitorForm->DelInterface(this);
}
//---------------------------------------------------------------------------

