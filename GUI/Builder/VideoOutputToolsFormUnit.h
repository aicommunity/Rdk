//---------------------------------------------------------------------------

#ifndef VideoOutputToolsFormUnitH
#define VideoOutputToolsFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
#include <ExtCtrls.hpp>
#include "rdk.h"

class TVideoOutputFrame;
//---------------------------------------------------------------------------
class TVideoOutputToolsForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *FigurePanel;
	TGroupBox *GroupBox1;
	TCheckListBox *GeometryCheckListBox;
	TGroupBox *GroupBox4;
	TColorBox *ColorBox;
	TButton *AddFigureButton;
	TButton *DelFigureButton;
	TButton *DelAllFiguresButton;
	TPanel *PointsPanel;
	TGroupBox *GroupBox2;
	TButton *AddPointButton;
	TButton *DelPointButton;
	TButton *DelAllPointsButton;
	TGroupBox *GroupBox3;
	TCheckListBox *PointsCheckListBox;
	TButton *EditFigureButton;
	TButton *EditPointButton;
	TEdit *PointNameEdit;
	void __fastcall AddFigureButtonClick(TObject *Sender);
	void __fastcall DelAllFiguresButtonClick(TObject *Sender);
	void __fastcall DelFigureButtonClick(TObject *Sender);
	void __fastcall GeometryCheckListBoxClick(TObject *Sender);
	void __fastcall GeometryCheckListBoxClickCheck(TObject *Sender);
	void __fastcall AddPointButtonClick(TObject *Sender);
	void __fastcall EditPointButtonClick(TObject *Sender);
	void __fastcall EditFigureButtonClick(TObject *Sender);
	void __fastcall PointsCheckListBoxClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoOutputToolsForm(TComponent* Owner,
	TVideoOutputFrame* _MyVideoOutputFrame,
	RDK::MGraphics<double>& _GeometryGraphics,
	RDK::MGraphics<double>& _SampleGeometryGraphics,
	RDK::MGeometry<double>& _Figure,
	size_t& _FigureIndex,
	bool& _FigureFlag,
	size_t& _PointIndex,
	int& _PointFlag);

	// �������� �����
	TVideoOutputFrame* MyVideoOutputFrame;

// ��������� ���������
RDK::MGraphics<double>& GeometryGraphics;

// ������� ����������� ���������
RDK::MGraphics<double>& SampleGeometryGraphics;

// ������� ����������� ������
RDK::MGeometry<double>& Figure;

// ������ ������� ������������� ������
size_t& FigureIndex;

// ���� �������� ������
bool& FigureFlag;

// ���� �������� �����
// 0 - ���������
// 1 - �������� ����� �����
// 2 - �������������� ������������
int& PointFlag;

// ������ ������� ������������� �����
size_t& PointIndex;
};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputToolsForm *VideoOutputToolsForm;
//---------------------------------------------------------------------------
#endif
