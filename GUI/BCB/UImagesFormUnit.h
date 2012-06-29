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
#include "TUVisualController.h"
//---------------------------------------------------------------------------
class TUImagesForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TUImagesFrame *ImagesFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TUImagesForm(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TUImagesForm *UImagesForm;
//---------------------------------------------------------------------------
#endif
