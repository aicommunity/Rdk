//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include <Vcl.FileCtrl.hpp>
#include "UGEngineControlFormUnit.h"
#include "TVideoGrabberControlFormUnit.h"
#include "UImagesFormUnit.h"
#include "UComponentsControlFormUnit.h"
#include "UComponentLinksFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "VideoOutputFormUnit.h"
#include "UClassesListFormUnit.h"
#include "UComponentsPerformanceFormUnit.h"
#include "UFavoriteComponentInfoFormUnit.h"
#include "UDrawEngineFormUnit.h"
#include "UCreateProjectWizardFormUnit.h"
#include "TUFileSystem.h"
#include "rdk_initdll.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFormUnit"
#pragma resource "*.dfm"
TUGEngineControlForm *UGEngineControlForm;
//---------------------------------------------------------------------------
__fastcall TUGEngineControlForm::TUGEngineControlForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 ProjectAutoSaveFlag=true;

 // Признак наличия открытого проекта
 ProjectOpenFlag=false;
}
//---------------------------------------------------------------------------
// Метод, вызываемый перед шагом расчета
void TUGEngineControlForm::ABeforeCalculate(void)
{
 for(int i=0;i<Env_GetNumInputImages();i++)
 {
  if(!VideoOutputForm->GetVideoOutputFrame(i))
   continue;
  RDK::UBitmap &source=VideoOutputForm->GetVideoOutputFrame(i)->BmpSource;

  if(source.GetByteLength()>0)
  {
   Env_SetInputRes(i, InputEnvImageWidth, InputEnvImageHeight);
   Env_SetInputImage(i,(unsigned char*)source.GetData(),source.GetWidth(), source.GetHeight(),source.GetColorModel());
  }
 }

}

// Метод, вызываемый после шага расчета
void TUGEngineControlForm::AAfterCalculate(void)
{
}

void TUGEngineControlForm::AUpdateInterface(void)
{
 CaptureVideo1->Caption=String("Capture Video (")+IntToStr(VideoOutputForm->GetActiveSource())+")";
 OpenVideo1->Caption=String("Open Video File (")+IntToStr(VideoOutputForm->GetActiveSource())+")";
 OpenImage1->Caption=String("Open Image (")+IntToStr(VideoOutputForm->GetActiveSource())+")";
 ToolButton6->Caption=CaptureVideo1->Caption;
 ToolButton8->Caption=OpenVideo1->Caption;
 ToolButton9->Caption=OpenImage1->Caption;

 Caption="Engine Control";
 if(ProjectOpenFlag)
 {
  Caption=Caption+String(" [")+ProjectFileName+"]";
 }
 StatusBar->SimpleText=UEngineMonitorForm->EngineMonitorFrame->StatusBar->SimpleText;
 StatusBar->Repaint();
 StatusBar->Update();
}

void __fastcall TUGEngineControlForm::FormShow(TObject *Sender)
{
 GraphicalEngineInit(0,1,1,320,240,1,ExceptionHandler);
 UImagesForm->ImagesFrame->SetReflectionXFlag(true);
 UpdateInterface();
}

