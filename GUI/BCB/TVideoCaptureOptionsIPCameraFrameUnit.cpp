//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsIPCameraFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsIPCameraFrame *VideoCaptureOptionsIPCameraFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsIPCameraFrame::TVideoCaptureOptionsIPCameraFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsIPCameraFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsIPCameraFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------