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
#include "myrdk.h"
#include "TUVisualController.h"

#pragma warn -8130

class TEngineThread: public TThread
{
protected: // ���������
/// ������ ������ � ���������� ���������, ����������� ������
int ChannelIndex;

/// ����� �����
int CalculateMode;

/// ����������� �������� ������� ����� ���������� ������� � ������ 0 � 2, ��
int MinInterstepsInterval;

/// ����� ��������� ������� ��������� ���������� �������
unsigned long long RealLastCalculationTime;

public:
HANDLE CalcEnable;

HANDLE CalcStarted;

HANDLE CalculationNotInProgress;

RDK::UBitmap Source;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TEngineThread(int channel_index, int calculate_mode, int min_inerval, bool CreateSuspended);
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
bool SetMinInterstepsInterval(int value);
// --------------------------


// --------------------------
// ���������� �������
// --------------------------
void __fastcall BeforeCalculate(void);

void __fastcall AfterCalculate(void);

virtual void __fastcall Execute(void);
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
	void __fastcall Start1Click(TObject *Sender);
	void __fastcall Pause1Click(TObject *Sender);
	void __fastcall Reset1Click(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall Step1Click(TObject *Sender);
	void __fastcall SaveClassesDescriptions1Click(TObject *Sender);
	void __fastcall LoadAllClassesDescriptions1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorFrame(TComponent* Owner);
	virtual __fastcall ~TUEngineMonitorFrame(void);

/// ����� ������
/// 0 - ������������ (�������������) �����
/// 1 - ������������� �����
int ChannelsMode;

// ����� ��������
// 0 - ������� ������
// 1 - ������ � �������� �������
// 2 - ������� ������ �� �������
std::vector<int> CalculateMode;

// ��������� ����� ���������� �������
std::vector<long long> LastCalculatedServerTimeStamp;

/// ����������� �������� ������� ����� ���������� ������� � ������ 0 � 2, ��
std::vector<int> MinInterstepsInterval;

protected:

// ������, ������������ ��� ������������� �������
// ������������ ��� �������� �����
std::vector<bool> CalculateSignal;

// ��������� ����� �������
std::vector<long long> ServerTimeStamp;

/// ������ ������� �������������� ���������
std::vector<TEngineThread*> ThreadChannels;

/// ����� ��������� ������� ��������� ���������� ������� � ������������ ������
std::vector<unsigned long long> RealLastCalculationTime;

public:
/// ���������� ������� ������
/// 0 - ������������ �����
/// 1 - ������������� �����
int GetChannelsMode(void) const;
void SetChannelsMode(int mode);

// ���������� ������� ��������
int GetCalculateMode(int channel_index) const;
void SetCalculateMode(int channel_index, int value);

void SetMinInterstepsInterval(int channel_index, int value);

// ���������� ��������� ������ �������
long long GetServerTimeStamp(int channel_index) const;
void SetServerTimeStamp(int channel_index, long long stamp);

/// ���������� ������ �������
int GetNumChannels(void) const;
bool SetNumChannels(int num);

void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorFrame *UEngineMonitorFrame;
//---------------------------------------------------------------------------
#endif
