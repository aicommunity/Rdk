//---------------------------------------------------------------------------

#ifndef TUVisualControllerFormUnitH
#define TUVisualControllerFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "myrdk.h"

//---------------------------------------------------------------------------
#pragma warn -8130
class TUVisualControllerForm : public TForm, public RDK::UIVisualController
{
__published:	// IDE-managed Components
private:	// User declarations
public:		// User declarations
	__fastcall TUVisualControllerForm(TComponent* Owner);
	__fastcall ~TUVisualControllerForm(void);

protected:
bool UpdateInterfaceFlag;

// Длинное имя управляемого компонента модели (опционально)
std::string ComponentControlName;

public:
// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед сбросом модели
virtual void BeforeReset(void);
virtual void ABeforeReset(void);

// Метод, вызываемый после сброса модели
virtual void AfterReset(void);
virtual void AAfterReset(void);

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

// Управление длинным именем управляемого компонента
// Длинное имя управляемого компонента модели (опционально)
const std::string& GetComponentControlName(void) const;
bool SetComponentControlName(const std::string& name);
// -----------------------------
};

// --------------------------
// Вспомогательные функции сериализации
// --------------------------
// Сохраняет данные положения формы в xml
void SaveFormPosition(RDK::Serialize::USerStorageXML &xml, TForm *form);

// Загружает данные положения формы из xml
void LoadFormPosition(RDK::Serialize::USerStorageXML &xml, TForm *form);
// --------------------------

//---------------------------------------------------------------------------
extern PACKAGE TUVisualControllerForm *UVisualControllerForm;
//---------------------------------------------------------------------------
#endif
