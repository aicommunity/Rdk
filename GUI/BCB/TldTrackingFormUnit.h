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

/// Тип источика видео
/// 0 - с входа трекера
/// 1 - с камеры, определяемой настройками 0 источника программы
int VideoSourceType;

/// Список трекеров
std::vector<std::pair<std::string,bool> > Trackers;

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед сбросом модели
virtual void ABeforeReset(void);

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

// Заполняет ComboBox списком видео входов Tld
void LoadVideoInputs(int num_inputs, TComboBox *box);

// Создание копии этого компонента
virtual TTldTrackingForm* New(TComponent *owner=0);

/// Обновляет список трекеров
void UpdateTrackersList(void);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TTldTrackingForm *TldTrackingForm;
//---------------------------------------------------------------------------
#endif
