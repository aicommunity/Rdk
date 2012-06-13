//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentLinksFrameUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UComponentIOFrameUnit"
#pragma resource "*.dfm"
TUComponentLinksFrame *UComponentLinksFrame;

//---------------------------------------------------------------------------
__fastcall TUComponentLinksFrame::TUComponentLinksFrame(TComponent* Owner)
	: TFrame(Owner)
{
// Network=0;
 Init();
}

void __fastcall TUComponentLinksFrame::Init(void)
{
/* if(!net)
  return;

 Network=net;
  */
 GroupBox1->DoubleBuffered=true;
 GroupBox2->DoubleBuffered=true;
 GroupBox3->DoubleBuffered=true;
// NANetFrameInputs->Net=Network;
 NANetFrameInputs->ShowModifier=1;
 NANetFrameInputs->Mode=1;

// NANetFrameOutputs->Net=Network;
 NANetFrameOutputs->ShowModifier=1;
 NANetFrameOutputs->Mode=2;

// NANetFrameLinks->Net=Network;
 NANetFrameLinks->ShowModifier=1;
 NANetFrameLinks->Mode=4;

// NANetFrameInputs->UpdateInterface();
// NANetFrameOutputs->UpdateInterface();
// NANetFrameLinks->UpdateInterface();
 Width=Width+1;
 Width=Width-1;
}

// Доступ к id наблюдаемого компонента
const std::string& TUComponentLinksFrame::GetViewComponentLongId(void) const
{
 return ViewComponentLongId;
}

void TUComponentLinksFrame::SetViewComponentLongId(const std::string& value)
{
 ViewComponentLongId=value;
 UpdateInterface();
}

// Обновление интерфейса
void TUComponentLinksFrame::UpdateInterface(void)
{
 NANetFrameInputs->UpdateInterface();
 NANetFrameOutputs->UpdateInterface();
 NANetFrameLinks->UpdateInterface();
}

void __fastcall TUComponentLinksFrame::CreateLink(void)
{
 if(NANetFrameOutputs->StringGrid->Row <= 0 || NANetFrameInputs->StringGrid->Row <= 0)
  return;

 int itemindex=StrToInt(NANetFrameOutputs->StringGrid->Cells[1][NANetFrameOutputs->StringGrid->Row]);
 std::string itemname=AnsiString(NANetFrameOutputs->StringGrid->Cells[2][NANetFrameOutputs->StringGrid->Row-itemindex]).c_str();
 int connindex=StrToInt(NANetFrameInputs->StringGrid->Cells[1][NANetFrameInputs->StringGrid->Row]);
 std::string connname=AnsiString(NANetFrameInputs->StringGrid->Cells[2][NANetFrameInputs->StringGrid->Row-(connindex+1)]).c_str();

 Model_CreateLink(itemname.c_str(), itemindex, connname.c_str(), connindex);
 //Network->CreateLink(itemname,itemindex,connname,connindex);
 NANetFrameLinks->UpdateInterface();
 NANetFrameInputs->UpdateInterface();
}

void __fastcall TUComponentLinksFrame::BreakLink(void)
{
 if(NANetFrameLinks->StringGrid->Row <= 0)
  return;
 int itemindex=StrToInt(NANetFrameLinks->StringGrid->Cells[1][NANetFrameLinks->StringGrid->Row]);
 std::string itemname=AnsiString(NANetFrameLinks->StringGrid->Cells[3][NANetFrameLinks->StringGrid->Row]).c_str();
 int connindex=StrToInt(NANetFrameLinks->StringGrid->Cells[2][NANetFrameLinks->StringGrid->Row]);
 std::string connname=AnsiString(NANetFrameLinks->StringGrid->Cells[4][NANetFrameLinks->StringGrid->Row]).c_str();

 Model_BreakLink(itemname.c_str(), itemindex, connname.c_str(), connindex);
 NANetFrameLinks->UpdateInterface();
}

void __fastcall TUComponentLinksFrame::BreakAll(void)
{
 Model_BreakAllComponentLinks(NANetFrameOutputs->ViewComponentLongId.c_str());
 NANetFrameLinks->UpdateInterface();
}

// Связывает все объекты по цепочке в порядке возрастания id
// Предварительно разрывает все существующие связи
void __fastcall TUComponentLinksFrame::ChainLinking(void)
{
 Model_ChainLinking(ViewComponentLongId.c_str());

 UpdateInterface();
}

// Связывает все объекты параллельно с выходами модели
// Предварительно разрывает все существующие связи
void __fastcall TUComponentLinksFrame::ParallelLinking(void)
{
 Model_ParallelLinking(ViewComponentLongId.c_str());

 UpdateInterface();
}


//---------------------------------------------------------------------------
void __fastcall TUComponentLinksFrame::FrameResize(TObject *Sender)
{
// GroupBox1->Width=ClientWidth;
 GroupBox1->Width=ClientWidth/2;
 GroupBox2->Width=ClientWidth/2;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TUComponentLinksFrame::HeaderControl1SectionClick(THeaderControl *HeaderControl,
		  THeaderSection *Section)
{
 switch(Section->Index)
 {
 case 0:
  CreateLink();
 break;

 case 1:
  BreakLink();
 break;

 case 2:
  BreakAll();
 break;

 case 3:
  ChainLinking();
 break;

 case 4:
  ParallelLinking();
 break;
 }

}
//---------------------------------------------------------------------------

