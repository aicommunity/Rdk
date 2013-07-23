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
#include "TUVisualControllerFrameUnit.h"
#include <vector>
#include "myrdk.h"
#include "TUVisualController.h"

#pragma warn -8130
//---------------------------------------------------------------------------

class TUEngineMonitorFrame : public TUVisualControllerFrame
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
	TMenuItem *ools1;
	TMenuItem *SaveClassesDescriptions1;
	TMenuItem *LoadAllClassesDescriptions1;
	void __fastcall Start1Click(TObject *Sender);
	void __fastcall Pause1Click(TObject *Sender);
	void __fastcall Reset1Click(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall Step1Click(TObject *Sender);
	void __fastcall SaveClassesDescriptions1Click(TObject *Sender);
	void __fastcall LoadAllClassesDescriptions1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorFrame(TComponent* Owner);

// Режим расчетов
// 0 - простой расчет
// 1 - расчет в реальном времени
// 2 - простой расчет по сигналу
int CalculateMode;

// Сигнал, выставляемый при необходимости расчета
// сбрасывается при итерации счета
bool CalculateSignal;

// Временная метка сервера
long long ServerTimeStamp;

// Управление режимом расчетов
int GetCalculateMode(void) const;
void SetCalculateMode(int value);

void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorFrame *UEngineMonitorFrame;
//---------------------------------------------------------------------------
#endif
