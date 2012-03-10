//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include "ComponentsListFrameUnit.h"
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
TComponentsListFrame *ComponentsListFrame;
//---------------------------------------------------------------------------
__fastcall TComponentsListFrame::TComponentsListFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

// Перерисовывает путь до текущего узла
void __fastcall TComponentsListFrame::UpdatePath(void)
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
		   /*
 RDK::UClassRegistry* root=CurrentSubTree;

 CurrentPath.resize(0);
 while(root)
 {
  CurrentPath.push_back(root->GetName().substr(0,root->GetName().find_first_of(':')));
  root=root->GetRoot();
 }

 for(int i=int(CurrentPath.size())-2;i>=0;i--)
 {
  THeaderSection *section=HeaderControl->Sections->Add();
  section->Text=CurrentPath[i].c_str();
  section->Width=HeaderControl->Canvas->TextWidth(section->Text)+20;
 }
                        */
 UpdateInterfaceFlag=false;
}

// Отрисовка фрейма
void TComponentsListFrame::UpdateInterface(void)
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
}

// Обновляет длинные имена выбранных компонент
void TComponentsListFrame::UpdateSelectedComponentInfo(void)
{
 if(StringGrid->Row<2 || StringGrid->Cells[0][StringGrid->Row] == "..")
  return;

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
const std::string& TComponentsListFrame::GetSelectedComponentId(void) const
{
 return AnsiString(StringGrid->Cells[0][StringGrid->Row]).c_str();
}

// Возвращает имя выбранного компонента
const std::string& TComponentsListFrame::GetSelectedComponentName(void) const
{
 return AnsiString(StringGrid->Cells[1][StringGrid->Row]).c_str();
}

// Длинное имя выделенного компонента
const std::string& TComponentsListFrame::GetSelectedComponentLongName(void) const
{
 return SelectedComponentName;
}

// Длинный строковой Id выделенного компонента
const std::string& TComponentsListFrame::GetSelectedComponentLongId(void) const
{
 return SelectedComponentId;
}

// Длинное имя текущего компонента
const std::string& TComponentsListFrame::GetCurrentComponentName(void) const
{
 return CurrentComponentName;
}

// Длинный строковой id текущего компонента
const std::string& TComponentsListFrame::GetCurrentComponentId(void) const
{
 return CurrentComponentId;
}


// Возвращает флаг запрета редактирования дерева узлов
bool TComponentsListFrame::GetTreeReadOnlyFlag(void)
{
 return TreeReadOnlyFlag;
}

// Возвращает флаг запрета редактирования параметров
bool TComponentsListFrame::GetParamReadOnlyFlag(void)
{
 return ParamReadOnlyFlag;
}

// Возвращает признак наличия изменений в данных
bool TComponentsListFrame::GetRegistryModified(void)
{
 return RegistryModified;
}
// -----------------------


// -----------------------
// Методы управления физическими данными
// -----------------------
// Очищает список изменений реестра
void __fastcall TComponentsListFrame::ClearDiffRegistry(void)
{
// DiffRegistry.Clear();
}


bool __fastcall TComponentsListFrame::SetCurrentSubTree(std::string &fullname)
{
// if(!Registry)
//  return false;

 return true;
}

void __fastcall TComponentsListFrame::SetTreeReadOnlyFlag(bool flag)
{
 TreeReadOnlyFlag=flag;
}

void __fastcall TComponentsListFrame::SetParamReadOnlyFlag(bool flag)
{
 ParamReadOnlyFlag=flag;
}

void __fastcall TComponentsListFrame::SetRegistryModified(bool flag)
{
 RegistryModified=flag;
}
// -----------------------

//---------------------------------------------------------------------------
void __fastcall TComponentsListFrame::FrameResize(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TComponentsListFrame::HeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
/* int start=(CurrentPath.size())-2;
 int stop=Section->Index;
 for(int i=start;i>stop;i--)
 {
  ComponentsListViewColumnClick(0,0);
 }*/
}
//---------------------------------------------------------------------------


void __fastcall TComponentsListFrame::StringGridDblClick(TObject *Sender)
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
 SelectedId=0;
 CurrentComponentName+=AnsiString(StringGrid->Cells[1][StringGrid->Row]).c_str();
 CurrentComponentId+=AnsiString(StringGrid->Cells[0][StringGrid->Row]).c_str();
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TComponentsListFrame::StringGridSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect)
{
 UpdateSelectedComponentInfo();
}
//---------------------------------------------------------------------------

void __fastcall TComponentsListFrame::StringGridKeyPress(TObject *Sender, System::WideChar &Key)

{
 if(Key == VK_RETURN)
  StringGridDblClick(Sender);
}
//---------------------------------------------------------------------------

