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

class TEngineThread: public TThread
{
protected: // Параметры
/// Индекс канала в библиотеке аналитики, управляемый тредом
int ChannelIndex;

public:
HANDLE CalcEnable;

RDK::UBitmap Source;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TEngineThread(int channel_index, bool CreateSuspended);
virtual __fastcall ~TEngineThread(void);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void __fastcall BeforeCalculate(void);

void __fastcall AfterCalculate(void);

virtual void __fastcall Execute(void);
// --------------------------


};

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
	virtual __fastcall ~TUEngineMonitorFrame(void);

/// Режим работы
/// 0 - однопоточный (одноканальный) режим
/// 1 - многопоточный режим
int ChannelsMode;

// Режим расчетов
// 0 - простой расчет
// 1 - расчет в реальном времени
// 2 - простой расчет по сигналу
std::vector<int> CalculateMode;

// Временная метка последнего расчета
std::vector<long long> LastCalculatedServerTimeStamp;

protected:

// Сигнал, выставляемый при необходимости расчета
// сбрасывается при итерации счета
std::vector<bool> CalculateSignal;

// Временная метка сервера
std::vector<long long> ServerTimeStamp;

/// Потоки запуска многоканальной аналитики
std::vector<TEngineThread*> ThreadChannels;


public:
/// Управление режимом работы
/// 0 - однопоточный режим
/// 1 - многопоточный режим
int GetChannelsMode(void) const;
void SetChannelsMode(int mode);

// Управление режимом расчетов
int GetCalculateMode(int channel_index) const;
void SetCalculateMode(int channel_index, int value);

// Управление временной меткой сервера
long long GetServerTimeStamp(int channel_index) const;
void SetServerTimeStamp(int channel_index, long long stamp);

/// Управление числом каналов
int GetNumChannels(void) const;
bool SetNumChannels(int num);

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
