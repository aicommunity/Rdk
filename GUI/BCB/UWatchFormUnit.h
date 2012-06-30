//---------------------------------------------------------------------------

#ifndef UWatchFormUnitH
#define UWatchFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Menus.hpp>
#include "UWatchFrameUnit.h"
#include "TUVisualControllerFormUnit.h"
#include "TUVisualControllerFrameUnit.h"
#pragma warn -8130
#include "TUVisualController.h"
//---------------------------------------------------------------------------
class TUWatchForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TPopupMenu *PageControlPopupMenu;
	TMenuItem *AddPage1;
	TMenuItem *DeletePage1;
	TMenuItem *RenamePage1;
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall ToolButton2Click(TObject *Sender);
	void __fastcall AddPage1Click(TObject *Sender);
	void __fastcall DeletePage1Click(TObject *Sender);
	void __fastcall RenamePage1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TUWatchForm(TComponent* Owner);

protected:	// Данные

public:	        // Методы
// ------------------------------
// Методы управления состоянием
// ------------------------------
void AUpdateInterface(void);
void ABeforeCalculate(void);
void AAfterCalculate(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::Serialize::USerStorageXML &xml);
// ------------------------------

// ------------------------------
// Методы управления страницами
// ------------------------------
// Удаляет все лишние вкладки (оставляет 1 начальную)
void ClearPages(void);

// Добавляет страницу
void AddPage(void);

// Удаляет страницу
void DelPage(int index);

// Переименовывает заголовок страницы
void RenamePage(int index, String new_name);
// ------------------------------

};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUWatchForm *UWatchForm;
//---------------------------------------------------------------------------
#endif
