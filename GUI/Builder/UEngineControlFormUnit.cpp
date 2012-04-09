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
}
//---------------------------------------------------------------------------
void TUEngineControlForm::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

 if(UComponentsPerformanceForm->Visible)
  UComponentsPerformanceForm->UComponentsPerformanceFrame->UpdateInterface();

 UpdateInterfaceFlag=false;
}

void __fastcall TUEngineControlForm::FormShow(TObject *Sender)
{
 EngineInit(0,ExceptionHandler);
 UpdateInterface();

 ProjectIni=new TMemIniFile("project.ini");

 String modelfilename=ProjectIni->ReadString("General","ModelFileName","");
 if(modelfilename.Length() != 0)
 {
  UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
 }

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
 if(ProjectIni)
 {
  ProjectIni->UpdateFile();
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

