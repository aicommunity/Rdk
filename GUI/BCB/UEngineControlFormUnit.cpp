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
#include "UWatchFormUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFormUnit"
#pragma resource "*.dfm"
TUEngineControlForm *UEngineControlForm;
//---------------------------------------------------------------------------
__fastcall TUEngineControlForm::TUEngineControlForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 ProjectAutoSaveFlag=true;
 ProjectOpenFlag=false;
}
//---------------------------------------------------------------------------
void TUEngineControlForm::ABeforeCalculate(void)
{

}

void TUEngineControlForm::AAfterCalculate(void)
{

}

void TUEngineControlForm::AUpdateInterface(void)
{
 if(UComponentsPerformanceForm->Visible)
  UComponentsPerformanceForm->UComponentsPerformanceFrame->UpdateInterface();

 Caption="Engine Control";
 if(ProjectOpenFlag)
 {
  Caption=Caption+String(" [")+ProjectName+"]";
 }

 StatusBar->SimpleText=UEngineMonitorForm->EngineMonitorFrame->StatusBar->SimpleText;
}

// Сохраняет параметры интерфейса в xml
void TUEngineControlForm::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{
}

// Загружает параметры интерфейса из xml
void TUEngineControlForm::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{
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

 if(ProjectOpenFlag)
 {
  ProjectName="";
  ProjectPath="";
 }
 ProjectOpenFlag=false;
 UpdateInterface();
}

// Открывает проект
void TUEngineControlForm::OpenProject(const String &FileName)
{
 CloseProject();
// ProjectSerialize.SetXml(&ProjectXml);
 ProjectXml.LoadFromFile(AnsiString(OpenDialog->FileName).c_str(),"");
 ProjectPath=ExtractFilePath(OpenDialog->FileName);
 ProjectName=ExtractFileName(OpenDialog->FileName);

 // Флаг автоматического сохранения проекта
 ProjectXml.SelectNodeRoot("Project/General");
 ProjectAutoSaveFlag=ProjectXml.ReadInteger("ProjectAutoSaveFlag",1);
 PredefinedStructure=ProjectXml.ReadInteger("PredefinedStructure",0);
 String modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();

 EngineInit(PredefinedStructure,ExceptionHandler);

 if(modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
 }

 ProjectXml.SelectNodeRoot(string("Project/Interfaces/"));
 RDK::UIVisualControllerStorage::LoadParameters(ProjectXml);

 UpdateInterface();
 ProjectOpenFlag=true;
}

// Сохраняет проект
void TUEngineControlForm::SaveProject(void)
{
 if(!ProjectOpenFlag)
  return;

 ProjectXml.SelectNodeRoot(string("Project/Interfaces/"));
 RDK::UIVisualControllerStorage::SaveParameters(ProjectXml);

 ProjectXml.SelectNodeRoot("Project/General");

 String modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();
 if(modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(modelfilename);
 }

 ProjectXml.WriteInteger("PredefinedStructure",PredefinedStructure);
 ProjectXml.WriteInteger("ProjectAutoSaveFlag",ProjectAutoSaveFlag);
 ProjectXml.SaveToFile(AnsiString(ProjectPath+ProjectName).c_str());
}


void __fastcall TUEngineControlForm::FormShow(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------




void __fastcall TUEngineControlForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 SaveProjectItemClick(Sender);
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
void __fastcall TUEngineControlForm::WatchWindow1Click(TObject *Sender)
{
 UWatchForm->Show();
}
//---------------------------------------------------------------------------

