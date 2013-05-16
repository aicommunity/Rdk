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
#include "TUVisualControllerFormUnit.h"
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TUClassesListForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TUClassesListFrame *ClassesListFrame;
	TPanel *Panel1;
	TButton *OkButton;
	TButton *CancelButton;
private:	// User declarations
public:		// User declarations
	__fastcall TUClassesListForm(TComponent* Owner);


// Создание копии этого компонента
virtual TUClassesListForm* New(TComponent *owner=0);
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassesListForm *UClassesListForm;
//---------------------------------------------------------------------------
#endif
