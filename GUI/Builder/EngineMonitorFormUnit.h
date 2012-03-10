//---------------------------------------------------------------------------

#ifndef EngineMonitorFormUnitH
#define EngineMonitorFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "EngineMonitorFrameUnit.h"
//---------------------------------------------------------------------------

void ExceptionHandler(void);

//---------------------------------------------------------------------------
class TEngineMonitorForm : public TForm
{
__published:	// IDE-managed Components
	TEngineMonitorFrame *EngineMonitorFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TEngineMonitorForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TEngineMonitorForm *EngineMonitorForm;
//---------------------------------------------------------------------------
#endif
