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
#include <vector>
#include <string>
#include "UComponentsListFormUnit.h"
#include "myrdk.h"
//---------------------------------------------------------------------------
class TUComponentsPerformanceFrame : public TFrame, public RDK::IVisualInterface
{
__published:	// IDE-managed Components
	TChart *Chart;
	TBarSeries *Series1;
	TBarSeries *Series2;
	TPopupMenu *PopupMenu;
	TMenuItem *SelectSource1;
	TMenuItem *N1;
	TMenuItem *ClearAll1;
	void __fastcall SelectSource1Click(TObject *Sender);
	void __fastcall ClearAll1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsPerformanceFrame(TComponent* Owner);
	__fastcall ~TUComponentsPerformanceFrame(void);

// Массив длинных имен наблюдаемых компонент
std::vector<std::string> ComponentNames;

// Флаг текущего обновления интерфейса
bool UpdateInterfaceFlag;

// Указатель на форму выбора компоненты-источника
TUComponentsListForm *MyComponentsListForm;

// --------------------------
// Методы управления фреймом
// --------------------------
void BeforeCalculate(void);
void AfterCalculate(void);

// Обновляет интерфейс
void UpdateInterface(void);

// Сохраняет информацию об источниках данных в заданный ini файл
void SaveToIni(TMemIniFile *ini, const String &section);

// Загружает информацию об источниках данных из заданного ini файла
void LoadFromIni(TMemIniFile *ini, const String &section);
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
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsPerformanceFrame *UComponentsPerformanceFrame;
//---------------------------------------------------------------------------
#endif
