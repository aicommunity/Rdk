//---------------------------------------------------------------------------

#ifndef UClassesListFrameUnitH
#define UClassesListFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include "TUVisualController.h"
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
//---------------------------------------------------------------------------
class TUClassesListFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TTabSheet *NameTabSheet;
	TTabSheet *LibsTabSheet;
	TStringGrid *StringGrid;
	TTreeView *TreeView;
	TTabSheet *LibsControlTabSheet;
	TSplitter *Splitter1;
	TGroupBox *GroupBox1;
	TStringGrid *LibsListStringGrid;
	TGroupBox *GroupBox2;
	TStringGrid *LibComponentListStringGrid;
	TEdit *SearchEdit;
	TPopupMenu *RTLibsPopupMenu;
	TPopupMenu *RTClassesPopupMenu;
	TMenuItem *CreateRuntimeLibrary1;
	TMenuItem *DeleteRuntimeLibrary1;
	TMenuItem *DeleteClass1;
	void __fastcall PageControlChange(TObject *Sender);
	void __fastcall StringGridMouseEnter(TObject *Sender);
	void __fastcall TreeViewMouseEnter(TObject *Sender);
	void __fastcall LibsListStringGridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall LibsListStringGridMouseEnter(TObject *Sender);
	void __fastcall LibComponentListStringGridMouseEnter(TObject *Sender);
	void __fastcall StringGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall LibComponentListStringGridMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall TreeViewMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall SearchEditChange(TObject *Sender);
	void __fastcall CreateRuntimeLibrary1Click(TObject *Sender);
	void __fastcall DeleteRuntimeLibrary1Click(TObject *Sender);
	void __fastcall CreateNewClass1Click(TObject *Sender);
	void __fastcall DeleteClass1Click(TObject *Sender);


private:	// User declarations
public:		// User declarations
	__fastcall TUClassesListFrame(TComponent* Owner);

protected:

std::string LibrariesListNames;
std::vector<std::string> LibraryNames,NewLibraryNames;
std::vector<std::string> TempLibraryNames;
std::map<std::string, std::vector<std::string> > LibraryClassNames;
std::vector<std::string> ClassNames;
bool RepaintNeeded;

public:
/// ��� ���������� �� ����� ��� ���������� � ��������� ����������
std::string NewComponentName;

public:

// ��������� ������
void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ���������� id ���������� ������
//int GetSelectedId(void);

// ���������� ��� ���������� ������
String GetSelectedName(void);

/// ������������� ��� ���������� ������
void SetSelectedName(const String &name);

// ���������� ��� ��������� ����������
String GetSelectedLibraryName(void);

/// ������������ ������ ������� � ��������� ����������
void DrawClassesList(int library_index, TStringGrid *classes_string_grid);
void DrawClassesList(const std::string &library_name, TStringGrid *classes_string_grid);

/// ������� ����� ����� � ��������� Runtime library
bool AddClassToRuntimeLibrary(const std::string &object_prototype_name, const std::string &class_name, const std::string &library_name);
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassesListFrame *UClassesListFrame;
//---------------------------------------------------------------------------
#endif
