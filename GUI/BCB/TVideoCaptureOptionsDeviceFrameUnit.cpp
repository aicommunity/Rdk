//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsDeviceFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsDeviceFrame *VideoCaptureOptionsDeviceFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsDeviceFrame::TVideoCaptureOptionsDeviceFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsDeviceFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsDeviceFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------
