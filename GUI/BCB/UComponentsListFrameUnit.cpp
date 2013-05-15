//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include "UComponentsListFrameUnit.h"
#include "rdk_initdll.h"
#include "myrdk.h"
#include "UCRPerseptronFormUnit.h"
#include "UCRSampleFormUnit.h"
#include "UCRTeacherPerseptronDLFormUnit.h"
#include "UCRTeacherPerseptronBPFormUnit.h"
#include "UComponentsListFormUnit.h"
#include "UListInputFormUnit.h"
#include "UDrawEngineFrameUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUComponentsListFrame *UComponentsListFrame;

// ���������� ������ ����
std::map<std::string, TUVisualControllerForm*> TUComponentsListFrame::ComponentControllers;

//---------------------------------------------------------------------------
__fastcall TUComponentsListFrame::TUComponentsListFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 ShowXMLComponentParameters=true;
 UpdateInterval=-1;
 DrawEngineFrame=0;
}
//---------------------------------------------------------------------------

// �������������� ���� �� �������� ����
void __fastcall TUComponentsListFrame::UpdatePath(void)
{
 UpdateInterfaceFlag=true;

 HeaderControl->Sections->Clear();

 CurrentPath.resize(0);

 RDK::separatestring(CurrentComponentName,CurrentPath,'.');

 for(int i=0;i<int(CurrentPath.size());i++)
 {
  THeaderSection *section=HeaderControl->Sections->Add();
  section->Text=CurrentPath[i].c_str();
  section->Width=HeaderControl->Canvas->TextWidth(section->Text)+20;
 }

 UpdateInterfaceFlag=false;
}

// ��������� ������
void TUComponentsListFrame::AUpdateInterface(void)
{
 if(ComponentControllers.empty())
 {
  ComponentControllers["UCRPerseptron"]=UCRPerseptronForm;
  ComponentControllers["UCRSample"]=UCRSampleForm;
  ComponentControllers["UCRTeacherPerseptronDL"]=UCRTeacherPerseptronDLForm;
  ComponentControllers["UCRTeacherPerseptronBP"]=UCRTeacherPerseptronBPForm;
 }


 int row=StringGrid->Row;

 if(SelectedId == 0)
  row=1;

 int numcomponents=Model_GetNumComponents(CurrentComponentName.c_str());
 StringGrid->ColCount=2;
 StringGrid->RowCount=numcomponents+2;
 StringGrid->Cells[0][1]="..";

 if(numcomponents<0)
 {
  UpdatePath();
  UpdateSelectedComponentInfo();
  UpdateParameters();
  UpdateInterfaceFlag=false;
  return;
 }

 std::vector<std::string> ids;
// ids.resize(numcomponents);
 std::string names=Model_GetComponentsNameList(CurrentComponentName.c_str());
 RDK::separatestring(names,ids,',');
 for(int i=0;i<numcomponents;i++)
 {
  StringGrid->Cells[1][i+2]=ids[i].c_str();
  std::string longid=CurrentComponentName;
  if(longid.size()>0)
   longid+=".";

  std::string sid=Model_GetComponentLongId((longid+ids[i]).c_str(),CurrentComponentName.c_str());//IntToStr(ids[i]);
  int id=RDK::atoi(sid);
  StringGrid->Cells[0][i+2]=IntToStr(id);
  if(id == SelectedId)
   row=i+2;
 }

 StringGrid->ColWidths[0]=30;
 StringGrid->ColWidths[1]=Width-StringGrid->ColWidths[0];

 StringGrid->Cells[0][0]="Id";
 StringGrid->Cells[1][0]="Component name";

 if(row<StringGrid->RowCount)
  StringGrid->Row=row;

 StringGrid->FixedRows=1;
 UpdateInterfaceFlag=false;
 UpdatePath();
 UpdateSelectedComponentInfo();

 if(PageControl1->ActivePage == TabSheet1)
  UpdateParameters();
 if(PageControl1->ActivePage == TabSheet2)
  UpdateState();
 if(PageControl1->ActivePage == TabSheet3)
 UpdateIO();
 if(PageControl1->ActivePage == TabSheet4)
  UpdateParametersList();
 if(PageControl1->ActivePage == TabSheet5)
  UpdateStatesList();
}


