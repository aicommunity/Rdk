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
#include "TUVisualController.h"
#include "TUVisualControllerFrameUnit.h"
//---------------------------------------------------------------------------
class TUComponentLinksFrame : public TUVisualControllerFrame
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
	TPanel *ButtonPanel;
	TButton *OkButton;
	TButton *CancelButton;
	void __fastcall FrameResize(TObject *Sender);
	void __fastcall HeaderControl1SectionClick(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall OkButtonClick(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TUComponentLinksFrame(TComponent* Owner);

protected:		// User declarations
// ������� id ������������ ���������� ������
//std::string ViewComponentLongId;

// ����� ������ ������
// 0 - ������� ����� � �������
// 1 - ��������� ����� �������� ����� � ��������
// 2 - ��������� ����� ������� ����� � ��������
// 3 - ��������� ����� ����������� ������ � ��������
int Mode;


public:
// ������ � id ������������ ����������
//const std::string& GetViewComponentLongId(void) const;
//void SetViewComponentLongId(const std::string& value);

// ������������� ���������� ����� ������������
void __fastcall Init(int mode, const std::string &component_name1, const std::string &owner_component_name, const std::string &component_name2="");

// ����� ������ �����������
void SetMode(int mode);

// ���������� ����������
void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ������� ��������� �����
void __fastcall CreateLink(void);

// ��������� ��������� �����
void __fastcall BreakLink(void);

// ��������� ��� �����
void __fastcall BreakAll(void);

// ���������� ��� ����� � ������ ������� ���������� �� ����� �������
void __fastcall SwitchLink(void);

// ��������� ��� ������� �� ������� � ������� ����������� id
// �������������� ��������� ��� ������������ �����
void __fastcall ChainLinking(void);

// ��������� ��� ������� ����������� � �������� ������
// �������������� ��������� ��� ������������ �����
void __fastcall ParallelLinking(void);


};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentLinksFrame *UComponentLinksFrame;
//---------------------------------------------------------------------------
#endif
