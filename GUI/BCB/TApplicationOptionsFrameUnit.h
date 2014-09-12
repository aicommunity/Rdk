//---------------------------------------------------------------------------

#ifndef TApplicationOptionsFrameUnitH
#define TApplicationOptionsFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Samples.Spin.hpp>
//---------------------------------------------------------------------------
class TApplicationOptionsFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TCheckBox *AutoStartProjectCheckBox;
	TLabeledEdit *ApplicationNameLabeledEdit;
	TLabeledEdit *MainFormInternalNameLabeledEdit;
	TLabeledEdit *AutoStartProjectNameLabeledEdit;
	TButton *BrowseAutoLoadingProjectNameButton;
	TButton *BrowseMainFormNameButton;
	TCheckBox *HideAdminFormCheckBox;
	TCheckBox *DisableAdminFormCheckBox;
	TCheckBox *MinimizeToTrayCheckBox;
	TCheckBox *StartMinimizedCheckBox;
	TLabel *Label1;
	TSpinEdit *ProjectHistorySpinEdit;
private:	// User declarations
public:		// User declarations
	__fastcall TApplicationOptionsFrame(TComponent* Owner);

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

virtual void ApplyOptions(void);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TApplicationOptionsFrame *ApplicationOptionsFrame;
//---------------------------------------------------------------------------
#endif
