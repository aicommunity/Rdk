//---------------------------------------------------------------------------

#ifndef TServerOptionsFrameUnitH
#define TServerOptionsFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TServerOptionsFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TLabeledEdit *BindAddressLabeledEdit;
	TLabeledEdit *BindPortLabeledEdit;
private:	// User declarations
public:		// User declarations
	__fastcall TServerOptionsFrame(TComponent* Owner);

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
virtual void AAfterReset(void);

// Обновление интерфейса
virtual void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

void ApplyOptions(void);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TServerOptionsFrame *ServerOptionsFrame;
//---------------------------------------------------------------------------
#endif