// ��������� ��������� ���������� � xml
void TUComponentsListFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// ��������� ��������� ���������� �� xml
void TUComponentsListFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 SelectedComponentName.clear();
 SelectedComponentId.clear();
 CurrentComponentName.clear();
 CurrentComponentId.clear();
 SelectedComponentParameterName.clear();
 SelectedComponentStateName.clear();
 CurrentPath.clear();

}

// ��������� ��������� ����������
void TUComponentsListFrame::UpdateParameters(void)
{
 if(PageControl1->ActivePage != TabSheet1)
  return;
 UpdateInterfaceFlag=true;

 if(ShowXMLComponentParameters)
 {
  Panel1->Visible=true;
  TempParams=Model_GetComponentParametersEx(GetSelectedComponentLongName().c_str());
  if(TempParams.Length() != LastParams.Length() || TempParams != LastParams)
   ParametersRichEdit->Text=LastParams=TempParams;
 }
 else
 {
  Panel1->Visible=false;
 }

 std::string name=Model_GetComponentClassName(SelectedComponentName.c_str());
 std::map<std::string, TUVisualControllerForm*>::iterator I=ComponentControllers.find(name);
 if(I != ComponentControllers.end() && I->second)
  GUI1->Enabled=true;
 else
  GUI1->Enabled=false;

 ParameterValueRichEdit->Text=ParametersListStringGrid->Cells[2][ParametersListStringGrid->Row];

 RegistryModified=false;
 UpdateInterfaceFlag=false;
}


// ��������� ��������� ����������
void TUComponentsListFrame::UpdateState(void)
{
 if(PageControl1->ActivePage != TabSheet2)
  return;
 UpdateInterfaceFlag=true;

 if(ShowXMLComponentParameters)
 {
  Panel1->Visible=true;
  TempStates=Model_GetComponentState(GetSelectedComponentLongName().c_str());
  if(TempStates.Length() != LastStates.Length() || TempStates != LastStates)
   StateRichEdit->Text=LastStates=TempStates;
 }
 else
 {
  Panel1->Visible=false;
 }

 StateValueRichEdit->Text=StatesListStringGrid->Cells[2][StatesListStringGrid->Row];
 RegistryModified=false;
 UpdateInterfaceFlag=false;
}

// ��������� ������ �����-������
void TUComponentsListFrame::UpdateIO(void)
{
 if(PageControl1->ActivePage != TabSheet3)
  return;
 UpdateInterfaceFlag=true;

 int num=Model_GetComponentNumOutputs(GetSelectedComponentLongName().c_str());
 OutputsStringGrid->RowCount=1+num;
 OutputsStringGrid->ColCount=1+3;

 OutputsStringGrid->Cells[0][0]="#";
 OutputsStringGrid->Cells[1][0]="Size";
 OutputsStringGrid->Cells[2][0]="Element Size";
 OutputsStringGrid->Cells[3][0]="Type";

 for(int i=0;i<num;i++)
 {
  OutputsStringGrid->Cells[0][i+1]=IntToStr(i);
  OutputsStringGrid->Cells[1][i+1]=Model_GetComponentOutputDataSize(GetSelectedComponentLongName().c_str(), i);
  OutputsStringGrid->Cells[2][i+1]=Model_GetComponentOutputDataSize(GetSelectedComponentLongName().c_str(), i);
  OutputsStringGrid->Cells[3][i+1]=Model_GetComponentOutputElementSize(GetSelectedComponentLongName().c_str(), i);
 }


 num=Model_GetComponentNumInputs(GetSelectedComponentLongName().c_str());
 InputsStringGrid->RowCount=1+num;
 InputsStringGrid->ColCount=1+3;

 InputsStringGrid->Cells[0][0]="#";
 InputsStringGrid->Cells[1][0]="Size";
 InputsStringGrid->Cells[2][0]="Element Size";
 InputsStringGrid->Cells[3][0]="Type";

 for(int i=0;i<num;i++)
 {
  InputsStringGrid->Cells[0][i+1]=IntToStr(i);
  InputsStringGrid->Cells[1][i+1]=Model_GetComponentInputDataSize(GetSelectedComponentLongName().c_str(), i);
  InputsStringGrid->Cells[2][i+1]=Model_GetComponentInputDataSize(GetSelectedComponentLongName().c_str(), i);
  InputsStringGrid->Cells[3][i+1]=Model_GetComponentInputElementSize(GetSelectedComponentLongName().c_str(), i);
 }

 UpdateInterfaceFlag=false;
}


