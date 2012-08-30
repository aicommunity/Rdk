//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include "UComponentsListFrameUnit.h"
#include "rdk_initdll.h"
#include "myrdk.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUComponentsListFrame *UComponentsListFrame;
//---------------------------------------------------------------------------
__fastcall TUComponentsListFrame::TUComponentsListFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 ShowXMLComponentParameters=true;
}
//---------------------------------------------------------------------------

// Перерисовывает путь до текущего узла
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

// Отрисовка фрейма
void TUComponentsListFrame::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

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

 std::vector<int> ids;
 ids.resize(numcomponents);
 Model_GetComponentsList(CurrentComponentId.c_str(), &ids[0]);
 for(int i=0;i<numcomponents;i++)
 {
  if(ids[i] == SelectedId)
   row=i+2;
  StringGrid->Cells[0][i+2]=IntToStr(ids[i]);
  std::string longid=CurrentComponentId;
  if(longid.size()>0)
   longid+=".";
  StringGrid->Cells[1][i+2]=Model_GetComponentName((longid+AnsiString(IntToStr(ids[i])).c_str()).c_str());
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

 UpdateParameters();
 UpdateState();
 UpdateIO();
 UpdateParametersList();
 UpdateStatesList();

}

// Обновляет параметры компонента
void TUComponentsListFrame::UpdateParameters(void)
{
 UpdateInterfaceFlag=true;

 if(ShowXMLComponentParameters)
 {
  Panel1->Visible=true;
  TempParams=Model_GetComponentParametersEx(GetSelectedComponentLongId().c_str());
  if(TempParams.Length() != LastParams.Length() || TempParams != LastParams)
   ParametersRichEdit->Text=LastParams=TempParams;
 }
 else
 {
  Panel1->Visible=false;
 }

 RegistryModified=false;
 UpdateInterfaceFlag=false;
}


// Обновляет состояние компонента
void TUComponentsListFrame::UpdateState(void)
{
 UpdateInterfaceFlag=true;

 if(ShowXMLComponentParameters)
 {
  Panel1->Visible=true;
  TempStates=Model_GetComponentState(GetSelectedComponentLongId().c_str());
  if(TempStates.Length() != LastStates.Length() || TempStates != LastStates)
   StateRichEdit->Text=LastStates=TempStates;
 }
 else
 {
  Panel1->Visible=false;
 }

 RegistryModified=false;
 UpdateInterfaceFlag=false;
}

// Обновляет данные ввода-вывода
void TUComponentsListFrame::UpdateIO(void)
{
 UpdateInterfaceFlag=true;

 int num=Model_GetComponentNumOutputs(GetSelectedComponentLongId().c_str());
 OutputsStringGrid->RowCount=1+num;
 OutputsStringGrid->ColCount=1+3;

 OutputsStringGrid->Cells[0][0]="#";
 OutputsStringGrid->Cells[1][0]="Size";
 OutputsStringGrid->Cells[2][0]="Element Size";
 OutputsStringGrid->Cells[3][0]="Type";

 for(int i=0;i<num;i++)
 {
  OutputsStringGrid->Cells[0][i+1]=IntToStr(i);
  OutputsStringGrid->Cells[1][i+1]=Model_GetComponentOutputDataSize(GetSelectedComponentLongId().c_str(), i);
  OutputsStringGrid->Cells[2][i+1]=Model_GetComponentOutputDataSize(GetSelectedComponentLongId().c_str(), i);
  OutputsStringGrid->Cells[3][i+1]=Model_GetComponentOutputElementSize(GetSelectedComponentLongId().c_str(), i);
 }


 num=Model_GetComponentNumInputs(GetSelectedComponentLongId().c_str());
 InputsStringGrid->RowCount=1+num;
 InputsStringGrid->ColCount=1+3;

 InputsStringGrid->Cells[0][0]="#";
 InputsStringGrid->Cells[1][0]="Size";
 InputsStringGrid->Cells[2][0]="Element Size";
 InputsStringGrid->Cells[3][0]="Type";

 for(int i=0;i<num;i++)
 {
  InputsStringGrid->Cells[0][i+1]=IntToStr(i);
  InputsStringGrid->Cells[1][i+1]=Model_GetComponentInputDataSize(GetSelectedComponentLongId().c_str(), i);
  InputsStringGrid->Cells[2][i+1]=Model_GetComponentInputDataSize(GetSelectedComponentLongId().c_str(), i);
  InputsStringGrid->Cells[3][i+1]=Model_GetComponentInputElementSize(GetSelectedComponentLongId().c_str(), i);
 }

 UpdateInterfaceFlag=false;
}


