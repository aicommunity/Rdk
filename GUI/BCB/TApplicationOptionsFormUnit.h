//---------------------------------------------------------------------------

#ifndef TApplicationOptionsFormUnitH
#define TApplicationOptionsFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include "TApplicationOptionsFrameUnit.h"
#include "TServerOptionsFrameUnit.h"
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TApplicationOptionsForm : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *GeneralTabSheet;
	TTabSheet *ServerTabSheet;
	TApplicationOptionsFrame *ApplicationOptionsFrame;
	TServerOptionsFrame *ServerOptionsFrame;
	TPanel *Panel1;
	TButton *OKButton;
	TButton *CancelButton;
	TButton *RestoreButton;
	void __fastcall RestoreButtonClick(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TApplicationOptionsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TApplicationOptionsForm *ApplicationOptionsForm;
//---------------------------------------------------------------------------
#endif
