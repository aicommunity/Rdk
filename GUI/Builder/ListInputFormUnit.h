#ifndef ListInputFormUnitH
#define ListInputFormUnitH

/* ***********************************************************
   @Copyright Alexander V. Bakhshiev, 2005-2009.
   E-mail:     alexab@ailab.ru
   url:     www.ailab.ru

   This file - part of the RDK


   version 2.0.0 25.06.2009
*********************************************************** */

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

using namespace std;
//---------------------------------------------------------------------------
class TListInputForm : public TForm
{
__published:    // IDE-managed Components
        TStringGrid *List;
        TPanel *Panel1;
        TBitBtn *BitBtn1;
        TBitBtn *BitBtn2;
        TEdit *Edit;
        TPanel *Panel2;
        TButton *Button1;
        void __fastcall FormHide(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall BitBtn1Click(TObject *Sender);
        void __fastcall ListDblClick(TObject *Sender);
        void __fastcall EditChange(TObject *Sender);
        void __fastcall EditKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
private:    // User declarations
public:        // User declarations
        __fastcall TListInputForm(TComponent* Owner);

protected:        // User declarations
        // Параметры окна по умолчанию
        int top, left, width, height;
        String caption;

public:        // User declarations
        // Указывает будет ли форма запроса требовать ввод только новой строки
        // или будет требовать выбор уже существующей
        // Если '0' - то ввод только новой
        // Если '1' - то только выбор из существующих
        // Если '2' - то только оба варианта выбор из существующих
        int PresentSelect;

        // Указывает будет ли форма запроса требовать обязателный ввод
        // не пустой строки
        // Если 'true' - то ввод обязателен
        // Если 'false' - то нет
        bool MustInput;

        // Указывает включена или выключена сортировка в списке
        bool Sort;

public:        // User declarations
        // Метод задаёт заголовок окна, список вариантов
        // и начальное значение поля ввода
        void __fastcall Init(string caption,vector<string> &listvals,string deftext);

        // Отображает сообщение об ошибке
        void __fastcall ShowError(string cap, string msg);
};
//---------------------------------------------------------------------------
extern PACKAGE TListInputForm *ListInputForm;
//---------------------------------------------------------------------------
#endif
