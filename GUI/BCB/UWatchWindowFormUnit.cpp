//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UWatchWindowFormUnit.h"
#include "UListInputFormUnit.h"
#include "UEngineMonitorFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UWatchFrameUnit"
#pragma resource "*.dfm"
TUWatchWindowForm *UWatchWindowForm;
//---------------------------------------------------------------------------
__fastcall TUWatchWindowForm::TUWatchWindowForm(TComponent* Owner)
        : TUVisualControllerForm(Owner)
{
// WatchFrame->Model=0;
 ModifyState=false;
}
//---------------------------------------------------------------------------

// ------------------------------
// Методы управления состоянием
// ------------------------------
// Возвращает 'true', если данные в сериях были изменены,
// или если серии были добавлены/удалены
bool __fastcall TUWatchWindowForm::GetModifyState(void)
{
 if(ModifyState || WatchFrame->GetModifyState())
  {
   ModifyState=false;
   return true;
  }

 return false;
}

// Возвращает текущий графический фрейм
TUWatchFrame* TUWatchWindowForm::GetCurrentWatchFrame(void)
{
 return WatchFrame;
}

void TUWatchWindowForm::AUpdateInterface(void)
{
 GetCurrentWatchFrame()->StepUpdate(false);
 GetCurrentWatchFrame()->Chart1->Repaint();
}

void TUWatchWindowForm::ABeforeCalculate(void)
{

}

void TUWatchWindowForm::AAfterCalculate(void)
{

}
// ------------------------------

void __fastcall TUWatchWindowForm::FormResize(TObject *Sender)
{
 ModifyState=true;
}
//---------------------------------------------------------------------------

void __fastcall TUWatchWindowForm::FormShow(TObject *Sender)
{
 ModifyState=true;
}
//---------------------------------------------------------------------------

void __fastcall TUWatchWindowForm::FormHide(TObject *Sender)
{
 ModifyState=true;
}
//---------------------------------------------------------------------------

void __fastcall TUWatchWindowForm::FormPaint(TObject *Sender)
{
// if(Caption != WatchFrame->Chart1->Title->Text->Strings[0])
//  WatchFrame->Chart1->Title->Text->Strings[0]=Caption;
}
//---------------------------------------------------------------------------

void __fastcall TUWatchWindowForm::ToolButton1Click(TObject *Sender)
{
 WatchFrame->TBSeriesModify(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUWatchWindowForm::ToolButton4Click(TObject *Sender)
{
 WatchFrame->Save();
}
//---------------------------------------------------------------------------

void __fastcall TUWatchWindowForm::ToolButton2Click(TObject *Sender)
{
 vector<string> listvals;

 // Подготовка формы запроса имени
 UListInputForm->PresentSelect=false;
 UListInputForm->MustInput=true;
 UListInputForm->Sort=false;

 UListInputForm->Init("Список имен открытых окно наблюдения",listvals,"");
 // ...подготовка формы запроса завершена

 if(UListInputForm->ShowModal() != mrOk)
  return;

 Caption=UListInputForm->Edit->Text;
}
//---------------------------------------------------------------------------


