

//---------------------------------------------------------------------------

#ifndef TServerBroadcasterCommonUnitH
#define TServerBroadcasterCommonUnitH

#include <System.Classes.hpp>
#include <string>
#include <list>
#include "TUVisualControllerFormUnit.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"

struct TServerMetadata
{
/// Временная метка результата
RDK::ULongTime TimeStamp;

/// Идентификатор сервера
std::string ServerId;

/// Идентификатор канала
std::string ChannelId;

/// Индекс канала
int ChannelIndex;

/// Метаданные Xml
std::string Metadata;
};

class TResultBroadcasterThread: public TThread
{
protected: // Параметры
/// Данные для отправки в билижайшую сессию
std::list<TServerMetadata> MetaList;

/// Флаг разрешения отправки
RDK::UELockVar<bool> SendEnableFlag;

public:
HANDLE SendEnable;
HANDLE MetaUnlockEvent;
HANDLE SendNotInProgressEvent;

protected:

RDK::USerStorageXML SendXml;
std::string SendString;

RDK::UTransferPacket SendPacket;
std::vector<unsigned char> SendBuffer;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TResultBroadcasterThread(bool CreateSuspended);
virtual __fastcall ~TResultBroadcasterThread(void);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual bool __fastcall GenerateSendString(void);

virtual bool __fastcall Send(void);
virtual bool __fastcall ASend(void)=0;

virtual bool __fastcall PeriodicallyActions(void);

virtual void __fastcall Execute(void);

/// Добавляет метаданные в очередь
virtual bool __fastcall AddMetadataSafe(int channel_index, double time_stamp, const std::string &component_name, const std::string &property_name);

/// Флаг разрешения отправки
bool GetSendEnableFlag(void) const;
bool SetSendEnableFlag(bool value);
// --------------------------
};

class TBroadcasterForm: public TUVisualControllerForm, public RDK::UBroadcasterInterface
{
protected: // Параметры
/// Флаг, определяющий разрешение на использование этого вещателя
bool BroadcastEnableFlag;

protected: // Данные
/// Указатель на контроллер движка
RDK::UEngineControl *EngineControl;

public:
// ---------------------------
// ---------------------------
__fastcall TBroadcasterForm(TComponent* Owner);
virtual __fastcall ~TBroadcasterForm(void);
// ---------------------------

// ---------------------------
// Методы доступа к параметрам
// ---------------------------
bool GetBroadcastEnableFlag(void) const;
bool SetBroadcastEnableFlag(bool value);
// ---------------------------

// ---------------------------
// Методы управления
// ---------------------------
/// Управление контроллером движка
/// Движок должен быть задан до регистрации или включения вещателя
RDK::UEngineControl *GetEngineControl(void);
bool SetEngineControl(RDK::UEngineControl *engine_control);

bool RegisterToEngineControl(void);
bool UnRegisterFromEngineControl(void);

// Сохраняет параметры интерфейса в xml
void ASaveParameters(RDK::USerStorageXML &xml);
virtual void AASaveParameters(RDK::USerStorageXML &xml)=0;

// Загружает параметры интерфейса из xml
void ALoadParameters(RDK::USerStorageXML &xml);
virtual void AALoadParameters(RDK::USerStorageXML &xml)=0;
// ---------------------------
};


//---------------------------------------------------------------------------
#endif