// ��������� ������ ������ ����������
void TUComponentsListFrame::UpdateParametersList(void)
{
 if(PageControl1->ActivePage != TabSheet4)
  return;
 UpdateInterfaceFlag=true;

 std::string xml_data=Model_GetComponentParametersEx(GetSelectedComponentLongName().c_str());
 RDK::USerStorageXML xml;
 xml.Load(xml_data,"");
 xml.SelectNode("Parameters");
 int num=xml.GetNumNodes();


 ParametersListStringGrid->RowCount=1+num;
 ParametersListStringGrid->ColCount=1+3;

 ParametersListStringGrid->Cells[0][0]="#";
 ParametersListStringGrid->Cells[1][0]="Name";
 ParametersListStringGrid->Cells[2][0]="Value";
 ParametersListStringGrid->Cells[3][0]="Description";

 for(int i=1;i<ParametersListStringGrid->ColCount;i++)
  ParametersListStringGrid->ColWidths[i]=10;

 for(int i=0;i<num;i++)
 {
  ParametersListStringGrid->Cells[0][i+1]=IntToStr(i);
  xml.SelectNode(i);
  ParametersListStringGrid->Cells[1][i+1]=xml.GetNodeName().c_str();
  if(xml.GetNumNodes() == 0)
   ParametersListStringGrid->Cells[2][i+1]=xml.GetNodeText().c_str();
  else
  {
   std::string value;
   xml.SaveFromNode(value);
   ParametersListStringGrid->Cells[2][i+1]=value.c_str();
  }
  int global_width=ParametersListStringGrid->ColWidths[0];
  ParametersListStringGrid->Cells[3][i+1]=xml.GetNodeAttribute("Header").c_str();
  int width=ParametersListStringGrid->Canvas->TextWidth(ParametersListStringGrid->Cells[1][i+1])+10;
  if(width>ParametersListStringGrid->Width/2)
   width=ParametersListStringGrid->Width/2;
  if(ParametersListStringGrid->ColWidths[1]<width)
   ParametersListStringGrid->ColWidths[1]=width;
  global_width+=ParametersListStringGrid->ColWidths[1];

  width=ParametersListStringGrid->Canvas->TextWidth(ParametersListStringGrid->Cells[2][i+1])+10;
  if(width>ParametersListStringGrid->Width/2)
   width=ParametersListStringGrid->Width/2;
  if(ParametersListStringGrid->ColWidths[2]<width)
   ParametersListStringGrid->ColWidths[2]=width;
  global_width+=ParametersListStringGrid->ColWidths[2];

  width=ParametersListStringGrid->Canvas->TextWidth(ParametersListStringGrid->Cells[3][i+1])+10;
  if(width < ParametersListStringGrid->Width-global_width)
   width=ParametersListStringGrid->Width-global_width;

  if(ParametersListStringGrid->ColWidths[3]<width)
   ParametersListStringGrid->ColWidths[3]=width;
  xml.SelectUp();
 }

 SelectedComponentParameterName=AnsiString(ParametersListStringGrid->Cells[1][1]).c_str();
 UpdateInterfaceFlag=false;
}

