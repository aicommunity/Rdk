//---------------------------------------------------------------------------

#ifndef VideoOutputFormUnitH
#define VideoOutputFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "VideoOutputFrameUnit.h"
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TVideoOutputForm : public TForm
{
__published:	// IDE-managed Components
	TVideoOutputFrame *VideoOutputFrame;
	void __fastcall VideoOutputFrame1ImageMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall VideoOutputFrameN1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoOutputForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputForm *VideoOutputForm;
//---------------------------------------------------------------------------
#endif
