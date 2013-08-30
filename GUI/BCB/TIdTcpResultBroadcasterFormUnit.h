//---------------------------------------------------------------------------

#ifndef TIdTcpResultBroadcasterFormUnitH
#define TIdTcpResultBroadcasterFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Menus.hpp>
#include "TUVisualControllerFormUnit.h"
#include "TIdTcpResultBroadcasterFrameUnit.h"
#include <vector>
//---------------------------------------------------------------------------
class TIdTcpResultBroadcasterForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TMainMenu *MainMenu;
	TMenuItem *Connect1;
	TMenuItem *ConnectAll1;
	TMenuItem *DisconnectAll1;
	TMenuItem *Servers1;
	TMenuItem *Add1;
	TMenuItem *Del1;
	TPopupMenu *PopupMenu1;
	TMenuItem *HttpBroadcaster1;
	void __fastcall Add1Click(TObject *Sender);
	void __fastcall Del1Click(TObject *Sender);
	void __fastcall ConnectAll1Click(TObject *Sender);
	void __fastcall DisconnectAll1Click(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall HttpBroadcaster1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TIdTcpResultBroadcasterForm(TComponent* Owner);


// ��������
std::vector<TIdTcpResultBroadcasterFrame*> Broadcasters;

/// ������� ���������� ���������� � ������� �� �������� � ������������ � �����������
bool AddMetadata(int channel_index, long long time_stamp);

/// ���������� ��������� �������� ����������
bool SendMetadata(void);

// �����, ���������� ����� ����� �������
void ABeforeCalculate(void);

// ��������� ���������
void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::USerStorageXML &xml);

// �������� ����� ����� ����������
TIdTcpResultBroadcasterForm* New(TComponent *owner);

// ����� ���������
int GetNumBroadcasters(void) const;

// ��������� ����� ��������
void AddBroadcaster(void);

// ������� ��������
void DelBroadcaster(int index);

// ������� ��� ��������
void ClearBroadcasters(void);

// ���������� ������ �������� ��������� (����������) ��������
int GetActiveBroadcaster(void) const;

// ���������� ����� ��������
TIdTcpResultBroadcasterFrame* GetBroadcasterFrame(int index);

// ���������� ����� ��������� (����������) ��������
TIdTcpResultBroadcasterFrame* GetActiveBroadcasterFrame(void);

// ���������� ��������, ��� ���, ���� index == -1
void Connect(int index=-1);

// ��������� ��������, ��� ���, ���� index == -1
void Disconnect(int index=-1);

};
//---------------------------------------------------------------------------
extern PACKAGE TIdTcpResultBroadcasterForm *IdTcpResultBroadcasterForm;
//---------------------------------------------------------------------------
#endif
