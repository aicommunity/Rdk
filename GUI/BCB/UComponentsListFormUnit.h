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

// Режим выбора
// 0 - компонента
// 1 - параметра
// 2 - перменной состояния
int Mode;

// Метод открытия диалога для выбора компонента
int ShowComponentSelect(void);

// Метод открытия диалога для выбора параметра
int ShowParameterSelect(void);

// Метод открытия диалога для выбора переменной состояния
int ShowStateSelect(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsListForm *UComponentsListForm;
//---------------------------------------------------------------------------
#endif
