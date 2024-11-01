//---------------------------------------------------------------------------

#ifndef UComponentsListFormUnitH
#define UComponentsListFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "UComponentsListFrameUnit.h"
#include "UComponentsListFrameUnit.h"
#include "TUVisualController.h"
#include "TUVisualControllerFormUnit.h"
#include "TUVisualControllerFrameUnit.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class TUComponentsListForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TButton *OkButton;
	TButton *CancelButton;
	TUComponentsListFrame *ComponentsListFrame1;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ComponentsListFrame1StringGridDblClick(TObject *Sender);
	void __fastcall ComponentsListFrame1ParametersListStringGridDblClick(TObject *Sender);
	void __fastcall ComponentsListFrame1StatesListStringGridDblClick(TObject *Sender);
	void __fastcall ComponentsListFrame1OutputsStringGridDblClick(TObject *Sender);
	void __fastcall ComponentsListFrame1InputsStringGridDblClick(TObject *Sender);



private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsListForm(TComponent* Owner);

// ����� ������
// 0 - ����������
// 1 - ���������
// 2 - ��������� ���������
// 3 - ����� ��� ������
int Mode;

// ����� �������� ������� ��� ������ ����������
int ShowComponentSelect(void);

// ����� �������� ������� ��� ������ ���������
int ShowParameterSelect(void);

// ����� �������� ������� ��� ������ ���������� ���������
int ShowStateSelect(void);

// ����� �������� ������� ��� ������ ����� ��� ������
int ShowIOSelect(void);

// �������� ����� ����� ����������
virtual TUComponentsListForm* New(TComponent *owner=0);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsListForm *UComponentsListForm;
//---------------------------------------------------------------------------
#endif
