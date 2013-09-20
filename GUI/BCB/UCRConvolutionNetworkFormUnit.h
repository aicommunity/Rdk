//---------------------------------------------------------------------------

#ifndef UCRCVNetworkFormUnitH
#define UCRCVNetworkFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFormUnit.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include "rdk_cpp_initdll.h"
//---------------------------------------------------------------------------
class TUCRConvolutionNetworkForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TPanel *Panel1;
	TStringGrid *WeightsStringGrid;
	TStringGrid *OutputsStringGrid;
	TComboBox *WeightsComboBox;
	TButton *Button1;
	void __fastcall WeightsComboBoxSelect(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUCRConvolutionNetworkForm(TComponent* Owner);

public:

//std::vector<std::vector<std::vector<double> > > Weights;
//std::vector<double> Outputs;
RDK::MDMatrix<double> Result;

RDK::UEPtr<RDK::UCRConvolutionNetwork> Network;

bool ShowWeightFlag;

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
virtual void AAfterReset(void);

// Обновление интерфейса
virtual void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// Создание копии этого компонента
virtual TUCRConvolutionNetworkForm* New(TComponent *owner=0);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUCRConvolutionNetworkForm *UCRConvolutionNetworkForm;
//---------------------------------------------------------------------------
#endif
