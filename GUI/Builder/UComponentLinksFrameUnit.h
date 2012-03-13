//---------------------------------------------------------------------------

#ifndef UComponentLinksFrameUnitH
#define UComponentLinksFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "UComponentIOFrameUnit.h"
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TUComponentLinksFrame : public TFrame
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TUComponentIOFrame *NANetFrameInputs;
	TGroupBox *GroupBox2;
	TUComponentIOFrame *NANetFrameOutputs;
	TGroupBox *GroupBox3;
	TUComponentIOFrame *NANetFrameLinks;
	TPanel *Panel1;
	TSplitter *Splitter1;
	THeaderControl *HeaderControl1;
	void __fastcall FrameResize(TObject *Sender);
	void __fastcall HeaderControl1SectionClick(THeaderControl *HeaderControl, THeaderSection *Section);

private:	// User declarations
public:		// User declarations
	__fastcall TUComponentLinksFrame(TComponent* Owner);

protected:		// User declarations
// ������� id ������������ ���������� ������
std::string ViewComponentLongId;

public:
// ������ � id ������������ ����������
const std::string& GetViewComponentLongId(void) const;
void SetViewComponentLongId(const std::string& value);

// ������������� ���������� ����� ������ ������������
void __fastcall Init(void);

// ���������� ����������
void UpdateInterface(void);

// ������� ��������� �����
void __fastcall CreateLink(void);

// ��������� ��������� �����
void __fastcall BreakLink(void);

// ��������� ��� �����
void __fastcall BreakAll(void);

// ��������� ��� ������� �� ������� � ������� ����������� id
// �������������� ��������� ��� ������������ �����
void __fastcall ChainLinking(void);


};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentLinksFrame *UComponentLinksFrame;
//---------------------------------------------------------------------------
#endif
