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
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TTldTrackingForm *TldTrackingForm;
//---------------------------------------------------------------------------
#endif
