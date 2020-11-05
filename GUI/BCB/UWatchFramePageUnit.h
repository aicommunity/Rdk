//---------------------------------------------------------------------------

#ifndef UWatchFramePageUnitH
#define UWatchFramePageUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include "TUVisualControllerFrameUnit.h"
#include "UWatchFrameUnit.h"
#include <Vcl.Menus.hpp>
//---------------------------------------------------------------------------
class TUWatchFramePage : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TPageControl *NTWPageControl;
	TPopupMenu *NTWPageControlPopupMenu;
	TMenuItem *AddPage1;
	TMenuItem *DeletePage1;
	TMenuItem *RanamePage1;
private:	// User declarations
public:		// User declarations

// ------------------------------
// Методы управления состоянием
// ------------------------------
void AUpdateInterface(void);
void ABeforeCalculate(void);
void AAfterCalculate(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// Создание копии этого компонента
virtual TUWatchFramePage* New(TComponent *owner=0);
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


	__fastcall TUWatchFramePage(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUWatchFramePage *UWatchFramePage;
//---------------------------------------------------------------------------
#endif
