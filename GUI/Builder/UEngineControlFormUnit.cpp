//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include "UEngineControlFormUnit.h"
#include "rdk_initdll.h"
#include "UComponentsControlFormUnit.h"
#include "UComponentLinksFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "UComponentsPerformanceFormUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUEngineControlForm *UEngineControlForm;
//---------------------------------------------------------------------------
__fastcall TUEngineControlForm::TUEngineControlForm(TComponent* Owner)
	: TForm(Owner)
{
 UpdateInterfaceFlag=false;
 ProjectAutoSaveFlag=true;
}
//---------------------------------------------------------------------------
void TUEngineControlForm::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

 if(UComponentsPerformanceForm->Visible)
  UComponentsPerformanceForm->UComponentsPerformanceFrame->UpdateInterface();

 Caption="Engine Control";
 if(ProjectIni)
 {
  Caption=Caption+String(" [")+ProjectName+"]";
 }

 UpdateInterfaceFlag=false;
}

// Создает новый проект
void TUEngineControlForm::CreateProject(const String &FileName)
{
 CloseProject();

 OpenProject(FileName);
}

// Закрывает существущий проект
void TUEngineControlForm::CloseProject(void)
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
void TUEngineControlForm::OpenProject(const String &FileName)
{
 CloseProject();
 ProjectIni=new TMemIniFile(OpenDialog->FileName);
 ProjectPath=ExtractFilePath(OpenDialog->FileName);
 ProjectName=ExtractFileName(OpenDialog->FileName);

 // Число входов среды
// NumEnvInputs=ProjectIni->ReadInteger("General","NumEnvInputs",1);

 // Число выходов среды
// NumEnvOutputs=ProjectIni->ReadInteger("General","NumEnvOutputs",1);

// InputEnvImageWidth=ProjectIni->ReadInteger("General","InputEnvImageWidth",360);
// InputEnvImageHeight=ProjectIni->ReadInteger("General","InputEnvImageHeight",240);

 // Флаг автоматического сохранения проекта
 ProjectAutoSaveFlag=ProjectIni->ReadInteger("General","ProjectAutoSaveFlag",1);

 PredefinedStructure=ProjectIni->ReadInteger("General","PredefinedStructure",0);

 EngineInit(PredefinedStructure,ExceptionHandler);
// for(int i=0;i<NumEnvInputs;i++)
//  VideoOutputForm->AddSource();

 String modelfilename=ProjectIni->ReadString("General","ModelFileName","");
 if(modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
 }

// UImagesForm->ImagesFrame->LoadFromIni(ProjectIni,"ImagesFrame");
 UComponentsPerformanceForm->UComponentsPerformanceFrame->LoadFromIni(ProjectIni,"PerformanceFrame");
// VideoOutputForm->LoadFromIni(ProjectIni,"VideoOutputForm");
// UComponentsPerformanceForm->UComponentsPerformanceFrame->AddAllComponents("Pipeline1");
 UpdateInterface();
}

// Сохраняет проект
void TUEngineControlForm::SaveProject(void)
{
 if(!ProjectIni)
  return;

// UImagesForm->ImagesFrame->SaveToIni(ProjectIni,"ImagesFrame");
 UComponentsPerformanceForm->UComponentsPerformanceFrame->SaveToIni(ProjectIni,"PerformanceFrame");
// VideoOutputForm->SaveToIni(ProjectIni,"VideoOutputForm");

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
// ProjectIni->WriteInteger("General","NumEnvInputs",NumEnvInputs);

 // Число выходов среды
// ProjectIni->WriteInteger("General","NumEnvOutputs",NumEnvOutputs);

// ProjectIni->WriteInteger("General","InputEnvImageWidth",InputEnvImageWidth);
// ProjectIni->WriteInteger("General","InputEnvImageHeight",InputEnvImageHeight);

 ProjectIni->UpdateFile();
}


void __fastcall TUEngineControlForm::FormShow(TObject *Sender)
{
// EngineInit(0,ExceptionHandler);
 UpdateInterface();
/*
 ProjectIni=new TMemIniFile("project.ini");

 String modelfilename=ProjectIni->ReadString("General","ModelFileName","");
 if(modelfilename.Length() != 0)
 {
  UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
 }
  */
}
//---------------------------------------------------------------------------




void __fastcall TUEngineControlForm::TimerTimer(TObject *Sender)
{
 Timer->Enabled=false;

 Step1Click(Sender);
 StatusBar->SimpleText=UEngineMonitorForm->EngineMonitorFrame->StatusBar->SimpleText;

 Timer->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TUEngineControlForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 SaveProjectItemClick(Sender);
 if(ProjectIni)
 {
  delete ProjectIni;
  ProjectIni=0;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::Start1Click(TObject *Sender)
{
// UImagesForm->ImagesFrame->SetNumCells(2,2);
// UImagesForm->ImagesFrame->LinkToComponent(0,0,"Pipeline1.ShowTZoneExtVector2",0);
// UImagesForm->ImagesFrame->LinkToComponent(0,1,"Pipeline1.TrackingSimple",1);
// UImagesForm->ImagesFrame->LinkToComponent(1,0,"Pipeline1.DifferenceFrameSimple",0);
// UImagesForm->ImagesFrame->LinkToComponent(1,1,"Pipeline1.BackgroundSimple",2);

 Timer->Enabled=true;
 UEngineMonitorForm->EngineMonitorFrame->Start1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::Pause1Click(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Pause1Click(Sender);
 Timer->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::EngineMonitor1Click(TObject *Sender)
{
 UEngineMonitorForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::Step1Click(TObject *Sender)
{
 Env_Calculate(0);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::ComponentsControl1Click(TObject *Sender)
{
 UComponentsControlForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::ComponentsLinks1Click(TObject *Sender)
{
 UComponentLinksForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::LoadModel1Click(TObject *Sender)
{
 UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile("");
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::SaveModel1Click(TObject *Sender)
{
 UComponentsControlForm->ComponentsControlFrame->SaveModelToFile("");
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::Reset1Click(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Reset1Click(Sender);
 Timer->Enabled=false;
}
//---------------------------------------------------------------------------


void __fastcall TUEngineControlForm::Performance1Click(TObject *Sender)
{
 UComponentsPerformanceForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::LoadProjectItemClick(TObject *Sender)
{
 if(!OpenDialog->Execute())
  return;

 OpenProject(OpenDialog->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::SaveProjectItemClick(TObject *Sender)
{
 SaveProject();
}
//---------------------------------------------------------------------------

