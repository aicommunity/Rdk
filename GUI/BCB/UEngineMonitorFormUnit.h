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
#include "TUVisualController.h"
#include "TUVisualControllerFormUnit.h"
#include "TUVisualControllerFrameUnit.h"
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
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// �������� ����� ����� ����������
virtual TUEngineMonitorForm* New(TComponent *owner=0);
};
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorForm *UEngineMonitorForm;
//---------------------------------------------------------------------------
#endif
