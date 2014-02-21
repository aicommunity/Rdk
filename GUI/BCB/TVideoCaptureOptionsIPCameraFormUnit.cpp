//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsIPCameraFormUnit.h"
#include "TVideoCaptureOptionsFormUnit.h"
#include "TVideoSourceThread.h"
#include "VideoOutputFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsIPCameraForm *VideoCaptureOptionsIPCameraForm;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsIPCameraForm::TVideoCaptureOptionsIPCameraForm(TComponent* Owner)
	: TVideoCaptureOptionsInterface(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ������� ����� ������� ����� ������
TVideoCaptureOptionsIPCameraForm* TVideoCaptureOptionsIPCameraForm::New(TComponent *owner)
{
 return new TVideoCaptureOptionsIPCameraForm(owner);
}

/// ��������� ��������� � ���� ����������
bool TVideoCaptureOptionsIPCameraForm::ReadParametersToGui(RDK::USerStorageXML &xml)
{
 IPCameraUrlEdit->Text=xml.ReadString("Url",AnsiString(IPCameraUrlEdit->Text).c_str()).c_str();
 IPCameraUserNameEdit->Text=xml.ReadString("UserName", AnsiString(IPCameraUserNameEdit->Text).c_str()).c_str();
 IPCameraUserPasswordEdit->Text=xml.ReadString("Password", AnsiString(IPCameraUserPasswordEdit->Text).c_str()).c_str();
 return true;
}

/// ���������� ��������� �� ����� ���������� � xml
bool TVideoCaptureOptionsIPCameraForm::WriteParametersToXml(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");
 xml.WriteString("Url",AnsiString(IPCameraUrlEdit->Text).c_str());
 xml.WriteString("UserName", AnsiString(IPCameraUserNameEdit->Text).c_str());
 xml.WriteString("Password", AnsiString(IPCameraUserPasswordEdit->Text).c_str());
 return true;
}
/// -------------------------------------
void __fastcall TVideoCaptureOptionsIPCameraForm::FormCreate(TObject *Sender)
{
 VideoSourceType=3;
 TVideoCaptureOptionsDesciption descr;
 descr.Form=this;
 descr.Name="IP Camera";
 descr.Position=1;
 VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,descr);
// VideoCaptureOptionsForm->AddVideoSourceOptionsFrame(VideoSourceType,this);
 VideoCaptureOptionsForm->AddVideoSourcePrototypes(VideoSourceType,new TVideoCaptureThreadVideoGrabberIpCamera(0,true));
}
//---------------------------------------------------------------------------
