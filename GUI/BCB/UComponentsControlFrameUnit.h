//---------------------------------------------------------------------------

#ifndef UComponentsControlFrameUnitH
#define UComponentsControlFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "UClassesListFrameUnit.h"
#include "UComponentsListFrameUnit.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
#include "UClassesListFrameUnit.h"
#include "UComponentsListFrameUnit.h"
#include "TUVisualController.h"
#include "TUVisualControllerFrameUnit.h"
//---------------------------------------------------------------------------
class TUComponentsControlFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TSplitter *Splitter1;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TPanel *Panel3;
	TButton *TakeObjectButton;
	TButton *ReturnObjectButton;
	THeaderControl *HeaderControl;
	TOpenTextFileDialog *OpenTextFileDialog;
	TSaveTextFileDialog *SaveTextFileDialog;
	TUClassesListFrame *ClassesListFrame;
	TUComponentsListFrame *ComponentsListFrame;
	void __fastcall TakeObjectButtonClick(TObject *Sender);
	void __fastcall ReturnObjectButtonClick(TObject *Sender);
	void __fastcall ClassesListFrameStringGridKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall HeaderControlSectionClick(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall ClassesListFrameStringGridDblClick(TObject *Sender);


private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsControlFrame(TComponent* Owner);

protected: // Данные

public: // Методы
void AUpdateInterface(void);

// Сохраняет выбранную модель
// Если filename == "", то открывает окно запроса диалога
void SaveModelToFile(const String &filename);

// Загружает выбранную модель
// Если filename == "", то открывает окно запроса диалога
void LoadModelFromFile(const String &filename);

};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsControlFrame *UComponentsControlFrame;
//---------------------------------------------------------------------------
#endif
