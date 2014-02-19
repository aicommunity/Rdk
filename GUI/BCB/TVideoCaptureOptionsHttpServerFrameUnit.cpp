//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsHttpServerFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsHttpServerFrame *VideoCaptureOptionsHttpServerFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsHttpServerFrame::TVideoCaptureOptionsHttpServerFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsHttpServerFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsHttpServerFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------