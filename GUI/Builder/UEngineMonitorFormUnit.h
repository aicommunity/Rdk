//---------------------------------------------------------------------------

#ifndef UEngineMonitorFormUnitH
#define UEngineMonitorFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "UEngineMonitorFrameUnit.h"
#include "UEngineMonitorFrameUnit.h"
//---------------------------------------------------------------------------

void ExceptionHandler(void);

//---------------------------------------------------------------------------
class TUEngineMonitorForm : public TForm
{
__published:	// IDE-managed Components
	TUEngineMonitorFrame *EngineMonitorFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorForm *UEngineMonitorForm;
//---------------------------------------------------------------------------
#endif
