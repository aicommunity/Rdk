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
//---------------------------------------------------------------------------
class TUClassesListFrame : public TFrame
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid;
	void __fastcall FrameResize(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUClassesListFrame(TComponent* Owner);

protected:
// Флаг обновления интерфейса
bool UpdateInterfaceFlag;

public:
// Отрисовка фрейма
void UpdateInterface(void);

// Возвращает id выбранного класса
int GetSelectedId(void);

// Возвращает имя выбранного класса
String GetSelectedName(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassesListFrame *UClassesListFrame;
//---------------------------------------------------------------------------
#endif
