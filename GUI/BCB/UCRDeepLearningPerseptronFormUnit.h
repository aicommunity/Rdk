//---------------------------------------------------------------------------

#ifndef UCRDeepLearningPerseptronFormUnitH
#define UCRDeepLearningPerseptronFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFormUnit.h"
#include "rdk_cpp_initdll.h"
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TUCRDeepLearningPerseptronForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TButton *ResetTrainingButton;
	TComboBox *ModeComboBox;
	TLabel *Label1;
	void __fastcall ResetTrainingButtonClick(TObject *Sender);
	void __fastcall ModeComboBoxSelect(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUCRDeepLearningPerseptronForm(TComponent* Owner);

RDK::UEPtr<RDK::UCRPerseptronDeepLearning> Learning;

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
virtual void AAfterReset(void);

// Обновление интерфейса
virtual void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::Serialize::USerStorageXML &xml);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUCRDeepLearningPerseptronForm *UCRDeepLearningPerseptronForm;
//---------------------------------------------------------------------------
#endif
