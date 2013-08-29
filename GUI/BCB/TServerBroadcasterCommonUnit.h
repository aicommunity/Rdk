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
std::string ChannelIndex;

/// Метаданные Xml
std::string Metadata;
};

class TResultBroadcasterThread: public TThread
{
protected: // Параметры
/// Данные для отправки в билижайшую сессию
std::list<TServerMetadata> MetaList;

public:
HANDLE SendEnable;
HANDLE MetaUnlockEvent;

protected:

RDK::USerStorageXML SendXml;
std::string SendString;

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
// --------------------------
};


//---------------------------------------------------------------------------
#endif
