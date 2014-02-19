//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsBmpSequenceFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsBmpSequenceFrame *VideoCaptureOptionsBmpSequenceFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsBmpSequenceFrame::TVideoCaptureOptionsBmpSequenceFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsBmpSequenceFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsBmpSequenceFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------