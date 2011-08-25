//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WatchWindowFormUnit.h"
#include "ListInputFormUnit.h"
//#include "MainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "WatchFrameUnit"
#pragma resource "*.dfm"
TWatchWindowForm *WatchWindowForm;
//---------------------------------------------------------------------------
__fastcall TWatchWindowForm::TWatchWindowForm(TComponent* Owner)
        : TForm(Owner)
{
// WatchFrame->Model=0;
 ModifyState=false;
}
//---------------------------------------------------------------------------

// ------------------------------
// ћетоды управлени€ состо€нием
// ------------------------------
// ¬озвращает 'true', если данные в сери€х были изменены,
// или если серии были добавлены/удалены
bool __fastcall TWatchWindowForm::GetModifyState(void)
{
 if(ModifyState || WatchFrame->GetModifyState())
  {
   ModifyState=false;
   return true;
  }

 return false;
}
// ------------------------------

void __fastcall TWatchWindowForm::FormResize(TObject *Sender)
{
 ModifyState=true;
}
//---------------------------------------------------------------------------

void __fastcall TWatchWindowForm::FormShow(TObject *Sender)
{
 ModifyState=true;
}
//---------------------------------------------------------------------------

void __fastcall TWatchWindowForm::FormHide(TObject *Sender)
{
 ModifyState=true;
}
//---------------------------------------------------------------------------

void __fastcall TWatchWindowForm::FormPaint(TObject *Sender)
{
// if(Caption != WatchFrame->Chart1->Title->Text->Strings[0])
//  WatchFrame->Chart1->Title->Text->Strings[0]=Caption;
}
//---------------------------------------------------------------------------

void __fastcall TWatchWindowForm::ToolButton1Click(TObject *Sender)
{
 WatchFrame->TBSeriesModify(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TWatchWindowForm::ToolButton4Click(TObject *Sender)
{
 WatchFrame->Save();
}
//---------------------------------------------------------------------------

void __fastcall TWatchWindowForm::ToolButton2Click(TObject *Sender)
{
 vector<string> listvals;

 // ѕодготовка формы запроса имени
 ListInputForm->PresentSelect=false;
 ListInputForm->MustInput=true;
 ListInputForm->Sort=false;

// MainForm->Modeler->GetWatchList(listvals);
 ListInputForm->Init("—писок имен открытых окно наблюдени€",listvals,"");
 // ...подготовка формы запроса завершена

 if(ListInputForm->ShowModal() != mrOk)
  return;

 Caption=ListInputForm->Edit->Text;
}
//---------------------------------------------------------------------------

