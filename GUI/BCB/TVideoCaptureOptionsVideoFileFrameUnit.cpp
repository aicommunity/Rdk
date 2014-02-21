//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsVideoFileFrameUnit.h"
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

 return true;
}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsVideoFileForm::WriteParametersToXml(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");
 xml.WriteString("FileName", AnsiString(VFNameEdit->Text).c_str());
 xml.WriteBool("ProcessAllFramesFlag",ProcessAllFramesVideoCheckBox->Checked);

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
// VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,this);
 VideoCaptureOptionsForm->AddVideoSourcePrototypes(VideoSourceType,new TVideoCaptureThreadVideoGrabberAvi(0,true));
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
