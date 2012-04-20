//---------------------------------------------------------------------------

#ifndef UImagesFormUnitH
#define UImagesFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include "UImagesFrameUnit.h"
#include "myrdk.h"
//---------------------------------------------------------------------------
class TUImagesForm : public TForm
{
__published:	// IDE-managed Components
	TUImagesFrame *ImagesFrame;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUImagesForm(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TUImagesForm *UImagesForm;
//---------------------------------------------------------------------------
#endif
