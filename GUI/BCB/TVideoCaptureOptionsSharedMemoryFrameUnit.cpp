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
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ��������� ��������� � ���� ����������
bool TVideoCaptureOptionsSharedMemoryFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// ���������� ��������� �� ����� ���������� � xml
bool TVideoCaptureOptionsSharedMemoryFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// ------------------------------------