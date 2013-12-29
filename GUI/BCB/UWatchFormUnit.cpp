//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UWatchFormUnit.h"
#include "UListInputFormUnit.h"
#include "UEngineMonitorFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UWatchFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUWatchForm *UWatchForm;
//---------------------------------------------------------------------------
__fastcall TUWatchForm::TUWatchForm(TComponent* Owner)
        : TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------

// ------------------------------
// Методы управления состоянием
// ------------------------------
void TUWatchForm::AUpdateInterface(void)
{

}

// Возврат интерфейса в исходное состояние
void TUWatchForm::AClearInterface(void)
{
 ClearPages();
}

void TUWatchForm::ABeforeCalculate(void)
{

}

void TUWatchForm::AAfterCalculate(void)
{
}

// Сохраняет параметры интерфейса в xml
void TUWatchForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("PageCount",PageControl->PageCount);
 xml.SelectNodeForce("Pages");
 for(int i=0;i<PageControl->PageCount;i++)
 {
  xml.WriteString(string("Caption_")+RDK::sntoa(i+1),AnsiString(PageControl->Pages[i]->Caption).c_str());
 }
 xml.SelectUp();
}

// Загружает параметры интерфейса из xml
void TUWatchForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 int count=xml.ReadInteger("PageCount",0);

 ClearPages();
 for(int i=1;i<count;i++)
  AddPage();

 xml.SelectNodeForce("Pages");
 for(int i=0;i<PageControl->PageCount;i++)
 {
  PageControl->Pages[i]->Caption=xml.ReadString(string("Caption_")+RDK::sntoa(i+1),std::string("Page")+RDK::sntoa(i+1)).c_str();
 }
 xml.SelectUp();
}

// Создание копии этого компонента
TUWatchForm* TUWatchForm::New(TComponent *owner)
{
 return new TUWatchForm(owner);
}
// ------------------------------


// ------------------------------
// Методы управления страницами
// ------------------------------
// Удаляет все лишние вкладки (оставляет 1 начальную)
void TUWatchForm::ClearPages(void)
{
 while(PageControl->PageCount > 1)
  delete PageControl->Pages[PageControl->PageCount-1];
}

// Добавляет страницу
void TUWatchForm::AddPage(void)
{
 TTabSheet* tab=new TTabSheet(PageControl);
 tab->PageControl=PageControl;
 TUWatchFrame *frame=new TUWatchFrame(0);
 frame->Name=frame->Name+String("_")+PageControl->PageCount;
 InsertComponent(frame);
 frame->Parent=tab;
 frame->Align=alClient;

 tab->Caption=String("Page")+IntToStr(PageControl->PageCount);
}

// Удаляет страницу
void TUWatchForm::DelPage(int index)
{
 if(index < PageControl->PageCount && index >= 0)
  delete PageControl->Pages[index];

}

// Переименовывает заголовок страницы
void TUWatchForm::RenamePage(int index, String new_name)
{
 if(index < PageControl->PageCount && index >= 0)
  PageControl->Pages[index]->Caption=new_name;
}
// ------------------------------


//---------------------------------------------------------------------------



void __fastcall TUWatchForm::FormPaint(TObject *Sender)
{
// if(Caption != WatchFrame->Chart1->Title->Text->Strings[0])
//  WatchFrame->Chart1->Title->Text->Strings[0]=Caption;
}
//---------------------------------------------------------------------------



void __fastcall TUWatchForm::ToolButton2Click(TObject *Sender)
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


void __fastcall TUWatchForm::AddPage1Click(TObject *Sender)
{
 AddPage();
}
//---------------------------------------------------------------------------

void __fastcall TUWatchForm::DeletePage1Click(TObject *Sender)
{
 if(PageControl->ActivePageIndex != 0)
  DelPage(PageControl->ActivePageIndex);
}
//---------------------------------------------------------------------------

void __fastcall TUWatchForm::RenamePage1Click(TObject *Sender)
{
// RenamePage(PageControl->ActivePageIndex);
}
//---------------------------------------------------------------------------

void __fastcall TUWatchForm::FormCreate(TObject *Sender)
{
 AddPage();
}
//---------------------------------------------------------------------------

