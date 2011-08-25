//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UANetFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TNANetFrame *NANetFrame;
//---------------------------------------------------------------------------
__fastcall TNANetFrame::TNANetFrame(TComponent* Owner)
	: TFrame(Owner)
{
 // Указатель на рассматриваемую сеть
 Net=0;

 // Режим работы:
 // 1 - показывать входы
 // 2 - показывать выходы
 // 3 - показывать входы и выходы
 // 4 - показывать существующие связи
 Mode=1;

 // Модификатор режима показа
 // 1 - показывать только входы (выходы, связи) своего уровня
 // 2 - показывать входы (выходы, связи) своего уровня, и всех вложенных сетей
 ShowModifier=1;


 // Флаг обновления интерфейса
 UpdateInterfaceFlag=false;
}
//---------------------------------------------------------------------------

// -----------------

// -----------------
// Методы управления
// -----------------
// Очищает таблицу
void __fastcall TNANetFrame::Clear(void)
{
 StringGrid->RowCount=1;
 StringGrid->ColCount=1;
}

// Заполняет таблицу входами сети
void __fastcall TNANetFrame::ShowInputs(void)
{
 if(!Net)
  return;

 int current_row=StringGrid->Row;

 StringGrid->ColCount=3;
 StringGrid->Cells[1][0]="#";
 StringGrid->Cells[2][0]="Вход";

 vector<LongIdT> buffer;
 string name;

 Net->GetConnectorsList(buffer,-1,Net);

 for(size_t i=0;i<buffer.size();i++)
 {
  NAConnector *cont=static_cast<NAConnector*>(Net->GetComponentL(buffer[i]));
  for(int j=-1;j<int(cont->NumInputs);j++)
  {
   StringGrid->RowCount=StringGrid->RowCount+1;
   StringGrid->Cells[0][StringGrid->RowCount-1]=IntToStr(int(i));
   name.clear();
   StringGrid->Cells[1][StringGrid->RowCount-1]=IntToStr(int(j));
   StringGrid->Cells[2][StringGrid->RowCount-1]=cont->GetLongName(Net,name).c_str();
  }
 }

 if(StringGrid->RowCount>1)
 {
  StringGrid->FixedRows=1;
  StringGrid->FixedCols=1;
 }

 if(StringGrid->RowCount>current_row && current_row>0)
  StringGrid->Row=current_row;
}

// Заполняет таблицу выходами сети
void __fastcall TNANetFrame::ShowOutputs(void)
{
 if(!Net)
  return;

 int current_row=StringGrid->Row;
 StringGrid->ColCount=3;
 StringGrid->Cells[1][0]="#";
 StringGrid->Cells[2][0]="Выход";

 vector<LongIdT> buffer;
 string name;

 Net->GetItemsList(buffer,-1,Net);

 for(size_t i=0;i<buffer.size();i++)
 {
  NAItem *cont=static_cast<NAItem*>(Net->GetComponentL(buffer[i]));
  for(size_t j=0;j<cont->NumOutputs;j++)
  {
   StringGrid->RowCount=StringGrid->RowCount+1;
   StringGrid->Cells[0][StringGrid->RowCount-1]=IntToStr(int(i))+":"+IntToStr(int(j));
   name.clear();
   StringGrid->Cells[1][StringGrid->RowCount-1]=IntToStr(int(j));
   StringGrid->Cells[2][StringGrid->RowCount-1]=cont->GetLongName(Net,name).c_str();
  }
 }

 if(StringGrid->RowCount>1)
 {
  StringGrid->FixedRows=1;
  StringGrid->FixedCols=1;
 }

 if(StringGrid->RowCount>current_row && current_row>0)
  StringGrid->Row=current_row;
}

// Заполняет таблицу установленными связями
void __fastcall TNANetFrame::ShowLinks(void)
{
 int current_row=StringGrid->Row;
 StringGrid->ColCount=5;
 StringGrid->Cells[1][0]="Out #";
 StringGrid->Cells[2][0]="In #";
 StringGrid->Cells[3][0]="Выход";
 StringGrid->Cells[4][0]="Вход";

 LinksListT linkslist;
 Net->GetLinks(linkslist, Net);
 StringGrid->RowCount=linkslist.size()+1;

 LinksListCIterator K,I,J,endI;
 pair<LinksListCIterator,LinksListCIterator> b;

 NAItem *item;
 NAConnector *connector;
// NameT s1,s2;

 I=linkslist.begin();
 endI=linkslist.end();
 string itemname,connname;
 int i=0;
 while(I != endI)
  {
   b=linkslist.equal_range(I->first);
   item=dynamic_cast<NAItem*>(Net->GetComponentL(I->first.Id));
   if(!item)
	{
	 continue;
	}

   string itemname;
   item->GetLongName(Net,itemname);
   for(K=b.first; K != b.second; ++K)
	{
	 connector=dynamic_cast<NAConnector*>(Net->GetComponentL(K->second.Id));
	 if(!connector)
	  {
	   continue;
	  }

	 string connname;
	 connector->GetLongName(Net,connname);
	 StringGrid->Cells[0][i+1]=IntToStr(int(i));
	 StringGrid->Cells[1][i+1]=StrToInt(I->first.Index);
	 StringGrid->Cells[2][i+1]=StrToInt(K->second.Index);
	 StringGrid->Cells[3][i+1]=itemname.c_str();
	 StringGrid->Cells[4][i+1]=connname.c_str();
	 ++i;
	}
   I=b.second;
  }

 if(StringGrid->RowCount>1)
 {
  StringGrid->FixedRows=1;
  StringGrid->FixedCols=1;
 }

 if(StringGrid->RowCount>current_row && current_row>0)
  StringGrid->Row=current_row;
}

// Отрисовывает текущее состояние фрейма
void __fastcall TNANetFrame::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

 Clear();
 switch(Mode)
 {
 case 1:
  ShowInputs();
 break;

 case 2:
  ShowOutputs();
 break;

 case 4:
  ShowLinks();
 break;
 }

 UpdateInterfaceFlag=false;
}
// -----------------

void __fastcall TNANetFrame::FrameResize(TObject *Sender)
{
 switch(Mode)
 {
 case 1:
  StringGrid->ColWidths[0]=40;
  StringGrid->ColWidths[1]=40;
  StringGrid->ColWidths[2]=StringGrid->Width-StringGrid->ColWidths[0]-StringGrid->ColWidths[1];
 break;

 case 2:
  StringGrid->ColWidths[0]=40;
  StringGrid->ColWidths[1]=40;
  StringGrid->ColWidths[2]=StringGrid->Width-StringGrid->ColWidths[0]-StringGrid->ColWidths[1];
 break;

 case 4:
  StringGrid->ColWidths[0]=40;
  StringGrid->ColWidths[1]=40;
  StringGrid->ColWidths[2]=40;
  StringGrid->ColWidths[3]=(StringGrid->Width-120)/2;
  StringGrid->ColWidths[4]=StringGrid->Width-(StringGrid->ColWidths[0]+
						StringGrid->ColWidths[1]+StringGrid->ColWidths[2]);
 break;
 }
}
//---------------------------------------------------------------------------

