//---------------------------------------------------------------------------

#ifndef TUVisualControllerFormUnitH
#define TUVisualControllerFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "../../Deploy/Include/myrdk.h"

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
static RDK::UELockVar<bool> CalculationModeFlag;

bool CalculationStepUpdatedFlag;

/// Флаг, разрешающий проверку на существование модели перед обновлением интерфейса
bool CheckModelFlag;

/// Время, потраченное на обновление интерфейса
unsigned long long UpdateTime;


public:
// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после загрузки проекта
virtual void AfterLoadProject(void);
virtual void AAfterLoadProject(void);

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
virtual void UpdateInterface(bool force_update=false);
virtual void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void ClearInterface(void);
virtual void AClearInterface(void);

// Возвращает уникальное имя интерфейса
virtual std::string GetName(void);

// Возвращает имя класса интерфейса
virtual std::string GetClassName(void);

// Возвращает интервал обновления интерфейса
virtual long GetUpdateInterval(void);

// Возвращает флаг разрешения обновления интерфейса даже если он не виден
virtual bool GetAlwaysUpdateFlag(void);

// Сохраняет параметры интерфейса в xml
virtual void SaveParameters(RDK::USerStorageXML &xml);
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void LoadParameters(RDK::USerStorageXML &xml);
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// Управление длинным именем управляемого компонента
// Длинное имя управляемого компонента модели (опционально)
const std::string& GetComponentControlName(void) const;
virtual bool SetComponentControlName(const std::string& name);

// Создание копии этого компонента
virtual TUVisualControllerForm* New(TComponent *owner=0)=0;

// Служебные методы управления интерфейсом
/// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
virtual void ResetCalculationStepUpdatedFlag(void);

/// Выставляет флаг прошедшей перерисовки в этой итерации счета
virtual void SetCalculationStepUpdatedFlag(void);

/// Возвращает состояние флага прошедшей перерисовки в этой итерации счета
virtual bool GetCalculationStepUpdatedFlag(void);

/// Возвращает время обновления интерфейса (мс)
virtual unsigned long long GetUpdateTime(void);
// -----------------------------

// --------------------------
// Вспомогательные функции сериализации
// --------------------------
// Сохраняет данные положения формы в xml
virtual void SaveFormPosition(RDK::USerStorageXML &xml);

// Загружает данные положения формы из xml
virtual void LoadFormPosition(RDK::USerStorageXML &xml);
// --------------------------
};

//---------------------------------------------------------------------------
extern PACKAGE TUVisualControllerForm *UVisualControllerForm;
//---------------------------------------------------------------------------
#endif
