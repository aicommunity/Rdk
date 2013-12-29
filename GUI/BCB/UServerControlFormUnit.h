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

/// Стандартная функция, осуществляющую декодирование параметров запроса
int StandardCommandRequestDecoder(UServerCommand &source, UServerCommand &dest);

/// Стандартная функция, осуществляющую кодирование параметров ответа
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
// Параметры сервера
// -----------------
/// Флаг разрешения запуска сервера при старте
bool AutoStartFlag;

/// Массив уникальных имен каналов
std::vector<std::string> ChannelNames;

/// Результаты измерений производительности, мс
std::vector<std::vector<long long> > ModelPerformanceResults;
std::vector<std::vector<long long> > TransportPerformanceResults;

std::string ServerName;

std::string ServerId;

std::string MetaComponentName;

std::string MetaComponentStateName;

/// Число шагов усреднения оценки производительности
int AverageIterations;
// -----------------

/// Указатель на функцию, осуществляющую декодирование параметров запроса
int (*CommandRequestDecoder)(UServerCommand &source, UServerCommand &dest);

/// Указатель на функцию, осуществляющую кодирование параметров ответа
int (*CommandResponseEncoder)(const std::string &response_type, RDK::UParamT &source, RDK::UParamT &dest);

/// Режим (тип) запроса
/// 0 - обращение к системе управления свервером (Control)
/// 1 - удаленный вызов процедур движка заданного канала
/// 1 - обращение к движку (Engine)
/// 2 - обращение к модели (Model)
/// 2 - обращение к среде выполнения (Env)
/// 3 - обращение к хранилищу компонент (Storage)
/// <0 - Неизвестный тип запроса
int Mode;

/// Обработанный список команды запроса
UServerCommand DecodedRequest;

/// Ответ
RDK::UParamT Response;

/// Упакованный для отправки ответ
RDK::UParamT EncodedResponse;

/// Тип ответа
std::string ResponseType;

/// Команда запроса
std::string Command;

/// Индекс канала запроса
std::string ChannelIndex;

/// Строка результирующего ответа от обработчика сервера
std::string ControlResponseString;

TMemoryStream* MemStream;
TBitmap *Bitmap;
RDK::UBitmap TempUBitmap;

// Индекс складывания данных в массив оценки производительности
int PerformancePushIndex;

//RDK::ExternalPtzControl PtzControl;

HANDLE CommandQueueUnlockEvent;

// Очередь команд
std::list<UServerCommand > CommandQueue;

std::map<std::string, RDK::UTransferReader> PacketReaders;

//RDK::UTransferPacket Packet;
std::string PacketXml;

UServerCommand CurrentProcessedCommand;

TThreadList *Clients;

TCriticalSection* CriticalSection;


const char* ControlRemoteCall(const char *request, int &return_value);

//const char* PtzRemoteCall(const char *request, int &return_value);

// Функция, обрабатывающая команды управления сервером
// Возвращает true если команда была найдена и обработана
//bool ProcessControlCommand(const std::string &cmd_name, UServerCommand &args, std::string &response_type, UParamT &response_data);
bool ProcessControlCommand(const UServerCommand &args, std::string &response_type, RDK::UParamT &response_data);

// Функция, обрабатывающая команды удаленного вызова процедур
// Возвращает true если команда была найдена и обработана
//bool ProcessRPCCommand(int channel, const std::string &cmd_name, UServerCommand &args, std::string &response_type, UParamT &response_data);
bool ProcessRPCCommand(const UServerCommand &args, std::string &response_type, RDK::UParamT &response_data);

// Метод, обрабатывающий команды управления PTZ камерами
bool ProcessPtzCommand(const UServerCommand &args, std::string &response_type, RDK::UParamT &response_data);

/// Кодирует строку в вектор
void ConvertStringToVector(const std::string &source, RDK::UParamT &dest);

/// Кодирует вектор в строку
void ConvertVectorToString(const RDK::UParamT &source, std::string &dest);

/// Декодирует параметр массива команды с именем 'param_name' в целое число
/// и записывает его в value
/// Возвращает 0 в случае успеха
//int DecodeParamAsInteger(const std::string &param_name, const UServerCommand &args, int &value);

/// Отправляет ответ на команду
void SendCommandResponse(const std::string &client_binding, RDK::UParamT &dest);

/// Отправляет сообщение об ошибке в ответ на команду
/// 0 - неизвестная ошибка
/// 1 - Команда не опознана
void SendCommandError(const std::string &client_binding, int request_id, int error_code);


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
/// Возвращает число каналов
int GetNumChannels(void) const;

/// Устанавливает число каналов
/// также выставляет число источников видео
int SetNumChannels(int value);

/// Возвращает тип источника видео для канала
/// в соответствии с режимами VideoOutputFrame
int GetChannelVideoSource(int channel_id);

/// Задает источник видео для канала
/// в соответствии с режимами VideoOutputFrame
int SetChannelVideoSource(int channel_id, int source_mode);

/// Возвращает имя канала
const std::string GetChannelName(int channel);

/// Устанавливает имя канала
bool SetChannelName(int channel, const std::string& name);

/// Сбрасывает аналитику выбранного канала в исходное состояние
/// или всех каналов, если channel_id<0
int ResetChannel(int channel_id);

/// Запускает выбранный канал
/// или все каналы, если channel_id<0
int StartChannel(int channel_id);

/// Останавливает выбранный канал
/// или все каналы, если channel_id<0
int StopChannel(int channel_id);

/// Регистрирует удаленный приемник метаданных
int RegisterMetadataReceiver(const std::string &address, int port,
		const std::string &component_name, const std::string &component_state);

/// Удаляет удаленный приемник метаданных
int UnRegisterMetadataReceiver(const std::string &address, int port);

/// Загружает проект аналитики для канала
/// или загружает проект для всех каналов, если channel_id<0
int LoadProject(int channel_id, const std::string &project_file_name);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUServerControlForm *UServerControlForm;
//---------------------------------------------------------------------------
#endif
