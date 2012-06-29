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
// ������ ���������� ����������
// ------------------------------
// ���������� 'true', ���� ������ � ������ ���� ��������,
// ��� ���� ����� ���� ���������/�������
bool __fastcall TUWatchWindowForm::GetModifyState(void)
{
 if(ModifyState || WatchFrame->GetModifyState())
  {
   ModifyState=false;
   return true;
  }

 return false;
}

// ���������� ������� ����������� �����
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

 // ���������� ����� ������� �����
 UListInputForm->PresentSelect=false;
 UListInputForm->MustInput=true;
 UListInputForm->Sort=false;

 UListInputForm->Init("������ ���� �������� ���� ����������",listvals,"");
 // ...���������� ����� ������� ���������

 if(UListInputForm->ShowModal() != mrOk)
  return;

 Caption=UListInputForm->Edit->Text;
}
//---------------------------------------------------------------------------


