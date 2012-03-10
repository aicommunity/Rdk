//---------------------------------------------------------------------------

#ifndef ClassesListFormUnitH
#define ClassesListFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "ClassesListFrameUnit.h"
//---------------------------------------------------------------------------
class TClassesListForm : public TForm
{
__published:	// IDE-managed Components
	TClassesListFrame *ClassesListFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TClassesListForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TClassesListForm *ClassesListForm;
//---------------------------------------------------------------------------
#endif
