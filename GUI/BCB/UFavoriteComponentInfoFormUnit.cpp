//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UFavoriteComponentInfoFormUnit.h"
#include "UFavoriteComponentInfoFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUFavoriteComponentInfoForm *UFavoriteComponentInfoForm;
//---------------------------------------------------------------------------
__fastcall TUFavoriteComponentInfoForm::TUFavoriteComponentInfoForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

// Обновляет интерфейс
void TUFavoriteComponentInfoForm::AUpdateInterface(void)
{

}

// Сохраняет параметры интерфейса в xml
void TUFavoriteComponentInfoForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("PageCount",PageControl->PageCount);
 xml.SelectNodeForce("Pages");
 for(int i=0;i<PageControl->PageCount;i++)
 {
  xml.WriteString(std::string("Caption_")+RDK::sntoa(i+1),AnsiString(PageControl->Pages[i]->Caption).c_str());
 }
 xml.SelectUp();
}

// Загружает параметры интерфейса из xml
void TUFavoriteComponentInfoForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 int count=xml.ReadInteger("PageCount",0);

 ClearPages();
 for(int i=1;i<count;i++)
  AddPage();

 xml.SelectNodeForce("Pages");
 for(int i=0;i<PageControl->PageCount;i++)
 {
  PageControl->Pages[i]->Caption=xml.ReadString(std::string("Caption_")+RDK::sntoa(i+1),std::string("Page")+RDK::sntoa(i+1)).c_str();
 }
 xml.SelectUp();
}


// ------------------------------
// Методы управления страницами
// ------------------------------
// Удаляет все лишние вкладки (оставляет 1 начальную)
void TUFavoriteComponentInfoForm::ClearPages(void)
{
 while(PageControl->PageCount > 1)
  delete PageControl->Pages[PageControl->PageCount-1];
}

// Добавляет страницу
void TUFavoriteComponentInfoForm::AddPage(void)
{
 TTabSheet* tab=new TTabSheet(PageControl);
 tab->PageControl=PageControl;
 TUFavoriteComponentInfoFrame *frame=new TUFavoriteComponentInfoFrame(0);
 frame->Name=frame->Name+String("_")+PageControl->PageCount;
 InsertComponent(frame);
 frame->Parent=tab;
 frame->Align=alClient;

 tab->Caption=String("Page")+IntToStr(PageControl->PageCount);
}

// Удаляет страницу
void TUFavoriteComponentInfoForm::DelPage(int index)
{
 if(index < PageControl->PageCount && index >= 0)
  delete PageControl->Pages[index];

}

// Переименовывает заголовок страницы
void TUFavoriteComponentInfoForm::RenamePage(int index, String new_name)
{
 if(index < PageControl->PageCount && index >= 0)
  PageControl->Pages[index]->Caption=new_name;
}
// ------------------------------
//---------------------------------------------------------------------------
void __fastcall TUFavoriteComponentInfoForm::AddPage1Click(TObject *Sender)
{
 AddPage();
}
//---------------------------------------------------------------------------
void __fastcall TUFavoriteComponentInfoForm::DeletePage1Click(TObject *Sender)
{
 if(PageControl->ActivePageIndex != 0)
  DelPage(PageControl->ActivePageIndex);
}
//---------------------------------------------------------------------------
void __fastcall TUFavoriteComponentInfoForm::RenamePage1Click(TObject *Sender)
{
 //
}
//---------------------------------------------------------------------------
void __fastcall TUFavoriteComponentInfoForm::FormCreate(TObject *Sender)
{
 AddPage();
}
//---------------------------------------------------------------------------
