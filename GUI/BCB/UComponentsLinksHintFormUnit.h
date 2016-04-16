//---------------------------------------------------------------------------

#ifndef UComponentsLinksHintFormUnitH
#define UComponentsLinksHintFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TUVisualControllerFrameUnit.h"
#include "UComponentIOFrameUnit.h"
//---------------------------------------------------------------------------
class TUComponentsLinksHintForm : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TSplitter *Splitter1;
	TUComponentIOFrame *InputsFrame;
	TUComponentIOFrame *OutputsFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsLinksHintForm(TComponent* Owner);

	/// ���������� �����
	void __fastcall ShowLinks(const std::string &component_name,
								const std::string &owner_name,
								int x, int y);

	/// ����������� ������ ����
	int CalcFormWidth(void);

	/// ����������� ������ ������� ����������� ������
	int CalcInputsHeight(void);

	/// ����������� ������ ������� ����������� �������
	int CalcOutputsHeight(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsLinksHintForm *UComponentsLinksHintForm;
//---------------------------------------------------------------------------
#endif
