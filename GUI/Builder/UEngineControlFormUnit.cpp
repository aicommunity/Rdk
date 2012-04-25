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
#include "UWatchWindowFormUnit.h"

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
void TUEngineControlForm::BeforeCalculate(void)
{

}

void TUEngineControlForm::AfterCalculate(void)
{

}

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

 StatusBar->SimpleText=UEngineMonitorForm->EngineMonitorFrame->StatusBar->SimpleText;
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

 // Флаг автоматического сохранения проекта
 ProjectAutoSaveFlag=ProjectIni->ReadInteger("General","ProjectAutoSaveFlag",1);

 PredefinedStructure=ProjectIni->ReadInteger("General","PredefinedStructure",0);

 EngineInit(PredefinedStructure,ExceptionHandler);

 String modelfilename=ProjectIni->ReadString("General","ModelFileName","");
 if(modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
 }

 UEngineMonitorForm->EngineMonitorFrame->LoadFromIni(ProjectIni,"EngineMonitorForm");
 UComponentsPerformanceForm->UComponentsPerformanceFrame->LoadFromIni(ProjectIni,"PerformanceFrame");
 UpdateInterface();
}

// Сохраняет проект
void TUEngineControlForm::SaveProject(void)
{
 if(!ProjectIni)
  return;

 UEngineMonitorForm->EngineMonitorFrame->SaveToIni(ProjectIni,"EngineMonitorForm");
 UComponentsPerformanceForm->UComponentsPerformanceFrame->SaveToIni(ProjectIni,"PerformanceFrame");

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

 ProjectIni->UpdateFile();
}


void __fastcall TUEngineControlForm::FormShow(TObject *Sender)
{
 if(UEngineMonitorForm)
  UEngineMonitorForm->AddInterface(this);
 UpdateInterface();
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
 UEngineMonitorForm->EngineMonitorFrame->Start1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::Pause1Click(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Pause1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::EngineMonitor1Click(TObject *Sender)
{
 UEngineMonitorForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::Step1Click(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Step1Click(Sender);
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


void __fastcall TUEngineControlForm::FormHide(TObject *Sender)
{
 if(UEngineMonitorForm)
  UEngineMonitorForm->DelInterface(this);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::WatchWindow1Click(TObject *Sender)
{
 UWatchWindowForm->Show();
}
//---------------------------------------------------------------------------

