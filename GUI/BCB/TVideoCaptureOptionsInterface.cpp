//---------------------------------------------------------------------------

#pragma hdrstop

#include "TVideoCaptureOptionsInterface.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

__fastcall TVideoCaptureOptionsInterface::TVideoCaptureOptionsInterface(TComponent* Owner)
 :TUVisualControllerForm (Owner)
{

}

/// ���������� ��� ��������������, �������������� ����� ������
int TVideoCaptureOptionsInterface::GetVideoSourceType(void)
{
 return VideoSourceType;
}


