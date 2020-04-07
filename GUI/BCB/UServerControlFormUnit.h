//---------------------------------------------------------------------------

#ifndef UServerControlFormUnitH
#define UServerControlFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include "TUVisualControllerFormUnit.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ValEdit.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include "TUHttpServerUnit.h"
#include "TUVisualControllerFrameUnit.h"

#include "../../Deploy/Include/rdk_cpp_initdll.h"
//#include <Web.Win.Sockets.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdContext.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdTCPServer.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <IdAuthentication.hpp>

class RDK_LIB_TYPE UServerTransportTcpVcl: public RDK::UServerTransportTcp
{
public:
/// Читает входящие байты из выбранного источника, контекст привязки
/// всегда определяется строкой вне зависимости от типа транспорта
virtual int ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes);
/// Отправить ответ на команду соответствующему получателю
virtual void SendResponseBuffer(std::vector<unsigned char> buffer, std::string &responce_addr);
/// Задает адрес и порт входящего интерфейса сервера
virtual void SetServerBinding(std::string &interface_address, int port);
/// Получение адреса интерфейса управления сервером
virtual std::string GetServerBindingInterfaceAddress(void);
/// Получить порт сервера
int GetServerBindingPort(void) const;
/// Инициировать остановку сервера, отключить все приемники
virtual void ServerStop();
};

class RDK_LIB_TYPE UServerControlVcl: public RDK::UServerControl
{
public:
// --------------------------
// Методы управления вещателями
// --------------------------
/// Регистрирует удаленный приемник метаданных
virtual int RegisterMetadataReceiver(const std::string &address, int port);

/// Удаляет удаленный приемник метаданных
virtual int UnRegisterMetadataReceiver(const std::string &address, int port);
// --------------------------

private:
// --------------------------
/// Управление числом каналов
/// Выполнение вспомогательных методов
/// Вызывается из UApplication
// --------------------------
virtual bool ASetNumChannels(int old_num);
virtual bool AInsertChannel(int index);
virtual bool ADeleteChannel(int index);
// --------------------------

public: // TODO: костыль
// --------------------------
// Вспомогательные методы
// --------------------------
// Метод, вызываемый после сброса модели
virtual void AfterReset(void);

// Метод, вызываемый после шага расчета
virtual void AfterCalculate(void);
// --------------------------
};


class RDK_LIB_TYPE URpcDecoderCommonVcl: public RDK::URpcDecoderCommon
{
/// Строка результирующего ответа от обработчика сервера
std::string ControlResponseString;

std::vector<RDK::UParamT> binary_data;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDecoderCommonVcl(void);
virtual ~URpcDecoderCommonVcl(void);
// --------------------------

// --------------------------
// Методы управления командами
// --------------------------
/// Проверяет, поддерживается ли команда диспетчером
/// ожидает, что команда уже декодирована иначе всегда возвращает false
virtual bool IsCmdSupported(const RDK::UEPtr<RDK::URpcCommand> &command) const;

/// Создает копию этого декодера
virtual URpcDecoderCommonVcl* New(void);

virtual std::string ARemoteCall(const std::string &cmd, RDK::USerStorageXML &xml, const std::string &component_name, int engine_index, int &return_value);
// --------------------------
};


