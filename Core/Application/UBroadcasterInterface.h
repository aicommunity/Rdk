//---------------------------------------------------------------------------

#ifndef UBroadcasterInterfaceH
#define UBroadcasterInterfaceH

#include "UIVisualController.h"
#include "../Engine/UTime.h"

namespace RDK {

class UEngineControl;

class RDK_LIB_TYPE UBroadcasterInterface
{
public:
// ---------------------------
// Методы доступа к параметрам
// ---------------------------
virtual bool GetBroadcastEnableFlag(void) const=0;
virtual bool SetBroadcastEnableFlag(bool value)=0;
// ---------------------------

// ---------------------------
// Методы управления
// ---------------------------
/// Управление контроллером движка
/// Движок должен быть задан до регистрации или включения вещателя
virtual UEngineControl *GetEngineControl(void)=0;
virtual bool SetEngineControl(UEngineControl *engine_control)=0;

/// Функция добавления метаданных в очередь на отправку в соответствии с настройками
virtual bool AddMetadata(int channel_index, double time_stamp)=0;

/// Инициирует процедуру отправки метаданных
virtual bool SendMetadata(void)=0;

virtual bool RegisterToEngineControl(void)=0;

virtual bool UnRegisterFromEngineControl(void)=0;
// ---------------------------
};

}
#endif
