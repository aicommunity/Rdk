//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoGrabberControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TVideoGrabberControlFrameUnit"
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
void TVideoGrabberControlForm::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{
}

// Загружает параметры интерфейса из xml
void TVideoGrabberControlForm::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{
}
// -----------------------------

