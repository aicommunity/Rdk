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
/// ��������� ����� ����������
RDK::ULongTime TimeStamp;

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

virtual bool __fastcall PeriodicallyActions(void);

virtual void __fastcall Execute(void);

/// ��������� ���������� � �������
virtual bool __fastcall AddMetadataSafe(int channel_index, RDK::ULongTime time_stamp, const std::string &component_name, const std::string &property_name);

/// ���� ���������� ��������
bool GetSendEnableFlag(void) const;
bool SetSendEnableFlag(bool value);
// --------------------------
};

class TBroadcasterForm: public TUVisualControllerForm
{
protected: // ���������
/// ����, ������������ ���������� �� ������������� ����� ��������
bool BroadcastEnableFlag;

public:
// ---------------------------
// ---------------------------
__fastcall TBroadcasterForm(TComponent* Owner);
virtual __fastcall ~TBroadcasterForm(void);
// ---------------------------

// ---------------------------
// ������ ������� � ����������
// ---------------------------
bool GetBroadcastEnableFlag(void) const;
bool SetBroadcastEnableFlag(bool value);
// ---------------------------

// ---------------------------
// ---------------------------
/// ������� ���������� ���������� � ������� �� �������� � ������������ � �����������
virtual bool AddMetadata(int channel_index, RDK::ULongTime time_stamp)=0;

/// ���������� ��������� �������� ����������
virtual bool SendMetadata(void)=0;

virtual bool RegisterToEngineMonitor(void);

virtual bool UnRegisterFromEngineMonitor(void);
// ---------------------------


// ��������� ��������� ���������� � xml
void ASaveParameters(RDK::USerStorageXML &xml);
virtual void AASaveParameters(RDK::USerStorageXML &xml)=0;

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::USerStorageXML &xml);
virtual void AALoadParameters(RDK::USerStorageXML &xml)=0;

};


//---------------------------------------------------------------------------
#endif