// ��������� ������ ������ ���������� ���������
void TUComponentsListFrame::UpdateStatesList(void)
{
 if(PageControl1->ActivePage != TabSheet5)
  return;
 UpdateInterfaceFlag=true;

 std::string xml_data=Model_GetComponentState(GetSelectedComponentLongName().c_str());
 RDK::USerStorageXML xml;
 xml.Load(xml_data,"");
 xml.SelectNode("State");
 int num=xml.GetNumNodes();


 StatesListStringGrid->RowCount=1+num;
 StatesListStringGrid->ColCount=1+3;

 StatesListStringGrid->Cells[0][0]="#";
 StatesListStringGrid->Cells[1][0]="Name";
 StatesListStringGrid->Cells[2][0]="Value";
 StatesListStringGrid->Cells[3][0]="Description";

 for(int i=1;i<StatesListStringGrid->ColCount;i++)
  StatesListStringGrid->ColWidths[i]=10;

 for(int i=0;i<num;i++)
 {
  StatesListStringGrid->Cells[0][i+1]=IntToStr(i);
  xml.SelectNode(i);
  StatesListStringGrid->Cells[1][i+1]=xml.GetNodeName().c_str();
  if(xml.GetNumNodes() == 0)
   StatesListStringGrid->Cells[2][i+1]=xml.GetNodeText().c_str();
  else
  {
   std::string value;
   xml.SaveFromNode(value);
   StatesListStringGrid->Cells[2][i+1]=value.c_str();
  }
  int global_width=StatesListStringGrid->ColWidths[0];
  StatesListStringGrid->Cells[3][i+1]=xml.GetNodeAttribute("Header").c_str();
  int width=StatesListStringGrid->Canvas->TextWidth(StatesListStringGrid->Cells[1][i+1])+10;
  if(width>StatesListStringGrid->Width/2)
   width=StatesListStringGrid->Width/2;
  if(StatesListStringGrid->ColWidths[1]<width)
   StatesListStringGrid->ColWidths[1]=width;
  global_width+=StatesListStringGrid->ColWidths[1];

  width=StatesListStringGrid->Canvas->TextWidth(StatesListStringGrid->Cells[2][i+1])+10;
  if(width>StatesListStringGrid->Width/2)
   width=StatesListStringGrid->Width/2;
  if(StatesListStringGrid->ColWidths[2]<width)
   StatesListStringGrid->ColWidths[2]=width;
  global_width+=StatesListStringGrid->ColWidths[2];

  width=StatesListStringGrid->Canvas->TextWidth(StatesListStringGrid->Cells[3][i+1])+10;
  if(width < StatesListStringGrid->Width-global_width)
   width=StatesListStringGrid->Width-global_width;

  if(StatesListStringGrid->ColWidths[3]<width)
   StatesListStringGrid->ColWidths[3]=width;
  xml.SelectUp();
 }

 SelectedComponentStateName=AnsiString(StatesListStringGrid->Cells[1][1]).c_str();
 UpdateInterfaceFlag=false;
/*
 if(PageControl1->ActivePage != TabSheet5)
  return;
 UpdateInterfaceFlag=true;

 std::string xml_data=Model_GetComponentState(GetSelectedComponentLongName().c_str());
 RDK::USerStorageXML xml;
 xml.Load(xml_data,"");
 xml.SelectNode("Parameters");
 int num=xml.GetNumNodes();


 StatesListStringGrid->RowCount=1+num;
 StatesListStringGrid->ColCount=1+3;

 StatesListStringGrid->Cells[0][0]="#";
 StatesListStringGrid->Cells[1][0]="Name";
 StatesListStringGrid->Cells[2][0]="Value";
 StatesListStringGrid->Cells[3][0]="Description";

 for(int i=0;i<num;i++)
 {
  StatesListStringGrid->Cells[0][i+1]=IntToStr(i);
  xml.SelectNode(i);
  StatesListStringGrid->Cells[1][i+1]=xml.GetNodeName().c_str();
  if(xml.GetNumNodes() == 0)
   StatesListStringGrid->Cells[2][i+1]=xml.GetNodeText().c_str();
  else
  {
   std::string value;
   xml.SaveFromNode(value);
   StatesListStringGrid->Cells[2][i+1]=value.c_str();
  }
  StatesListStringGrid->Cells[3][i+1]=xml.GetNodeAttribute("Header").c_str();
  xml.SelectUp();
 }

 SelectedComponentStateName=AnsiString(StatesListStringGrid->Cells[1][1]).c_str();
 UpdateInterfaceFlag=false;
 */
}

