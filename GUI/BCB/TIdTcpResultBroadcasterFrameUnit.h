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
	void __fastcall IdHTTPConnected(TObject *Sender);
	void __fastcall IdHTTPDisconnected(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TIdTcpResultBroadcasterFrame(TComponent* Owner);
	__fastcall ~TIdTcpResultBroadcasterFrame(void);


TMemoryStream *MemStream;
TBitmap *Bitmap;

std::string Metadata;

bool ConnectionEstablishedFlag;

long long LastSentTimeStamp;

// --------------------------
// Методы управления фреймом
// --------------------------
void ABeforeCalculate(void);
void AAfterCalculate(void);

void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// Создание копии этого компонента
TIdTcpResultBroadcasterFrame* New(TComponent *owner);
// --------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TIdTcpResultBroadcasterFrame *IdTcpResultBroadcasterFrame;
//---------------------------------------------------------------------------
#endif
