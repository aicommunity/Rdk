//---------------------------------------------------------------------------

#ifndef UComponentLinksFormUnitH
#define UComponentLinksFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "UComponentLinksFrameUnit.h"
//---------------------------------------------------------------------------
class TUComponentLinksForm : public TForm
{
__published:	// IDE-managed Components
	TUComponentLinksFrame *UComponentLinksFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentLinksForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentLinksForm *UComponentLinksForm;
//---------------------------------------------------------------------------
#endif