//---------------------------------------------------------------------------
class TUServerControlForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TTabSheet *ControlTabSheet;
	TTabSheet *OptionsTabSheet;
	TPanel *Panel1;
	TGroupBox *GroupBox1;
	TLabeledEdit *NumberOfChannelsLabeledEdit;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TChart *PerformanceChart;
	TBarSeries *Series1;
	TPanel *Panel;
	TRadioButton *FpsRadioButton;
	TRadioButton *RadioButton2;
	TPanel *Panel2;
	TUHttpServerFrame *UHttpServerFrame;
	TPanel *Panel3;
	TPanel *Panel4;
	TButton *ApplyOptionsButton;
	TButton *ReturnOptionsButton;
	TStringGrid *ChannelNamesStringGrid;
	TBarSeries *Series2;
	TBarSeries *Series3;
	TTimer *CommandTimer;
	TIdTCPServer *IdTCPServer;
	TTimer *ServerRestartTimer;
	TLabeledEdit *ServerNameLabeledEdit;
	TLabeledEdit *ServerIdLabeledEdit;
	TGroupBox *GroupBox4;
	TLabeledEdit *MetadataComponentNameLabeledEdit;
	TLabeledEdit *MetadataComponentStateNameLabeledEdit;
	TTabSheet *HttpServerTabSheet;
	TTimer *HttpCommandTimer;
	TTimer *HttpServerRestartTimer;
	TIdHTTP *IdHTTPServer;
	TGroupBox *GroupBox5;
	TLabeledEdit *HttpAddressPortLabeledEdit;
	TPanel *Panel5;
	TButton *Button3;
	TButton *Button4;
	TPanel *HttpServerIndicationPanel;
	TLabel *HttpServerIndicationLabel;
	TTabSheet *TcpServerTabSheet;
	TPanel *Panel6;
	TGroupBox *GroupBox6;
	TPanel *TcpServerIndicatorPanel;
	TLabel *TcpServerIndicatorLabel;
	TLabeledEdit *BindingAddressLabeledEdit;
	TLabeledEdit *ServerControlPortLabeledEdit;
	TButton *ServerStopButton;
	TButton *ServerStartButton;
	TLabeledEdit *HttpLoginLabeledEdit;
	TLabeledEdit *HttpPasswordLabeledEdit;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ServerStartButtonClick(TObject *Sender);
	void __fastcall ServerStopButtonClick(TObject *Sender);
	void __fastcall ReturnOptionsButtonClick(TObject *Sender);
	void __fastcall ApplyOptionsButtonClick(TObject *Sender);
	void __fastcall ChannelNamesStringGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall PageControlChange(TObject *Sender);
	void __fastcall CommandTimerTimer(TObject *Sender);
	void __fastcall IdTCPServerDisconnect(TIdContext *AContext);
	void __fastcall IdTCPServerExecute(TIdContext *AContext);
	void __fastcall IdTCPServerConnect(TIdContext *AContext);
	void __fastcall ServerRestartTimerTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall IdHTTPServerAuthorization(TObject *Sender, TIdAuthentication *Authentication,
          bool &Handled);







private:	// User declarations

public:		// User declarations
	__fastcall TUServerControlForm(TComponent* Owner);
	virtual __fastcall ~TUServerControlForm(void);

TMemoryStream* MemStream;
TBitmap *Bitmap;
RDK::UBitmap TempUBitmap;

RDK::UELockVar<RDK::URpcCommandInternal> CurrentProcessedMainThreadCommand;

/// Устанавливает параметры сервера
bool SetServerBinding(const std::string &interface_address, int port);
/// Возвращает параметры сервера
std::string GetServerBindingInterfaceAddress(void);
int GetServerBindingPort(void) const;

void ServerStop();

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
virtual void AAfterReset(void);

// Метод, вызываемый после шага расчета
virtual void AAfterCalculate(void);

// Обновление интерфейса
virtual void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------

// -----------------------------
// Обработчики команд сервера
// -----------------------------
/// Возвращает тип источника видео для канала
/// в соответствии с режимами VideoOutputFrame
int GetChannelVideoSource(int channel_id);

/// Задает источник видео для канала
/// в соответствии с режимами VideoOutputFrame
int SetChannelVideoSource(int channel_id, int source_mode);

/// Проверяет подключен ли видеоисточник
int CheckChannelVideoSourceConnection(int channel_id);

/// Загружает проект аналитики для канала
/// или загружает проект для всех каналов, если channel_id<0
int LoadProject(int channel_id, const std::string &project_file_name);

/// Сохраняет проект
int SaveProject(void);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUServerControlForm *UServerControlForm;
//---------------------------------------------------------------------------
#endif
