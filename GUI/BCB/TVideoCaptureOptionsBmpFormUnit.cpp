//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsBmpFormUnit.h"
#include "TVideoCaptureOptionsFormUnit.h"
#include "TVideoSourceThread.h"
#include "VideoOutputFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsBmpForm *VideoCaptureOptionsBmpForm;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsBmpForm::TVideoCaptureOptionsBmpForm(TComponent* Owner)
	: TVideoCaptureOptionsInterface(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
TVideoCaptureOptionsBmpForm* TVideoCaptureOptionsBmpForm::New(TComponent *owner)
{
 return new TVideoCaptureOptionsBmpForm(owner);
}

/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsBmpForm::ReadParametersToGui(RDK::USerStorageXML &xml)
{
 ImageFileNameEdit->Text=xml.ReadString("FileName",AnsiString(ImageFileNameEdit->Text).c_str()).c_str();
 try
 {
  PictureFileFpsLabeledEdit->Text=xml.ReadFloat("Fps",StrToFloat(PictureFileFpsLabeledEdit->Text));
 }
 catch(EConvertError &exception)
 {
 }

 return true;
}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsBmpForm::WriteParametersToXml(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");
 xml.WriteString("FileName",AnsiString(ImageFileNameEdit->Text).c_str());
 try
 {
  xml.WriteFloat("Fps",StrToFloat(PictureFileFpsLabeledEdit->Text));
 }
 catch(EConvertError &exception)
 {
 }

 return true;
}
/// -------------------------------------
void __fastcall TVideoCaptureOptionsBmpForm::FormCreate(TObject *Sender)
{
 VideoSourceType=0;
 TVideoCaptureOptionsDesciption descr;

 descr.Form=this;
 descr.Name="Picture File";
 descr.Position=3;
 VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,descr);
 VideoCaptureOptionsForm->AddVideoSourcePrototypes(VideoSourceType,new TVideoCaptureThreadBmp(0,true));
}
//---------------------------------------------------------------------------
void __fastcall TVideoCaptureOptionsBmpForm::OpenImageFileButtonClick(TObject *Sender)

{
 if(!PicturesOpenDialog->Execute())
  return;

/* double fps=25.0;
 std::string s_fps=AnsiString(PictureFileFpsLabeledEdit->Text).c_str();
 std::string::size_type i=s_fps.find_first_of(",");
 if(i != std::string::npos)
  s_fps[i]='.';

 fps=RDK::atof(s_fps);*/
/*
 String FileName;
 if(UGEngineControlForm->ProjectPath == ExtractFilePath(PicturesOpenDialog->FileName) || PictureTruncPathCheckBox->Checked == true)
 {
  FileName=ExtractFileName(PicturesOpenDialog->FileName);
  ImageFileNameEdit->Text=FileName;
  SelectMode(2);
  VideoOutputFrame->InitByBmp(UGEngineControlForm->ProjectPath+FileName,fps);
 }
 else*/
 {
  ImageFileNameEdit->Text=PicturesOpenDialog->FileName;
//  SelectMode(2);
//  VideoOutputFrame->InitByBmp(FileName,fps);
 }

// UpdateInterface();
}
//---------------------------------------------------------------------------
