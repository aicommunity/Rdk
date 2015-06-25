//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsVideoFileFormUnit.h"
#include "TVideoCaptureOptionsFormUnit.h"
#include "TVideoSourceThread.h"
#include "VideoOutputFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsVideoFileForm *VideoCaptureOptionsVideoFileForm;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsVideoFileForm::TVideoCaptureOptionsVideoFileForm(TComponent* Owner)
	: TVideoCaptureOptionsInterface(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
TVideoCaptureOptionsVideoFileForm* TVideoCaptureOptionsVideoFileForm::New(TComponent *owner)
{
 return new TVideoCaptureOptionsVideoFileForm(owner);
}

/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsVideoFileForm::ReadParametersToGui(RDK::USerStorageXML &xml)
{
 VFNameEdit->Text=xml.ReadString("FileName", AnsiString(VFNameEdit->Text).c_str()).c_str();
 ProcessAllFramesVideoCheckBox->Checked=xml.ReadBool("ProcessAllFramesFlag",ProcessAllFramesVideoCheckBox->Checked);
 FpsLabeledEdit->Text=xml.ReadString("Fps", "0").c_str();
 RepeatVideoCheckBox->Checked=xml.ReadBool("RepeatFlag", false);

 DesiredWidthLabeledEdit->Text=xml.ReadString("DesiredWidth",AnsiString(DesiredWidthLabeledEdit->Text).c_str()).c_str();
 DesiredHeightLabeledEdit->Text=xml.ReadString("DesiredHeight",AnsiString(DesiredHeightLabeledEdit->Text).c_str()).c_str();
 DesiredResFlagCheckBox->Checked=xml.ReadBool("DesiredResolutionFlag",false);

 return true;
}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsVideoFileForm::WriteParametersToXml(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");
 xml.WriteString("FileName", AnsiString(VFNameEdit->Text).c_str());
 xml.WriteBool("ProcessAllFramesFlag",ProcessAllFramesVideoCheckBox->Checked);
 xml.WriteString("Fps", AnsiString(FpsLabeledEdit->Text).c_str());
 xml.WriteBool("RepeatFlag", RepeatVideoCheckBox->Checked);

 xml.WriteString("DesiredWidth",AnsiString(DesiredWidthLabeledEdit->Text).c_str());
 xml.WriteString("DesiredHeight",AnsiString(DesiredHeightLabeledEdit->Text).c_str());
 xml.WriteBool("DesiredResolutionFlag",DesiredResFlagCheckBox->Checked);

 return true;
}
/// -------------------------------------
void __fastcall TVideoCaptureOptionsVideoFileForm::FormCreate(TObject *Sender)
{
 VideoSourceType=1;
 TVideoCaptureOptionsDesciption descr;
 descr.Form=this;
 descr.Name="Video File";
 descr.Position=2;
 VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,descr);
 if(!VideoCaptureOptionsForm->CheckVideoSourcePrototypes(VideoSourceType))
 {
  TVideoCaptureThreadNewVideoGrabber* tvg=new TVideoCaptureThreadNewVideoGrabber(0,true);
  tvg->SetSourceMode(1);
  VideoCaptureOptionsForm->AddVideoSourcePrototypes(VideoSourceType,tvg);
 }

//  VideoCaptureOptionsForm->AddVideoSourcePrototypes(VideoSourceType,new TVideoCaptureThreadVideoGrabberAvi(0,true));
}
//---------------------------------------------------------------------------
void __fastcall TVideoCaptureOptionsVideoFileForm::VFBrowseButtonClick(TObject *Sender)

{
 if(!VideoOpenDialog->Execute())
  return;

// String FileName;
/* if(UGEngineControlForm->ProjectPath == ExtractFilePath(VideoOpenDialog->FileName) || VideoTruncPathCheckBox->Checked == true)
 {
  FileName=ExtractFileName(VideoOpenDialog->FileName);
  VFNameEdit->Text=FileName;
//  SelectMode(1);
//  VideoOutputFrame->InitByAvi(UGEngineControlForm->ProjectPath+FileName);
 }
 else
 {*/
  VFNameEdit->Text=VideoOpenDialog->FileName;
//  SelectMode(1);
//  VideoOutputFrame->InitByAvi(FileName);
// }
// UpdateInterface();
}
//---------------------------------------------------------------------------
