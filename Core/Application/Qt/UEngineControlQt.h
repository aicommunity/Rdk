//---------------------------------------------------------------------------

#ifndef UEngineControlQtH
#define UEngineControlQtH
//---------------------------------------------------------------------------
#include <vector>
#include <map>
#include <QTimer>
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "TUVisualController.h"
#include "TServerBroadcasterCommonUnit.h"

class RDK_LIB_TYPE UEngineControlVcl: public RDK::UEngineControl
{
protected:
/// Таймер для однопоточного режима
QTimer *Timer;


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
virtual RDK::UEngineControlThread* CreateEngineThread(RDK::UEngineControl* engine_control, int channel_index);

/// Создание нового треда расчета
virtual RDK::UEngineStateThread* CreateEngineStateThread(RDK::UEngineControl* engine_control);

/// Запускает аналитику выбранного канала, или всех, если channel_index == -1
virtual void StartChannel(int channel_index);

/// Останавливает аналитику выбранного канала, или всех, если channel_index == -1
virtual void PauseChannel(int channel_index);
// --------------------------

protected:
/// Функция таймера
void TimerTimer(void);
};

#endif
