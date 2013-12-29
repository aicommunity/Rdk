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
protected: // Параметры
/// Адрес принимающей стороны
std::string Address;

/// Порт принимающей стороны
int Port;

protected: // Данные
// Управляющий фрейм
TIdTcpResultBroadcasterFrame *Frame;

bool ConnectionEstablishedFlag;

long long LastSentTimeStamp;

TIdTCPClient *IdTCPClient;



public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TTcpResultBroadcasterThread(TIdTcpResultBroadcasterFrame * frame, bool CreateSuspended);
virtual __fastcall ~TTcpResultBroadcasterThread(void);
// --------------------------

// --------------------------
// Управление потоком
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
// Методы управления фреймом
// --------------------------
/// Инициализация канала связи в соответствии с настройками
bool Init(void);
bool UnInit(void);

/// Функция добавления метаданных в очередь на отправку в соответствии с настройками
bool AddMetadata(int channel_index, long long time_stamp);

void ABeforeCalculate(void);
void AAfterCalculate(void);

void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

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
