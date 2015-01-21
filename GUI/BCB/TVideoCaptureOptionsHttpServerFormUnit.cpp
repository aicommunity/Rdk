//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsHttpServerFormUnit.h"
#include "TVideoCaptureOptionsFormUnit.h"
#include "TVideoSourceThread.h"
#include "VideoOutputFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsHttpServerForm *VideoCaptureOptionsHttpServerForm;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsHttpServerForm::TVideoCaptureOptionsHttpServerForm(TComponent* Owner)
	: TVideoCaptureOptionsInterface(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
TVideoCaptureOptionsHttpServerForm* TVideoCaptureOptionsHttpServerForm::New(TComponent *owner)
{
 return new TVideoCaptureOptionsHttpServerForm(owner);
}

/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsHttpServerForm::ReadParametersToGui(RDK::USerStorageXML &xml)
{
 try
 {
  xml.WriteInteger("ListenPort",StrToInt(ListerPortEdit->Text));
 }
 catch(EConvertError &exception)
 {
  return false;
 }

 return true;
}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsHttpServerForm::WriteParametersToXml(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");
 try
 {
  ListerPortEdit->Text=IntToStr(xml.ReadInteger("ListenPort",StrToInt(ListerPortEdit->Text)));
 }
 catch(EConvertError &exception)
 {
  return false;
 }

 return true;
}
/// -------------------------------------
void __fastcall TVideoCaptureOptionsHttpServerForm::FormCreate(TObject *Sender)
{
 VideoSourceType=5;
 TVideoCaptureOptionsDesciption descr;
 descr.Form=this;
 descr.Name="Http Server";
 descr.Position=5;
 VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,descr);
// VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,this);
 if(!VideoCaptureOptionsForm->CheckVideoSourcePrototypes(VideoSourceType))
  VideoCaptureOptionsForm->AddVideoSourcePrototypes(VideoSourceType,new TVideoCaptureThreadHttpServer(0,true));
}
//---------------------------------------------------------------------------
