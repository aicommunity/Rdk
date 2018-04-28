//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentLinksFrameUnit.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
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
 UpdateInterval=-1;
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
  GroupBox3->Height=ClientHeight/2;
  GroupBox1->Align=alRight;
  GroupBox2->Align=alClient;
  GroupBox3->Visible=true;
  Splitter1->Visible=true;

  NANetFrameInputs->ShowModifier=2;
  NANetFrameInputs->Mode=1;
  NANetFrameOutputs->ShowModifier=2;
  NANetFrameOutputs->Mode=2;
  NANetFrameLinks->ShowModifier=2;
  NANetFrameLinks->Mode=4;

  GroupBox1->Caption=" Inputs ";
  GroupBox2->Caption=" Outputs ";
 break;

 case 1:
  ButtonPanel->Visible=true;
  Panel1->Visible=false;
  OkButton->Caption="Create";
  GroupBox1->Visible=true;
  GroupBox2->Visible=true;
  GroupBox3->Align=alBottom;
  GroupBox3->Height=ClientHeight/2;
  GroupBox1->Align=alRight;
  GroupBox2->Align=alClient;
  GroupBox3->Visible=true;
  Splitter1->Visible=true;

  NANetFrameInputs->ShowModifier=2;
  NANetFrameInputs->Mode=1;
  NANetFrameOutputs->ShowModifier=2;
  NANetFrameOutputs->Mode=2;
  NANetFrameLinks->ShowModifier=2;
  NANetFrameLinks->Mode=4;

  GroupBox1->Caption=" Inputs ";
  GroupBox2->Caption=" Outputs ";
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

  NANetFrameInputs->ShowModifier=2;
  NANetFrameInputs->Mode=1;
  NANetFrameOutputs->ShowModifier=2;
  NANetFrameOutputs->Mode=2;
  NANetFrameLinks->ShowModifier=2;
  NANetFrameLinks->Mode=4;

  GroupBox1->Caption=" Inputs ";
  GroupBox2->Caption=" Outputs ";
 break;

 case 3:
  ButtonPanel->Visible=true;
  Panel1->Visible=false;
  OkButton->Caption="Switch";
  GroupBox1->Visible=true;
  GroupBox2->Visible=true;
  GroupBox3->Align=alBottom;
  GroupBox3->Height=ClientHeight/2;
  GroupBox1->Align=alRight;
  GroupBox2->Align=alClient;
  GroupBox3->Visible=false;
  Splitter1->Visible=false;

  NANetFrameInputs->ShowModifier=2;
  NANetFrameInputs->Mode=2;
  NANetFrameOutputs->ShowModifier=2;
  NANetFrameOutputs->Mode=2;
  NANetFrameLinks->ShowModifier=2;
  NANetFrameLinks->Mode=4;

  GroupBox1->Caption=" Outputs 2 ";
  GroupBox2->Caption=" Outputs 1 ";
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


// Возврат интерфейса в исходное состояние
void TUComponentLinksFrame::AClearInterface(void)
{

}


