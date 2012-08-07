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
  RDK::UBitmap &source=VideoOutputForm->GetVideoOutputFrame(i)->BmpSource;

  if(source.GetByteLength()>0)
  {
   Env_SetInputRes(i, source.GetWidth(), source.GetHeight());
   Env_SetInputImage(i,(unsigned char*)source.GetData(),source.GetWidth(), source.GetHeight(),source.GetColorModel());
  }
 }

}

// Метод, вызываемый после шага расчета
void TUGEngineControlForm::AAfterCalculate(void)
{
 StatusBar->SimpleText=UEngineMonitorForm->EngineMonitorFrame->StatusBar->SimpleText;
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
  Caption=Caption+String(" [")+ProjectName+"]";
 }
}

void __fastcall TUGEngineControlForm::FormShow(TObject *Sender)
{
 GraphicalEngineInit(0,1,1,320,240,1,ExceptionHandler);
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

 if(ProjectOpenFlag)
 {
  ProjectName="";
  ProjectPath="";
 }
 ProjectOpenFlag=false;
 UpdateInterface();
}

// Открывает проект
void TUGEngineControlForm::OpenProject(const String &FileName)
{
 CloseProject();

 ProjectXml.LoadFromFile(AnsiString(OpenDialog->FileName).c_str(),"");
 ProjectPath=ExtractFilePath(OpenDialog->FileName);
 ProjectName=ExtractFileName(OpenDialog->FileName);

 ProjectXml.SelectNodeRoot("Project/General");
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

 String modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();

 GraphicalEngineInit(PredefinedStructure,NumEnvInputs,NumEnvOutputs,InputEnvImageWidth, InputEnvImageHeight ,1,ExceptionHandler);
 Model_SetDefaultTimeStep(DefaultTimeStep);

 for(int i=0;i<NumEnvInputs;i++)
  VideoOutputForm->AddSource();

 if(modelfilename.Length() != 0)
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

 Model_SetGlobalTimeStep("",GlobalTimeStep);

 ProjectXml.SelectNodeRoot(std::string("Project/Interfaces/"));
 RDK::UIVisualControllerStorage::LoadParameters(ProjectXml);

 UpdateInterface();
 ProjectOpenFlag=true;

/* String modelfilename=ProjectIni->ReadString("General","ModelFileName","");
 if(modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
 }

 Model_SetGlobalTimeStep("",GlobalTimeStep);

// UImagesForm->ImagesFrame->LoadFromIni(ProjectIni,"ImagesFrame");
// UComponentsPerformanceForm->UComponentsPerformanceFrame->LoadFromIni(ProjectIni,"PerformanceFrame");
// VideoOutputForm->LoadFromIni(ProjectIni,"VideoOutputForm");
// UEngineMonitorForm->EngineMonitorFrame->LoadFromIni(ProjectIni,"EngineMonitorForm");
 UpdateInterface();
 */
}

// Сохраняет проект
void TUGEngineControlForm::SaveProject(void)
{
 if(!ProjectOpenFlag)
  return;

 ProjectXml.SelectNodeRoot(std::string("Project/Interfaces/"));
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

 ProjectXml.SaveToFile(AnsiString(ProjectPath+ProjectName).c_str());
}
//---------------------------------------------------------------------------
void __fastcall TUGEngineControlForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 SaveProjectItemClick(Sender);
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





