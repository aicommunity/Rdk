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
//---------------------------------------------------------------------------
class TUComponentsListForm : public TForm
{
__published:	// IDE-managed Components
	TUComponentsListFrame *ComponentsListFrame1;
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsListForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsListForm *UComponentsListForm;
//---------------------------------------------------------------------------
#endif
