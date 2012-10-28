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
#include "UClassesListFormUnit.h"
#include "UWatchFormUnit.h"
#include "UFavoriteComponentInfoFormUnit.h"
#include "UDrawEngineFormUnit.h"
#include "UCreateProjectWizardFormUnit.h"

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
 StatusBar->Repaint();
 StatusBar->Update();
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
void TUEngineControlForm::CreateProject(const String &FileName, const String &model_comp_name, const String &model_file_name)
{
 CloseProject();
 ProjectXml.Destroy();
 ProjectXml.SelectNodeRoot("Project/General");
 ProjectXml.WriteInteger("PredefinedStructure",PredefinedStructure);
 ProjectXml.WriteInteger("ProjectAutoSaveFlag",ProjectAutoSaveFlag);

 // Шаг счета по умолчанию
 ProjectXml.WriteInteger("DefaultTimeStep",DefaultTimeStep);

 // Глобальный шаг счета модели
 ProjectXml.WriteInteger("GlobalTimeStep",GlobalTimeStep);

 ProjectXml.WriteInteger("CalculationMode",CalculationMode);

 ProjectXml.SaveToFile(AnsiString(FileName).c_str());
 OpenProject(FileName);

 if(ProjectOpenFlag)
 {
  if(PredefinedStructure == 0 && model_comp_name.Length()>0)
  {
   Model_Create(AnsiString(model_comp_name).c_str());
  }

  if(PredefinedStructure == 0 && model_file_name.Length()>0)
  {
	UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(model_file_name);
  }
 }
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
 ProjectXml.LoadFromFile(AnsiString(FileName).c_str(),"");
 ProjectPath=ExtractFilePath(FileName);
 ProjectName=ExtractFileName(FileName);

 // Флаг автоматического сохранения проекта
 ProjectXml.SelectNodeRoot("Project/General");
 ProjectAutoSaveFlag=ProjectXml.ReadInteger("ProjectAutoSaveFlag",1);

 // Флаг автоматического сохранения проекта
 ProjectAutoSaveStateFlag=ProjectXml.ReadInteger("ProjectAutoSaveStateFlag",1);

 PredefinedStructure=ProjectXml.ReadInteger("PredefinedStructure",0);
 String modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();

 // Шаг счета по умолчанию
 DefaultTimeStep=ProjectXml.ReadInteger("DefaultTimeStep",2000);

 // Глобальный шаг счета модели
 GlobalTimeStep=ProjectXml.ReadInteger("GlobalTimeStep",2000);

 CalculationMode=ProjectXml.ReadInteger("CalculationMode",0);

 EngineInit(PredefinedStructure,ExceptionHandler);

 if(PredefinedStructure == 0 && modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
 }

 String paramsfilename=ProjectXml.ReadString("ParametersFileName","").c_str();

 if(paramsfilename.Length() != 0)
 {
  if(ExtractFilePath(paramsfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(ProjectPath+paramsfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(paramsfilename);
 }

 if(ProjectAutoSaveStateFlag)
 {
  String statesfilename=ProjectXml.ReadString("StatesFileName","").c_str();

  if(statesfilename.Length() != 0)
  {
   if(ExtractFilePath(statesfilename).Length() == 0)
    UComponentsControlForm->ComponentsControlFrame->LoadStatesFromFile(ProjectPath+statesfilename);
   else
    UComponentsControlForm->ComponentsControlFrame->LoadStatesFromFile(statesfilename);
  }
 }

 if(Model_Check())
 {
  Model_SetGlobalTimeStep("",GlobalTimeStep);
 }

 String interfacefilename=ProjectXml.ReadString("InterfaceFileName","").c_str();
 if(interfacefilename.Length() != 0)
 {
  if(ExtractFilePath(interfacefilename.c_str()).Length() == 0)
   InterfaceXml.LoadFromFile(AnsiString(ProjectPath+interfacefilename).c_str(),"Interfaces");
  else
   InterfaceXml.LoadFromFile(AnsiString(interfacefilename).c_str(),"Interfaces");

  InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
  RDK::UIVisualControllerStorage::LoadParameters(InterfaceXml);
 }

 UEngineMonitorForm->EngineMonitorFrame->SetCalculateMode(CalculationMode);
 RDK::UIVisualControllerStorage::UpdateInterface();
 ProjectOpenFlag=true;
}

// Сохраняет проект
void TUEngineControlForm::SaveProject(void)
{
 if(!ProjectOpenFlag)
  return;

 InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 RDK::UIVisualControllerStorage::SaveParameters(InterfaceXml);

 String interfacefilename=ProjectXml.ReadString("InterfaceFileName","").c_str();
 if(interfacefilename.Length() != 0)
 {
  if(ExtractFilePath(interfacefilename).Length() == 0)
   InterfaceXml.SaveToFile(AnsiString(ProjectPath+interfacefilename).c_str());
  else
   InterfaceXml.SaveToFile(AnsiString(interfacefilename).c_str());
 }
 else
 {
  ProjectXml.WriteString("InterfaceFileName","Interface.xml");
  InterfaceXml.SaveToFile(AnsiString(ProjectPath+interfacefilename).c_str());
 }

 ProjectXml.SelectNodeRoot("Project/General");

 String modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();
 if(modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(modelfilename);
 }
 else
 {
  ProjectXml.WriteString("ModelFileName","model.xml");
  UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(ProjectPath+"model.xml");
 }


 String paramsfilename=ProjectXml.ReadString("ParametersFileName","").c_str();
 if(paramsfilename.Length() == 0)
 {
  paramsfilename="Parameters.xml";
  ProjectXml.WriteString("ParametersFileName",AnsiString(paramsfilename).c_str());
 }

 if(paramsfilename.Length() != 0)
 {
  if(ExtractFilePath(paramsfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->SaveParametersToFile(ProjectPath+paramsfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->SaveParametersToFile(paramsfilename);
 }

 if(ProjectAutoSaveStateFlag)
 {
  String statesfilename=ProjectXml.ReadString("StatesFileName","").c_str();
  if(statesfilename.Length() == 0)
  {
   statesfilename="States.xml";
   ProjectXml.WriteString("StatesFileName",AnsiString(statesfilename).c_str());
  }

  if(statesfilename.Length() != 0)
  {
   if(ExtractFilePath(statesfilename).Length() == 0)
    UComponentsControlForm->ComponentsControlFrame->SaveStatesToFile(ProjectPath+statesfilename);
   else
    UComponentsControlForm->ComponentsControlFrame->SaveStatesToFile(statesfilename);
  }
 }


 ProjectXml.WriteInteger("PredefinedStructure",PredefinedStructure);
 ProjectXml.WriteInteger("ProjectAutoSaveFlag",ProjectAutoSaveFlag);

 // Флаг автоматического сохранения проекта
 ProjectXml.WriteInteger("ProjectAutoSaveStateFlag",ProjectAutoSaveStateFlag);

 // Шаг счета по умолчанию
 ProjectXml.WriteInteger("DefaultTimeStep",DefaultTimeStep);

 // Глобальный шаг счета модели
 ProjectXml.WriteInteger("GlobalTimeStep",GlobalTimeStep);

 ProjectXml.WriteInteger("CalculationMode",CalculationMode);


 ProjectXml.SaveToFile(AnsiString(ProjectPath+ProjectName).c_str());
}


void __fastcall TUEngineControlForm::FormShow(TObject *Sender)
{
 EngineInit(0,ExceptionHandler);
 UpdateInterface();
}
//---------------------------------------------------------------------------




void __fastcall TUEngineControlForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 CloseProject();
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

void __fastcall TUEngineControlForm::CreateProjectItemClick(TObject *Sender)
{
 if(UCreateProjectWizardForm->ShowModal() == mrOk)
 {
  CloseProject();
  PredefinedStructure=UCreateProjectWizardForm->PredefinedStructure;
  ProjectAutoSaveFlag=UCreateProjectWizardForm->ProjectAutoSaveFlagCheckBox->Checked;
  DefaultTimeStep=StrToInt(UCreateProjectWizardForm->ProjectTimeStepEdit->Text);
  GlobalTimeStep=DefaultTimeStep;
  CalculationMode=UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex;

  CreateProject(UCreateProjectWizardForm->ProjectDirectoryLabeledEdit->Text+String("\\Project.ini"),UCreateProjectWizardForm->UClassesListFrame1->GetSelectedName(),UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text);
 }
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::CreateModelClick(TObject *Sender)
{
 if(UClassesListForm->ShowModal() != mrOk)
  return;

 Model_Destroy();
 Model_Create(AnsiString(UClassesListForm->ClassesListFrame->GetSelectedName()).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::FavoriteInformation1Click(TObject *Sender)
{
 UFavoriteComponentInfoForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::DrawEngine1Click(TObject *Sender)
{
 UDrawEngineForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::ReloadParameters1Click(TObject *Sender)
{
 String paramsfilename=ProjectXml.ReadString("ParametersFileName","").c_str();
 if(paramsfilename.Length() == 0)
 {
  paramsfilename="Parameters.xml";
  ProjectXml.WriteString("ParametersFileName",AnsiString(paramsfilename).c_str());
 }

 if(paramsfilename.Length() != 0)
 {
  if(ExtractFilePath(paramsfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(ProjectPath+paramsfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(paramsfilename);
 }
}
//---------------------------------------------------------------------------

