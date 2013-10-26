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
	TTimer *Timer1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
private:	// User declarations
public:		// User declarations


TEngineThread *Thread;
RDK::UTransferPacket Packet;
RDK::UTransferReader PacketReader;
//RDK::UParamT ClientBuffer;
std::string PacketXml;

//HANDLE PacketReceivedEvent;
//HANDLE PacketReaderUnlockEvent;

// ���������� �������
int PrepareCommandXml(RDK::USerStorageXML &xml, const char* cmd, int channel);

// ���������� ������� ��� ������
int PreparePtzControlXml(RDK::USerStorageXML &xml, const std::string &cmd, const std::string &camera, int channel);

// ������� �������
void SendControlCommand(RDK::USerStorageXML &xml);

// ����� ������ �� id ��������
// ���������� true � ������ ������
bool FindPacketById(int id, RDK::USerStorageXML &xml);

// �������� ������ �� �������
//const char* WaitData(void);

	__fastcall TDllMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDllMainForm *DllMainForm;
//---------------------------------------------------------------------------
#endif
