//---------------------------------------------------------------------------

#ifndef ComponentsListFormUnitH
#define ComponentsListFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "ComponentsListFrameUnit.h"
//---------------------------------------------------------------------------
class TComponentsListForm : public TForm
{
__published:	// IDE-managed Components
	TComponentsListFrame *ComponentsListFrame1;
private:	// User declarations
public:		// User declarations
	__fastcall TComponentsListForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TComponentsListForm *ComponentsListForm;
//---------------------------------------------------------------------------
#endif
