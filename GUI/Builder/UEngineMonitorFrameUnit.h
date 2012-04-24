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
#include <vector>
#include "myrdk.h"

//---------------------------------------------------------------------------

class TUEngineMonitorFrame : public TFrame, public RDK::IVisualInterface
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
	void __fastcall Start1Click(TObject *Sender);
	void __fastcall Pause1Click(TObject *Sender);
	void __fastcall Reset1Click(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall Step1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUEngineMonitorFrame(TComponent* Owner);

// ����� ��������
// 0 - ������� ������
// 1 - ������ � �������� �������
int CalculateMode;

bool UpdateInterfaceFlag;

// ������ ������������, ������� ������ ���� ������� ����� �������
std::vector<RDK::IVisualInterface*> InterfaceUpdaters;

// ���������� ������� ��������
int GetCalculateMode(void) const;
void SetCalculateMode(int value);

// ��������� ���������� � ������
void AddInterface(RDK::IVisualInterface *value);

// ������� ���������� �� ������
void DelInterface(RDK::IVisualInterface *value);

void UpdateInterface(void);

// ��������� ���������� � �������� ini ����
void SaveToIni(TMemIniFile *ini, const String &section);

// ��������� ���������� �� ��������� ini �����
void LoadFromIni(TMemIniFile *ini, const String &section);
};
//---------------------------------------------------------------------------
extern PACKAGE TUEngineMonitorFrame *UEngineMonitorFrame;
//---------------------------------------------------------------------------
#endif
