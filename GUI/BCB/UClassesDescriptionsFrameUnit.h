//---------------------------------------------------------------------------

#ifndef UClassesDescriptionsFrameUnitH
#define UClassesDescriptionsFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.ExtCtrls.hpp>
#include "UClassesListFrameUnit.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
//---------------------------------------------------------------------------
class TUClassesDescriptionsFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TUClassesListFrame *ClassesListFrame;
	TPanel *Panel3;
	TLabeledEdit *ClassNameLabeledEdit;
	TLabeledEdit *ClassHeaderLabeledEdit;
	TGroupBox *ClassDescriptionGroupBox;
	TRichEdit *ClassDescriptionRichEdit;
	TGroupBox *GroupBox1;
	TStringGrid *PropertiesListStringGrid;
	TPanel *Panel4;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TRichEdit *ClassesAdditionalInfoRichEdit;
	TRichEdit *PropertyDescriptionRichEdit;
	TButton *ApplyButton;
	TButton *RestoreButton;
	TButton *SaveDescriptionsButton;
	void __fastcall ClassesListFrameTreeViewClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUClassesDescriptionsFrame(TComponent* Owner);

/// Флаг, выставляемый, если описание текущего класса было изменено
bool IsDescrModified;

/// Имя класса текущего редактируемого описания
String CurrentClassName;

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
virtual TUClassesDescriptionsFrame* New(TComponent *owner=0);

/// Сохраняет
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassesDescriptionsFrame *UClassesDescriptionsFrame;
//---------------------------------------------------------------------------
#endif
