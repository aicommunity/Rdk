//---------------------------------------------------------------------------

#ifndef UClassesDescriptionsFormUnitH
#define UClassesDescriptionsFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFrameUnit.h"
#include "UClassesDescriptionsFrameUnit.h"
#include "TUVisualControllerFormUnit.h"
//---------------------------------------------------------------------------
class TUClassesDescriptionsForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TUClassesDescriptionsFrame *ClassesDescriptionsFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TUClassesDescriptionsForm(TComponent* Owner);

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
virtual void AAfterReset(void);

// Обновление интерфейса
virtual void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// Создание копии этого компонента
virtual TUClassesDescriptionsForm* New(TComponent *owner=0);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassesDescriptionsForm *UClassesDescriptionsForm;
//---------------------------------------------------------------------------
#endif
