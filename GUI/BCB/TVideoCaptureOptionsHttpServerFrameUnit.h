//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsHttpServerFrameUnitH
#define TVideoCaptureOptionsHttpServerFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TVideoCaptureOptionsInterface.h"
#include "TUVisualControllerFormUnit.h"

//---------------------------------------------------------------------------
class TVideoCaptureOptionsHttpServerForm : public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TEdit *ListerPortEdit;
	TLabel *Label10;
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsHttpServerForm(TComponent* Owner);


/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ������� ����� ������� ����� ������
virtual TVideoCaptureOptionsHttpServerForm* New(TComponent *owner);

/// ��������� ��������� � ���� ����������
virtual bool ReadParametersToGui(RDK::USerStorageXML &xml);

/// ���������� ��������� �� ����� ���������� � xml
virtual bool WriteParametersToXml(RDK::USerStorageXML &xml);
/// -------------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsHttpServerForm *VideoCaptureOptionsHttpServerForm;
//---------------------------------------------------------------------------
#endif
