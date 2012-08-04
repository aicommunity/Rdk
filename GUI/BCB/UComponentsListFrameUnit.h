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
#include <string>
#include <vector>
#include "TUVisualController.h"
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
	THeaderControl *IOHeaderControl;
	TGroupBox *GroupBox;
	TGroupBox *GroupBox2;
	TSplitter *Splitter2;
	TStringGrid *OutputsStringGrid;
	TStringGrid *InputsStringGrid;
	TTabSheet *TabSheet4;
	TTabSheet *TabSheet5;
	THeaderControl *HeaderControl1;
	THeaderControl *HeaderControl2;
	TStringGrid *ParametersListStringGrid;
	TStringGrid *StatesListStringGrid;
	void __fastcall FrameResize(TObject *Sender);
	void __fastcall StringGridDblClick(TObject *Sender);
	void __fastcall StringGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall StringGridKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall StringGridClick(TObject *Sender);
	void __fastcall ParametersHeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section);
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
		int SelectedComponentOutput;

		// ��������� ���� �������
		int SelectedComponentInput;

		// true ���� ��������� ��������� �������� ������ �������
		bool TreeReadOnlyFlag;

		// true ���� ��������� ��������� �������� ����������
		bool ParamReadOnlyFlag;

		// true ���� ������ ��� �������
		bool RegistryModified;

		// ���������-���������� ����������� ���������� � ���� xml
		bool ShowXMLComponentParameters;

protected: // ��������� ����������		// ������� ������� ������ ���� ������ � ������� ����� ���������� ������
		int SelectedId;

public:
	__fastcall TUComponentsListFrame(TComponent* Owner);

		// -----------------------
        // ������ ������� � ���������� ������
		// -----------------------
		// ���������� id ���������� ����������
		const std::string GetSelectedComponentId(void) const;

		// ���������� ��� ���������� ����������
		const std::string GetSelectedComponentName(void) const;

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

		// ������� ��������� id �������� ����������
		const std::string& GetCurrentComponentId(void) const;

		// ��������� ����� �������
		int GetSelectedComponentOutput(void) const;

		// ��������� ���� �������
		int GetSelectedComponentInput(void) const;

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
void UpdateInterface(void);

// ��������� ������� ����� ��������� ���������
void UpdateSelectedComponentInfo(void);

// ��������� ��������� ����������
void UpdateParameters(void);

// ��������� ��������� ����������
void UpdateState(void);

// ��������� ������ �����-������
void UpdateIO(void);

// ��������� ������ ������ ����������
void UpdateParametersList(void);

// ��������� ������ ������ ���������� ���������
void UpdateStatesList(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsListFrame *UComponentsListFrame;
//---------------------------------------------------------------------------
#endif
