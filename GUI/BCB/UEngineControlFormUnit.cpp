//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include <Vcl.FileCtrl.hpp>
#include <System.IOUtils.hpp>
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
#include "TUFileSystem.h"

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
  Caption=Caption+String(" [")+ProjectFileName+"]";
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

 if(PredefinedStructure == 0 && model_file_name.Length()>0)
 {
  ProjectXml.WriteString("ModelFileName",AnsiString(model_file_name).c_str());
 }

 ProjectXml.WriteString("InterfaceFileName","Interface.xml");
 ProjectXml.WriteString("ParametersFileName","Parameters.xml");
 ProjectXml.WriteString("StatesFileName","States.xml");

 ProjectXml.SaveToFile(AnsiString(FileName).c_str());
 OpenProject(FileName);

 if(ProjectOpenFlag)
 {
  if(PredefinedStructure == 0 && model_comp_name.Length()>0)
  {
   Model_Create(AnsiString(model_comp_name).c_str());
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
  ProjectFileName="";
  ProjectPath="";
 }
 ProjectOpenFlag=false;
// UpdateInterface();
}

// Открывает проект
void TUEngineControlForm::OpenProject(const String &FileName)
{
 CloseProject();
// ProjectSerialize.SetXml(&ProjectXml);
 ProjectXml.LoadFromFile(AnsiString(FileName).c_str(),"");
 ProjectPath=ExtractFilePath(FileName);
 ProjectFileName=ExtractFileName(FileName);

 // Флаг автоматического сохранения проекта
 ProjectXml.SelectNodeRoot("Project/General");

 ProjectName=ProjectXml.ReadString("ProjectName","NoName").c_str();

 ProjectAutoSaveFlag=ProjectXml.ReadInteger("ProjectAutoSaveFlag",1);

 // Флаг автоматического сохранения проекта
 ProjectAutoSaveStateFlag=ProjectXml.ReadInteger("ProjectAutoSaveStateFlag",0);

 PredefinedStructure=ProjectXml.ReadInteger("PredefinedStructure",0);
 String modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();

 // Шаг счета по умолчанию
 DefaultTimeStep=ProjectXml.ReadInteger("DefaultTimeStep",2000);

 // Глобальный шаг счета модели
 GlobalTimeStep=ProjectXml.ReadInteger("GlobalTimeStep",2000);

 CalculationMode=ProjectXml.ReadInteger("CalculationMode",0);

 String descriptionfilename=ProjectXml.ReadString("ProjectDescriptionFileName","").c_str();

 if(descriptionfilename.Length() != 0 && FileExists(descriptionfilename))
 {
  TRichEdit* RichEdit=new TRichEdit(this);
  RichEdit->Parent=this;
  RichEdit->PlainText=true;
  RichEdit->Visible=false;

  if(ExtractFilePath(descriptionfilename).Length() == 0)
   RichEdit->Lines->LoadFromFile(ProjectPath+descriptionfilename);
  else
   RichEdit->Lines->LoadFromFile(descriptionfilename);
  ProjectDescription=RichEdit->Text;
  delete RichEdit;
 }

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

 InterfaceXml.Destroy();
 String interfacefilename=ProjectXml.ReadString("InterfaceFileName","").c_str();
 if(interfacefilename.Length() != 0)
 {
  if(ExtractFilePath(interfacefilename.c_str()).Length() == 0)
   InterfaceXml.LoadFromFile(AnsiString(ProjectPath+interfacefilename).c_str(),"Interfaces");
  else
   InterfaceXml.LoadFromFile(AnsiString(interfacefilename).c_str(),"Interfaces");

  InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 }
 else
  InterfaceXml.Create(std::string("Interfaces"));

 RDK::UIVisualControllerStorage::LoadParameters(InterfaceXml);

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

 String descriptionfilename=ProjectXml.ReadString("ProjectDescriptionFileName","").c_str();
 if(descriptionfilename.Length() == 0)
 {
  descriptionfilename="Description.rtf";
  ProjectXml.WriteString("ProjectDescriptionFileName",AnsiString(descriptionfilename).c_str());
 }

 if(descriptionfilename.Length() != 0)
 {
  TRichEdit* RichEdit=new TRichEdit(this);
  RichEdit->Parent=this;
  RichEdit->PlainText=true;
  RichEdit->Visible=false;
  RichEdit->Text=ProjectDescription;

  if(ExtractFilePath(descriptionfilename).Length() == 0)
   RichEdit->Lines->SaveToFile(ProjectPath+descriptionfilename);
  else
   RichEdit->Lines->SaveToFile(descriptionfilename);
  delete RichEdit;
 }

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

 ProjectXml.WriteString("ProjectName",AnsiString(ProjectName).c_str());

 ProjectXml.SaveToFile(AnsiString(ProjectPath+ProjectFileName).c_str());
}


