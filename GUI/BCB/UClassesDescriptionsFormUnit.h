//---------------------------------------------------------------------------

#ifndef UClassesDescriptionsFormUnitH
#define UClassesDescriptionsFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFrameUnit.h"
#include "UClassesDescriptionsFrameUnit.h"
#include "TUVisualControllerFormUnit.h"
//---------------------------------------------------------------------------
class TUClassesDescriptionsForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TUClassesDescriptionsFrame *ClassesDescriptionsFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TUClassesDescriptionsForm(TComponent* Owner);

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������ ������
virtual void AAfterReset(void);

// ���������� ����������
virtual void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// �������� ����� ����� ����������
virtual TUClassesDescriptionsForm* New(TComponent *owner=0);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassesDescriptionsForm *UClassesDescriptionsForm;
//---------------------------------------------------------------------------
#endif
