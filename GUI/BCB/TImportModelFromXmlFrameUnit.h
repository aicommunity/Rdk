//---------------------------------------------------------------------------

#ifndef TImportModelFromXmlFrameUnitH
#define TImportModelFromXmlFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TUVisualControllerFrameUnit.h"

//---------------------------------------------------------------------------
class TImportModelFromXmlFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TButton *Button1;
	TButton *Button2;
	TButton *Button3;
private:	// User declarations
public:		// User declarations
	__fastcall TImportModelFromXmlFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TImportModelFromXmlFrame *ImportModelFromXmlFrame;
//---------------------------------------------------------------------------
#endif
