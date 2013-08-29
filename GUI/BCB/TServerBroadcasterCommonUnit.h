//---------------------------------------------------------------------------

#ifndef TServerBroadcasterCommonUnitH
#define TServerBroadcasterCommonUnitH

#include <System.Classes.hpp>
#include <string>
#include <list>
#include "myrdk.h"

struct TServerMetadata
{
/// ��������� ����� ����������
long long TimeStamp;

/// ������������� �������
std::string ServerId;

/// ������������� ������
std::string ChannelId;

/// ������ ������
std::string ChannelIndex;

/// ���������� Xml
std::string Metadata;
};

class TResultBroadcasterThread: public TThread
{
protected: // ���������
/// ������ ��� �������� � ���������� ������
std::list<TServerMetadata> MetaList;

public:
HANDLE SendEnable;
HANDLE MetaUnlockEvent;

protected:

RDK::USerStorageXML SendXml;
std::string SendString;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TResultBroadcasterThread(bool CreateSuspended);
virtual __fastcall ~TResultBroadcasterThread(void);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual bool __fastcall GenerateSendString(void);

virtual bool __fastcall Send(void);
virtual bool __fastcall ASend(void)=0;

virtual void __fastcall Execute(void);
// --------------------------
};


//---------------------------------------------------------------------------
#endif
