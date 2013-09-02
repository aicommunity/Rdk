//---------------------------------------------------------------------------

#ifndef TServerBroadcasterCommonUnitH
#define TServerBroadcasterCommonUnitH

#include <System.Classes.hpp>
#include <string>
#include <list>
#include "myrdk.h"

struct TServerMetadata
{
/// Временная метка результата
long long TimeStamp;

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

virtual void __fastcall Execute(void);

/// Добавляет метаданные в очередь
virtual bool __fastcall AddMetadataSafe(int channel_index, long long time_stamp, const std::string &component_name, const std::string &property_name);

/// Флаг разрешения отправки
bool GetSendEnableFlag(void) const;
bool SetSendEnableFlag(bool value);
// --------------------------
};


//---------------------------------------------------------------------------
#endif