void __fastcall TUEngineControlForm::FormShow(TObject *Sender)
{
 // Грузим шрифты
 std::vector<std::string> font_names;
 std::string font_path=AnsiString(ExtractFilePath(Application->ExeName)+"Fonts\\").c_str();
 FindFilesList(font_path, "*.fnt", true, font_names);
 if(font_names.empty())
 {
  font_path=AnsiString("..\\..\\Fonts\\").c_str();
  FindFilesList(font_path, "*.fnt", true, font_names);
 }

 RDK::ClearClobalFonts();
 RDK::UBitmapFont font;
 for(size_t i=0;i<font_names.size();i++)
 {
  RDK::AddGlobalFont(font_path+font_names[i]);
 }

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
 UCreateProjectWizardForm->ClearWizard();
 UCreateProjectWizardForm->Caption="Create Project Wizard";
 if(UCreateProjectWizardForm->ShowModal() == mrOk)
 {
  CloseProject();
  PredefinedStructure=UCreateProjectWizardForm->PredefinedStructure;
  ProjectAutoSaveFlag=UCreateProjectWizardForm->ProjectAutoSaveFlagCheckBox->Checked;
  DefaultTimeStep=StrToInt(UCreateProjectWizardForm->ProjectTimeStepEdit->Text);
  GlobalTimeStep=DefaultTimeStep;
  CalculationMode=UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex;

  String model_file_name;
  String comp_name;

  if(UCreateProjectWizardForm->ModelFileNameRadioButton->Checked)
   model_file_name=UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text;

  if(UCreateProjectWizardForm->RootModelComponentNameRadioButton->Checked)
   comp_name=UCreateProjectWizardForm->UClassesListFrame1->GetSelectedName();

  CreateProject(UCreateProjectWizardForm->ProjectDirectoryLabeledEdit->Text+String("\\Project.ini"),comp_name,model_file_name);
  ProjectName=UCreateProjectWizardForm->ProjectNameLabeledEdit->Text;
  ProjectDescription=UCreateProjectWizardForm->ProjectDescriptionRichEdit->Text;
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

void __fastcall TUEngineControlForm::CopyProject1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 const SELDIRHELP = 1000;
 String chosenDir=ExtractFilePath(Application->ExeName);

 if(SelectDirectory("Select project directory", ExtractFilePath(Application->ExeName), chosenDir,TSelectDirExtOpts() << sdNewFolder << sdNewUI))
 {
  SaveProject();
  CopyDir(AnsiString(ProjectPath).c_str(), AnsiString(chosenDir+"\\").c_str(), "*.*");
 }
}
//---------------------------------------------------------------------------

void __fastcall TUEngineControlForm::ProjectOptions1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 UCreateProjectWizardForm->ProjectDirectoryLabeledEdit->Text=ProjectPath;
 UCreateProjectWizardForm->ProjectNameLabeledEdit->Text=ProjectName;
 UCreateProjectWizardForm->ProjectDescriptionRichEdit->Text=ProjectDescription;
 UCreateProjectWizardForm->ProjectTypeRadioGroup->ItemIndex=0;
 UCreateProjectWizardForm->ProjectAutoSaveFlagCheckBox->Checked=ProjectAutoSaveFlag;
 UCreateProjectWizardForm->ProjectTimeStepEdit->Text=IntToStr(DefaultTimeStep);
 UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex=CalculationMode;

 UCreateProjectWizardForm->PredefinedStructure=PredefinedStructure;
 if(PredefinedStructure)
 {
  UCreateProjectWizardForm->PredefinedModelRadioButton->Checked=true;
  UCreateProjectWizardForm->ModelFileNameRadioButton->Checked=false;
 }
 else
 {
  UCreateProjectWizardForm->PredefinedModelRadioButton->Checked=false;
  UCreateProjectWizardForm->ModelFileNameRadioButton->Checked=true;
  UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text=ProjectXml.ReadString("ModelFileName","").c_str();
 }

 UCreateProjectWizardForm->Caption="Update Project Wizard";
 if(UCreateProjectWizardForm->ShowModal() == mrOk)
 {
  CloseProject();
  PredefinedStructure=UCreateProjectWizardForm->PredefinedStructure;
  ProjectAutoSaveFlag=UCreateProjectWizardForm->ProjectAutoSaveFlagCheckBox->Checked;
  DefaultTimeStep=StrToInt(UCreateProjectWizardForm->ProjectTimeStepEdit->Text);
  GlobalTimeStep=DefaultTimeStep;
  CalculationMode=UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex;

  String model_file_name;
  String comp_name;

  if(UCreateProjectWizardForm->ModelFileNameRadioButton->Checked)
   model_file_name=UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text;

  if(UCreateProjectWizardForm->RootModelComponentNameRadioButton->Checked)
   comp_name=UCreateProjectWizardForm->UClassesListFrame1->GetSelectedName();

  CreateProject(UCreateProjectWizardForm->ProjectDirectoryLabeledEdit->Text+String("\\Project.ini"),comp_name,model_file_name);

  ProjectName=UCreateProjectWizardForm->ProjectNameLabeledEdit->Text;
  ProjectDescription=UCreateProjectWizardForm->ProjectDescriptionRichEdit->Text;
 }
}
//---------------------------------------------------------------------------

