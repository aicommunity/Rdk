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
#include "VideoOutputFormUnit.h"
#include "VideoOutputFrameUnit.h"
#include "myrdk.h"

#include <Vcl.ComCtrls.hpp>

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

class TVideoGetBitmapFrameThread : public TThread
{
protected: // ���������
/// �������� �����
int SourceMode;

/// ������ ������ � ���������� ���������, ����������� ������
int ChannelIndex;

protected: // ������
/// ��������� ����� ���������� �����
//long long LastTimeStamp;

/// ������ �����������
RDK::UBitmap Source[2];

/// ��������� �� ������� ����������� ��� ������
RDK::UBitmap* ReadSource;

/// ��������� �� ������� ����������� ��� ������
RDK::UBitmap* WriteSource;

/// ��������� �� ���������
TTVideoRegistratorFrame *Frame;

protected: // �������
/// ��������� �� ����� ������� �����
HANDLE FrameNotInProgress;

/// ���������� ������. ������������ �� ����� ������� � �����������
HANDLE SourceUnlock;
HANDLE SourceWriteUnlock;

/// ������������ �� ����� ������ ������������
HANDLE CaptureEnabled;

/// ������������ �� ����� �������� �������
HANDLE CalcCompleteEvent;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoGetBitmapFrameThread(TTVideoRegistratorFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoGetBitmapFrameThread(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
/// �������� �����
int GetSourceMode(void) const;

/// ������ ������ � ���������� ���������, ����������� ������
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ��������� �� ���������
TTVideoRegistratorFrame* GetFrame(void) const;
bool SetFrame(TTVideoRegistratorFrame * frame);
// --------------------------

// --------------------------
// ���������� ���������
// --------------------------
/// ������������ �� ���������� ������� ������ �����
HANDLE GetFrameNotInProgress(void) const;

/// ���������� ������. ������������ �� ����� ������� � �����������
HANDLE GetSourceUnlock(void) const;

/// ������������ �� ����� ������ ������������
HANDLE GetCaptureEnabled(void) const;

/// ������������ �� ����� �������� �������
HANDLE GetCalcCompleteEvent(void) const;
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void)=0;

virtual void __fastcall Execute(void);

/// ���������� ����� ����������� � �����������
bool ReadSourceSafe(RDK::UBitmap& dest, bool reflect);
bool ReadSourceSafe(Graphics::TBitmap *dest, bool reflect);

/// ���������� ����������� � ���� � �����������
bool WriteSourceSafe(const RDK::UBitmap& src, bool reflect);
bool WriteSourceSafe(Graphics::TBitmap *src, bool reflect);
};
//---------------------------------------------------------------------------
class TVideoGetBitmapFrameFromVideoThread : public TVideoGetBitmapFrameThread
{
protected:// ���������
int FrameIndex;

protected: // ��������� �����������
RDK::UBitmap TempBitmap;
//Graphics::TBitmap* TempBitmap;

protected: // ������
TVideoOutputFrame* VideoOutputFrame;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoGetBitmapFrameFromVideoThread(TTVideoRegistratorFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoGetBitmapFrameFromVideoThread(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
bool SetFrameIndex(const int &value);
const int& GetFrameIndex(void) const;
// --------------------------

// --------------------------
// ���������� �������
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
protected: // ��������� �����������
RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

// ������ � ���������� ���������
std::string ComponentName;
std::string PropertyName;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoGetBitmapFrameFromComponentThread(TTVideoRegistratorFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoGetBitmapFrameFromComponentThread(void);
// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);
// --------------------------
// --------------------------
// ���������� �����������
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
	TMemo *LogMemo;
	TButton *SaveToIniButton;
	TLabeledEdit *PortLabeledEdit;
	TLabeledEdit *FrameRateLabeledEdit;
	TButton *ClearMemoButton;
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
	TPageControl *PageControl;
	TTabSheet *NetworkStreamingTabSheet;
	TTabSheet *RecordTabSheet;
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
	TGroupBox *SourceControlGroupBox;
	TLabeledEdit *ChannelIndexLabeledEdit;
	TComboBox *SorceModeComboBox;
	TLabel *SourceModeLabel;
	TPanel *ControlPanel;
	TPanel *LogPanel;
	TGroupBox *VideoControlGroupBox;
	TSplitter *Splitter1;
	TSplitter *Splitter2;
	void __fastcall NetworkStreamingButtonClick(TObject *Sender);
	void __fastcall StopNetworkStreamingButtonClick(TObject *Sender);
	void __fastcall VideoGrabberVideoFromBitmapsNextFrameNeeded(TObject *Sender, bool FirstSample);
	void __fastcall ClearMemoButtonClick(TObject *Sender);
	void __fastcall SaveToIniButtonClick(TObject *Sender);
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


private:	// User declarations
public:		// User declarations
// ������ ����� ��������� �� ����� ���� ����������
int FrameIndex;

// ��������
// 0 - bmp handle from component
// 1 - bmp handle from frame
int Mode;

// ����� ��������� �����
TVideoGetBitmapFrameThread *BitmapFrameThread;

// ������
// ��������� ��������������� � TBitmap ����� � ������
TBitmap *InputFrameBitmap;

// ���������������� ���� � ������
unsigned char *frame;

// ������ �������� ����� ������ � ���������
std::map<int, std::string> Errors;

// ���� ������������� ������
// true ���� �����������������
bool InitCamFlag;

// ���� ������ ������
// true ���� ��������
bool StartCamFlag;

// ���� ������ preview
bool PreviewFlag;

// ������
// ���������� ������� ������
void FillErrorsArray(void);

// ������������ ������
bool WriteLogMessage(const int &err);

// ��������� ����� ������ preview
bool SetPreviewFlag(const bool &value);

// ��������������
int Init(void);

// ����������������
int UnInit(void);

// �������������� ��������� direct network streaming TVideoGrabber
int InitStreamingSettings(void);

// �������������� ��������� ������ � ���� TVideoGrabber
int InitRecordingSettings(void);

// �������� ���� � ������ ����� DLL � ����� ��� � InputFrameBitmap
int GetBitmapFrame(void);

// �������� � ���������� TBitmap InputFrameBitmap ��� �������� ����� � ������
int PrepareBitmapFrame(void);

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� �������
void ABeforeReset(void);

// �����, ���������� ����� ����� �������
virtual void ABeforeCalculate(void);

// �����, ���������� ����� ���� �������
virtual void AAfterCalculate(void);

// ���������� ����������
virtual void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------

	__fastcall TTVideoRegistratorFrame(TComponent* Owner);
	__fastcall ~TTVideoRegistratorFrame(void);
};
//int framewidth_;
//int frameheight_;
//double framerate_=-1;
//unsigned long framecount_=-1;
//-----------------------------------------------
//---------------------------------------------------------------------------
extern PACKAGE TTVideoRegistratorFrame *TVideoRegistratorFrame;
//---------------------------------------------------------------------------
#endif