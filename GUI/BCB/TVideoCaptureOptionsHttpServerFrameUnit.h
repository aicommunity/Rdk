//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsHttpServerFrameUnitH
#define TVideoCaptureOptionsHttpServerFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TVideoCaptureOptionsInterface.h"
//---------------------------------------------------------------------------
class TVideoCaptureOptionsHttpServerFrame : public TFrame, public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TEdit *ListerPortEdit;
	TLabel *Label10;
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsHttpServerFrame(TComponent* Owner);


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
extern PACKAGE TVideoCaptureOptionsHttpServerFrame *VideoCaptureOptionsHttpServerFrame;
//---------------------------------------------------------------------------
#endif