// Создает новый проект
void TUGEngineControlForm::CreateProject(const String &FileName, const String &model_comp_name, const String &model_file_name)
{
 CloseProject();
 ProjectXml.Destroy();
 ProjectXml.SelectNodeRoot("Project/General");
 ProjectXml.WriteInteger("PredefinedStructure",PredefinedStructure);
 ProjectXml.WriteInteger("ProjectAutoSaveFlag",ProjectAutoSaveFlag);

 // Число входов среды
 ProjectXml.WriteInteger("NumEnvInputs",NumEnvInputs);

 // Число выходов среды
 ProjectXml.WriteInteger("NumEnvOutputs",NumEnvOutputs);

 ProjectXml.WriteInteger("InputEnvImageWidth",InputEnvImageWidth);
 ProjectXml.WriteInteger("InputEnvImageHeight",InputEnvImageHeight);

 // Шаг счета по умолчанию
 ProjectXml.WriteInteger("DefaultTimeStep",DefaultTimeStep);

 // Глобальный шаг счета модели
 ProjectXml.WriteInteger("GlobalTimeStep",GlobalTimeStep);

 ProjectXml.WriteBool("ReflectionFlag",ReflectionFlag);

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
void TUGEngineControlForm::CloseProject(void)
{
 if(ProjectAutoSaveFlag)
  SaveProject();

 if(ProjectOpenFlag)
 {
  ProjectFileName="";
  ProjectPath="";
 }
 ProjectOpenFlag=false;
 EngineUnInit();
// UpdateInterface();
}

// Открывает проект
void TUGEngineControlForm::OpenProject(const String &FileName)
{
 CloseProject();

 ProjectXml.LoadFromFile(AnsiString(FileName).c_str(),"");
 ProjectPath=ExtractFilePath(FileName);
 ProjectFileName=ExtractFileName(FileName);

 ProjectXml.SelectNodeRoot("Project/General");

 ProjectName=ProjectXml.ReadString("ProjectName","NoName").c_str();

 // Число входов среды
 NumEnvInputs=ProjectXml.ReadInteger("NumEnvInputs",1);

 // Число выходов среды
 NumEnvOutputs=ProjectXml.ReadInteger("NumEnvOutputs",1);

 InputEnvImageWidth=ProjectXml.ReadInteger("InputEnvImageWidth",360);
 InputEnvImageHeight=ProjectXml.ReadInteger("InputEnvImageHeight",240);

 PredefinedStructure=ProjectXml.ReadInteger("PredefinedStructure",0);

 // Флаг автоматического сохранения проекта
 ProjectAutoSaveFlag=ProjectXml.ReadInteger("ProjectAutoSaveFlag",1);

 // Флаг автоматического сохранения проекта
 ProjectAutoSaveStateFlag=ProjectXml.ReadInteger("ProjectAutoSaveStateFlag",1);

 // Шаг счета по умолчанию
 DefaultTimeStep=ProjectXml.ReadInteger("DefaultTimeStep",30);

 // Глобальный шаг счета модели
 GlobalTimeStep=ProjectXml.ReadInteger("GlobalTimeStep",30);

 ReflectionFlag=ProjectXml.ReadBool("ReflectionFlag",true);

 CalculationMode=ProjectXml.ReadInteger("CalculationMode",0);

 String descriptionfilename=ProjectXml.ReadString("ProjectDescriptionFileName","").c_str();

 if(descriptionfilename.Length() != 0)
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

 String modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();

 GraphicalEngineInit(PredefinedStructure,NumEnvInputs,NumEnvOutputs,InputEnvImageWidth, InputEnvImageHeight ,ReflectionFlag,ExceptionHandler);
 Model_SetDefaultTimeStep(DefaultTimeStep);

 for(int i=0;i<NumEnvInputs;i++)
  VideoOutputForm->AddSource();

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
 RDK::UIVisualControllerStorage::LoadParameters(InterfaceXml);

 UEngineMonitorForm->EngineMonitorFrame->SetCalculateMode(CalculationMode);
 RDK::UIVisualControllerStorage::UpdateInterface();
 ProjectOpenFlag=true;
}

// Сохраняет проект
void TUGEngineControlForm::SaveProject(void)
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

 // Число входов среды
 ProjectXml.WriteInteger("NumEnvInputs",NumEnvInputs);

 // Число выходов среды
 ProjectXml.WriteInteger("NumEnvOutputs",NumEnvOutputs);

 ProjectXml.WriteInteger("InputEnvImageWidth",InputEnvImageWidth);
 ProjectXml.WriteInteger("InputEnvImageHeight",InputEnvImageHeight);

 // Шаг счета по умолчанию
 ProjectXml.WriteInteger("DefaultTimeStep",DefaultTimeStep);

 // Глобальный шаг счета модели
 ProjectXml.WriteInteger("GlobalTimeStep",GlobalTimeStep);

 ProjectXml.WriteBool("ReflectionFlag",ReflectionFlag);

 ProjectXml.WriteInteger("CalculationMode",CalculationMode);

 ProjectXml.WriteString("ProjectName",AnsiString(ProjectName).c_str());

 ProjectXml.SaveToFile(AnsiString(ProjectPath+ProjectFileName).c_str());
}
//---------------------------------------------------------------------------
void __fastcall TUGEngineControlForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 CloseProject();
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
 if(!VideoOutputForm->GetActiveVideoOutputFrame())
  return;

 VideoOutputForm->GetActiveVideoOutputFrame()->MyVideoGrabberControlForm->VideoGrabberControlFrame->OpenImageFileButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::OpenVideo1Click(TObject *Sender)
{
 if(!VideoOutputForm->GetActiveVideoOutputFrame())
  return;

 VideoOutputForm->GetActiveVideoOutputFrame()->MyVideoGrabberControlForm->VideoGrabberControlFrame->VFBrowseButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::CaptureVideo1Click(TObject *Sender)
{
 if(!VideoOutputForm->GetActiveVideoOutputFrame())
  return;

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

void __fastcall TUGEngineControlForm::CreateProjectItemClick(TObject *Sender)
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

  NumEnvInputs=StrToInt(UCreateProjectWizardForm->NumInputsLabeledEdit->Text);
  NumEnvOutputs=StrToInt(UCreateProjectWizardForm->NumOutputsLabeledEdit->Text);
  InputEnvImageWidth=StrToInt(UCreateProjectWizardForm->ImageWidthLabeledEdit->Text);
  InputEnvImageHeight=StrToInt(UCreateProjectWizardForm->ImageHeightLabeledEdit->Text);
  ReflectionFlag=UCreateProjectWizardForm->UpendInputImageCheckBox->Checked;
  CalculationMode=UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex;

  CreateProject(UCreateProjectWizardForm->ProjectDirectoryLabeledEdit->Text+String("\\Project.ini"),UCreateProjectWizardForm->UClassesListFrame1->GetSelectedName(),UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text);

  ProjectName=UCreateProjectWizardForm->ProjectNameLabeledEdit->Text;
  ProjectDescription=UCreateProjectWizardForm->ProjectDescriptionRichEdit->Text;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::CreateModelClick(TObject *Sender)
{
 if(UClassesListForm->ShowModal() != mrOk)
  return;

 Model_Destroy();
 Model_Create(AnsiString(UClassesListForm->ClassesListFrame->GetSelectedName()).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::FavoriteInformation1Click(TObject *Sender)
{
 UFavoriteComponentInfoForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::DrawEngine1Click(TObject *Sender)
{
 UDrawEngineForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::ReloadParameters1Click(TObject *Sender)
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



void __fastcall TUGEngineControlForm::CopyProject1Click(TObject *Sender)
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

void __fastcall TUGEngineControlForm::ProjectOptions1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 UCreateProjectWizardForm->ProjectDirectoryLabeledEdit->Text=ProjectPath;
 UCreateProjectWizardForm->ProjectNameLabeledEdit->Text=ProjectName;
 UCreateProjectWizardForm->ProjectDescriptionRichEdit->Text=ProjectDescription;
 UCreateProjectWizardForm->ProjectTypeRadioGroup->ItemIndex=1;
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

  UCreateProjectWizardForm->NumInputsLabeledEdit->Text=IntToStr(NumEnvInputs);
  UCreateProjectWizardForm->NumOutputsLabeledEdit->Text=IntToStr(NumEnvOutputs);
  UCreateProjectWizardForm->ImageWidthLabeledEdit->Text=IntToStr(InputEnvImageWidth);
  UCreateProjectWizardForm->ImageHeightLabeledEdit->Text=IntToStr(InputEnvImageHeight);
  UCreateProjectWizardForm->UpendInputImageCheckBox->Checked=ReflectionFlag;

 UCreateProjectWizardForm->Caption="Update Project Wizard";
 if(UCreateProjectWizardForm->ShowModal() == mrOk)
 {
  CloseProject();
  PredefinedStructure=UCreateProjectWizardForm->PredefinedStructure;
  ProjectAutoSaveFlag=UCreateProjectWizardForm->ProjectAutoSaveFlagCheckBox->Checked;
  DefaultTimeStep=StrToInt(UCreateProjectWizardForm->ProjectTimeStepEdit->Text);
  GlobalTimeStep=DefaultTimeStep;
  CalculationMode=UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex;

  CreateProject(UCreateProjectWizardForm->ProjectDirectoryLabeledEdit->Text+String("\\Project.ini"),UCreateProjectWizardForm->UClassesListFrame1->GetSelectedName(),UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text);

  ProjectName=UCreateProjectWizardForm->ProjectNameLabeledEdit->Text;
  ProjectDescription=UCreateProjectWizardForm->ProjectDescriptionRichEdit->Text;
 }
}
//---------------------------------------------------------------------------

