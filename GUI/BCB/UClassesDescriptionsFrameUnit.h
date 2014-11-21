//---------------------------------------------------------------------------

#ifndef UClassesDescriptionsFrameUnitH
#define UClassesDescriptionsFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.ExtCtrls.hpp>
#include "UClassesListFrameUnit.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
//---------------------------------------------------------------------------
class TUClassesDescriptionsFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TUClassesListFrame *ClassesListFrame;
	TPanel *Panel3;
	TLabeledEdit *ClassNameLabeledEdit;
	TLabeledEdit *ClassFullNameLabeledEdit;
	TGroupBox *ClassDescriptionGroupBox;
	TRichEdit *ClassDescriptionRichEdit;
	TGroupBox *GroupBox1;
	TStringGrid *StringGrid1;
	TPanel *Panel4;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TRichEdit *RichEdit1;
	TRichEdit *RichEdit2;
	TButton *Button1;
	TButton *Button2;
private:	// User declarations
public:		// User declarations
	__fastcall TUClassesDescriptionsFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassesDescriptionsFrame *UClassesDescriptionsFrame;
//---------------------------------------------------------------------------
#endif
