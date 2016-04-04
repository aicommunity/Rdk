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
class TEngineMonitorThread;
class TEngineThread;

class RDK_LIB_TYPE UEngineControlVcl: public RDK::UEngineControl
{
public:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UEngineControlVcl(void);
virtual ~UEngineControlVcl(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
/// �������� ������ ����� �������
virtual RDK::UEngineControlThread* CreateEngineThread(RDK::UEngineControl* engine_control, int engine_index);

/// �������� ������ ����� �������
virtual RDK::UEngineStateThread* CreateEngineStateThread(RDK::UEngineControl* engine_control);

/// ��������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
virtual void StartEngine(int channel_index);

/// ������������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
virtual void PauseEngine(int channel_index);
// --------------------------
};

/// ����� ����������� �� ���������� ������� ����������
class RDK_LIB_TYPE TEngineMonitorThread: public RDK::UEngineStateThread
{
protected: // ������ ��������� �������
/// ��������� ���������� ������������
/// 0 - ����������
/// 1 - ������ ����������
/// 2 - ������ ������� �� �� �����������
RDK::UELockVar<std::vector<int> > VideoCaptureStates;

protected: // ���������� ������
/// ��������� ������� ������� ������ ��������� ������ �������
std::vector<double> VideoCaptureStateTime;

/// ��������� ������� ������� ��������� �������
std::vector<double> VideoCaptureSuccessTime;

/// ������� ��������� �������� ������� ��������� �������
std::vector<std::vector<double> > AvgCaptureIterations;

public:

protected: // ��������� ����������


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
TEngineMonitorThread(RDK::UEngineControl* engine_control);
virtual ~TEngineMonitorThread(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
// --------------------------

// --------------------------
// ������ ������� � ������ ��������� �������
// --------------------------
/// ���������� ������ ��������� ���������� ������������
std::vector<int> ReadVideoCaptureStates(void) const;
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ���������� �����-�������� ������
virtual UEngineControlVcl* GetEngineControl(void);

virtual void AdditionExecute(void);
// --------------------------
};



class RDK_LIB_TYPE TEngineThread: public RDK::UEngineControlThread
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
TEngineThread(RDK::UEngineControl* engine_control, int channel_index);
virtual ~TEngineThread(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
// --------------------------


// --------------------------
// ���������� �������
// --------------------------
/// ���������� �����-�������� ������
virtual UEngineControlVcl* GetEngineControl(void);

virtual void ABeforeCalculate(void);
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
	TTimer *Timer;
	TMenuItem *ools1;
	TMenuItem *SaveClassesDescriptions1;
	TMenuItem *LoadAllClassesDescriptions1;
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall SaveClassesDescriptions1Click(TObject *Sender);
	void __fastcall LoadAllClassesDescriptions1Click(TObject *Sender);
	void __fastcall RichEditMouseEnter(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorFrame(TComponent* Owner);
	virtual __fastcall ~TUEngineMonitorFrame(void);

protected:

public:
void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

/// ������ � ����� ����������� ��������� ������� �������
const TEngineMonitorThread* GetEngineMonitorThread(void) const;

TEngineThread* GetThreadChannel(int i);
};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorFrame *UEngineMonitorFrame;
//---------------------------------------------------------------------------
#endif
