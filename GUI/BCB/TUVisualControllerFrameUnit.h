//---------------------------------------------------------------------------

#ifndef TUVisualControllerFrameUnitH
#define TUVisualControllerFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <string>
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

// Флаг, разрешающий обновление интерфейса, даже если объект не видим
bool AlwaysUpdateFlag;

// Длинное имя управляемого компонента модели (опционально)
std::string ComponentControlName;

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

// Обновление интерфейса
// Если force_update == true, то интерфейс обновляется
// вне зависимости от UpdateInterval
virtual void UpdateInterface(bool force_update=true);
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
//---------------------------------------------------------------------------
extern PACKAGE TUVisualControllerFrame *UVisualControllerFrame;
//---------------------------------------------------------------------------
#pragma warn .8130
#endif
