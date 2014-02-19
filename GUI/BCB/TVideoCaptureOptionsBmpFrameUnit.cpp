//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsBmpFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsBmpFrame *VideoCaptureOptionsBmpFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsBmpFrame::TVideoCaptureOptionsBmpFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsBmpFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsBmpFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------