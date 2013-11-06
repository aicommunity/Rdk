//---------------------------------------------------------------------------

#ifndef DllMainFormUnitH
#define DllMainFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <IdIcmpClient.hpp>
#include <IdRawBase.hpp>
#include <IdRawClient.hpp>
#include <vector>
#include <map>
#include <string>
#include "../../../Deploy/Include/myrdk.h"
//---------------------------------------------------------------------------
class TEngineThread: public TThread
{
protected: // ���������
TIdTCPClient *IdTCPClient;

RDK::UParamT ClientBuffer;

RDK::UTransferReader *PacketReader;

public:
HANDLE CalcEnable;

HANDLE CalcStarted;

HANDLE CalculationNotInProgress;

HANDLE PacketReceivedEvent;

HANDLE PacketReaderUnlockEvent;

RDK::UBitmap Source;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TEngineThread(TIdTCPClient* client, RDK::UTransferReader &packetreader, bool CreateSuspended);
virtual __fastcall ~TEngineThread(void);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
void __fastcall BeforeCalculate(void);

void __fastcall AfterCalculate(void);

virtual void __fastcall Execute(void);
// --------------------------


};


class TDllMainForm : public TForm
{
__published:	// IDE-managed Components
	TIdTCPClient *IdTCPClient;
	TIdIcmpClient *IdIcmpClient;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations


TEngineThread *Thread;
RDK::UTransferPacket Packet;
RDK::UTransferReader PacketReader;
std::string PacketXml;


// ���������� �������
int PrepareCommandXml(RDK::USerStorageXML &xml, const char* cmd, int channel);

// ���������� ������� ��� ������
int PreparePtzControlXml(RDK::USerStorageXML &xml, const std::string &cmd, const std::string &camera, int channel);

// ������� �������
void SendControlCommand(RDK::USerStorageXML &xml);

// ����� ������ �� id ��������
// ���������� true � ������ ������
int FindPacketById(int cmdId, RDK::USerStorageXML &xml);

// �������� ������ �������
int WaitServerResponse(int cmdId, RDK::USerStorageXML &response, int timeout);

// ��������� ������ �� ������� sep
int SeparateString(const std::string &str, std::vector<std::string> &output, char sep, int num=0, int *lastpos=0);
	__fastcall TDllMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDllMainForm *DllMainForm;
//---------------------------------------------------------------------------
#endif
