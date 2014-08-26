//---------------------------------------------------------------------------

#ifndef UStringListInputFormUnitH
#define UStringListInputFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include "TUVisualControllerFormUnit.h"

struct TUParameterDescription
{
/// Имя параметра
std::string Name;

/// Значение параметра
std::string Value;

TUParameterDescription(void);
TUParameterDescription(const TUParameterDescription &copy);

bool operator == (const TUParameterDescription &copy);
bool operator != (const TUParameterDescription &copy);
};
//---------------------------------------------------------------------------
class TUStringListInputForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TGroupBox *GroupBox1;
	TButton *OkButton;
	TButton *CancelButton;
	TStringGrid *StringGrid;
	TButton *RestoreButton;
	TEdit *Edit;
	void __fastcall OkButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall RestoreButtonClick(TObject *Sender);
	void __fastcall EditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall StringGridClick(TObject *Sender);
	void __fastcall StringGridDblClick(TObject *Sender);
	void __fastcall StringGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall FormShow(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TUStringListInputForm(TComponent* Owner);

	std::vector<TUParameterDescription> ParametersList;
	std::vector<TUParameterDescription> ResultParametersList;

/// Число параметров
int GetNumParameters(void);
bool SetNumParameters(int value);

/// Имя параметра с заданным индексом
const std::string& GetParameterName(int index);
bool SetParameterName(int index, const std::string &value);

/// Значение параметра с заданным индексом
const std::string& GetParameterValue(int index);
bool SetParameterValue(int index, const std::string &value, bool as_default);

/// Заголовок окна
std::string GetCaption(void);
bool SetCaption(const std::string &value);

/// Обновляет интерфейс
void AUpdateInterface(void);

};
//---------------------------------------------------------------------------
extern PACKAGE TUStringListInputForm *UStringListInputForm;
//---------------------------------------------------------------------------
#endif
