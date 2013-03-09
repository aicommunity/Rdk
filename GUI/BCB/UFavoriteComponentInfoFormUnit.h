//---------------------------------------------------------------------------

#ifndef UFavoriteComponentInfoFormUnitH
#define UFavoriteComponentInfoFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFormUnit.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Menus.hpp>
//---------------------------------------------------------------------------
class TUFavoriteComponentInfoForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TPopupMenu *PopupMenu;
	TMenuItem *AddPage1;
	TMenuItem *DeletePage1;
	TMenuItem *RenamePage1;
	void __fastcall AddPage1Click(TObject *Sender);
	void __fastcall DeletePage1Click(TObject *Sender);
	void __fastcall RenamePage1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUFavoriteComponentInfoForm(TComponent* Owner);

// Обновляет интерфейс
void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
void ALoadParameters(RDK::USerStorageXML &xml);

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
};
//---------------------------------------------------------------------------
extern PACKAGE TUFavoriteComponentInfoForm *UFavoriteComponentInfoForm;
//---------------------------------------------------------------------------
#endif
