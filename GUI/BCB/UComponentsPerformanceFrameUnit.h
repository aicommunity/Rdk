//---------------------------------------------------------------------------

#ifndef UComponentsPerformanceFrameUnitH
#define UComponentsPerformanceFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.Series.hpp>
#include <Vcl.Menus.hpp>
#include <IniFiles.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.Samples.Spin.hpp>
#include <vector>
#include <string>
#include "UComponentsListFormUnit.h"
#include "myrdk.h"
#include "TUVisualController.h"
//---------------------------------------------------------------------------
#pragma warn -8130
class TUComponentsPerformanceFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TChart *Chart;
	TBarSeries *Series1;
	TBarSeries *Series2;
	TPopupMenu *PopupMenu;
	TMenuItem *SelectSource1;
	TMenuItem *N1;
	TMenuItem *ClearAll1;
	TPanel *Panel1;
	TRadioGroup *ShowModeRadioGroup;
	TGroupBox *GroupBox1;
	TSpinEdit *AverageIntervalSpinEdit;
	void __fastcall SelectSource1Click(TObject *Sender);
	void __fastcall ClearAll1Click(TObject *Sender);
	void __fastcall AverageIntervalSpinEditChange(TObject *Sender);
	void __fastcall ShowModeRadioGroupClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsPerformanceFrame(TComponent* Owner);
	__fastcall ~TUComponentsPerformanceFrame(void);

// Массив длинных имен наблюдаемых компонент
std::vector<std::string> ComponentNames;

// Результаты измерений
std::vector<std::vector<long long> > ComponentData;

// Указатель на форму выбора компоненты-источника
TUComponentsListForm *MyComponentsListForm;

// Число итераций усреднения
int AverageIterations;

// --------------------------
// Методы управления фреймом
// --------------------------
void ABeforeCalculate(void);
void AAfterCalculate(void);

// Обновляет интерфейс
void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
void ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// Возвращает число наблюдаемых компонент
std::size_t GetNumComponents(void) const;

// Добавляет компонент для мониторинга
void AddComponent(const std::string &componentname);

// Добавляет все компоненты, содержащиеся непосредственно в выбранном компоненте
void AddAllComponents(const std::string &componentname);

// Удаляет наблюдаемый компонент
void DelComponent(const std::string &componentname);
void DelComponent(std::size_t index);

// Удаляет все наблюдаемые компоненты
void ClearComponents(void);
};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsPerformanceFrame *UComponentsPerformanceFrame;
//---------------------------------------------------------------------------
#endif
