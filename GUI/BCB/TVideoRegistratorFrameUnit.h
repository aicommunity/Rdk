//---------------------------------------------------------------------------

#ifndef TVideoRegistratorFrameUnitH
#define TVideoRegistratorFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "VidGrab.hpp"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Graphics.hpp>
#include <IniFiles.hpp>
#include <Vcl.Menus.hpp>

#include "TUVisualController.h"
#include "TUVisualControllerFrameUnit.h"
#include "UComponentsListFormUnit.h"

#include "myrdk.h"

#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>

#include <string>
#include <vector>
#include <map>

#define XOK 0
#define XCAMSNOTFOUND 7
#define XREINITSYSREM 8
#define XCAMALREADYINIT -1
#define XCAMALREADYSTART -2
#define XNOIP 1
#define XINITERROR 9
#define XSTARTERROR 2
#define XCAMNOTINIT 4
#define XFRAMEREADERROR 3
#define XCAMNOTSTARTED 5
#define XCAMFRAMETYPEERROR 6
#define XCAMCOULDNOTSTOP 10
//---------------------------------------------------------------------------
class TTVideoRegistratorFrame;
class TVideoOutputFrame;

class TVideoGetBitmapFrameThread : public TThread
{
protected: // Параметры
/// Источник видео
int SourceMode;

/// Индекс канала в библиотеке аналитики, управляемый тредом
int ChannelIndex;

protected: // Данные
/// Данные изображения
RDK::UBitmap Source[2];

/// Указатель на текущее изображение для чтения
RDK::UBitmap* ReadSource;

/// Указатель на текущее изображение для записи
RDK::UBitmap* WriteSource;

/// Указатель на владельца
TTVideoRegistratorFrame *Frame;

protected: // События
/// Снимается на время захвата кадра
HANDLE FrameNotInProgress;

/// Выставлено всегда. Сбрасывается на время доступа к изображению
HANDLE SourceUnlock;
HANDLE SourceWriteUnlock;

/// Сбрасывается на время ожидания расчета
HANDLE CalcCompleteEvent;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoGetBitmapFrameThread(TTVideoRegistratorFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoGetBitmapFrameThread(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Источник видео
int GetSourceMode(void) const;

/// Индекс канала в библиотеке аналитики, управляемый тредом
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Указатель на владельца
TTVideoRegistratorFrame* GetFrame(void) const;
bool SetFrame(TTVideoRegistratorFrame * frame);
// --------------------------

// --------------------------
// Управление событиями
// --------------------------
/// Выставляется по завершении захвата нового кадра
HANDLE GetFrameNotInProgress(void) const;

/// Выставлено всегда. Сбрасывается на время доступа к изображению
HANDLE GetSourceUnlock(void) const;

/// Сбрасывается на время ожидания расчета
HANDLE GetCalcCompleteEvent(void) const;
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void)=0;

virtual void __fastcall Execute(void);

/// Возвращает копию изображения с блокировкой
bool ReadSourceSafe(RDK::UBitmap& dest, bool reflect);
bool ReadSourceSafe(Graphics::TBitmap *dest, bool reflect);

/// Записывает изображение в тред с блокировкой
bool WriteSourceSafe(const RDK::UBitmap& src, bool reflect);
bool WriteSourceSafe(Graphics::TBitmap *src, bool reflect);
};
//---------------------------------------------------------------------------
class TVideoGetBitmapFrameFromVideoThread : public TVideoGetBitmapFrameThread
{
protected:// Параметры
int FrameIndex;

protected: // Временные изображения
RDK::UBitmap TempBitmap;

protected: // Данные
// Источник данных
TVideoOutputFrame* VideoOutputFrame;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoGetBitmapFrameFromVideoThread(TTVideoRegistratorFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoGetBitmapFrameFromVideoThread(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
bool SetFrameIndex(const int &value);
const int& GetFrameIndex(void) const;
// --------------------------
bool SetVideoFrame(TVideoOutputFrame* videoFrame);
TVideoOutputFrame* GetVideoFrame(void) const;
// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);
// --------------------------
};

class TVideoGetBitmapFrameFromComponentThread : public TVideoGetBitmapFrameThread
{
protected: // Временные изображения
RDK::UBitmap TempSource;
RDK::UBitmap TempBitmap;

// Данные о компоненте источнике
std::string ComponentName;
std::string PropertyName;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoGetBitmapFrameFromComponentThread(TTVideoRegistratorFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoGetBitmapFrameFromComponentThread(void);
// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);
// --------------------------
// --------------------------
// Управление параметрами
// --------------------------
bool SetComponentName(const std::string &comp_name);
const std::string GetComponentName(void);

bool SetPropertyName(const std::string &property_name);
const std::string GetPropertyName(void);
};
//---------------------------------------------------------------------------
class TTVideoRegistratorFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TButton *StopNetworkStreamingButton;
	TButton *NetworkStreamingButton;
	TLabeledEdit *PortLabeledEdit;
	TLabeledEdit *FrameRateLabeledEdit;
	TVideoGrabber *VideoGrabber;
	TLabeledEdit *BitRateLabeledEdit;
	TLabeledEdit *VideoWidthLabeledEdit;
	TLabeledEdit *VideoHeightLabeledEdit;
	TLabeledEdit *BufferWindowLabeledEdit;
	TLabeledEdit *MaxKeyFrameSpacingLabeledEdit;
	TGroupBox *StreamingSettingsGroupBox;
	TButton *GetStreamingHostButton;
	TButton *InitButton;
	TLabeledEdit *MaxUsersLabeledEdit;
	TImage *PreviewImage;
	TButton *StartRecordingButton;
	TPageControl *ModePageControl;
	TTabSheet *NetworkStreamingTabSheet;
	TTabSheet *RecordingTabSheet;
	TPanel *VideoPanel;
	TButton *StopRecordingButton;
	TComboBox *RecordingMethodComboBox;
	TLabel *RecordingMethodLabel;
	TLabeledEdit *RecordingFileNameLabeledEdit;
	TLabeledEdit *RecordingFrameRateLabeledEdit;
	TLabeledEdit *ComponentNameLabeledEdit;
	TLabeledEdit *ComponentPropertyNameLabeledEdit;
	TLabeledEdit *FrameIndexLabeledEdit;
	TButton *StartPreviewButton;
	TButton *StopButton;
	TGroupBox *ComponentSourceControlGroupBox;
	TLabeledEdit *ChannelIndexLabeledEdit;
	TPanel *ControlPanel;
	TGroupBox *VideoControlGroupBox;
	TSplitter *Splitter1;
	TButton *BrowseComponentButton;
	TPageControl *SourcePageControl;
	TTabSheet *SourceComponentTabSheet;
	TTabSheet *SourceFrameTabSheet;
	TGroupBox *FrameSourceControlGroupBox;
	TComboBox *VideoCompressorComboBox;
	TLabel *VideoCompressorLabel;
	TLabeledEdit *RecordWidthLabeledEdit;
	TLabeledEdit *RecordHeightLabeledEdit;
	TGroupBox *RecordingSettingsGroupBox;
	TGroupBox *NetworkStreamingControlGroupBox;
	TGroupBox *RecordingControlGroupBox;
	TButton *BrowseFileNameButton;
	TFileOpenDialog *RecordingFileOpen;
	TButton *VideoCompressorSettingsButton;
	TComboBox *VideoCompressionModeComboBox;
	TLabel *VideoCompressionModeLabel;
	TGroupBox *RecordingTimerGroupBox;
	TComboBox *RecordingModeComboBox;
	TLabeledEdit *RecordingTimerLabeledEdit;
	TGroupBox *PreAllocatedRecordingGroupBox;
	TCheckBox *UsePreallocatedFileCheckBox;
	TLabeledEdit *PreallocatedFileSizeLabeledEdit;
	TButton *PreallocatedFileCreateButton;
	TScrollBox *RecordingSettingsScrollBox;
	TGroupBox *GroupBox1;
	TRadioGroup *StoragePathRadioGroup;
	TLabeledEdit *StoragePathLabeledEdit;
	TButton *BrowseStoragePathButton;
	TOpenDialog *StoragePathOpenDialog;
	void __fastcall NetworkStreamingButtonClick(TObject *Sender);
	void __fastcall StopNetworkStreamingButtonClick(TObject *Sender);
	void __fastcall VideoGrabberVideoFromBitmapsNextFrameNeeded(TObject *Sender, bool FirstSample);
	void __fastcall GetStreamingHostButtonClick(TObject *Sender);
	void __fastcall InitButtonClick(TObject *Sender);
	void __fastcall VideoGrabberFrameCaptureCompleted(TObject *Sender, Pointer FrameBitmap,
          int BitmapWidth, int BitmapHeight, DWORD FrameNumber, __int64 FrameTime,
          TFrameCaptureDest DestType, UnicodeString FileName, bool Success,
          int FrameId);
	void __fastcall StartRecordingButtonClick(TObject *Sender);
	void __fastcall StopRecordingButtonClick(TObject *Sender);
	void __fastcall VideoGrabberAVIDurationUpdated(TObject *Sender, UnicodeString FileName,
          DWORD FrameCount, double &FrameRate, __int64 &Duration);
	void __fastcall StartPreviewButtonClick(TObject *Sender);
	void __fastcall StopButtonClick(TObject *Sender);
	void __fastcall BrowseComponentButtonClick(TObject *Sender);
	void __fastcall BrowseFileNameButtonClick(TObject *Sender);
	void __fastcall VideoCompressorComboBoxChange(TObject *Sender);
	void __fastcall VideoCompressorSettingsButtonClick(TObject *Sender);
	void __fastcall PreallocatedFileCreateButtonClick(TObject *Sender);
	void __fastcall RecordingMethodComboBoxChange(TObject *Sender);
	void __fastcall StoragePathRadioGroupClick(TObject *Sender);
	void __fastcall BrowseStoragePathButtonClick(TObject *Sender);


private:	// User declarations
public:		// User declarations
// Индекс этого источника на форме всех источников
int FrameIndex;

// Источник
// 0 - bmp handle from component
// 1 - bmp handle from frame
int Mode;

// Поток получения кадра
TVideoGetBitmapFrameThread *BitmapFrameThread;

// Указатель на форму выбора компоненты-источника
TUComponentsListForm *MyComponentsListForm;

// Данные
// Хранилище декодированного в TBitmap кадра с камеры
TBitmap *InputFrameBitmap;

// Недекодированный кадр с камеры
unsigned char *frame;

// Массив хранения кодов ошибок и сообщений
std::map<int, std::string> Errors;

// Флаг инициализации камеры
// true если инициализированна
bool InitCamFlag;

// Флаг работы камеры
// true если запущена
bool StartCamFlag;

// Флаг показа preview
bool PreviewFlag;

// Методы
void __fastcall AssignListToComboBox (TComboBox* ComboBox, String List, int Index);

// Заполнение массива ошибок
void FillErrorsArray(void);

// Логгирование ошибок
bool WriteLogMessage(const int &err);

// Установка флага показа preview
bool SetPreviewFlag(const bool &value);

// Инициализирует
int Init(void);

// Деинициализирует
int UnInit(void);

// Инициализирует настройки direct network streaming TVideoGrabber
int InitStreamingSettings(void);

// Инициализирует настройки записи в файл TVideoGrabber
int InitRecordingSettings(void);

// Получает кадр с камеры через DLL и пишет его в InputFrameBitmap
int GetBitmapFrame(void);

// Создание и подготовка TBitmap InputFrameBitmap для хранения кадра с камеры
int PrepareBitmapFrame(void);

// Установка фрейма источника при работе от фрейма
bool SetVideoFrameSource(TVideoOutputFrame* sourceFrame);
TVideoOutputFrame* GetVideoFrameSourc(void);


void __fastcall RefreshDeviceControls (void);
// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед сбросом
void ABeforeReset(void);

// Метод, вызываемый перед шагом расчета
virtual void ABeforeCalculate(void);

// Метод, вызываемый после шага расчета
virtual void AAfterCalculate(void);

// Обновление интерфейса
virtual void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------

	__fastcall TTVideoRegistratorFrame(TComponent* Owner);
	__fastcall ~TTVideoRegistratorFrame(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TTVideoRegistratorFrame *TVideoRegistratorFrame;
//---------------------------------------------------------------------------
#endif
