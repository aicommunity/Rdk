//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsBmpSequenceFormUnit.h"
#include "TVideoCaptureOptionsFormUnit.h"
#include "TVideoSourceThread.h"
#include "VideoOutputFrameUnit.h"
#include <Vcl.FileCtrl.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsBmpSequenceForm *VideoCaptureOptionsBmpSequenceForm;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsBmpSequenceForm::TVideoCaptureOptionsBmpSequenceForm(TComponent* Owner)
	: TVideoCaptureOptionsInterface(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
TVideoCaptureOptionsBmpSequenceForm* TVideoCaptureOptionsBmpSequenceForm::New(TComponent *owner)
{
 return new TVideoCaptureOptionsBmpSequenceForm(owner);
}

/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsBmpSequenceForm::ReadParametersToGui(RDK::USerStorageXML &xml)
{
 ImageSequencePathEdit->Text=xml.ReadString("PathName",AnsiString(ImageSequencePathEdit->Text).c_str()).c_str();
 RepeatSequenceCheckBox->Checked=xml.ReadBool("RepeatFlag",RepeatSequenceCheckBox->Checked);
 try
 {
  ImageSequenceFpsLabeledEdit->Text=xml.ReadFloat("Fps",StrToFloat(ImageSequenceFpsLabeledEdit->Text));
 }
 catch(EConvertError &exception)
 {
//  return false;
 }
 return true;
}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsBmpSequenceForm::WriteParametersToXml(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");
 xml.WriteString("PathName",AnsiString(ImageSequencePathEdit->Text).c_str());
 xml.WriteBool("RepeatFlag",RepeatSequenceCheckBox->Checked);
 try
 {
  xml.WriteFloat("Fps",StrToFloat(ImageSequenceFpsLabeledEdit->Text));
 }
 catch(EConvertError &exception)
 {
//  return false;
 }

 return true;
}
/// -------------------------------------
void __fastcall TVideoCaptureOptionsBmpSequenceForm::FormCreate(TObject *Sender)
{
 VideoSourceType=4;
 TVideoCaptureOptionsDesciption descr;
 descr.Form=this;
 descr.Name="Bmp Sequence";
 descr.Position=4;
 VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,descr);
// VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,this);
 if(!VideoCaptureOptionsForm->CheckVideoSourcePrototypes(VideoSourceType))
  VideoCaptureOptionsForm->AddVideoSourcePrototypes(VideoSourceType,new TVideoCaptureThreadBmpSequence(0,true));
}
//---------------------------------------------------------------------------
void __fastcall TVideoCaptureOptionsBmpSequenceForm::ImageSequencePathBrowseButtonClick(TObject *Sender)

{
 String chosenDir="";//ExtractFilePath(Application->ExeName);

 if(SelectDirectory("Select image sequence directory", ExtractFilePath(Application->ExeName), chosenDir,TSelectDirExtOpts() << sdNewFolder << sdNewUI))
 {
  ImageSequencePathEdit->Text=chosenDir;
 }

// SelectMode(4);
/* double fps=25.0;
 std::string s_fps=AnsiString(ImageSequenceFpsLabeledEdit->Text).c_str();
 std::string::size_type i=s_fps.find_first_of(",");
 if(i != std::string::npos)
  s_fps[i]='.';

 fps=RDK::atof(s_fps);
  */
// VideoOutputFrame->InitByImageSequence(ImageSequencePathEdit->Text,fps);

// UpdateInterface();
}
//---------------------------------------------------------------------------
