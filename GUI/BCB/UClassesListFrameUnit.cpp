//---------------------------------------------------------------------------

#include <vcl.h>
#include <vector>
#pragma hdrstop

#include "UClassesListFrameUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUClassesListFrame *UClassesListFrame;
//---------------------------------------------------------------------------
__fastcall TUClassesListFrame::TUClassesListFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 UpdateInterfaceFlag=false;
}
//---------------------------------------------------------------------------

// Отрисовка фрейма
void TUClassesListFrame::AUpdateInterface(void)
{
 int numclasses=Storage_GetNumClasses();

 int row=StringGrid->Row;
 StringGrid->ColCount=2;
 StringGrid->RowCount=numclasses+1;

 std::vector<int> ids;
 ids.resize(numclasses);
 Storage_GetClassesList(&ids[0]);
 for(int i=0;i<numclasses;i++)
 {
  StringGrid->Cells[0][i+1]=IntToStr(ids[i]);
  StringGrid->Cells[1][i+1]=Storage_GetClassName(ids[i]);
 }

 StringGrid->ColWidths[0]=30;
 StringGrid->ColWidths[1]=Width-StringGrid->ColWidths[0];

 StringGrid->Cells[0][0]="Id";
 StringGrid->Cells[1][0]="Class name";

 if(row<StringGrid->RowCount)
  StringGrid->Row=row;
}


// Возвращает id выбранного класса
int TUClassesListFrame::GetSelectedId(void)
{
 return StrToInt(StringGrid->Cells[0][StringGrid->Row]);
}

// Возвращает имя выбранного класса
String TUClassesListFrame::GetSelectedName(void)
{
 return StringGrid->Cells[1][StringGrid->Row];
}
void __fastcall TUClassesListFrame::FrameResize(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------

