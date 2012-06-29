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
#include "myrdk.h"
#include "TUVisualController.h"

class TVideoOutputFrame;
//---------------------------------------------------------------------------
class TVideoOutputToolsForm : public TUVisualControllerForm
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
	void __fastcall FormShow(TObject *Sender);
	void __fastcall DelAllPointsButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoOutputToolsForm(TComponent* Owner,
	TVideoOutputFrame* _MyVideoOutputFrame,
	RDK::MGraphics<double,2>& _GeometryGraphics,
//	RDK::MGraphics<double,2>& _SampleGeometryGraphics,
	RDK::MGeometry<double,2>& _Figure,
	int& _FigureIndex,
//	bool& _FigureFlag,
	int& _PointIndex,
	int& _PointFlag);

	// Владелец формы
	TVideoOutputFrame* MyVideoOutputFrame;

// Отрисовка геометрии
RDK::MGraphics<double,2>& GeometryGraphics;

// Образец формируемой геометрии
//RDK::MGraphics<double,2>& SampleGeometryGraphics;

// Текущая создаваемая фигура
RDK::MGeometry<double,2>& Figure;

// Индекс текущей редактируемой фигуры
int& FigureIndex;

// Флаг создания фигуры
//bool& FigureFlag;

// Флаг создания точки
// 0 - выключено
// 1 - создание новой точки
// 2 - редактирование существующей
int& PointFlag;

// Индекс текущей редактируемой точки
int& PointIndex;
};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputToolsForm *VideoOutputToolsForm;
//---------------------------------------------------------------------------
#endif
