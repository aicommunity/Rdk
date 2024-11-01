//---------------------------------------------------------------------------

#ifndef UComponentsListFrameUnitH
#define UComponentsListFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <ToolWin.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ValEdit.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.Menus.hpp>
#include "TEnchancedStringGrid.h"
#include <string>
#include <vector>
#include "TUVisualController.h"
class TUDrawEngineFrame;
//---------------------------------------------------------------------------
class TUComponentsListFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TRichEdit *ParametersRichEdit;
	THeaderControl *ParametersHeaderControl;
	TPanel *Panel2;
	TStringGrid *StringGrid;
	THeaderControl *HeaderControl;
	TSplitter *Splitter1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	THeaderControl *StateHeaderControl;
	TRichEdit *StateRichEdit;
	TGroupBox *GroupBox;
	TGroupBox *GroupBox2;
	TSplitter *Splitter2;
	TStringGrid *OutputsStringGrid;
	TStringGrid *InputsStringGrid;
	TPopupMenu *PopupMenu;
	TMenuItem *GUI1;
	TPanel *ClassNamePanel;
	TMenuItem *Moveup1;
	TMenuItem *Movedown1;
	TMenuItem *Rename1;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *Delete1;
	TMenuItem *Reset1;
	TTabSheet *TabSheet6;
	TEnchancedSG *EnchancedSG1;
	THeaderControl *HeaderControl2;
	TRichEdit *NiceParamValRichEdit;
	TSplitter *Splitter5;
	TTabSheet *TabSheet7;
	THeaderControl *HeaderControl4;
	TRichEdit *NiceStateValRichEdit;
	TEnchancedSG *EnchancedSG2;
	TSplitter *Splitter6;
	TMenuItem *Calculate1;
	TMenuItem *N4;
	TMenuItem *CopynametoClipboard1;
	TMenuItem *CopyclasstoClipboard2;
	TMenuItem *CopylongnametoClipboard1;
	TMenuItem *N5;
	TMenuItem *Init1;
	TMenuItem *UnInit1;
	TMenuItem *Clone1;
	void __fastcall FrameResize(TObject *Sender);
	void __fastcall StringGridDblClick(TObject *Sender);
	void __fastcall StringGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall StringGridClick(TObject *Sender);
	void __fastcall ParametersRichEditChange(TObject *Sender);
	void __fastcall StateRichEditChange(TObject *Sender);
	void __fastcall StateHeaderControlSectionClick(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall OutputsStringGridClick(TObject *Sender);
	void __fastcall OutputsStringGridDblClick(TObject *Sender);
	void __fastcall OutputsStringGridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall InputsStringGridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall InputsStringGridDblClick(TObject *Sender);
	void __fastcall InputsStringGridClick(TObject *Sender);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall ParametersListStringGridClick(TObject *Sender);
	void __fastcall ParametersListStringGridDblClick(TObject *Sender);
	void __fastcall ParametersListStringGridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall StatesListStringGridClick(TObject *Sender);
	void __fastcall StatesListStringGridDblClick(TObject *Sender);
	void __fastcall StatesListStringGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall GUI1Click(TObject *Sender);
	void __fastcall ParametersHeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section);
	void __fastcall HeaderControl3SectionClick(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall HeaderControl1SectionClick(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall Moveup1Click(TObject *Sender);
	void __fastcall Movedown1Click(TObject *Sender);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall Delete1Click(TObject *Sender);
	void __fastcall StringGridKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall StringGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Reset1Click(TObject *Sender);
	void __fastcall StringGridMouseEnter(TObject *Sender);
	void __fastcall OutputsStringGridMouseEnter(TObject *Sender);
	void __fastcall InputsStringGridMouseEnter(TObject *Sender);
	void __fastcall EnchancedSG1BasicStringGridDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
	void __fastcall HeaderControl2SectionClick(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall NiceParamValRichEditMouseEnter(TObject *Sender);
	void __fastcall EnchancedSG1BasicStringGridSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
	void __fastcall EnchancedSG2BasicStringGridSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
	void __fastcall NiceParamValRichEditChange(TObject *Sender);
	void __fastcall NiceStateValRichEditChange(TObject *Sender);
	void __fastcall NiceStateValRichEditMouseEnter(TObject *Sender);
	void __fastcall HeaderControl4SectionClick(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall EnchancedSG2MouseEnter(TObject *Sender);
	void __fastcall EnchancedSG1BasicStringGridMouseEnter(TObject *Sender);
	void __fastcall EnchancedSG2BasicStringGridMouseEnter(TObject *Sender);
	void __fastcall EnchancedSG1BasicStringGridDblClick(TObject *Sender);
	void __fastcall EnchancedSG1BasicStringGridKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG2BasicStringGridDblClick(TObject *Sender);
	void __fastcall EnchancedSG2BasicStringGridKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG2txtDoubleEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG2txtStringEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG2cmbListEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG2txtBorderedIntEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG2txtIntEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG1txtDoubleEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG1txtStringEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG1cmbListEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG1txtBorderedIntEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG1txtIntEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EnchancedSG2BasicStringGridDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
	void __fastcall EnchancedSG1PastevaluefromClipboard1Click(TObject *Sender);
	void __fastcall EnchancedSG2PastevaluefromClipboard1Click(TObject *Sender);
	void __fastcall Calculate1Click(TObject *Sender);
	void __fastcall CopynametoClipboard1Click(TObject *Sender);
	void __fastcall CopyclasstoClipboard2Click(TObject *Sender);
	void __fastcall CopylongnametoClipboard1Click(TObject *Sender);
	void __fastcall EnchancedSG1BasicStringGridClick(TObject *Sender);
	void __fastcall EnchancedSG2BasicStringGridClick(TObject *Sender);
	void __fastcall FrameMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta,
          TPoint &MousePos, bool &Handled);
	void __fastcall Init1Click(TObject *Sender);
	void __fastcall UnInit1Click(TObject *Sender);
	void __fastcall Clone1Click(TObject *Sender);























protected:		// User declarations
		// ������� ��� ����������� ����������
		std::string SelectedComponentName;

		// ������� ��������� Id ����������� ����������
		std::string SelectedComponentId;

		// ������� ��� �������� ����������
		std::string CurrentComponentName;

		// ������� ��������� id �������� ����������
		std::string CurrentComponentId;

		// ��� ����������� ��������� ����������� ����������
		std::string SelectedComponentParameterName;

		// ��� ���������� ���������� ��������� ����������� ����������
		std::string SelectedComponentStateName;

		// ���� �� �������� ����������
		std::vector<std::string> CurrentPath;

		// ��������� ����� �������
		std::string SelectedComponentOutput;

		// ��������� ���� �������
		std::string SelectedComponentInput;

		// ��� ����������� �������� (���������� �� ���� ��� ��� ��������)
		std::string SelectedComponentPropertyName;

		// true ���� ��������� ��������� �������� ������ �������
		bool TreeReadOnlyFlag;

		// true ���� ��������� ��������� �������� ����������
		bool ParamReadOnlyFlag;

		// true ���� ������ ��� �������
		bool RegistryModified;

		// ���������-���������� ����������� ���������� � ���� xml
		bool ShowXMLComponentParameters;

public: // ������ ������������ ���� ��� ���������� ������������
		// ���������� ������ ����
		static std::map<std::string, TUVisualControllerForm*> ComponentControllers;
//		static std::map<std::string, std::pair<TUVisualControllerForm*,std::vector<TUVisualControllerForm*> > > ComponentControllers;

		TUDrawEngineFrame *DrawEngineFrame;

protected: // ��������� ����������		// ������� ������� ������ ���� ������ � ������� ����� ���������� ������
		int SelectedId;

		// ������ �������� ����������� ����������
		AnsiString LastParams;

		// ��������� ���������� ��� ������ ����������
		AnsiString TempParams;

		// ������ �������� ����������� ���������� ���������
		AnsiString LastStates;

		// ��������� ���������� ��� ������ ���������� ���������
		AnsiString TempStates;

public:
	__fastcall TUComponentsListFrame(TComponent* Owner);

		// -----------------------
        // ������ ������� � ���������� ������
		// -----------------------
		// ���������� id ���������� ����������
		const std::string GetSelectedComponentId(void) const;

		// ���������� ��� ���������� ����������
		const std::string GetSelectedComponentName(void) const;

		// �������� ��������� �� ��������� ��������� �����
		void SetSelectedComponentName(const std::string &name);

		// ������� ��� ����������� ����������
		const std::string& GetSelectedComponentLongName(void) const;

		// ������� ��������� Id ����������� ����������
		const std::string& GetSelectedComponentLongId(void) const;

		// ��� ����������� ��������� ����������� ����������
		const std::string& GetSelectedComponentParameterName(void) const;

		// ��� ���������� ���������� ��������� ����������� ����������
		const std::string& GetSelectedComponentStateName(void) const;

		// ������� ��� �������� ����������
		const std::string& GetCurrentComponentName(void) const;

		// ���������� �� ������� ���� �� ����� ���������� ����������
		void SelectComponentByName(const std::string& name);

		// ����������� �� ������� �����
		void SelectUp(void);

		// ������� ��������� id �������� ����������
		const std::string& GetCurrentComponentId(void) const;

		// ��������� ����� �������
		const std::string& GetSelectedComponentOutput(void) const;

		// ��������� ���� �������
		const std::string& GetSelectedComponentInput(void) const;

		// ��� ����������� �������� (���������� �� ���� ��� ��� ��������)
		const std::string& GetSelectedComponentPropertyName(void) const;

		// ���������-���������� ����������� ���������� � ���� xml
		bool GetShowXMLComponentParameters(void) const;

		// ���������� ���� ������� �������������� ������ �����
		bool GetTreeReadOnlyFlag(void);

		// ���������� ���� ������� �������������� ����������
		bool GetParamReadOnlyFlag(void);

		// ���������� ������� ������� ��������� � ������
		bool GetRegistryModified(void);
		// -----------------------

		// -----------------------
		// ������ ���������� ����������� �������
		// -----------------------
		// ������� ������ ��������� �������
		void __fastcall ClearDiffRegistry(void);

		// ������ ������� ���� � ������ ������ 'fullname'
		bool __fastcall SetCurrentSubTree(std::string &fullname);

		// ���������-���������� ����������� ���������� � ���� xml
		void SetShowXMLComponentParameters(bool flag);

		// ���������/��������� �������������� ������ �����
		void __fastcall SetTreeReadOnlyFlag(bool flag);

		// ���������/��������� �������������� ����������
		void __fastcall SetParamReadOnlyFlag(bool flag);

		// ������������� ������� ������� ��������� � ������
		void __fastcall SetRegistryModified(bool flag);
        // -----------------------


// �������������� ���� �� �������� ����
void __fastcall UpdatePath(void);

// ��������� ������
void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::USerStorageXML &xml);

// ��������� ������� ����� ��������� ���������
void UpdateSelectedComponentInfo(void);

// ��������� ��������� ����������
void UpdateParameters(void);

// ��������� ��������� ����������
void UpdateState(void);

// ��������� ������ �����-������
void UpdateIO(void);

// ��������� ������ ����������� ������ ����������
void UpdateNiceParamsList(TEnchancedSG *frame);

// ��������� ������ ����������� ������ ����������
void UpdateNiceStatesList(TEnchancedSG *frame);

};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsListFrame *UComponentsListFrame;
//---------------------------------------------------------------------------
#endif
