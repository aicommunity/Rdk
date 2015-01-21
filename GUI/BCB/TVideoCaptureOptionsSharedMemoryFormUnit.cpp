//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsSharedMemoryFormUnit.h"
#include "TVideoCaptureOptionsFormUnit.h"
#include "TVideoSourceThread.h"
#include "VideoOutputFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsSharedMemoryForm *VideoCaptureOptionsSharedMemoryForm;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsSharedMemoryForm::TVideoCaptureOptionsSharedMemoryForm(TComponent* Owner)
	: TVideoCaptureOptionsInterface(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Создает копию объекта этого класса
TVideoCaptureOptionsSharedMemoryForm* TVideoCaptureOptionsSharedMemoryForm::New(TComponent *owner)
{
 return new TVideoCaptureOptionsSharedMemoryForm(owner);
}

/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsSharedMemoryForm::ReadParametersToGui(RDK::USerStorageXML &xml)
{
 try
 {
  PipeIndexEdit->Text=IntToStr(xml.ReadInteger("PipeIndex",StrToInt(PipeIndexEdit->Text)));
 }
 catch(EConvertError &exception)
 {
  return false;
 }
 PipeUidEdit->Text=xml.ReadString("PipeName",AnsiString(PipeUidEdit->Text).c_str()).c_str();

 return true;
}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsSharedMemoryForm::WriteParametersToXml(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");
 try
 {
  xml.WriteInteger("PipeIndex",StrToInt(PipeIndexEdit->Text));
 }
 catch(EConvertError &exception)
 {
  return false;
 }
 xml.WriteString("PipeName",AnsiString(PipeUidEdit->Text).c_str());

 return true;
}
/// ------------------------------------
void __fastcall TVideoCaptureOptionsSharedMemoryForm::FormCreate(TObject *Sender)

{
 VideoSourceType=6;
 TVideoCaptureOptionsDesciption descr;
 descr.Form=this;
 descr.Name="Shared Memory";
 descr.Position=6;
 VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,descr);
// VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,this);
 if(!VideoCaptureOptionsForm->CheckVideoSourcePrototypes(VideoSourceType))
  VideoCaptureOptionsForm->AddVideoSourcePrototypes(VideoSourceType,new TVideoCaptureThreadSharedMemory(0,true));
}
//---------------------------------------------------------------------------
