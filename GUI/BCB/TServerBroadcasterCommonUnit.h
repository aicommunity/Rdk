//---------------------------------------------------------------------------

#ifndef TServerBroadcasterCommonUnitH
#define TServerBroadcasterCommonUnitH

#include <System.Classes.hpp>
#include <string>
#include <list>
#include "TUVisualControllerFormUnit.h"
#include "myrdk.h"

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
bool SendEnableFlag;

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
virtual bool __fastcall AddMetadataSafe(int channel_index, RDK::ULongTime time_stamp, const std::string &component_name, const std::string &property_name);

/// Флаг разрешения отправки
bool GetSendEnableFlag(void) const;
bool SetSendEnableFlag(bool value);
// --------------------------
};

class TBroadcasterForm: public TUVisualControllerForm
{
protected: // Параметры
/// Флаг, определяющий разрешение на использование этого вещателя
bool BroadcastEnableFlag;

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
// ---------------------------
/// Функция добавления метаданных в очередь на отправку в соответствии с настройками
virtual bool AddMetadata(int channel_index, RDK::ULongTime time_stamp)=0;

/// Инициирует процедуру отправки метаданных
virtual bool SendMetadata(void)=0;

virtual bool RegisterToEngineMonitor(void);

virtual bool UnRegisterFromEngineMonitor(void);
// ---------------------------


// Сохраняет параметры интерфейса в xml
void ASaveParameters(RDK::USerStorageXML &xml);
virtual void AASaveParameters(RDK::USerStorageXML &xml)=0;

// Загружает параметры интерфейса из xml
void ALoadParameters(RDK::USerStorageXML &xml);
virtual void AALoadParameters(RDK::USerStorageXML &xml)=0;

};


//---------------------------------------------------------------------------
#endif
