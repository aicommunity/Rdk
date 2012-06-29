//---------------------------------------------------------------------------

#ifndef UWatchWindowFormUnitH
#define UWatchWindowFormUnitH
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
#pragma warn -8130
#include "TUVisualController.h"
//---------------------------------------------------------------------------
class TUWatchWindowForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
		TToolBar *ToolBar1;
		TToolButton *ToolButton1;
		TToolButton *ToolButton2;
		TToolButton *ToolButton3;
		TToolButton *ToolButton4;
		TImageList *ImageList1;
	TUWatchFrame *WatchFrame;
        void __fastcall FormResize(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall ToolButton1Click(TObject *Sender);
        void __fastcall ToolButton4Click(TObject *Sender);
        void __fastcall ToolButton2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TUWatchWindowForm(TComponent* Owner);

protected:	// Данные
        // -----------------------------------------
        // Данные о состоянии
        // -----------------------------------------
        // Содержит 'true' если данные серий были изменены
        // или были добавлены/удалены новые серии
        bool ModifyState;
        // -----------------------------------------

public:	        // Методы
		// ------------------------------
		// Методы управления состоянием
		// ------------------------------
		// Возвращает 'true', если данные в сериях были изменены,
		// или если серии были добавлены/удалены
		bool __fastcall GetModifyState(void);

		// Возвращает текущий графический фрейм
		TUWatchFrame* GetCurrentWatchFrame(void);

		void AUpdateInterface(void);
		void ABeforeCalculate(void);
		void AAfterCalculate(void);
		// ------------------------------

};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUWatchWindowForm *UWatchWindowForm;
//---------------------------------------------------------------------------
#endif
