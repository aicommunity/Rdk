//---------------------------------------------------------------------------

#ifndef UClassesListFormUnitH
#define UClassesListFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "UClassesListFrameUnit.h"
#include "UClassesListFrameUnit.h"
#include "TUVisualController.h"
//---------------------------------------------------------------------------
class TUClassesListForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TUClassesListFrame *ClassesListFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TUClassesListForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassesListForm *UClassesListForm;
//---------------------------------------------------------------------------
#endif
