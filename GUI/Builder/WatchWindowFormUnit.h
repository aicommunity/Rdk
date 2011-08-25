//---------------------------------------------------------------------------

#ifndef WatchWindowFormUnitH
#define WatchWindowFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include "WatchFrameUnit.h"
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TWatchWindowForm : public TForm
{
__published:	// IDE-managed Components
        TWatchFrame *WatchFrame;
        TToolBar *ToolBar1;
        TToolButton *ToolButton1;
        TToolButton *ToolButton2;
        TToolButton *ToolButton3;
        TToolButton *ToolButton4;
        TImageList *ImageList1;
        void __fastcall FormResize(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall ToolButton1Click(TObject *Sender);
        void __fastcall ToolButton4Click(TObject *Sender);
        void __fastcall ToolButton2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TWatchWindowForm(TComponent* Owner);

protected:	// ƒанные
        // -----------------------------------------
        // ƒанные о состо€нии
        // -----------------------------------------
        // —одержит 'true' если данные серий были изменены
        // или были добавлены/удалены новые серии
        bool ModifyState;
        // -----------------------------------------

public:	        // ћетоды
        // ------------------------------
        // ћетоды управлени€ состо€нием
        // ------------------------------
        // ¬озвращает 'true', если данные в сери€х были изменены,
        // или если серии были добавлены/удалены
        bool __fastcall GetModifyState(void);
        // ------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TWatchWindowForm *WatchWindowForm;
//---------------------------------------------------------------------------
#endif
