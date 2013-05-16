//---------------------------------------------------------------------------

#ifndef UComponentsPerformanceFormUnitH
#define UComponentsPerformanceFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "UComponentsPerformanceFrameUnit.h"
#include "TUVisualController.h"
#include "TUVisualControllerFormUnit.h"
#include "TUVisualControllerFrameUnit.h"
//---------------------------------------------------------------------------
class TUComponentsPerformanceForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TUComponentsPerformanceFrame *UComponentsPerformanceFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsPerformanceForm(TComponent* Owner);

// Создание копии этого компонента
virtual TUComponentsPerformanceForm* New(TComponent *owner=0);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsPerformanceForm *UComponentsPerformanceForm;
//---------------------------------------------------------------------------
#endif
