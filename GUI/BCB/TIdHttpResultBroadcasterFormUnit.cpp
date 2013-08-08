//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TIdHttpResultBroadcasterFormUnit.h"
#include "UGEngineControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TIdHttpResultBroadcasterForm *IdHttpResultBroadcasterForm;
//---------------------------------------------------------------------------
__fastcall TIdHttpResultBroadcasterForm::TIdHttpResultBroadcasterForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

// Метод, вызываемый перед шагом расчета
void TIdHttpResultBroadcasterForm::ABeforeCalculate(void)
{
}

// Обновляет интерфейс
void TIdHttpResultBroadcasterForm::AUpdateInterface(void)
{
}

// Сохраняет параметры интерфейса в xml
void TIdHttpResultBroadcasterForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("NumBroadcasters",GetNumBroadcasters());
}

// Загружает параметры интерфейса из xml
void TIdHttpResultBroadcasterForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 int num=xml.ReadInteger("NumBroadcasters",1);
 ClearBroadcasters();
 for(int i=0;i<num;i++)
  AddBroadcaster();
 UpdateInterface();
}

// Создание копии этого компонента
TIdHttpResultBroadcasterForm* TIdHttpResultBroadcasterForm::New(TComponent *owner)
{
 return new TIdHttpResultBroadcasterForm(owner);
}

// Число вещателей
int TIdHttpResultBroadcasterForm::GetNumBroadcasters(void) const
{
 return PageControl->PageCount;
}

// Добавляет новый вещатель
void TIdHttpResultBroadcasterForm::AddBroadcaster(void)
{
 TTabSheet *sheet=new TTabSheet(PageControl);
 sheet->PageControl=PageControl;
 sheet->PageIndex=PageControl->PageCount-1;
 Broadcasters.resize(PageControl->PageCount);

 size_t index=Broadcasters.size()-1;

 Broadcasters[index]=new TIdHttpResultBroadcasterFrame(0);
 Broadcasters[index]->Name=Broadcasters[index]->Name+String("_")+PageControl->PageCount;

 InsertComponent(Broadcasters[index]);
 Broadcasters[index]->Parent=sheet;
 Broadcasters[index]->Align=alClient;
 PageControl->Pages[index]->Caption=IntToStr(int(index));
}

// Удаляет вещатель
void TIdHttpResultBroadcasterForm::DelBroadcaster(int index)
{
 if(index<0 || index >=int(Broadcasters.size()))
  return;

 delete Broadcasters[index];
 Broadcasters.erase(Broadcasters.begin()+index);
 delete PageControl->Pages[index];
}

// Удаляет все вещатели
void TIdHttpResultBroadcasterForm::ClearBroadcasters(void)
{
 for(size_t i=0;i<Broadcasters.size();i++)
 {
  delete Broadcasters[i];
  delete PageControl->Pages[0];
 }

 Broadcasters.clear();
}

// Возвращает индекс текущего активного (выбранного) вещателя
int TIdHttpResultBroadcasterForm::GetActiveBroadcaster(void) const
{
 return PageControl->ActivePageIndex;
}

// Возвращает фрейм вещателя
TIdHttpResultBroadcasterFrame* TIdHttpResultBroadcasterForm::GetBroadcasterFrame(int index)
{
 if(index<0 || index >=int(Broadcasters.size()))
  return 0;

 return Broadcasters[index];
}

// Возвращает фрейм активного (выбранного) вещателя
TIdHttpResultBroadcasterFrame* TIdHttpResultBroadcasterForm::GetActiveBroadcasterFrame(void)
{
 if(GetActiveBroadcaster()>=0)
  return Broadcasters[GetActiveBroadcaster()];

 return 0;
}

// Подключает вещатель, или все, если index == -1
void TIdHttpResultBroadcasterForm::Connect(int index)
{
 if(index>=0 && index<GetNumBroadcasters())
  Broadcasters[index]->ConnectButtonClick(this);
 else
 {
  for(int i=0;i<GetNumBroadcasters();i++)
   Broadcasters[i]->ConnectButtonClick(this);
 }
}

// Отключает вещатель, или все, если index == -1
void TIdHttpResultBroadcasterForm::Disconnect(int index)
{
 if(index>=0 && index<GetNumBroadcasters())
  Broadcasters[index]->DisconnectButtonClick(this);
 else
 {
  for(int i=0;i<GetNumBroadcasters();i++)
   Broadcasters[i]->DisconnectButtonClick(this);
 }
}

//---------------------------------------------------------------------------
void __fastcall TIdHttpResultBroadcasterForm::Add1Click(TObject *Sender)
{
 AddBroadcaster();
}
//---------------------------------------------------------------------------
void __fastcall TIdHttpResultBroadcasterForm::Del1Click(TObject *Sender)
{
 if(GetNumBroadcasters()<=0)
  return;

 DelBroadcaster(PageControl->ActivePageIndex);
}
//---------------------------------------------------------------------------
void __fastcall TIdHttpResultBroadcasterForm::ConnectAll1Click(TObject *Sender)
{
 Connect();
}
//---------------------------------------------------------------------------
void __fastcall TIdHttpResultBroadcasterForm::DisconnectAll1Click(TObject *Sender)
{
 Disconnect();
}
//---------------------------------------------------------------------------

void __fastcall TIdHttpResultBroadcasterForm::FormDestroy(TObject *Sender)
{
// UGEngineControlForm->SpecialForms.erase("TIdHttpResultBroadcasterForm");
}
//---------------------------------------------------------------------------

void __fastcall TIdHttpResultBroadcasterForm::FormCreate(TObject *Sender)
{
 UGEngineControlForm->SpecialForms["TIdHttpResultBroadcasterForm"]=this;
 UGEngineControlForm->AddBroadcasterMenu(HttpBroadcaster1,PopupMenu1);
}
//---------------------------------------------------------------------------

void __fastcall TIdHttpResultBroadcasterForm::HttpBroadcaster1Click(TObject *Sender)

{
 UGEngineControlForm->AddSpecialFormPage("TIdHttpResultBroadcasterForm");
 Show();
}
//---------------------------------------------------------------------------

