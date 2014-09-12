//---------------------------------------------------------------------------

#ifndef TServerOptionsFrameUnitH
#define TServerOptionsFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TServerOptionsFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TLabeledEdit *BindAddressLabeledEdit;
	TLabeledEdit *BindPortLabeledEdit;
private:	// User declarations
public:		// User declarations
	__fastcall TServerOptionsFrame(TComponent* Owner);

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������ ������
virtual void AAfterReset(void);

// ���������� ����������
virtual void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

void ApplyOptions(void);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TServerOptionsFrame *ServerOptionsFrame;
//---------------------------------------------------------------------------
#endif
