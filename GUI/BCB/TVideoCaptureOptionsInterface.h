//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsInterfaceH
#define TVideoCaptureOptionsInterfaceH

#include "myrdk.h"
#include "TUVisualControllerFormUnit.h"

class TVideoCaptureOptionsInterface: public TUVisualControllerForm
{
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsInterface(TComponent* Owner);

int VideoSourceType;

public:
/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ���������� ��� ��������������, �������������� ����� ������
int GetVideoSourceType(void);

/// ��������� ��������� � ���� ����������
virtual bool ReadParametersToGui(RDK::USerStorageXML &xml)=0;

/// ���������� ��������� �� ����� ���������� � xml
virtual bool WriteParametersToXml(RDK::USerStorageXML &xml)=0;
/// -------------------------------------

};
#endif
