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

// Флаг, разрешающий обновление интерфейса, даже если объект не видим
bool AlwaysUpdateFlag;

// Длинное имя управляемого компонента модели (опционально)
std::string ComponentControlName;

// Заголовок окна без указания управляемого компонента
std::string PureFormCaption;

// Интервал обновления интерфейса в миллисекундах при счете
// (если =0) то интерфейс не обновляется при счете
long UpdateInterval;

// Время последнего обновления
DWORD LastUpdateTime;

public:
// Флаг, сообщающий что идет расчет
static bool CalculationModeFlag;

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

// Если force_update == true, то интерфейс обновляется
// вне зависимости от UpdateInterval
virtual void UpdateInterface(bool force_update=true);
virtual void AUpdateInterface(void);

// Возвращает уникальное имя интерфейса
virtual std::string GetName(void);

// Сохраняет параметры интерфейса в xml
virtual void SaveParameters(RDK::USerStorageXML &xml);
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void LoadParameters(RDK::USerStorageXML &xml);
virtual void ALoadParameters(RDK::USerStorageXML &xml);

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
void SaveFormPosition(RDK::USerStorageXML &xml, TForm *form);

// Загружает данные положения формы из xml
void LoadFormPosition(RDK::USerStorageXML &xml, TForm *form);
// --------------------------

//---------------------------------------------------------------------------
extern PACKAGE TUVisualControllerForm *UVisualControllerForm;
//---------------------------------------------------------------------------
#endif
