//---------------------------------------------------------------------------

#ifndef UComponentsControlFormUnitH
#define UComponentsControlFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "UComponentsControlFrameUnit.h"
#include "TUVisualController.h"
#include "TUVisualControllerFrameUnit.h"
#include "TUVisualControllerFormUnit.h"
//---------------------------------------------------------------------------
class TUComponentsControlForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TUComponentsControlFrame *ComponentsControlFrame;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ComponentsListFrameStringGridClick(TObject *Sender);
	void __fastcall ComponentsListFrameStringGridDblClick(TObject *Sender);
	void __fastcall ComponentsListFrameStringGridSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
	void __fastcall ComponentsListFrameStringGridKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall ClassesListFrameStringGridDblClick(TObject *Sender);
	void __fastcall ClassesListFrameStringGridKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall ComponentsControlFrameTakeObjectButtonClick(TObject *Sender);
	void __fastcall ComponentsControlFrameReturnObjectButtonClick(TObject *Sender);




private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsControlForm(TComponent* Owner);


// Создание копии этого компонента
virtual TUComponentsControlForm* New(TComponent *owner=0);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsControlForm *UComponentsControlForm;
//---------------------------------------------------------------------------
#endif
