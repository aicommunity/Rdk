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
class TEngineMonitorThread;
class TEngineThread;

class RDK_LIB_TYPE UEngineControlVcl: public RDK::UEngineControl
{
public:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineControlVcl(void);
virtual ~UEngineControlVcl(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
/// Создание нового треда расчета
virtual RDK::UEngineControlThread* CreateEngineThread(RDK::UEngineControl* engine_control, int engine_index);

/// Создание нового треда расчета
virtual RDK::UEngineStateThread* CreateEngineStateThread(RDK::UEngineControl* engine_control);

/// Запускает аналитику выбранного канала, или всех, если channel_index == -1
virtual void StartEngine(int channel_index);

/// Останавливает аналитику выбранного канала, или всех, если channel_index == -1
virtual void PauseEngine(int channel_index);
// --------------------------
};

/// Класс мониторинга за состоянием модулей приложения
class RDK_LIB_TYPE TEngineMonitorThread: public RDK::UEngineStateThread
{
protected: // Данные состояния модулей
/// Состояние источников видеозахвата
/// 0 - остановлен
/// 1 - захват остановлен
/// 2 - захват запущен но не выполняется
RDK::UELockVar<std::vector<int> > VideoCaptureStates;

protected: // Внутренние данные
/// Последние моменты времени опроса состояния тредов расчета
std::vector<double> VideoCaptureStateTime;

/// Последние моменты времени успешного расчета
std::vector<double> VideoCaptureSuccessTime;

/// История последних моментов времени успешного захвата
std::vector<std::vector<double> > AvgCaptureIterations;

public:

protected: // Временные переменные


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
TEngineMonitorThread(RDK::UEngineControl* engine_control);
virtual ~TEngineMonitorThread(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к данным состояния модулей
// --------------------------
/// Возвращает вектор состояний источников видеозахвата
std::vector<int> ReadVideoCaptureStates(void) const;
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
/// Возвращает класс-владелец потока
virtual UEngineControlVcl* GetEngineControl(void);

virtual void AdditionExecute(void);
// --------------------------
};



class RDK_LIB_TYPE TEngineThread: public RDK::UEngineControlThread
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
TEngineThread(RDK::UEngineControl* engine_control, int channel_index);
virtual ~TEngineThread(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
// --------------------------


// --------------------------
// Управление потоком
// --------------------------
/// Возвращает класс-владелец потока
virtual UEngineControlVcl* GetEngineControl(void);

virtual void ABeforeCalculate(void);
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
	TTimer *Timer;
	TMenuItem *ools1;
	TMenuItem *SaveClassesDescriptions1;
	TMenuItem *LoadAllClassesDescriptions1;
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall SaveClassesDescriptions1Click(TObject *Sender);
	void __fastcall LoadAllClassesDescriptions1Click(TObject *Sender);
	void __fastcall RichEditMouseEnter(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorFrame(TComponent* Owner);
	virtual __fastcall ~TUEngineMonitorFrame(void);

protected:

public:
void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

/// Доступ к треду мониторинга состояния модулей сервера
const TEngineMonitorThread* GetEngineMonitorThread(void) const;

TEngineThread* GetThreadChannel(int i);
};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorFrame *UEngineMonitorFrame;
//---------------------------------------------------------------------------
#endif