// ��������� ������� ����� ��������� ���������
void TUComponentsListFrame::UpdateSelectedComponentInfo(void)
{
 if(StringGrid->Row<1)
 {
  SelectedComponentName="";
  SelectedComponentId="";
  return;
 }
 else
 if(StringGrid->Cells[0][StringGrid->Row] == "..")
 {
  SelectedComponentName=CurrentComponentName;
  SelectedComponentId=CurrentComponentId;
  return;
 }

 SelectedComponentName=AnsiString(StringGrid->Cells[1][StringGrid->Row]).c_str();
 if(CurrentComponentName.size()>0)
  SelectedComponentName=CurrentComponentName+std::string(".")+SelectedComponentName;

 SelectedComponentId=AnsiString(StringGrid->Cells[0][StringGrid->Row]).c_str();
 if(CurrentComponentId.size()>0)
  SelectedComponentId=CurrentComponentId+std::string(".")+SelectedComponentId;

 ClassNamePanel->Caption=Model_GetComponentClassName(SelectedComponentName.c_str());
}

// -----------------------
// ������ ������� � ���������� ������
// -----------------------
// ���������� id ���������� ����������
const std::string TUComponentsListFrame::GetSelectedComponentId(void) const
{
 return AnsiString(StringGrid->Cells[0][StringGrid->Row]).c_str();
}

// ���������� ��� ���������� ����������
const std::string TUComponentsListFrame::GetSelectedComponentName(void) const
{
 return AnsiString(StringGrid->Cells[1][StringGrid->Row]).c_str();
}

// �������� ��������� �� ��������� ��������� �����
void TUComponentsListFrame::SetSelectedComponentName(const std::string &name)
{
 for(int i=0;i<StringGrid->RowCount;i++)
 {
  if(StringGrid->Cells[1][i] == name.c_str())
  {
   StringGrid->Row=i;
   bool CanSelect;
   StringGridSelectCell(this, StringGrid->Col, i, CanSelect);
   break;
  }
 }
}

// ������� ��� ����������� ����������
const std::string& TUComponentsListFrame::GetSelectedComponentLongName(void) const
{
 return SelectedComponentName;
}

// ������� ��������� Id ����������� ����������
const std::string& TUComponentsListFrame::GetSelectedComponentLongId(void) const
{
 return SelectedComponentId;
}

// ��� ����������� ��������� ����������� ����������
const std::string& TUComponentsListFrame::GetSelectedComponentParameterName(void) const
{
 return SelectedComponentParameterName;
}

// ��� ���������� ���������� ��������� ����������� ����������
const std::string& TUComponentsListFrame::GetSelectedComponentStateName(void) const
{
 return SelectedComponentStateName;
}

// ������� ��� �������� ����������
const std::string& TUComponentsListFrame::GetCurrentComponentName(void) const
{
 return CurrentComponentName;
}

// ���������� �� ������� ���� �� ����� ���������� ����������
void TUComponentsListFrame::SelectComponentByName(const std::string& name)
{
 SetSelectedComponentName(name);
 StringGridDblClick(this);
}

// ����������� �� ������� �����
void TUComponentsListFrame::SelectUp(void)
{
 SetSelectedComponentName("");
 StringGridDblClick(this);
}

// ������� ��������� id �������� ����������
const std::string& TUComponentsListFrame::GetCurrentComponentId(void) const
{
 return CurrentComponentId;
}

// ��������� ����� �������
int TUComponentsListFrame::GetSelectedComponentOutput(void) const
{
 return SelectedComponentOutput;
}

// ��������� ���� �������
int TUComponentsListFrame::GetSelectedComponentInput(void) const
{
 return SelectedComponentInput;
}

// ���������-���������� ����������� ���������� � ���� xml
bool TUComponentsListFrame::GetShowXMLComponentParameters(void) const
{
 return ShowXMLComponentParameters;
}

// ���������� ���� ������� �������������� ������ �����
bool TUComponentsListFrame::GetTreeReadOnlyFlag(void)
{
 return TreeReadOnlyFlag;
}

// ���������� ���� ������� �������������� ����������
bool TUComponentsListFrame::GetParamReadOnlyFlag(void)
{
 return ParamReadOnlyFlag;
}