void __fastcall TUComponentLinksFrame::CreateLink(void)
{
 if(NANetFrameOutputs->StringGrid->Row <= 0 || NANetFrameInputs->StringGrid->Row <= 0)
  return;

 std::string itemname=AnsiString(NANetFrameOutputs->StringGrid->Cells[2][NANetFrameOutputs->StringGrid->Row]).c_str();
 std::string connname=AnsiString(NANetFrameInputs->StringGrid->Cells[2][NANetFrameInputs->StringGrid->Row]).c_str();

 if(itemname.empty())
 {
  for(int i=NANetFrameOutputs->StringGrid->Row-1;i>=1;i--)
  {
   itemname=AnsiString(NANetFrameOutputs->StringGrid->Cells[2][i]).c_str();
   if(!itemname.empty())
	break;
  }
 }

 if(connname.empty())
 {
  for(int i=NANetFrameInputs->StringGrid->Row-1;i>=1;i--)
  {
   connname=AnsiString(NANetFrameInputs->StringGrid->Cells[2][i]).c_str();
   if(!connname.empty())
	break;
  }
 }

 if(!NANetFrameLinks->ViewComponentOwnerLongId.empty())
 {
  itemname.insert(0,NANetFrameLinks->ViewComponentOwnerLongId+std::string("."));
  connname.insert(0,NANetFrameLinks->ViewComponentOwnerLongId+std::string("."));
 }
 std::string conn_property_name=AnsiString(NANetFrameInputs->StringGrid->Cells[3][NANetFrameInputs->StringGrid->Row]).c_str();
 std::string item_property_name=AnsiString(NANetFrameOutputs->StringGrid->Cells[3][NANetFrameOutputs->StringGrid->Row]).c_str();

 if(item_property_name.find("DataOutput")!=std::string::npos && (conn_property_name.find("DataInput")!=std::string::npos || conn_property_name.find("-1")!=std::string::npos))
 {
  int item_index, conn_index;
  item_index = RDK::atoi(item_property_name.substr(item_property_name.find_last_of("DataOutput")+1));
  if(conn_property_name.find("DataInput")!=std::string::npos)
   conn_index = RDK::atoi(conn_property_name.substr(conn_property_name.find_last_of("DataInput")+1));
  else
   conn_index = RDK::atoi(conn_property_name);
  Model_CreateLink(itemname.c_str(), item_index, connname.c_str(), conn_index);
 }
 else
  Model_CreateLinkByName(itemname.c_str(), item_property_name.c_str(), connname.c_str(), conn_property_name.c_str());
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
 std::string connname=AnsiString(NANetFrameLinks->StringGrid->Cells[5][NANetFrameLinks->StringGrid->Row]).c_str();
 std::string conn_property_name=AnsiString(NANetFrameLinks->StringGrid->Cells[6][NANetFrameLinks->StringGrid->Row]).c_str();
 std::string item_property_name=AnsiString(NANetFrameLinks->StringGrid->Cells[4][NANetFrameLinks->StringGrid->Row]).c_str();
 if(!NANetFrameLinks->ViewComponentOwnerLongId.empty())
 {
  itemname.insert(0,NANetFrameLinks->ViewComponentOwnerLongId+std::string("."));
  connname.insert(0,NANetFrameLinks->ViewComponentOwnerLongId+std::string("."));
 }

// Model_BreakLink(itemname.c_str(), itemindex, connname.c_str(), connindex);
 Model_BreakLinkByName(itemname.c_str(), item_property_name.c_str(), connname.c_str(), conn_property_name.c_str());
 NANetFrameLinks->UpdateInterface();
}

void __fastcall TUComponentLinksFrame::BreakAll(void)
{
 Model_BreakAllComponentLinks(NANetFrameOutputs->ViewComponentLongId.c_str());
 NANetFrameLinks->UpdateInterface();
}

// Перемещает все связи с выхода первого компонента на выход второго
void __fastcall TUComponentLinksFrame::SwitchLink(void)
{
 if(NANetFrameOutputs->StringGrid->Row <= 0 || NANetFrameInputs->StringGrid->Row <= 0)
  return;

 std::string itemname1=AnsiString(NANetFrameOutputs->StringGrid->Cells[2][NANetFrameOutputs->StringGrid->Row]).c_str();
 std::string itemname2=AnsiString(NANetFrameInputs->StringGrid->Cells[2][NANetFrameInputs->StringGrid->Row]).c_str();

 if(itemname1.empty())
 {
  for(int i=NANetFrameOutputs->StringGrid->Row-1;i>=1;i--)
  {
   itemname1=AnsiString(NANetFrameOutputs->StringGrid->Cells[2][i]).c_str();
   if(!itemname1.empty())
	break;
  }
 }

 if(itemname2.empty())
 {
  for(int i=NANetFrameInputs->StringGrid->Row-1;i>=1;i--)
  {
   itemname2=AnsiString(NANetFrameInputs->StringGrid->Cells[2][i]).c_str();
   if(!itemname2.empty())
	break;
  }
 }

 if(!NANetFrameLinks->ViewComponentOwnerLongId.empty())
 {
  itemname1.insert(0,NANetFrameLinks->ViewComponentOwnerLongId+std::string("."));
  itemname2.insert(0,NANetFrameLinks->ViewComponentOwnerLongId+std::string("."));
 }

 std::string item_property_name1=AnsiString(NANetFrameOutputs->StringGrid->Cells[3][NANetFrameOutputs->StringGrid->Row]).c_str();
 std::string item_property_name2=AnsiString(NANetFrameInputs->StringGrid->Cells[3][NANetFrameInputs->StringGrid->Row]).c_str();

 if(item_property_name1.find("DataOutput")!=std::string::npos || item_property_name2.find("DataOutput")!=std::string::npos)
 {
  Application->MessageBox(L"Old I/O properties not supported", L"Error!", MB_OK);
  return;
 }
 else
 {
  Model_SwitchOutputLinks(itemname1.c_str(), item_property_name1.c_str(), itemname2.c_str(), item_property_name2.c_str());
 }
 NANetFrameLinks->UpdateInterface();
 NANetFrameInputs->UpdateInterface();
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

 case 3:
  SwitchLink();
 break;
 }
}
//---------------------------------------------------------------------------


