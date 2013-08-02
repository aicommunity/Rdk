//---------------------------------------------------------------------------

#ifndef TIdHttpResultBroadcasterFrameUnitH
#define TIdHttpResultBroadcasterFrameUnitH
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
class TIdHttpResultBroadcasterFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TIdHTTP *IdHTTP;
	TGroupBox *GroupBox2;
	TLabeledEdit *ServerAddressLabeledEdit;
	TLabeledEdit *XmlComponentNameLabeledEdit;
	TLabeledEdit *XmlComponentStateNameLabeledEdit;
	TLabeledEdit *ImagesComponentOutputNameLabeledEdit;
	TLabeledEdit *ImagesComponentNameLabeledEdit;
	TCheckBox *EnableXmlTranslationCheckBox;
	TCheckBox *EnableImagesTranslationCheckBox;
	TPanel *Panel1;
	TButton *ConnectButton;
	TButton *DisconnectButton;
	TLabeledEdit *ChannelIndexLabeledEdit;
	TPageProducer *PageProducer;
	void __fastcall ConnectButtonClick(TObject *Sender);
	void __fastcall DisconnectButtonClick(TObject *Sender);
	void __fastcall IdHTTPConnected(TObject *Sender);
	void __fastcall IdHTTPDisconnected(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TIdHttpResultBroadcasterFrame(TComponent* Owner);
	__fastcall ~TIdHttpResultBroadcasterFrame(void);


TMemoryStream *MemStream;
TBitmap *Bitmap;

bool ConnectionEstablishedFlag;

// --------------------------
// ������ ���������� �������
// --------------------------
void ABeforeCalculate(void);
void AAfterCalculate(void);

void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// �������� ����� ����� ����������
TIdHttpResultBroadcasterFrame* New(TComponent *owner);
// --------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TIdHttpResultBroadcasterFrame *IdHttpResultBroadcasterFrame;
//---------------------------------------------------------------------------
#endif
