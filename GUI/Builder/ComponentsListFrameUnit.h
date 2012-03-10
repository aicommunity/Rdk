//---------------------------------------------------------------------------

#ifndef ComponentsListFrameUnitH
#define ComponentsListFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <ToolWin.hpp>
#include <string>
#include <vector>
//---------------------------------------------------------------------------
class TComponentsListFrame : public TFrame
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid;
	THeaderControl *HeaderControl;
	void __fastcall FrameResize(TObject *Sender);
	void __fastcall HeaderControlSectionClick(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall StringGridDblClick(TObject *Sender);
	void __fastcall StringGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall StringGridKeyPress(TObject *Sender, System::WideChar &Key);

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

protected: // ��������� ����������
		bool UpdateInterfaceFlag;

		// ������� ������� ������ ���� ������ � ������� ����� ���������� ������
		int SelectedId;

public:
	__fastcall TComponentsListFrame(TComponent* Owner);

		// -----------------------
        // ������ ������� � ���������� ������
		// -----------------------
		// ���������� id ���������� ����������
		const std::string& GetSelectedComponentId(void) const;

		// ���������� ��� ���������� ����������
		const std::string& GetSelectedComponentName(void) const;

		// ������� ��� ����������� ����������
		const std::string& GetSelectedComponentLongName(void) const;

		// ������� ��������� Id ����������� ����������
		const std::string& GetSelectedComponentLongId(void) const;

		// ������� ��� �������� ����������
		const std::string& GetCurrentComponentName(void) const;

		// ������� ��������� id �������� ����������
		const std::string& GetCurrentComponentId(void) const;

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


};
//---------------------------------------------------------------------------
extern PACKAGE TComponentsListFrame *ComponentsListFrame;
//---------------------------------------------------------------------------
#endif
