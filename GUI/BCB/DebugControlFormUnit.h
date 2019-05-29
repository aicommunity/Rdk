//---------------------------------------------------------------------------

#ifndef DebugControlFormUnitH
#define DebugControlFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "UGEngineControlFormUnit.h"
#include "TUVisualControllerFormUnit.h"
#include "TUVisualControllerFrameUnit.h"
#include "DebugControlFrameUnit.h"
//---------------------------------------------------------------------------
class TDebugControlForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TDebugControlFrame *DebugControlFrame1;
private:	// User declarations
public:		// User declarations
	__fastcall TDebugControlForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDebugControlForm *DebugControlForm;
//---------------------------------------------------------------------------
#endif
