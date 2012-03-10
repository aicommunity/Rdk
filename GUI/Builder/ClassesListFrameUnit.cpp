//---------------------------------------------------------------------------

#include <vcl.h>
#include <vector>
#pragma hdrstop

#include "ClassesListFrameUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TClassesListFrame *ClassesListFrame;
//---------------------------------------------------------------------------
__fastcall TClassesListFrame::TClassesListFrame(TComponent* Owner)
	: TFrame(Owner)
{
 UpdateInterfaceFlag=false;
}
//---------------------------------------------------------------------------

// Отрисовка фрейма
void TClassesListFrame::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

 int numclasses=Storage_GetNumClasses();

 int row=StringGrid->Row;
 StringGrid->ColCount=2;
 StringGrid->RowCount=numclasses+1;

 std::vector<int> ids;
 ids.resize(numclasses);
 Storage_GetClassesList(&ids[0]);
 for(size_t i=0;i<numclasses;i++)
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

 UpdateInterfaceFlag=false;
}


// Возвращает id выбранного класса
int TClassesListFrame::GetSelectedId(void)
{
 return StrToInt(StringGrid->Cells[0][StringGrid->Row]);
}

// Возвращает имя выбранного класса
String TClassesListFrame::GetSelectedName(void)
{
 return StringGrid->Cells[1][StringGrid->Row];
}
void __fastcall TClassesListFrame::FrameResize(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------

