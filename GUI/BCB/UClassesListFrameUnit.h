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
//---------------------------------------------------------------------------
class TUClassesListFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid;
	void __fastcall FrameResize(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUClassesListFrame(TComponent* Owner);

protected:

public:
// Отрисовка фрейма
void AUpdateInterface(void);

// Возвращает id выбранного класса
int GetSelectedId(void);

// Возвращает имя выбранного класса
String GetSelectedName(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassesListFrame *UClassesListFrame;
//---------------------------------------------------------------------------
#endif
