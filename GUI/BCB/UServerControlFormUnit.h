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

#include "myrdk.h"
#include <Web.Win.Sockets.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdContext.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdTCPServer.hpp>

//typedef std::map<std::string,std::vector<char> > UServerCommand;
typedef std::pair<std::string, RDK::UParamT> UServerCommand;

/// ����������� �������, �������������� ������������� ���������� �������
int StandardCommandRequestDecoder(UServerCommand &source, UServerCommand &dest);

/// ����������� �������, �������������� ����������� ���������� ������
int StandardCommandResponseEncoder(const std::string &response_type, RDK::UParamT &source, RDK::UParamT &dest);

//---------------------------------------------------------------------------
class TUServerControlForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TTabSheet *ControlTabSheet;
	TTabSheet *OptionsTabSheet;
	TPanel *Panel1;
	TGroupBox *GroupBox1;
	TLabeledEdit *ServerControlPortLabeledEdit;
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
	TButton *ServerStartButton;
	TButton *ServerStopButton;
	TButton *ApplyOptionsButton;
	TButton *ReturnOptionsButton;
	TStringGrid *ChannelNamesStringGrid;
	TBarSeries *Series2;
	TBarSeries *Series3;
	TTimer *CommandTimer;
	TTcpServer *TcpServer;
	TIdTCPServer *IdTCPServer;
	TTimer *ServerRestartTimer;
	TLabeledEdit *ServerNameLabeledEdit;
	TLabeledEdit *ServerIdLabeledEdit;
	TLabeledEdit *BindingAddressLabeledEdit;
	TGroupBox *GroupBox4;
	TLabeledEdit *MetadataComponentNameLabeledEdit;
	TLabeledEdit *MetadataComponentStateNameLabeledEdit;
	void __fastcall UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
          TIdHTTPResponseInfo *AResponseInfo);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ServerStartButtonClick(TObject *Sender);
	void __fastcall ServerStopButtonClick(TObject *Sender);
	void __fastcall ReturnOptionsButtonClick(TObject *Sender);
	void __fastcall ApplyOptionsButtonClick(TObject *Sender);
	void __fastcall ChannelNamesStringGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall PageControlChange(TObject *Sender);
	void __fastcall CommandTimerTimer(TObject *Sender);
	void __fastcall TcpServerAccept(TObject *Sender, TCustomIpClient *ClientSocket);
	void __fastcall TcpServerListening(TObject *Sender);
	void __fastcall TcpServerGetThread(TObject *Sender, TClientSocketThread *&ClientSocketThread);
	void __fastcall IdTCPServerDisconnect(TIdContext *AContext);
	void __fastcall IdTCPServerExecute(TIdContext *AContext);
	void __fastcall IdTCPServerConnect(TIdContext *AContext);
	void __fastcall ServerRestartTimerTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);







private:	// User declarations
public:		// User declarations
	__fastcall TUServerControlForm(TComponent* Owner);
	virtual __fastcall ~TUServerControlForm(void);

// -----------------
// ��������� �������
// -----------------
/// ���� ���������� ������� ������� ��� ������
bool AutoStartFlag;

/// ������ ���������� ���� �������
std::vector<std::string> ChannelNames;

/// ���������� ��������� ������������������, ��
std::vector<std::vector<long long> > ModelPerformanceResults;
std::vector<std::vector<long long> > TransportPerformanceResults;

std::string ServerName;

std::string ServerId;

std::string MetaComponentName;

std::string MetaComponentStateName;

/// ����� ����� ���������� ������ ������������������
int AverageIterations;
// -----------------

/// ��������� �� �������, �������������� ������������� ���������� �������
int (*CommandRequestDecoder)(UServerCommand &source, UServerCommand &dest);

/// ��������� �� �������, �������������� ����������� ���������� ������
int (*CommandResponseEncoder)(const std::string &response_type, RDK::UParamT &source, RDK::UParamT &dest);

/// ����� (���) �������
/// 0 - ��������� � ������� ���������� ��������� (Control)
/// 1 - ��������� ����� �������� ������ ��������� ������
/// 1 - ��������� � ������ (Engine)
/// 2 - ��������� � ������ (Model)
/// 2 - ��������� � ����� ���������� (Env)
/// 3 - ��������� � ��������� ��������� (Storage)
/// <0 - ����������� ��� �������
int Mode;

/// ������������ ������ ������� �������
UServerCommand DecodedRequest;

/// �����
RDK::UParamT Response;

/// ����������� ��� �������� �����
RDK::UParamT EncodedResponse;