// ���������� ������� ������� ��������� � ������
bool TUComponentsListFrame::GetRegistryModified(void)
{
 return RegistryModified;
}
// -----------------------


// -----------------------
// ������ ���������� ����������� �������
// -----------------------
// ������� ������ ��������� �������
void __fastcall TUComponentsListFrame::ClearDiffRegistry(void)
{
// DiffRegistry.Clear();
}


bool __fastcall TUComponentsListFrame::SetCurrentSubTree(std::string &fullname)
{
// if(!Registry)
//  return false;

 return true;
}

// ���������-���������� ����������� ���������� � ���� xml
void TUComponentsListFrame::SetShowXMLComponentParameters(bool flag)
{
 ShowXMLComponentParameters=flag;
}

void __fastcall TUComponentsListFrame::SetTreeReadOnlyFlag(bool flag)
{
 TreeReadOnlyFlag=flag;
}

void __fastcall TUComponentsListFrame::SetParamReadOnlyFlag(bool flag)
{
 ParamReadOnlyFlag=flag;
}

void __fastcall TUComponentsListFrame::SetRegistryModified(bool flag)
{
 RegistryModified=flag;
}
// -----------------------

//---------------------------------------------------------------------------
void __fastcall TUComponentsListFrame::FrameResize(TObject *Sender)
{
// UpdateInterface();
}
//---------------------------------------------------------------------------



void __fastcall TUComponentsListFrame::StringGridDblClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 if(StringGrid->Row <= 0)
  return;

 int num_components=Model_GetNumComponents(GetSelectedComponentLongName().c_str());
// if(!num_components)
//  return;

 if(StringGrid->Row == 1 && StringGrid->Cells[0][StringGrid->Row] == "..")
 {
  std::string::size_type i=0,j=0;
  i=CurrentComponentName.find_last_of(".");
  j=CurrentComponentId.find_last_of(".");

  if(i != std::string::npos)
  {
   CurrentComponentName=CurrentComponentName.substr(0,i);
  }
  else
   CurrentComponentName="";
  if(j != std::string::npos)
  {
   SelectedId=StrToInt(CurrentComponentId.substr(j+1).c_str());
   CurrentComponentId=CurrentComponentId.substr(0,j);
  }
  else
  {
   if(CurrentComponentId != "")
	SelectedId=StrToInt(CurrentComponentId.c_str());
   else
    SelectedId=0;
   CurrentComponentId="";
  }

  UpdateInterface();
  if(DrawEngineFrame)
   DrawEngineFrame->SetNet(GetCurrentComponentName());
//   DrawEngineFrame->SelectComponent(GetCurrentComponentName());
  return;
 }

 if(CurrentComponentName.size()>0)
 {
  CurrentComponentName+=".";
  CurrentComponentId+=".";
 }
 SelectedId=StrToInt(StringGrid->Cells[0][StringGrid->Row]);
 CurrentComponentName+=AnsiString(StringGrid->Cells[1][StringGrid->Row]).c_str();
 CurrentComponentId+=AnsiString(StringGrid->Cells[0][StringGrid->Row]).c_str();
 UpdateInterface();

 if(DrawEngineFrame)
