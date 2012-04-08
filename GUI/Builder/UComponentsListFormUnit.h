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
//---------------------------------------------------------------------------
class TUComponentsListForm : public TForm
{
__published:	// IDE-managed Components
	TUComponentsListFrame *ComponentsListFrame1;
	TPanel *Panel1;
	TButton *OkButton;
	TButton *CancelButton;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ComponentsListFrame1StringGridDblClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsListForm(TComponent* Owner);

// Метод открытия диалога для выбора компонента
int ShowComponentSelect(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsListForm *UComponentsListForm;
//---------------------------------------------------------------------------
#endif
