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
	TButton *Button1;
	TGroupBox *GroupBox1;
	TComboBox *LayersComboBox;
	TImage *Image;
	TSplitter *Splitter1;
	void __fastcall ResetTrainingButtonClick(TObject *Sender);
	void __fastcall ModeComboBoxSelect(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUCRDeepLearningPerseptronForm(TComponent* Owner);

RDK::UEPtr<RDK::UCRPerseptronDeepLearning> Learning;
RDK::UEPtr<RDK::UCRPerseptron> Perseptron;
//std::vector<std::vector<std::vector<double> > > Weights;
//std::vector<std::vector<double> > Outputs;

RDK::UBitmap Bmp;

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

void DrawImageFromPerseptronData(const std::vector<double> &data, int image_width, int image_height, double min_output,
								 double max_output, RDK::UBitmap &result);

};
//---------------------------------------------------------------------------
extern PACKAGE TUCRDeepLearningPerseptronForm *UCRDeepLearningPerseptronForm;
//---------------------------------------------------------------------------
#endif
