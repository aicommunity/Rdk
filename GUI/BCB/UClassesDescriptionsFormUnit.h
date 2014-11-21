//---------------------------------------------------------------------------

#ifndef UClassesDescriptionsFormUnitH
#define UClassesDescriptionsFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFrameUnit.h"
#include "UClassesDescriptionsFrameUnit.h"
#include "TUVisualControllerFormUnit.h"
//---------------------------------------------------------------------------
class TUClassesDescriptionsForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TUClassesDescriptionsFrame *ClassesDescriptionsFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TUClassesDescriptionsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassesDescriptionsForm *UClassesDescriptionsForm;
//---------------------------------------------------------------------------
#endif
