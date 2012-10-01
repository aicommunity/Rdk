//---------------------------------------------------------------------------

#ifndef UComponentLinksFormUnitH
#define UComponentLinksFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "UComponentLinksFrameUnit.h"
#include "TUVisualController.h"
#include "TUVisualControllerFormUnit.h"
#include "TUVisualControllerFrameUnit.h"
//---------------------------------------------------------------------------
class TUComponentLinksForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TUComponentLinksFrame *UComponentLinksFrame;
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentLinksForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentLinksForm *UComponentLinksForm;
//---------------------------------------------------------------------------
#endif
