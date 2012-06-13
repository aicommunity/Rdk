//---------------------------------------------------------------------------

#ifndef UEngineMonitorFormUnitH
#define UEngineMonitorFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "UEngineMonitorFrameUnit.h"
#include "UEngineMonitorFrameUnit.h"
//---------------------------------------------------------------------------

void ExceptionHandler(void);

//---------------------------------------------------------------------------
class TUEngineMonitorForm : public TForm
{
__published:	// IDE-managed Components
	TUEngineMonitorFrame *EngineMonitorFrame;
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorForm(TComponent* Owner);


// Добавляет обработчик в список
void AddInterface(RDK::UIVisualController *value);

// Удаляет обработчик из списка
void DelInterface(RDK::UIVisualController *value);
};
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorForm *UEngineMonitorForm;
//---------------------------------------------------------------------------
#endif
