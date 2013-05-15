//---------------------------------------------------------------------------

#ifndef TldTrackingFormUnitH
#define TldTrackingFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualController.h"
#include <Vcl.ExtCtrls.hpp>
#include "TUVisualControllerFrameUnit.h"
#include "VideoOutputFrameUnit.h"
#include "TUVisualControllerFormUnit.h"
#include <Vcl.Dialogs.hpp>
//---------------------------------------------------------------------------
class TTldTrackingForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TPanel *Panel3;
	TVideoOutputFrame *VideoOutputFrame1;
	TButton *GetFrameButton;
	TButton *StartTrackingButton;
	TButton *StopTrackingButton;
	TCheckBox *EnableLogCheckBox;
	TButton *GotoFirstFrameButton;
	TLabel *Label1;
	TComboBox *VideoSourceComboBox;
	TButton *SendObjectToButton;
	TComboBox *ObjectReceiverComboBox;
	TLabel *Label2;
	TComboBox *ObjectReceiver2ComboBox;
	TLabel *Label3;
	TButton *CopyTrackerDataButton;
	TButton *SaveTrackerDataButton;
	TButton *LoadTrackerDataButton;
	TOpenDialog *OpenDialog1;
	void __fastcall StartTrackingButtonClick(TObject *Sender);
	void __fastcall StopTrackingButtonClick(TObject *Sender);
	void __fastcall GetFrameButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall EnableLogCheckBoxClick(TObject *Sender);
	void __fastcall GotoFirstFrameButtonClick(TObject *Sender);
	void __fastcall SendObjectToButtonClick(TObject *Sender);
	void __fastcall CopyTrackerDataButtonClick(TObject *Sender);
	void __fastcall SaveTrackerDataButtonClick(TObject *Sender);
	void __fastcall LoadTrackerDataButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TTldTrackingForm(TComponent* Owner);

RDK::UBitmap ResultBmp;

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������� ������
virtual void ABeforeReset(void);

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

// ��������� ComboBox ������� ����� ������ Tld
void LoadVideoInputs(int num_inputs, TComboBox *box);

// �������� ����� ����� ����������
virtual TTldTrackingForm* New(TComponent *owner=0);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TTldTrackingForm *TldTrackingForm;
//---------------------------------------------------------------------------
#endif
