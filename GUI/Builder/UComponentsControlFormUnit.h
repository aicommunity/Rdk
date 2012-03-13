//---------------------------------------------------------------------------

#ifndef UComponentsControlFormUnitH
#define UComponentsControlFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "UComponentsControlFrameUnit.h"
#include "UComponentsControlFrameUnit.h"
//---------------------------------------------------------------------------
class TUComponentsControlForm : public TForm
{
__published:	// IDE-managed Components
	TUComponentsControlFrame *ComponentsControlFrame;
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsControlForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsControlForm *UComponentsControlForm;
//---------------------------------------------------------------------------
#endif
