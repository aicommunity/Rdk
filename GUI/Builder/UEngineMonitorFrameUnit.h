//---------------------------------------------------------------------------

#ifndef UEngineMonitorFrameUnitH
#define UEngineMonitorFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <IniFiles.hpp>
#include <vector>
#include "myrdk.h"

//---------------------------------------------------------------------------

class TUEngineMonitorFrame : public TFrame, public RDK::IVisualInterface
{
__published:	// IDE-managed Components
	TRichEdit *RichEdit;
	TStatusBar *StatusBar;
	TMainMenu *MainMenu;
	TMenuItem *Control1;
	TMenuItem *Calculate1;
	TMenuItem *Start1;
	TMenuItem *Pause1;
	TMenuItem *Reset1;
	TTimer *Timer;
	TMenuItem *Step1;
	void __fastcall Start1Click(TObject *Sender);
	void __fastcall Pause1Click(TObject *Sender);
	void __fastcall Reset1Click(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall Step1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorFrame(TComponent* Owner);

// Режим расчетов
// 0 - простой расчет
// 1 - расчет в реальном времени
int CalculateMode;

bool UpdateInterfaceFlag;

// Список обработчиков, которые должны быть вызваны после расчета
std::vector<RDK::IVisualInterface*> InterfaceUpdaters;

// Управление режимом расчетов
int GetCalculateMode(void) const;
void SetCalculateMode(int value);

// Добавляет обработчик в список
void AddInterface(RDK::IVisualInterface *value);

// Удаляет обработчик из списка
void DelInterface(RDK::IVisualInterface *value);

void UpdateInterface(void);

// Сохраняет информацию в заданный ini файл
void SaveToIni(TMemIniFile *ini, const String &section);

// Загружает информацию из заданного ini файла
void LoadFromIni(TMemIniFile *ini, const String &section);
};
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorFrame *UEngineMonitorFrame;
//---------------------------------------------------------------------------
#endif
