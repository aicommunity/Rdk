//---------------------------------------------------------------------------

#ifndef TImportModelFromXmlFormUnitH
#define TImportModelFromXmlFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFormUnit.h"
#include "TImportModelFromXmlFrameUnit.h"
#include "TUVisualControllerFrameUnit.h"

//---------------------------------------------------------------------------
class TImportModelFromXmlForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TImportModelFromXmlFrame *ImportModelFromXmlFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TImportModelFromXmlForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TImportModelFromXmlForm *ImportModelFromXmlForm;
//---------------------------------------------------------------------------
#endif
