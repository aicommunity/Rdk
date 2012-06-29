//---------------------------------------------------------------------------

#ifndef UComponentsListFormUnitH
#define UComponentsListFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "UComponentsListFrameUnit.h"
#include "UComponentsListFrameUnit.h"
#include <Vcl.ExtCtrls.hpp>
#include "TUVisualController.h"
//---------------------------------------------------------------------------
class TUComponentsListForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TUComponentsListFrame *ComponentsListFrame1;
	TPanel *Panel1;
	TButton *OkButton;
	TButton *CancelButton;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ComponentsListFrame1StringGridDblClick(TObject *Sender);
	void __fastcall ComponentsListFrame1ParametersListStringGridDblClick(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsListForm(TComponent* Owner);

// ����� ������
// 0 - ����������
// 1 - ���������
// 2 - ��������� ���������
int Mode;

// ����� �������� ������� ��� ������ ����������
int ShowComponentSelect(void);

// ����� �������� ������� ��� ������ ���������
int ShowParameterSelect(void);

// ����� �������� ������� ��� ������ ���������� ���������
int ShowStateSelect(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsListForm *UComponentsListForm;
//---------------------------------------------------------------------------
#endif
