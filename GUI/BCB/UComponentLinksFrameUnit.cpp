//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentLinksFrameUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UComponentIOFrameUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUComponentLinksFrame *UComponentLinksFrame;

//---------------------------------------------------------------------------
__fastcall TUComponentLinksFrame::TUComponentLinksFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 Mode=0;

 GroupBox1->DoubleBuffered=true;
 GroupBox2->DoubleBuffered=true;
 GroupBox3->DoubleBuffered=true;
 NANetFrameInputs->ShowModifier=2;
 NANetFrameInputs->Mode=1;

 NANetFrameOutputs->ShowModifier=2;
 NANetFrameOutputs->Mode=2;

 NANetFrameLinks->ShowModifier=2;
 NANetFrameLinks->Mode=4;

 Width=Width+1;
 Width=Width-1;
}

void __fastcall TUComponentLinksFrame::Init(int mode, const std::string &component_name1, const std::string &owner_component_name, const std::string &component_name2)
{
 SetMode(mode);

 NANetFrameInputs->ViewComponentOwnerLongId=owner_component_name;
 NANetFrameInputs->ViewComponentLongId=component_name2;
 NANetFrameOutputs->ViewComponentOwnerLongId=owner_component_name;
 NANetFrameOutputs->ViewComponentLongId=component_name1;
 NANetFrameLinks->ViewComponentOwnerLongId=owner_component_name;
 NANetFrameLinks->ViewComponentLongId=component_name1;
}

// Доступ к id наблюдаемого компонента
/*const std::string& TUComponentLinksFrame::GetViewComponentLongId(void) const
{
 return ViewComponentLongId;
}

void TUComponentLinksFrame::SetViewComponentLongId(const std::string& value)
{
 if(ViewComponentLongId == value)
  return;

 ViewComponentLongId=value;
 UpdateInterface();
}  */

// Выбор режима отображения
void TUComponentLinksFrame::SetMode(int mode)
{
 if(Mode == mode)
  return;

 Mode=mode;
 switch(Mode)
 {
 case 0:
  ButtonPanel->Visible=false;
  Panel1->Visible=true;
  GroupBox1->Visible=true;
  GroupBox2->Visible=true;
  GroupBox3->Align=alBottom;
  GroupBox3->Visible=true;
  Splitter1->Visible=true;
 break;

 case 1:
  ButtonPanel->Visible=true;
  Panel1->Visible=false;
  OkButton->Caption="Create";
  GroupBox1->Visible=true;
  GroupBox2->Visible=true;
  GroupBox3->Align=alBottom;
  GroupBox3->Visible=true;
  Splitter1->Visible=true;
 break;

 case 2:
  ButtonPanel->Visible=true;
  Panel1->Visible=false;
  OkButton->Caption="Break";
  GroupBox1->Visible=false;
  GroupBox2->Visible=false;
  GroupBox3->Align=alClient;
  GroupBox3->Visible=true;
  Splitter1->Visible=false;
 break;
 }
}

// Обновление интерфейса
void TUComponentLinksFrame::AUpdateInterface(void)
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
 if(!NANetFrameLinks->ViewComponentOwnerLongId.empty())
 {
  itemname=NANetFrameLinks->ViewComponentOwnerLongId+std::string(".")+itemname;
  connname=NANetFrameLinks->ViewComponentOwnerLongId+std::string(".")+connname;
 }

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
 Model_ChainLinking(NANetFrameLinks->ViewComponentLongId.c_str());

 UpdateInterface();
}

// Связывает все объекты параллельно с выходами модели
// Предварительно разрывает все существующие связи
void __fastcall TUComponentLinksFrame::ParallelLinking(void)
{
 Model_ParallelLinking(NANetFrameLinks->ViewComponentLongId.c_str());

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

void __fastcall TUComponentLinksFrame::OkButtonClick(TObject *Sender)
{
 switch(Mode)
 {
 case 1:
  CreateLink();
 break;

 case 2:
  BreakLink();
 break;
 }
}
//---------------------------------------------------------------------------

