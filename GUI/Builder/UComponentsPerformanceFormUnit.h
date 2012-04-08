//---------------------------------------------------------------------------

#ifndef UComponentsPerformanceFormUnitH
#define UComponentsPerformanceFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "UComponentsPerformanceFrameUnit.h"
//---------------------------------------------------------------------------
class TUComponentsPerformanceForm : public TForm
{
__published:	// IDE-managed Components
	TUComponentsPerformanceFrame *UComponentsPerformanceFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsPerformanceForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsPerformanceForm *UComponentsPerformanceForm;
//---------------------------------------------------------------------------
#endif
