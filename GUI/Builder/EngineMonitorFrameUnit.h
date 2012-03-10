//---------------------------------------------------------------------------

#ifndef EngineMonitorFrameUnitH
#define EngineMonitorFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>

//---------------------------------------------------------------------------

class TEngineMonitorFrame : public TFrame
{
__published:	// IDE-managed Components
	TRichEdit *RichEdit;
	TStatusBar *StatusBar;
private:	// User declarations
public:		// User declarations
	__fastcall TEngineMonitorFrame(TComponent* Owner);

	void UpdateInterface(void);

	bool UpdateInterfaceFlag;
};
//---------------------------------------------------------------------------
extern PACKAGE TEngineMonitorFrame *EngineMonitorFrame;
//---------------------------------------------------------------------------
#endif
