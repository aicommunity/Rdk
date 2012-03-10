//---------------------------------------------------------------------------

#ifndef ClassesListFrameUnitH
#define ClassesListFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
//---------------------------------------------------------------------------
class TClassesListFrame : public TFrame
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid;
	void __fastcall FrameResize(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TClassesListFrame(TComponent* Owner);

bool UpdateInterfaceFlag;

// Отрисовка фрейма
void UpdateInterface(void);

// Возвращает id выбранного класса
int GetSelectedId(void);

// Возвращает имя выбранного класса
String GetSelectedName(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TClassesListFrame *ClassesListFrame;
//---------------------------------------------------------------------------
#endif
