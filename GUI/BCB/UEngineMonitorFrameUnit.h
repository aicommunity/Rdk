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
#include "TServerBroadcasterCommonUnit.h"

#pragma warn -8130

class TEngineThread: public TThread
{
protected: // Параметры
/// Индекс канала в библиотеке аналитики, управляемый тредом
int ChannelIndex;

/// Режим счета
int CalculateMode;

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
int MinInterstepsInterval;

/// Метка реального времени окончания последнего расчета
unsigned long long RealLastCalculationTime;

public:
HANDLE CalcEnable;

HANDLE CalcStarted;

HANDLE CalculationNotInProgress;

RDK::UBitmap Source;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TEngineThread(int channel_index, int calculate_mode, int min_inerval, bool CreateSuspended);
virtual __fastcall ~TEngineThread(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Режим счета
int GetCalculateMode(void) const;
bool SetCalculateMode(int value);

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
int GetMinInterstepsInterval(void) const;
bool SetMinInterstepsInterval(int value);
// --------------------------


// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

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
	void __fastcall RichEditMouseEnter(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorFrame(TComponent* Owner);
	virtual __fastcall ~TUEngineMonitorFrame(void);

/// Режим работы
/// 0 - однопоточный (одноканальный) режим
/// 1 - многопоточный режим
int ChannelsMode;

/// Режим использования времени для расчета
/// 0 - системное время
/// 1 - время источника данных
int CalculationTimeSourceMode;

// Режим расчетов
// 0 - простой расчет
// 1 - расчет в реальном времени
// 2 - простой расчет по сигналу
std::vector<int> CalculateMode;

// Временная метка последнего расчета
std::vector<long long> LastCalculatedServerTimeStamp;

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
std::vector<int> MinInterstepsInterval;

protected:

// Сигнал, выставляемый при необходимости расчета
// сбрасывается при итерации счета
std::vector<bool> CalculateSignal;

// Временная метка сервера
std::vector<long long> ServerTimeStamp;

/// Потоки запуска многоканальной аналитики
std::vector<TEngineThread*> ThreadChannels;

/// Метка реального времени окончания последнего расчета в однопоточном режиме
std::vector<unsigned long long> RealLastCalculationTime;

std::vector<RDK::UEPtr<TBroadcasterForm> > BroadcastersList;

HANDLE ThreadCalcCompleteEvent;

public:
/// Управление режимом работы
/// 0 - однопоточный режим
/// 1 - многопоточный режим
int GetChannelsMode(void) const;
void SetChannelsMode(int mode);

// Управление режимом расчетов
int GetCalculateMode(int channel_index) const;
void SetCalculateMode(int channel_index, int value);

/// Режим использования времени для расчета
/// 0 - системное время
/// 1 - время источника данных
int GetCalculationTimeSourceMode(void) const;
bool SetCalculationTimeSourceMode(int value);

void SetMinInterstepsInterval(int channel_index, int value);

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

/// Регистрирует вещатель метаданных
void RegisterMetadataBroadcaster(TBroadcasterForm *broadcaster);

/// Снимает регистрацию вещателя метаданных
void UnRegisterMetadataBroadcaster(TBroadcasterForm *broadcaster);

/// Отправляет метаданные во все зарегистрированные вещатели
virtual bool AddMetadata(int channel_index, long long time_stamp);

/// Инициирует процедуру отправки метаданных всеми зарегистрированными вещателями
virtual bool SendMetadata(void);

/// Запускает аналитику выбранного канала, или всех, если channel_index == -1
virtual void StartChannel(int channel_index);

/// Останавливает аналитику выбранного канала, или всех, если channel_index == -1
virtual void PauseChannel(int channel_index);

/// Сбрасывает аналитику выбранного канала, или всех, если channel_index == -1
virtual void ResetChannel(int channel_index);
};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorFrame *UEngineMonitorFrame;
//---------------------------------------------------------------------------
#endif
