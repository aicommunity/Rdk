//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UClassesDescriptionsFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma link "UClassesDescriptionsFrameUnit"
#pragma resource "*.dfm"
TUClassesDescriptionsForm *UClassesDescriptionsForm;
//---------------------------------------------------------------------------
__fastcall TUClassesDescriptionsForm::TUClassesDescriptionsForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
void TUClassesDescriptionsForm::AAfterReset(void)
{

}

// Обновление интерфейса
void TUClassesDescriptionsForm::AUpdateInterface(void)
{

}

// Сохраняет параметры интерфейса в xml
void TUClassesDescriptionsForm::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TUClassesDescriptionsForm::ALoadParameters(RDK::USerStorageXML &xml)
{

}

// Создание копии этого компонента
TUClassesDescriptionsForm* TUClassesDescriptionsForm::New(TComponent *owner)
{
 return new TUClassesDescriptionsForm(owner);
}
// -----------------------------
//---------------------------------------------------------------------------
