//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include "UComponentsListFrameUnit.h"
#include "rdk_initdll.h"

// Разделяет строку на составлящие через сепаратор 'sep'
// Возвращает число полученных строк
template<typename CharT>
int separatestring(const std::basic_string<CharT> &str, std::vector<std::basic_string<CharT> > &output, CharT sep, int num=0, int *lastpos=0)
{
 typename std::basic_string<CharT>::size_type i=0,j=0;
 int size=0;
 int nnum=(num>0)?num-1:0;

 if(lastpos) *lastpos=0;
 output.resize(0);
 if(!str.size())
  return 0;

 while(i != std::string::npos && (nnum>=0) )
 {
  i=str.find_first_of(sep,j);
  if(i == j)
  {
   j++;
   continue;
  }

  ++size;
  output.resize(size);
  if(num)
   nnum--;
  if(i == std::string::npos)
   output[size-1]=str.substr(j);
  else
   output[size-1]=str.substr(j,i-j);
  j=i+1;
  if(j >= str.size())
   break;
 }

 if(lastpos) *lastpos=i;

 return size;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUComponentsListFrame *UComponentsListFrame;
//---------------------------------------------------------------------------
__fastcall TUComponentsListFrame::TUComponentsListFrame(TComponent* Owner)
	: TFrame(Owner)
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

 separatestring(CurrentComponentName,CurrentPath,'.');

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
 for(size_t i=0;i<numcomponents;i++)
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
}

// Обновляет параметры компонента
void TUComponentsListFrame::UpdateParameters(void)
{
 UpdateInterfaceFlag=true;

 if(ShowXMLComponentParameters)
 {
  Panel1->Visible=true;
  ParametersRichEdit->Text=Model_GetComponentParametersEx(GetSelectedComponentLongId().c_str());
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
  StateRichEdit->Text=Model_GetComponentState(GetSelectedComponentLongId().c_str());
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
  OutputsStringGrid->Cells[1][i+1]=Model_GetComponentOutputDataSize(GetSelectedComponentLongId().c_str(), i);
  OutputsStringGrid->Cells[2][i+1]=Model_GetComponentOutputElementSize(GetSelectedComponentLongId().c_str(), i);
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
  InputsStringGrid->Cells[1][i+1]=Model_GetComponentInputDataSize(GetSelectedComponentLongId().c_str(), i);
  InputsStringGrid->Cells[2][i+1]=Model_GetComponentInputElementSize(GetSelectedComponentLongId().c_str(), i);
 }

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

