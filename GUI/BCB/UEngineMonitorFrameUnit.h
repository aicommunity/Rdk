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
#include <map>
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "TUVisualController.h"
#include "TServerBroadcasterCommonUnit.h"

#pragma warn -8130

class TUEngineMonitorFrame;

/// Класс мониторинга за состоянием модулей приложения
class TEngineMonitorThread: public TThread
{
protected: // Параметры

protected: // Данные состояния модулей
/// Состояние тредов расчета
/// 0 - запущен
/// 1 - расчет остановлен
/// 2 - расчет запущен, но не выполняется
RDK::UELockVar<std::vector<int> > CalcThreadStates;

/// Состояние источников видеозахвата
/// 0 - остановлен
/// 1 - захват остановлен
/// 2 - захват запущен но не выполняется
RDK::UELockVar<std::vector<int> > VideoCaptureStates;

protected: // Внутренние данные
TUEngineMonitorFrame *EngineMonitorFrame;

/// Последние моменты времени опроса состояния тредов расчета
std::vector<double> CalcThreadStateTime;

/// Последние моменты времени успешного расчета
std::vector<RDK::ULongTime> CalcThreadSuccessTime;

/// Последние моменты времени опроса состояния тредов расчета
std::vector<double> VideoCaptureStateTime;

/// Последние моменты времени успешного расчета
std::vector<double> VideoCaptureSuccessTime;

/// Длина очереди для расчета среднего времени обработки канала
/// (число итераций для усреднения)
int NumAvgIterations;

/// Число раз, в которое время расчета должно превысить среднее время расчета
/// чтобы считать, что произошел останов
double AvgThreshold;

/// История последних моментов времени успешного расчета
std::vector<std::vector<double> > AvgIterations;

/// История последних моментов времени успешного захвата
std::vector<std::vector<double> > AvgCaptureIterations;

public:
// Событие состояния расчета. Выставлено на время активности расчета. Сбрасывается по стопу
HANDLE CalcState;

HANDLE CalcEnable;

HANDLE CalcStarted;

HANDLE CalculationNotInProgress;

protected: // Временные переменные


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TEngineMonitorThread(TUEngineMonitorFrame *engine_monitor_frame, bool CreateSuspended);
virtual __fastcall ~TEngineMonitorThread(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к данным состояния модулей
// --------------------------
/// Возвращает вектор состояний тредов
std::vector<int> ReadCalcThreadStates(void) const;

/// Возвращает вектор состояний источников видеозахвата
std::vector<int> ReadVideoCaptureStates(void) const;
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Execute(void);
// --------------------------
};



class TEngineThread: public TThread
{
protected: // Параметры
/// Индекс канала в библиотеке аналитики, управляемый тредом
int ChannelIndex;

/// Режим счета
RDK::UELockVar<int> CalculateMode;

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
RDK::UELockVar<RDK::UTime> MinInterstepsInterval;

/// Метка реального времени окончания последнего расчета
RDK::UELockVar<RDK::ULongTime> RealLastCalculationTime;

public:
// Событие состояния расчета. Выставлено на время активности расчета. Сбрасывается по стопу
HANDLE CalcState;

HANDLE CalcEnable;

HANDLE CalcStarted;

HANDLE CalculationNotInProgress;

RDK::UBitmap Source;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TEngineThread(int channel_index, int calculate_mode, RDK::UTime min_inerval, bool CreateSuspended);
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
bool SetMinInterstepsInterval(RDK::UTime value);
// --------------------------


// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Execute(void);

RDK::ULongTime GetRealLastCalculationTime(void) const;
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
	TPanel *Panel1;
	TCheckBox *ShowDebugMessagesCheckBox;
	TCheckBox *AutoupdatePropertiesCheckBox;
	void __fastcall Start1Click(TObject *Sender);
	void __fastcall Pause1Click(TObject *Sender);
	void __fastcall Reset1Click(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall Step1Click(TObject *Sender);
	void __fastcall SaveClassesDescriptions1Click(TObject *Sender);
	void __fastcall LoadAllClassesDescriptions1Click(TObject *Sender);
	void __fastcall RichEditMouseEnter(TObject *Sender);
	void __fastcall ShowDebugMessagesCheckBoxClick(TObject *Sender);
	void __fastcall AutoupdatePropertiesCheckBoxClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorFrame(TComponent* Owner);
	virtual __fastcall ~TUEngineMonitorFrame(void);

/// Режим работы
/// 0 - однопоточный (одноканальный) режим
/// 1 - многопоточный режим
RDK::UELockVar<int> ChannelsMode;

/// Режим использования времени для расчета
/// 0 - системное время
/// 1 - время источника данных
RDK::UELockVar<int> CalculationTimeSourceMode;

// Режим расчетов
// 0 - простой расчет
// 1 - расчет в реальном времени
// 2 - простой расчет по сигналу
std::vector<RDK::UELockVar<int> > CalculateMode;

// Временная метка последнего расчета
std::vector<RDK::UELockVar<RDK::ULongTime> > LastCalculatedServerTimeStamp;

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
std::vector<RDK::UELockVar<int> > MinInterstepsInterval;

protected:

// Сигнал, выставляемый при необходимости расчета
// сбрасывается при итерации счета
std::vector<RDK::UELockVar<bool> > CalculateSignal;

// Сигнал активности расчета
// сбрасывается по стопу
std::vector<RDK::UELockVar<bool> > CalculateState;

// Временная метка сервера
std::vector<RDK::UELockVar<RDK::ULongTime> > ServerTimeStamp;

/// Потоки запуска многоканальной аналитики
std::vector<TEngineThread*> ThreadChannels;

/// Метка реального времени окончания последнего расчета в однопоточном режиме
std::vector<RDK::UELockVar<RDK::ULongTime> > RealLastCalculationTime;

std::vector<RDK::UEPtr<TBroadcasterForm> > BroadcastersList;

HANDLE ThreadCalcCompleteEvent;

/// Поток мониторинга состояния сервера
TEngineMonitorThread *EngineMonitorThread;

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

void SetMinInterstepsInterval(int channel_index, RDK::UTime value);

// Управление временной меткой сервера
RDK::ULongTime GetServerTimeStamp(int channel_index) const;
void SetServerTimeStamp(int channel_index, RDK::ULongTime stamp);

/// Управление числом каналов
int GetNumChannels(void) const;
bool SetNumChannels(int num);
bool InsertChannel(int index);
bool DeleteChannel(int index);

void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

/// Регистрирует вещатель метаданных
void RegisterMetadataBroadcaster(TBroadcasterForm *broadcaster);

/// Снимает регистрацию вещателя метаданных
void UnRegisterMetadataBroadcaster(TBroadcasterForm *broadcaster);

/// Отправляет метаданные во все зарегистрированные вещатели
virtual bool AddMetadata(int channel_index, RDK::ULongTime time_stamp);

/// Инициирует процедуру отправки метаданных всеми зарегистрированными вещателями
virtual bool SendMetadata(void);

/// Запускает аналитику выбранного канала, или всех, если channel_index == -1
virtual void StartChannel(int channel_index);

/// Останавливает аналитику выбранного канала, или всех, если channel_index == -1
virtual void PauseChannel(int channel_index);

/// Сбрасывает аналитику выбранного канала, или всех, если channel_index == -1
virtual void ResetChannel(int channel_index);

/// Проверяет состояние расчета по id канала
/// 0 - Не считает
/// 1 - Идет расчет
virtual int CheckCalcState(int channel_id) const;

/// Доступ к треду мониторинга состояния модулей сервера
const TEngineMonitorThread* GetEngineMonitorThread(void) const;

/// Вклчает мониторинг сервера
void StartEngineMonitorThread(void);

/// Останавливает мониторинг сервера
void StopEngineMonitorThread(void);

TEngineThread* GetThreadChannel(int i);

/// Возвращает текущее время источника данных
double GetSourceTime(int i) const;

/// Применяет текущее время источников данных к каналам
void ApplySourceTime(void);
};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorFrame *UEngineMonitorFrame;
//---------------------------------------------------------------------------
#endif
