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

/// Стандартная функция, осуществляющую декодирование параметров запроса
int StandardCommandRequestDecoder(std::map<std::string,std::vector<char> > &source, std::map<std::string,std::vector<char> > &dest);

/// Стандартная функция, осуществляющую кодирование параметров ответа
int StandardCommandResponseEncoder(const std::string &response_type, std::vector<char> &source, std::vector<char> &dest);

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
	void __fastcall UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
          TIdHTTPResponseInfo *AResponseInfo);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ServerStartButtonClick(TObject *Sender);
	void __fastcall ServerStopButtonClick(TObject *Sender);
	void __fastcall ReturnOptionsButtonClick(TObject *Sender);
	void __fastcall ApplyOptionsButtonClick(TObject *Sender);
	void __fastcall ChannelNamesStringGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);




private:	// User declarations
public:		// User declarations
	__fastcall TUServerControlForm(TComponent* Owner);

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

/// Число шагов усреднения оценки производительности
int AverageIterations;
// -----------------

/// Указатель на функцию, осуществляющую декодирование параметров запроса
int (*CommandRequestDecoder)(std::map<std::string,std::vector<char> > &source, std::map<std::string,std::vector<char> > &dest);

/// Указатель на функцию, осуществляющую кодирование параметров ответа
int (*CommandResponseEncoder)(const std::string &response_type, std::vector<char> &source, std::vector<char> &dest);

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
std::map<std::string,std::vector<char> > DecodedRequest;

/// Ответ
std::vector<char> Response;

/// Упакованный для отправки ответ
std::vector<char> EncodedResponse;

/// Тип ответа
std::string ResponseType;

/// Команда запроса
std::string Command;

/// Индекс канала запроса
std::string ChannelIndex;

TMemoryStream* MemStream;
TBitmap *Bitmap;
RDK::UBitmap TempUBitmap;

// Индекс складывания данных в массив оценки производительности
int PerformancePushIndex;

// Функция, обрабатывающая команды управления сервером
// Возвращает true если команда была найдена и обработана
bool ProcessControlCommand(const std::string &cmd_name, std::map<std::string,std::vector<char> > &args, std::string &response_type, std::vector<char> &response_data);

// Функция, обрабатывающая команды удаленного вызова процедур
// Возвращает true если команда была найдена и обработана
bool ProcessRPCCommand(int channel, const std::string &cmd_name, std::map<std::string,std::vector<char> > &args, std::string &response_type, std::vector<char> &response_data);

/// Кодирует строку в вектор
void ConvertStringToVector(const std::string &source, std::vector<char> &dest);

/// Кодирует вектор в строку
void ConvertVectorToString(const std::vector<char> &source, std::string &dest);

/// Декодирует параметр массива команды с именем 'param_name' в целое число
/// и записывает его в value
/// Возвращает 0 в случае успеха
int DecodeParamAsInteger(const std::string &param_name, const std::map<std::string,std::vector<char> > &args, int &value);

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
virtual void AAfterReset(void);

// Метод, вызываемый после шага расчета
virtual void AAfterCalculate(void);

// Обновление интерфейса
virtual void AUpdateInterface(void);

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

/// Загружает проект аналитики для канала
/// или загружает проект для всех каналов, если channel_id<0
int LoadProject(int channel_id, const std::string &project_file_name);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUServerControlForm *UServerControlForm;
//---------------------------------------------------------------------------
#endif
