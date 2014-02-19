//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsSharedMemoryFrameUnitH
#define TVideoCaptureOptionsSharedMemoryFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TVideoCaptureOptionsInterface.h"
//---------------------------------------------------------------------------
class TVideoCaptureOptionsSharedMemoryFrame : public TFrame, public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TLabel *Label11;
	TLabel *Label12;
	TEdit *PipeIndexEdit;
	TEdit *PipeUidEdit;
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsSharedMemoryFrame(TComponent* Owner);

/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ��������� ��������� � ���� ����������
virtual bool LoadParamters(RDK::USerStorageXML &xml);

/// ���������� ��������� �� ����� ���������� � xml
virtual bool SaveParamters(RDK::USerStorageXML &xml);
/// -------------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsSharedMemoryFrame *VideoCaptureOptionsSharedMemoryFrame;
//---------------------------------------------------------------------------
#endif
