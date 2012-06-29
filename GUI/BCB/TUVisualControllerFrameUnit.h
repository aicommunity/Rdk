//---------------------------------------------------------------------------

#ifndef TUVisualControllerFrameUnitH
#define TUVisualControllerFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "myrdk.h"
//---------------------------------------------------------------------------
#pragma warn -8130
class TUVisualControllerFrame : public TFrame, public RDK::UIVisualController
{
__published:	// IDE-managed Components
private:	// User declarations
public:		// User declarations
	__fastcall TUVisualControllerFrame(TComponent* Owner);
	__fastcall ~TUVisualControllerFrame(void);

bool UpdateInterfaceFlag;

public:
// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед шагом расчета
virtual void BeforeCalculate(void);
virtual void ABeforeCalculate(void);

// Метод, вызываемый после шага расчета
virtual void AfterCalculate(void);
virtual void AAfterCalculate(void);

// Обновление интерфейса
virtual void UpdateInterface(void);
virtual void AUpdateInterface(void);

// Возвращает уникальное имя интерфейса
virtual std::string GetName(void);

// Сохраняет параметры интерфейса в xml
virtual void SaveParameters(RDK::Serialize::USerStorageXML &xml);
virtual void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void LoadParameters(RDK::Serialize::USerStorageXML &xml);
virtual void ALoadParameters(RDK::Serialize::USerStorageXML &xml);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUVisualControllerFrame *UVisualControllerFrame;
//---------------------------------------------------------------------------
#pragma warn .8130
#endif
