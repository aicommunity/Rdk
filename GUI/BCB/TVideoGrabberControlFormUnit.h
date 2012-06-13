//---------------------------------------------------------------------------

#ifndef TVideoGrabberControlFormUnitH
#define TVideoGrabberControlFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TVideoGrabberControlFrameUnit.h"
//---------------------------------------------------------------------------
class TVideoGrabberControlForm : public TForm
{
__published:	// IDE-managed Components
	TVideoGrabberControlFrame *VideoGrabberControlFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TVideoGrabberControlForm(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoGrabberControlForm *VideoGrabberControlForm;
//---------------------------------------------------------------------------
#endif
