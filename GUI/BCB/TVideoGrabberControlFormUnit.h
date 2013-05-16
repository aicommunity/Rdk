//---------------------------------------------------------------------------

#ifndef TVideoGrabberControlFormUnitH
#define TVideoGrabberControlFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TVideoGrabberControlFrameUnit.h"
#include "TUVisualController.h"
#include "TUVisualControllerFormUnit.h"
#include "TUVisualControllerFrameUnit.h"
//---------------------------------------------------------------------------
class TVideoGrabberControlForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TVideoGrabberControlFrame *VideoGrabberControlFrame;
private:	// User declarations
public:		// User declarations
	__fastcall TVideoGrabberControlForm(TComponent* Owner);


// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед шагом расчета
void ABeforeCalculate(void);

// Метод, вызываемый после шага расчета
void AAfterCalculate(void);

// Обновление интерфейса
void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
void ALoadParameters(RDK::USerStorageXML &xml);

// Создание копии этого компонента
virtual TVideoGrabberControlForm* New(TComponent *owner=0);
// -----------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoGrabberControlForm *VideoGrabberControlForm;
//---------------------------------------------------------------------------
#endif
