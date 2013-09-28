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
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.CheckLst.hpp>
//---------------------------------------------------------------------------
class TTldTrackingForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TPanel *Panel3;
	TVideoOutputFrame *VideoOutputFrame1;
	TButton *GetFrameButton;
	TCheckBox *EnableLogCheckBox;
	TButton *CopyTrackerDataButton;
	TOpenDialog *OpenDialog1;
	TButton *SaveTrackerDataButton;
	TButton *LoadTrackerDataButton;
	TButton *StartTrackingButton;
	TButton *StopTrackingButton;
	TPageControl *PageControl1;
	TTabSheet *RectangleTabSheet;
	TLabel *Label2;
	TButton *SendObjectToButton;
	TComboBox *ObjectReceiverComboBox;
	TTabSheet *PointsTabSheet;
	TButton *SendPointsButton;
	TGroupBox *GroupBox1;
	TLabeledEdit *RectWidthLabeledEdit;
	TLabeledEdit *RectHeightLabeledEdit;
	TCheckBox *CheckBox1;
	TTimer *Timer1;
	TPanel *Panel4;
	TRadioGroup *InitInputModeRadioGroup;
	TCheckListBox *TrackersCheckListBox;
	void __fastcall StartTrackingButtonClick(TObject *Sender);
	void __fastcall StopTrackingButtonClick(TObject *Sender);
	void __fastcall GetFrameButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall EnableLogCheckBoxClick(TObject *Sender);
	void __fastcall SendObjectToButtonClick(TObject *Sender);
	void __fastcall CopyTrackerDataButtonClick(TObject *Sender);
	void __fastcall SaveTrackerDataButtonClick(TObject *Sender);
	void __fastcall LoadTrackerDataButtonClick(TObject *Sender);
	void __fastcall PointsTabSheetShow(TObject *Sender);
	void __fastcall SendPointsButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall TrackersCheckListBoxClickCheck(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TTldTrackingForm(TComponent* Owner);

RDK::UBitmap ResultBmp;

/// ��� �������� �����
/// 0 - � ����� �������
/// 1 - � ������, ������������ ����������� 0 ��������� ���������
int VideoSourceType;

/// ������ ��������
std::vector<std::pair<std::string,bool> > Trackers;

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

/// ��������� ������ ��������
void UpdateTrackersList(void);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TTldTrackingForm *TldTrackingForm;
//---------------------------------------------------------------------------
#endif
