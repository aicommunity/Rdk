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
#include "TUVisualController.h"
//---------------------------------------------------------------------------

void ExceptionHandler(void);

//---------------------------------------------------------------------------
class TUEngineMonitorForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TUEngineMonitorFrame *EngineMonitorFrame;
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorForm(TComponent* Owner);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::Serialize::USerStorageXML &xml);
};
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorForm *UEngineMonitorForm;
//---------------------------------------------------------------------------
#endif