/// ��� ������
std::string ResponseType;

/// ������� �������
std::string Command;

/// ������ ������ �������
std::string ChannelIndex;

/// ������ ��������������� ������ �� ����������� �������
std::string ControlResponseString;

TMemoryStream* MemStream;
TBitmap *Bitmap;
RDK::UBitmap TempUBitmap;

// ������ ����������� ������ � ������ ������ ������������������
int PerformancePushIndex;

//RDK::ExternalPtzControl PtzControl;

HANDLE CommandQueueUnlockEvent;

// ������� ������
std::list<UServerCommand > CommandQueue;

std::map<std::string, RDK::UTransferReader> PacketReaders;

//RDK::UTransferPacket Packet;
std::string PacketXml;

UServerCommand CurrentProcessedCommand;

TThreadList *Clients;

TCriticalSection* CriticalSection;


const char* ControlRemoteCall(const char *request, int &return_value);

//const char* PtzRemoteCall(const char *request, int &return_value);

// �������, �������������� ������� ���������� ��������
// ���������� true ���� ������� ���� ������� � ����������
//bool ProcessControlCommand(const std::string &cmd_name, UServerCommand &args, std::string &response_type, UParamT &response_data);
bool ProcessControlCommand(const UServerCommand &args, std::string &response_type, RDK::UParamT &response_data);

// �������, �������������� ������� ���������� ������ ��������
// ���������� true ���� ������� ���� ������� � ����������
//bool ProcessRPCCommand(int channel, const std::string &cmd_name, UServerCommand &args, std::string &response_type, UParamT &response_data);
bool ProcessRPCCommand(const UServerCommand &args, std::string &response_type, RDK::UParamT &response_data);

// �����, �������������� ������� ���������� PTZ ��������
bool ProcessPtzCommand(const UServerCommand &args, std::string &response_type, RDK::UParamT &response_data);

/// �������� ������ � ������
void ConvertStringToVector(const std::string &source, RDK::UParamT &dest);

/// �������� ������ � ������
void ConvertVectorToString(const RDK::UParamT &source, std::string &dest);

/// ���������� �������� ������� ������� � ������ 'param_name' � ����� �����
/// � ���������� ��� � value
/// ���������� 0 � ������ ������
//int DecodeParamAsInteger(const std::string &param_name, const UServerCommand &args, int &value);

/// ���������� ����� �� �������
void SendCommandResponse(const std::string &client_binding, RDK::UParamT &dest);

/// ���������� ��������� �� ������ � ����� �� �������
/// 0 - ����������� ������
/// 1 - ������� �� ��������
void SendCommandError(const std::string &client_binding, int request_id, int error_code);


// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������ ������
virtual void AAfterReset(void);

// �����, ���������� ����� ���� �������
virtual void AAfterCalculate(void);

// ���������� ����������
virtual void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------

// -----------------------------
// ����������� ������ �������
// -----------------------------
/// ���������� ����� �������
int GetNumChannels(void) const;

/// ������������� ����� �������
/// ����� ���������� ����� ���������� �����
int SetNumChannels(int value);

/// ���������� ��� ��������� ����� ��� ������
/// � ������������ � �������� VideoOutputFrame
int GetChannelVideoSource(int channel_id);

/// ������ �������� ����� ��� ������
/// � ������������ � �������� VideoOutputFrame
int SetChannelVideoSource(int channel_id, int source_mode);

/// ���������� ��� ������
const std::string GetChannelName(int channel);

/// ������������� ��� ������
bool SetChannelName(int channel, const std::string& name);

/// ���������� ��������� ���������� ������ � �������� ���������
/// ��� ���� �������, ���� channel_id<0
int ResetChannel(int channel_id);

/// ��������� ��������� �����
/// ��� ��� ������, ���� channel_id<0
int StartChannel(int channel_id);

/// ������������� ��������� �����
/// ��� ��� ������, ���� channel_id<0
int StopChannel(int channel_id);

/// ������������ ��������� �������� ����������
int RegisterMetadataReceiver(const std::string &address, int port,
		const std::string &component_name, const std::string &component_state);

/// ������� ��������� �������� ����������
int UnRegisterMetadataReceiver(const std::string &address, int port);

/// ��������� ������ ��������� ��� ������
/// ��� ��������� ������ ��� ���� �������, ���� channel_id<0
int LoadProject(int channel_id, const std::string &project_file_name);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUServerControlForm *UServerControlForm;
//---------------------------------------------------------------------------
#endif