//  DrawEngineFrame->SelectComponent(GetCurrentComponentName());
  DrawEngineFrame->SetNet(GetCurrentComponentName());
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StringGridSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect)
{
/* if(UpdateInterfaceFlag)
  return;

 if(StringGrid->Row<=1 || StringGrid->Cells[0][StringGrid->Row] == "" || StringGrid->Cells[0][StringGrid->Row] == "..")
 {
  SelectedId=0;
 }
 else
 if(SelectedId == StrToInt(StringGrid->Cells[0][StringGrid->Row]))
  return;
 else
  SelectedId=StrToInt(StringGrid->Cells[0][StringGrid->Row]);
 UpdateSelectedComponentInfo();
 UpdateParameters();
 UpdateState();
 UpdateIO();
 UpdateParametersList();
 UpdateStatesList();*/
 StringGridClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StringGridKeyPress(TObject *Sender, System::WideChar &Key)
{
 if(Key == VK_RETURN)
 {
  StringGridDblClick(Sender);
 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StringGridClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 if(StringGrid->Row<=1 || StringGrid->Cells[0][StringGrid->Row] == "" || StringGrid->Cells[0][StringGrid->Row] == "..")
 {
  SelectedId=0;
 }
 else
 if(SelectedId == StrToInt(StringGrid->Cells[0][StringGrid->Row]))
  return;
 else
  SelectedId=StrToInt(StringGrid->Cells[0][StringGrid->Row]);
 UpdateSelectedComponentInfo();
 if(PageControl1->ActivePage == TabSheet1)
  UpdateParameters();
 if(PageControl1->ActivePage == TabSheet2)
  UpdateState();
 if(PageControl1->ActivePage == TabSheet3)
 UpdateIO();
 if(PageControl1->ActivePage == TabSheet4)
  UpdateParametersList();
 if(PageControl1->ActivePage == TabSheet5)
  UpdateStatesList();

 if(DrawEngineFrame)
  DrawEngineFrame->SelectComponent(GetSelectedComponentName());
}
//---------------------------------------------------------------------------


void __fastcall TUComponentsListFrame::ParametersRichEditChange(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;
 RegistryModified=true;
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StateRichEditChange(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;
 RegistryModified=true;
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StateHeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
 if(Section->Index == 0)
 {
  Model_SetComponentState(GetSelectedComponentLongName().c_str(),AnsiString(StateRichEdit->Text).c_str());
  UpdateInterface();
 }
 else
 if(Section->Index == 1)
 {
  UpdateState();
 }
 else
 if(Section->Index == 2)
 {

 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::OutputsStringGridClick(TObject *Sender)
{
 SelectedComponentOutput=OutputsStringGrid->Row-1;
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::OutputsStringGridDblClick(TObject *Sender)
{
 OutputsStringGridClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::OutputsStringGridSelectCell(TObject *Sender,
		  int ACol, int ARow, bool &CanSelect)
{
 OutputsStringGridClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::InputsStringGridSelectCell(TObject *Sender,
		  int ACol, int ARow, bool &CanSelect)
{
 InputsStringGridClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::InputsStringGridDblClick(TObject *Sender)
{
 InputsStringGridClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::InputsStringGridClick(TObject *Sender)
{
 SelectedComponentInput=InputsStringGrid->Row-1;
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::PageControl1Change(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::ParametersListStringGridClick(TObject *Sender)

{
 SelectedComponentParameterName=AnsiString(ParametersListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::ParametersListStringGridDblClick(TObject *Sender)

{
 SelectedComponentParameterName=AnsiString(ParametersListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::ParametersListStringGridSelectCell(TObject *Sender,
		  int ACol, int ARow, bool &CanSelect)
{
 SelectedComponentParameterName=AnsiString(ParametersListStringGrid->Cells[1][ARow]).c_str();
 ParameterValueRichEdit->Text=ParametersListStringGrid->Cells[2][ARow];
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StatesListStringGridClick(TObject *Sender)
{
 SelectedComponentStateName=AnsiString(StatesListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StatesListStringGridDblClick(TObject *Sender)
{
 SelectedComponentStateName=AnsiString(StatesListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StatesListStringGridSelectCell(TObject *Sender, int ACol,
		  int ARow, bool &CanSelect)
{
 SelectedComponentStateName=AnsiString(StatesListStringGrid->Cells[1][ARow]).c_str();
 StateValueRichEdit->Text=StatesListStringGrid->Cells[2][ARow];
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::GUI1Click(TObject *Sender)
{
 std::string name=Model_GetComponentClassName(SelectedComponentName.c_str());
 std::map<std::string, TUVisualControllerForm*>::iterator I=ComponentControllers.find(name);
 if(I != ComponentControllers.end() && I->second)
 {
  I->second->SetComponentControlName(SelectedComponentName);
  I->second->Show();
  I->second->UpdateInterface(true);
 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::ParametersHeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
 if(Section->Index == 0)
 {
  Model_SetComponentParameters(GetSelectedComponentLongName().c_str(),AnsiString(ParametersRichEdit->Text).c_str());
  UpdateInterface();
 }
 else
 if(Section->Index == 1)
 {
  UpdateParameters();
 }
 else
 if(Section->Index == 2)
 {

 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::HeaderControl3SectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
 if(ParametersListStringGrid->Row<0 || ParametersListStringGrid->Row>=ParametersListStringGrid->RowCount)
  return;
 if(Section->Index == 0)
 {
  Model_SetComponentParameterValue(GetSelectedComponentLongName().c_str(),AnsiString(ParametersListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str(), AnsiString(ParameterValueRichEdit->Text).c_str());
  UpdateInterface();
 }
 else
 if(Section->Index == 1)
 {
//  if(Application->MessageBox(L"Are you sure you want to change this value in all components of the same class?",L"Warning",MB_YESNO) != ID_YES)
//   return;
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   Model_SetGlobalComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(), Model_GetComponentClassName(GetSelectedComponentLongName().c_str()),AnsiString(ParametersListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str(), AnsiString(ParameterValueRichEdit->Text).c_str());
   UpdateInterface();
  }
 }
 else
 if(Section->Index == 2)
 {
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   std::string global_owner_stringid=Model_GetComponentClassName(CurrentComponentName.c_str());
   Model_SetGlobalOwnerComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(), Model_GetComponentClassName(GetSelectedComponentLongName().c_str()),global_owner_stringid.c_str(),AnsiString(ParametersListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str(), AnsiString(ParameterValueRichEdit->Text).c_str());
   UpdateInterface();
  }
 }
 else
 if(Section->Index == 3)
 {
  UpdateParameters();
 }
 else
 if(Section->Index == 4)
 {

 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::HeaderControl1SectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
 if(StatesListStringGrid->Row<0 || StatesListStringGrid->Row>=StatesListStringGrid->RowCount)
  return;
 if(Section->Index == 0)
 {
  Model_SetComponentStateValue(GetSelectedComponentLongName().c_str(),AnsiString(StatesListStringGrid->Cells[1][StatesListStringGrid->Row]).c_str(), AnsiString(StateValueRichEdit->Text).c_str());
  UpdateInterface();
 }
 else
 if(Section->Index == 1)
 {
//  if(Application->MessageBox(L"Are you sure you want to change this value in all components of the same class?",L"Warning",MB_YESNO) != ID_YES)
//   return;
  if(UComponentsListForm->Visible)
   UComponentsListForm->Hide();
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   Model_SetGlobalComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(),Model_GetComponentClassName(GetSelectedComponentLongName().c_str()),AnsiString(StatesListStringGrid->Cells[1][StatesListStringGrid->Row]).c_str(), AnsiString(StateValueRichEdit->Text).c_str());
   UpdateInterface();
  }
 }
 else
 if(Section->Index == 2)
 {
  if(UComponentsListForm->Visible)
   UComponentsListForm->Hide();
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   std::string global_owner_stringid=Model_GetComponentClassName(CurrentComponentName.c_str());
   Model_SetGlobalOwnerComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(),Model_GetComponentClassName(GetSelectedComponentLongName().c_str()),global_owner_stringid.c_str(),AnsiString(StatesListStringGrid->Cells[1][StatesListStringGrid->Row]).c_str(), AnsiString(StateValueRichEdit->Text).c_str());
   UpdateInterface();
  }
 }
 else
 if(Section->Index == 3)
 {
  UpdateState();
 }
 else
 if(Section->Index == 4)
 {

 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::Moveup1Click(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::Movedown1Click(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::Rename1Click(TObject *Sender)
{
 vector<string> listvals;
 UListInputForm->PresentSelect=false;
 UListInputForm->MustInput=true;
 UListInputForm->Init("Select new name",listvals,SelectedComponentName);
 if(UListInputForm->ShowModal() != mrOk)
  return;
 // std::string name=Model_GetComponentClassName(SelectedComponentName.c_str());
 std::string new_name=AnsiString(UListInputForm->Edit->Text).c_str();
 if(new_name == SelectedComponentName)
  return;

 Model_SetComponentPropertyData(SelectedComponentName.c_str(),"Name",&new_name);
 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------

