//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsVideoFileFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsVideoFileFrame *VideoCaptureOptionsVideoFileFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsVideoFileFrame::TVideoCaptureOptionsVideoFileFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsVideoFileFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsVideoFileFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------