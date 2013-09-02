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
int ChannelIndex;

/// ���������� Xml
std::string Metadata;
};

class TResultBroadcasterThread: public TThread
{
protected: // ���������
/// ������ ��� �������� � ���������� ������
std::list<TServerMetadata> MetaList;

/// ���� ���������� ��������
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

/// ��������� ���������� � �������
virtual bool __fastcall AddMetadataSafe(int channel_index, long long time_stamp, const std::string &component_name, const std::string &property_name);

/// ���� ���������� ��������
bool GetSendEnableFlag(void) const;
bool SetSendEnableFlag(bool value);
// --------------------------
};


//---------------------------------------------------------------------------
#endif
