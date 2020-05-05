//---------------------------------------------------------------------------

#ifndef TMatrixFormUnitH
#define TMatrixFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include "TUVisualControllerFormUnit.h"
//---------------------------------------------------------------------------
class TMatrixForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPanel *ComponentNamePanel;
	TPanel *Panel2;
	TPanel *Panel3;
	TButton *OkButton;
	TButton *CancelButton;
	TStringGrid *StringGrid;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall StringGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);

private:	// User declarations
public:		// User declarations
	__fastcall TMatrixForm(TComponent* Owner);

/// Строка и столбец выбранные в матрице
int SelectedRow,SelectedCol;

/// Имя компонента
std::string ComponentMatrixName;

/// Имя свойства
std::string PropertyMatrixName;

/// Тип свойства
/// 0 - неизвестно
/// 1 - MDMatrix<double>
/// 2 - MDMatrix<int>
/// 3 - MDVector<double>
/// 4 - MDVector<int>
int PropertyType;

// Если force_update == true, то интерфейс обновляется
// вне зависимости от UpdateInterval
virtual void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

/// Выбирает матрицу для наблюдения
bool SelectMatrix(const std::string &comp_name, const std::string &prop_name);
};
//---------------------------------------------------------------------------
extern PACKAGE TMatrixForm *MatrixForm;
//---------------------------------------------------------------------------
#endif
