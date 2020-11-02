//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UWatchFramePageUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUWatchFramePage *UWatchFramePage;
//---------------------------------------------------------------------------
__fastcall TUWatchFramePage::TUWatchFramePage(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
}
//---------------------------------------------------------------------------

// ------------------------------
// Методы управления состоянием
// ------------------------------
void TUWatchFramePage::AUpdateInterface(void)
{
	//
}

// Возврат интерфейса в исходное состояние
void TUWatchFramePage::AClearInterface(void)
{
 ClearPages();
}

void TUWatchFramePage::ABeforeCalculate(void)
{
	//
}

void TUWatchFramePage::AAfterCalculate(void)
{
	//
}

// Сохраняет параметры интерфейса в xml
void TUWatchFramePage::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("PageCount",NTWPageControl->PageCount);
 xml.SelectNodeForce("Pages");
 for(int i=0;i<NTWPageControl->PageCount;i++)
 {
  xml.WriteString(string("Caption_")+RDK::sntoa(i+1),AnsiString(NTWPageControl->Pages[i]->Caption).c_str());
 }
 xml.SelectUp();
}

// Загружает параметры интерфейса из xml
void TUWatchFramePage::ALoadParameters(RDK::USerStorageXML &xml)
{
 int count=xml.ReadInteger("PageCount",0);

 ClearPages();
 for(int i=1;i<count;i++)
  AddPage();

 xml.SelectNodeForce("Pages");
 for(int i=0;i<NTWPageControl->PageCount;i++)
 {
  NTWPageControl->Pages[i]->Caption=xml.ReadString(string("Caption_")+RDK::sntoa(i+1),std::string("Page")+RDK::sntoa(i+1)).c_str();
 }
 xml.SelectUp();
}

// Создание копии этого компонента
TUWatchFramePage* TUWatchFramePage::New(TComponent *owner)
{
 return new TUWatchFramePage(owner);
}
// ------------------------------


// ------------------------------
// Методы управления страницами
// ------------------------------
// Удаляет все лишние вкладки (оставляет 1 начальную)
void TUWatchFramePage::ClearPages(void)
{
 while(NTWPageControl->PageCount > 1)
  delete NTWPageControl->Pages[NTWPageControl->PageCount-1];
}

// Добавляет страницу
void TUWatchFramePage::AddPage(void)
{
 TTabSheet* tab=new TTabSheet(NTWPageControl);
 tab->PageControl=NTWPageControl;
 TUWatchFrame *frame=new TUWatchFrame(0);
 frame->Name=frame->Name+String("_")+NTWPageControl->PageCount;
 InsertComponent(frame);
 frame->Parent=tab;
 frame->Align=alClient;

 tab->Caption=String("Page")+IntToStr(NTWPageControl->PageCount);
}

// Удаляет страницу
void TUWatchFramePage::DelPage(int index)
{
 if(index < NTWPageControl->PageCount && index >= 0)
  delete NTWPageControl->Pages[index];

}

// Переименовывает заголовок страницы
void TUWatchFramePage::RenamePage(int index, String new_name)
{
 if(index < NTWPageControl->PageCount && index >= 0)
  NTWPageControl->Pages[index]->Caption=new_name;
}
// ------------------------------