// Обновляет данные списка параметров
void TUComponentsListFrame::UpdateParametersList(void)
{
 UpdateInterfaceFlag=true;

 std::string xml_data=Model_GetComponentParametersEx(GetSelectedComponentLongId().c_str());
 RDK::Serialize::USerStorageXML xml;
 xml.Load(xml_data,"");
 xml.SelectNode("Parameters");
 int num=xml.GetNumNodes();


 ParametersListStringGrid->RowCount=1+num;
 ParametersListStringGrid->ColCount=1+3;

 ParametersListStringGrid->Cells[0][0]="#";
 ParametersListStringGrid->Cells[1][0]="Name";
 ParametersListStringGrid->Cells[2][0]="Value";
 ParametersListStringGrid->Cells[3][0]="Description";

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
  ParametersListStringGrid->Cells[3][i+1]=xml.GetNodeAttribute("Header").c_str();
  xml.SelectUp();
 }

 SelectedComponentParameterName=AnsiString(ParametersListStringGrid->Cells[1][1]).c_str();
 UpdateInterfaceFlag=false;
}

// Обновляет данные списка переменных состояния
void TUComponentsListFrame::UpdateStatesList(void)
{
 UpdateInterfaceFlag=true;

 std::string xml_data=Model_GetComponentParametersEx(GetSelectedComponentLongId().c_str());
 RDK::Serialize::USerStorageXML xml;
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
}

// Обновляет длинные имена выбранных компонент
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
}

// -----------------------
// Методы доступа к физическим данным
// -----------------------
// Возвращает id выбранного компонента
const std::string TUComponentsListFrame::GetSelectedComponentId(void) const
{
 return AnsiString(StringGrid->Cells[0][StringGrid->Row]).c_str();
}

// Возвращает имя выбранного компонента
const std::string TUComponentsListFrame::GetSelectedComponentName(void) const
{
 return AnsiString(StringGrid->Cells[1][StringGrid->Row]).c_str();
}

// Длинное имя выделенного компонента
const std::string& TUComponentsListFrame::GetSelectedComponentLongName(void) const
{
 return SelectedComponentName;
}

// Длинный строковой Id выделенного компонента
const std::string& TUComponentsListFrame::GetSelectedComponentLongId(void) const
{
 return SelectedComponentId;
}

// Имя выделенного параметра выделенного компонента
const std::string& TUComponentsListFrame::GetSelectedComponentParameterName(void) const
{
 return SelectedComponentParameterName;
}

// Имя выделенной переменной состояния выделенного компонента
const std::string& TUComponentsListFrame::GetSelectedComponentStateName(void) const
{
 return SelectedComponentStateName;
}

// Длинное имя текущего компонента
const std::string& TUComponentsListFrame::GetCurrentComponentName(void) const
{
 return CurrentComponentName;
}

// Длинный строковой id текущего компонента
const std::string& TUComponentsListFrame::GetCurrentComponentId(void) const
{
 return CurrentComponentId;
}

// Выбранный выход объекта
int TUComponentsListFrame::GetSelectedComponentOutput(void) const
{
 return SelectedComponentOutput;
}

// Выбранный вход объекта
int TUComponentsListFrame::GetSelectedComponentInput(void) const
{
 return SelectedComponentInput;
}

// Включение-выключение отображения параметров в виде xml
bool TUComponentsListFrame::GetShowXMLComponentParameters(void) const
{
 return ShowXMLComponentParameters;
}

// Возвращает флаг запрета редактирования дерева узлов
bool TUComponentsListFrame::GetTreeReadOnlyFlag(void)
{
 return TreeReadOnlyFlag;
}

// Возвращает флаг запрета редактирования параметров
bool TUComponentsListFrame::GetParamReadOnlyFlag(void)
{
 return ParamReadOnlyFlag;
}

// Возвращает признак наличия изменений в данных
bool TUComponentsListFrame::GetRegistryModified(void)
{
 return RegistryModified;
}
// -----------------------


// -----------------------
// Методы управления физическими данными
// -----------------------
// Очищает список изменений реестра
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

// Включение-выключение отображения параметров в виде xml
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
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StringGridSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect)
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
 UpdateParameters();
 UpdateState();
 UpdateIO();
 UpdateParametersList();
 UpdateStatesList();
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
 UpdateParameters();
 UpdateState();
 UpdateIO();
 UpdateParametersList();
 UpdateStatesList();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::ParametersHeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
 if(Section->Index == 0)
 {
  Model_SetComponentParameters(GetSelectedComponentLongId().c_str(),AnsiString(ParametersRichEdit->Text).c_str());
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
  Model_SetComponentState(GetSelectedComponentLongId().c_str(),AnsiString(StateRichEdit->Text).c_str());
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
}
//---------------------------------------------------------------------------

