//---------------------------------------------------------------------------

#ifndef TIdTcpResultBroadcasterFrameUnitH
#define TIdTcpResultBroadcasterFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Web.HTTPApp.hpp>
#include <Web.HTTPProd.hpp>
#include "TServerBroadcasterCommonUnit.h"

class TIdTcpResultBroadcasterFrame;

class TTcpResultBroadcasterThread: public TResultBroadcasterThread
{
protected: // ���������
/// ����� ����������� �������
std::string Address;

/// ���� ����������� �������
int Port;

protected: // ������
// ����������� �����
TIdTcpResultBroadcasterFrame *Frame;

bool ConnectionEstablishedFlag;

long long LastSentTimeStamp;

TIdTCPClient *IdTCPClient;



public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TTcpResultBroadcasterThread(TIdTcpResultBroadcasterFrame * frame, bool CreateSuspended);
virtual __fastcall ~TTcpResultBroadcasterThread(void);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual bool Init(const std::string &address, int port);
virtual const std::string& GetAddress(void) const;
virtual int GetPort(void) const;

virtual void Connect(void);
virtual void Disconnect(void);

bool __fastcall ASend(void);

void __fastcall IdTCPConnected(TObject *Sender);
void __fastcall IdTCPDisconnected(TObject *Sender);
// --------------------------
};


//---------------------------------------------------------------------------
class TIdTcpResultBroadcasterFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox2;
	TLabeledEdit *ServerAddressLabeledEdit;
	TLabeledEdit *XmlComponentNameLabeledEdit;
	TLabeledEdit *XmlComponentStateNameLabeledEdit;
	TCheckBox *EnableXmlTranslationCheckBox;
	TPanel *Panel1;
	TButton *ConnectButton;
	TButton *DisconnectButton;
	TLabeledEdit *ChannelIndexLabeledEdit;
	TIdTCPClient *IdTCPClient;
	void __fastcall ConnectButtonClick(TObject *Sender);
	void __fastcall DisconnectButtonClick(TObject *Sender);
	void __fastcall EnableXmlTranslationCheckBoxClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TIdTcpResultBroadcasterFrame(TComponent* Owner);
	__fastcall ~TIdTcpResultBroadcasterFrame(void);

TTcpResultBroadcasterThread* Thread;

TMemoryStream *MemStream;
TBitmap *Bitmap;

std::string Metadata;

bool ConnectionEstablishedFlag;

long long LastSentTimeStamp;

// --------------------------
// ������ ���������� �������
// --------------------------
/// ������������� ������ ����� � ������������ � �����������
bool Init(void);
bool UnInit(void);

/// ������� ���������� ���������� � ������� �� �������� � ������������ � �����������
bool AddMetadata(int channel_index, long long time_stamp);

void ABeforeCalculate(void);
void AAfterCalculate(void);

void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// �������� ����� ����� ����������
TIdTcpResultBroadcasterFrame* New(TComponent *owner);
// --------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TIdTcpResultBroadcasterFrame *IdTcpResultBroadcasterFrame;
//---------------------------------------------------------------------------
#endif
