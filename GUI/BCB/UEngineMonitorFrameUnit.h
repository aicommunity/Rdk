//---------------------------------------------------------------------------

#ifndef UEngineMonitorFrameUnitH
#define UEngineMonitorFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <IniFiles.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <vector>
#include <map>
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "TUVisualController.h"
#include "TServerBroadcasterCommonUnit.h"

#pragma warn -8130

class TUEngineMonitorFrame;

/// ����� ����������� �� ���������� ������� ����������
class TEngineMonitorThread: public TThread
{
protected: // ���������

protected: // ������ ��������� �������
/// ��������� ������ �������
/// 0 - �������
/// 1 - ������ ����������
/// 2 - ������ �������, �� �� �����������
RDK::UELockVar<std::vector<int> > CalcThreadStates;

/// ��������� ���������� ������������
/// 0 - ����������
/// 1 - ������ ����������
/// 2 - ������ ������� �� �� �����������
RDK::UELockVar<std::vector<int> > VideoCaptureStates;

protected: // ���������� ������
TUEngineMonitorFrame *EngineMonitorFrame;

/// ��������� ������� ������� ������ ��������� ������ �������
std::vector<double> CalcThreadStateTime;

/// ��������� ������� ������� ��������� �������
std::vector<RDK::ULongTime> CalcThreadSuccessTime;

/// ��������� ������� ������� ������ ��������� ������ �������
std::vector<double> VideoCaptureStateTime;

/// ��������� ������� ������� ��������� �������
std::vector<double> VideoCaptureSuccessTime;

/// ����� ������� ��� ������� �������� ������� ��������� ������
/// (����� �������� ��� ����������)
int NumAvgIterations;

/// ����� ���, � ������� ����� ������� ������ ��������� ������� ����� �������
/// ����� �������, ��� ��������� �������
double AvgThreshold;

/// ������� ��������� �������� ������� ��������� �������
std::vector<std::vector<double> > AvgIterations;

/// ������� ��������� �������� ������� ��������� �������
std::vector<std::vector<double> > AvgCaptureIterations;

public:
// ������� ��������� �������. ���������� �� ����� ���������� �������. ������������ �� �����
HANDLE CalcState;

HANDLE CalcEnable;

HANDLE CalcStarted;

HANDLE CalculationNotInProgress;

protected: // ��������� ����������


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TEngineMonitorThread(TUEngineMonitorFrame *engine_monitor_frame, bool CreateSuspended);
virtual __fastcall ~TEngineMonitorThread(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
// --------------------------

// --------------------------
// ������ ������� � ������ ��������� �������
// --------------------------
/// ���������� ������ ��������� ������
std::vector<int> ReadCalcThreadStates(void) const;

/// ���������� ������ ��������� ���������� ������������
std::vector<int> ReadVideoCaptureStates(void) const;
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Execute(void);
// --------------------------
};



class TEngineThread: public TThread
{
protected: // ���������
/// ������ ������ � ���������� ���������, ����������� ������
int ChannelIndex;

/// ����� �����
RDK::UELockVar<int> CalculateMode;

/// ����������� �������� ������� ����� ���������� ������� � ������ 0 � 2, ��
RDK::UELockVar<RDK::UTime> MinInterstepsInterval;

/// ����� ��������� ������� ��������� ���������� �������
RDK::UELockVar<RDK::ULongTime> RealLastCalculationTime;

public:
// ������� ��������� �������. ���������� �� ����� ���������� �������. ������������ �� �����
HANDLE CalcState;

HANDLE CalcEnable;

HANDLE CalcStarted;

HANDLE CalculationNotInProgress;

RDK::UBitmap Source;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TEngineThread(int channel_index, int calculate_mode, RDK::UTime min_inerval, bool CreateSuspended);
virtual __fastcall ~TEngineThread(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
/// ����� �����
int GetCalculateMode(void) const;
bool SetCalculateMode(int value);

/// ����������� �������� ������� ����� ���������� ������� � ������ 0 � 2, ��
int GetMinInterstepsInterval(void) const;
bool SetMinInterstepsInterval(RDK::UTime value);
// --------------------------


// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Execute(void);

RDK::ULongTime GetRealLastCalculationTime(void) const;
// --------------------------


};

//---------------------------------------------------------------------------

class TUEngineMonitorFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TRichEdit *RichEdit;
	TStatusBar *StatusBar;
	TMainMenu *MainMenu;
	TMenuItem *Control1;
	TMenuItem *Calculate1;
	TMenuItem *Start1;
	TMenuItem *Pause1;
	TMenuItem *Reset1;
	TTimer *Timer;
	TMenuItem *Step1;
	TMenuItem *ools1;
	TMenuItem *SaveClassesDescriptions1;
	TMenuItem *LoadAllClassesDescriptions1;
	TPanel *Panel1;
	TCheckBox *ShowDebugMessagesCheckBox;
	TCheckBox *AutoupdatePropertiesCheckBox;
	void __fastcall Start1Click(TObject *Sender);
	void __fastcall Pause1Click(TObject *Sender);
	void __fastcall Reset1Click(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall Step1Click(TObject *Sender);
	void __fastcall SaveClassesDescriptions1Click(TObject *Sender);
	void __fastcall LoadAllClassesDescriptions1Click(TObject *Sender);
	void __fastcall RichEditMouseEnter(TObject *Sender);
	void __fastcall ShowDebugMessagesCheckBoxClick(TObject *Sender);
	void __fastcall AutoupdatePropertiesCheckBoxClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorFrame(TComponent* Owner);
	virtual __fastcall ~TUEngineMonitorFrame(void);

/// ����� ������
/// 0 - ������������ (�������������) �����
/// 1 - ������������� �����
RDK::UELockVar<int> ChannelsMode;

/// ����� ������������� ������� ��� �������
/// 0 - ��������� �����
/// 1 - ����� ��������� ������
RDK::UELockVar<int> CalculationTimeSourceMode;

// ����� ��������
// 0 - ������� ������
// 1 - ������ � �������� �������
// 2 - ������� ������ �� �������
std::vector<RDK::UELockVar<int> > CalculateMode;

// ��������� ����� ���������� �������
std::vector<RDK::UELockVar<RDK::ULongTime> > LastCalculatedServerTimeStamp;

/// ����������� �������� ������� ����� ���������� ������� � ������ 0 � 2, ��
std::vector<RDK::UELockVar<int> > MinInterstepsInterval;

protected:

// ������, ������������ ��� ������������� �������
// ������������ ��� �������� �����
std::vector<RDK::UELockVar<bool> > CalculateSignal;

// ������ ���������� �������
// ������������ �� �����
std::vector<RDK::UELockVar<bool> > CalculateState;

// ��������� ����� �������
std::vector<RDK::UELockVar<RDK::ULongTime> > ServerTimeStamp;

/// ������ ������� �������������� ���������
std::vector<TEngineThread*> ThreadChannels;

/// ����� ��������� ������� ��������� ���������� ������� � ������������ ������
std::vector<RDK::UELockVar<RDK::ULongTime> > RealLastCalculationTime;

std::vector<RDK::UEPtr<TBroadcasterForm> > BroadcastersList;

HANDLE ThreadCalcCompleteEvent;

/// ����� ����������� ��������� �������
TEngineMonitorThread *EngineMonitorThread;

public:
/// ���������� ������� ������
/// 0 - ������������ �����
/// 1 - ������������� �����
int GetChannelsMode(void) const;
void SetChannelsMode(int mode);

// ���������� ������� ��������
int GetCalculateMode(int channel_index) const;
void SetCalculateMode(int channel_index, int value);

/// ����� ������������� ������� ��� �������
/// 0 - ��������� �����
/// 1 - ����� ��������� ������
int GetCalculationTimeSourceMode(void) const;
bool SetCalculationTimeSourceMode(int value);

void SetMinInterstepsInterval(int channel_index, RDK::UTime value);

// ���������� ��������� ������ �������
RDK::ULongTime GetServerTimeStamp(int channel_index) const;
void SetServerTimeStamp(int channel_index, RDK::ULongTime stamp);

/// ���������� ������ �������
int GetNumChannels(void) const;
bool SetNumChannels(int num);
bool InsertChannel(int index);
bool DeleteChannel(int index);

void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

/// ������������ �������� ����������
void RegisterMetadataBroadcaster(TBroadcasterForm *broadcaster);

/// ������� ����������� �������� ����������
void UnRegisterMetadataBroadcaster(TBroadcasterForm *broadcaster);

/// ���������� ���������� �� ��� ������������������ ��������
virtual bool AddMetadata(int channel_index, RDK::ULongTime time_stamp);

/// ���������� ��������� �������� ���������� ����� ������������������� ����������
virtual bool SendMetadata(void);

/// ��������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
virtual void StartChannel(int channel_index);

/// ������������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
virtual void PauseChannel(int channel_index);

/// ���������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
virtual void ResetChannel(int channel_index);

/// ��������� ��������� ������� �� id ������
/// 0 - �� �������
/// 1 - ���� ������
virtual int CheckCalcState(int channel_id) const;

/// ������ � ����� ����������� ��������� ������� �������
const TEngineMonitorThread* GetEngineMonitorThread(void) const;

/// ������� ���������� �������
void StartEngineMonitorThread(void);

/// ������������� ���������� �������
void StopEngineMonitorThread(void);

TEngineThread* GetThreadChannel(int i);

/// ���������� ������� ����� ��������� ������
double GetSourceTime(int i) const;

/// ��������� ������� ����� ���������� ������ � �������
void ApplySourceTime(void);
};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorFrame *UEngineMonitorFrame;
//---------------------------------------------------------------------------
#endif
