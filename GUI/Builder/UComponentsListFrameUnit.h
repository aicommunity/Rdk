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
#include <string>
#include <vector>
//---------------------------------------------------------------------------
class TUComponentsListFrame : public TFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TRichEdit *ParametersRichEdit;
	THeaderControl *ParametersHeaderControl;
	TPanel *Panel2;
	TStringGrid *StringGrid;
	THeaderControl *HeaderControl;
	TSplitter *Splitter1;
	void __fastcall FrameResize(TObject *Sender);
	void __fastcall StringGridDblClick(TObject *Sender);
	void __fastcall StringGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall StringGridKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall StringGridClick(TObject *Sender);
	void __fastcall ParametersHeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section);

protected:		// User declarations
		// ������� ��� ����������� ����������
		std::string SelectedComponentName;

		// ������� ��������� Id ����������� ����������
		std::string SelectedComponentId;

		// ������� ��� �������� ����������
		std::string CurrentComponentName;

		// ������� ��������� id �������� ����������
		std::string CurrentComponentId;

		// ���� �� �������� ����������
		std::vector<std::string> CurrentPath;

		// true ���� ��������� ��������� �������� ������ �������
		bool TreeReadOnlyFlag;

		// true ���� ��������� ��������� �������� ����������
		bool ParamReadOnlyFlag;

		// true ���� ������ ��� �������
		bool RegistryModified;

		// ���������-���������� ����������� ���������� � ���� xml
		bool ShowXMLComponentParameters;

protected: // ��������� ����������
		bool UpdateInterfaceFlag;

		// ������� ������� ������ ���� ������ � ������� ����� ���������� ������
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

		// ������� ��� �������� ����������
		const std::string& GetCurrentComponentName(void) const;

		// ������� ��������� id �������� ����������
		const std::string& GetCurrentComponentId(void) const;

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


};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsListFrame *UComponentsListFrame;
//---------------------------------------------------------------------------
#endif
