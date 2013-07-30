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
	TValueListEditor *ChannelNamesValueListEditor;
	TButton *ApplyChannelsNumberButton;
	TGroupBox *GroupBox3;
	TChart *PerformanceChart;
	TBarSeries *Series1;
	TPanel *Panel;
	TRadioButton *FpsRadioButton;
	TRadioButton *RadioButton2;
	TPanel *Panel2;
	TUHttpServerFrame *UHttpServerFrame;
	void __fastcall UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
          TIdHTTPResponseInfo *AResponseInfo);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUServerControlForm(TComponent* Owner);

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

// Функция, обрабатывающая команды управления сервером
void ProcessControlCommand(const std::string &cmd_name, std::map<std::string,std::vector<char> > &args, std::string &response_type, std::vector<char> &response_data);

// Функция, обрабатывающая команды удаленного вызова процедур
void ProcessRPCCommand(int channel, const std::string &cmd_name, std::map<std::string,std::vector<char> > &args, std::string &response_type, std::vector<char> &response_data);

/// Кодирует строку в вектор
void ConvertStringToVector(const std::string &source, std::vector<char> &dest);


// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
virtual void AAfterReset(void);

// Обновление интерфейса
virtual void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUServerControlForm *UServerControlForm;
//---------------------------------------------------------------------------
#endif
