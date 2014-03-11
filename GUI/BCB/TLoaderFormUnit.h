//---------------------------------------------------------------------------

#ifndef TLoaderFormUnitH
#define TLoaderFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
//---------------------------------------------------------------------------
class TLoaderForm : public TForm
{
__published:	// IDE-managed Components
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
	__fastcall TLoaderForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoaderForm *LoaderForm;
//---------------------------------------------------------------------------
#endif
