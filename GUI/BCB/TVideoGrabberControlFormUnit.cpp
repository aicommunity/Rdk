//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoGrabberControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TVideoGrabberControlFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TVideoGrabberControlForm *VideoGrabberControlForm;
//---------------------------------------------------------------------------
__fastcall TVideoGrabberControlForm::TVideoGrabberControlForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------
// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед шагом расчета
void TVideoGrabberControlForm::ABeforeCalculate(void)
{
}

// Метод, вызываемый после шага расчета
void TVideoGrabberControlForm::AAfterCalculate(void)
{
}

// Обновление интерфейса
void TVideoGrabberControlForm::AUpdateInterface(void)
{
}

// Сохраняет параметры интерфейса в xml
void TVideoGrabberControlForm::ASaveParameters(RDK::USerStorageXML &xml)
{
}

// Загружает параметры интерфейса из xml
void TVideoGrabberControlForm::ALoadParameters(RDK::USerStorageXML &xml)
{
}

// Создание копии этого компонента
TVideoGrabberControlForm* TVideoGrabberControlForm::New(TComponent *owner)
{
 return new TVideoGrabberControlForm(owner);
}
// -----------------------------



