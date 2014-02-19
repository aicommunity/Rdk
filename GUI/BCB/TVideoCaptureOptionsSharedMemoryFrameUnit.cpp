//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsSharedMemoryFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsSharedMemoryFrame *VideoCaptureOptionsSharedMemoryFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsSharedMemoryFrame::TVideoCaptureOptionsSharedMemoryFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// Методы загрузки/сохранения параметров
/// -------------------------------------
/// Считывает параметры в поля интерфейса
bool TVideoCaptureOptionsSharedMemoryFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// Записывает параметры из полей интерфейса в xml
bool TVideoCaptureOptionsSharedMemoryFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// ------------------------------------