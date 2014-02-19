//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsInterfaceH
#define TVideoCaptureOptionsInterfaceH

#include "myrdk.h"

class TVideoCaptureOptionsInterface
{
public:
/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ��������� ��������� � ���� ����������
virtual bool LoadParamters(RDK::USerStorageXML &xml)=0;

/// ���������� ��������� �� ����� ���������� � xml
virtual bool SaveParamters(RDK::USerStorageXML &xml)=0;
/// -------------------------------------

};
#endif
