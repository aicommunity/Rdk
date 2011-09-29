//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TUClassRegistryFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUClassRegistryFrame *UClassRegistryFrame;
//---------------------------------------------------------------------------
__fastcall TUClassRegistryFrame::TUClassRegistryFrame(TComponent* Owner)
    : TFrame(Owner)
{
 Registry=CurrentSubTree=CurrentRoot=0;
 RegistryModified=false;
 UpdateInterfaceFlag=false;
}
//---------------------------------------------------------------------------
// -----------------------
// Методы доступа к физическим данным
// -----------------------
// Возвращает указатель на текущий реестр
RDK::UClassRegistry* TUClassRegistryFrame::GetRegistry(void)
{
 return Registry;
}

// Возвращает указатель на изменения реестра
RDK::UClassRegistry* TUClassRegistryFrame::GetDiffRegistry(void)
{
 return &DiffRegistry;
}

// Возвращает указатель на выбранный узел
RDK::UClassRegistry* TUClassRegistryFrame::GetCurrentSubTree(void)
{
 return CurrentSubTree;
}

// Возвращает флаг запрета редактирования дерева узлов
bool TUClassRegistryFrame::GetTreeReadOnlyFlag(void)
{
 return TreeReadOnlyFlag;
}

// Возвращает флаг запрета редактирования параметров
bool TUClassRegistryFrame::GetParamReadOnlyFlag(void)
{
 return ParamReadOnlyFlag;
}

// Возвращает признак наличия изменений в данных
bool TUClassRegistryFrame::GetRegistryModified(void)
{
 return RegistryModified;
}
// -----------------------


// -----------------------
// Методы управления физическими данными
// -----------------------
void __fastcall TUClassRegistryFrame::SetRegistry(RDK::UClassRegistry *reg)
{
 TTreeNode *node;

 Registry=reg;
 ClearDiffRegistry();

 ListView->Items->Clear();
 ListView->Groups->Clear();
 if(!Registry)
  {
   CurrentSubTree=0;
   return;
  }

 CurrentRoot=Registry->FindSubTree(CurrentRootName);
 if(!CurrentRoot)
  CurrentRoot=Registry;

 CurrentRootName=CurrentRoot->GetFullName(Registry);

 CurrentSubTree=Registry->FindSubTree(CurrentComponentName);
 if(!CurrentSubTree)
 {
  std::vector<std::string> subtree;
  CurrentRoot->GetSubTrees(subtree);
  if(subtree.size())
  {
   CurrentSubTree=CurrentRoot->FindSubTree(subtree[0]);
   CurrentComponentName=CurrentSubTree->GetFullName(Registry);
  }
  else
   CurrentComponentName="";
 }

 UpdateInterface();
 RegistryModified=false;
}

// Очищает список изменений реестра
void __fastcall TUClassRegistryFrame::ClearDiffRegistry(void)
{
 DiffRegistry.Clear();
}


bool __fastcall TUClassRegistryFrame::SetCurrentSubTree(std::string &fullname)
{
 if(!Registry)
  return false;

 return true;
}

void __fastcall TUClassRegistryFrame::SetTreeReadOnlyFlag(bool flag)
{
 TreeReadOnlyFlag=flag;
}

void __fastcall TUClassRegistryFrame::SetParamReadOnlyFlag(bool flag)
{
 ParamReadOnlyFlag=flag;
}

void __fastcall TUClassRegistryFrame::SetRegistryModified(bool flag)
{
 RegistryModified=flag;
}
// -----------------------

// -----------------------
// Методы доступа к виртуальным данным
// -----------------------
// Возвращает указатель на список параметров выделенного узла
// или 0, если не выделено ничего
RDK::UClassRegData* __fastcall TUClassRegistryFrame::GetSelectedData(void)
{
 if(CurrentSubTree)
  return &(CurrentSubTree->operator()());

 return 0;
}
// -----------------------


// -----------------------
// Методы управления
// -----------------------
// Обновляет весь интерфейс
void __fastcall TUClassRegistryFrame::UpdateInterface(void)
{
 UpdateComponentList();
 UpdateParams();
}

// Перерисовывает список компонент
void __fastcall TUClassRegistryFrame::UpdateComponentList(void)
{
 return;
 if(!CurrentRoot)
  CurrentRoot=Registry;

 if(!CurrentRoot)
  return;

 UpdateComponentList(*CurrentRoot);
 UpdateParams();
 UpdatePath();
}

// Перерисовывает список параметров
void __fastcall TUClassRegistryFrame::UpdateParams(void)
{
 if(!CurrentSubTree)
 {
  UpdateComponentList();
  return;
 }

 UpdateParams(CurrentSubTree->operator ~());
}

// Перерисовывает путь до текущего узла
void __fastcall TUClassRegistryFrame::UpdatePath(void)
{
 UpdateInterfaceFlag=true;

 HeaderControl->Sections->Clear();

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

 UpdateInterfaceFlag=false;
}
// -----------------------



// -----------------------
// Скрытые методы управления
// -----------------------
// Перерисовывает список компонент
void __fastcall TUClassRegistryFrame::UpdateComponentList(RDK::UClassRegistry &root)
{
 return;

 TListGroup* group=0;
 TListItem* item=0;

 UpdateInterfaceFlag=true;

 if(!CurrentRoot)
  CurrentRoot=&root;

 std::vector<std::string> subtree;
 CurrentRoot->GetSubTrees(subtree);

 if(ComponentsListView->Groups->Count <= 0)
  group=ComponentsListView->Groups->Add();
 else
  group=ComponentsListView->Groups->Items[0];

 int itemscount=ComponentsListView->Items->Count;
 for(int i=ComponentsListView->Items->Count-1;i>=int(subtree.size());i--)
  ComponentsListView->Items->Delete(i);

 for(int i=0;i<int(subtree.size())-itemscount;i++)
 {
  item=ComponentsListView->Items->Add();
  item->GroupID=group->GroupID;
 }

 group->Header=CurrentRoot->GetName().c_str();

 itemscount=ComponentsListView->Items->Count;
 for(size_t i=0;i<subtree.size();i++)
 {
  item=ComponentsListView->Items->Item[i];
  item->Caption=subtree[i].c_str();
  item->Caption.EnsureUnicode();
  if(CurrentSubTree && CurrentSubTree->GetName() == subtree[i])
   ComponentsListView->ItemIndex=i;
 }
 ComponentsListView->Columns->Items[0]->Width=ComponentsListView->ClientWidth;

 UpdateInterfaceFlag=false;
}

// Перерисовывает список параметров
void __fastcall TUClassRegistryFrame::UpdateParams(RDK::UClassRegData &data)
{
 RDK::UClassRegDataIterator I;
 RDK::URegDataIterator rI,rJ;
 int i;
 AnsiString s;
 int currrow;
 TListGroup* group=0;
 TListItem* item=0;

 UpdateInterfaceFlag=true;

 ListView->Columns->Items[0]->Width=ListView->ClientWidth/2;
 ListView->Columns->Items[1]->Width=ListView->ClientWidth/2;

 currrow=ListView->ItemIndex;

 // Подготовка списка
 int itemscount=ListView->Groups->Count;
 for(int i=ListView->Groups->Count-1;i>=int(data.size());i--)
  ListView->Groups->Delete(i);

 for(int i=0;i<int(data.size())-itemscount;i++)
  ListView->Groups->Add();

 int numitems=0; // Число реально добавленых элементов в список
 I=data.begin();
 while(I != data.end())
 {
  numitems+=I->second.size();
  ++I;
 }

 itemscount=ListView->Items->Count;
 for(int i=ListView->Items->Count-1;i>=numitems;i--)
  ListView->Items->Delete(i);

 for(int i=0;i<numitems-itemscount;i++)
 {
  item=ListView->Items->Add();
  item->SubItems->Add("");
 }

 // Собствено заполнение списка
 int groupid=0;
 int itemid=0;
 I=data.begin();
 if(I != data.end())
  {
   while(I != data.end())
    {
     group=ListView->Groups->Items[groupid];
     group->Header=I->first.c_str();
     group->Header.EnsureUnicode();
     rI=I->second.begin();
     rJ=I->second.end();
     while(rI != rJ)
     {
      item=ListView->Items->Item[itemid];
      item->Caption=rI->first.c_str();
      item->Caption.EnsureUnicode();
      item->SubItems->Strings[0]=rI->second.c_str();
      item->SubItems->Strings[0].EnsureUnicode();
      item->GroupID=group->GroupID;
      ++rI;
      ++itemid;
     }
     ++I; ++groupid;
    }
  }

 if(ListView->Items->Count > currrow)
  ListView->ItemIndex=currrow;
 else
 if(ListView->Items->Count > 0)
  ListView->ItemIndex=ListView->Items->Count-1;

 UpdateInterfaceFlag=false;
}
// -----------------------

//---------------------------------------------------------------------------
void __fastcall TUClassRegistryFrame::RestoreBitBtnClick(TObject *Sender)
{
 int itemindex=ListView->ItemIndex;
 if(itemindex<0)
  return;
 LabeledEdit->Text=ListView->Items->Item[itemindex]->SubItems->Strings[0];
}
//---------------------------------------------------------------------------

void __fastcall TUClassRegistryFrame::SetBitBtnClick(TObject *Sender)
{
 RDK::UClassRegData *regdata=GetSelectedData();

 if(!regdata)
  return;

 int itemindex=ListView->ItemIndex;
 if(itemindex<0)
  return;

 (*regdata)[SelectedClassName][AnsiString(ListView->Items->Item[itemindex]->Caption).c_str()]
 = AnsiString(LabeledEdit->Text).c_str();

 std::string fullname=CurrentSubTree->GetFullName(Registry);
 DiffRegistry.CreateBranch(fullname);
 RDK::UClassRegData &diffregdata=DiffRegistry();
 diffregdata[SelectedClassName][AnsiString(ListView->Items->Item[itemindex]->Caption).c_str()]
 = AnsiString(LabeledEdit->Text).c_str();

 UpdateParams();
}
//---------------------------------------------------------------------------

void __fastcall TUClassRegistryFrame::LabeledEditKeyPress(TObject *Sender, wchar_t &Key)

{
 if(Key == VK_RETURN)
  {
   SetBitBtnClick(Sender);
  }
}
//---------------------------------------------------------------------------
void __fastcall TUClassRegistryFrame::ListViewSelectItem(TObject *Sender, TListItem *Item,
          bool Selected)
{
 if(UpdateInterfaceFlag)
  return;

 int itemindex=ListView->ItemIndex;
 if(itemindex<0)
  return;
 int groupid=ListView->Items->Item[itemindex]->GroupID;
 SelectedClassName=AnsiString(ListView->Groups->Items[groupid]->Header).c_str();

 LabeledEdit->EditLabel->Caption=ListView->Items->Item[itemindex]->Caption
                                +" value:";
 LabeledEdit->Text=ListView->Items->Item[itemindex]->SubItems->Strings[0];//ValueListEditor->Cells[1][ARow];
}
//---------------------------------------------------------------------------
void __fastcall TUClassRegistryFrame::ComponentsListViewResize(TObject *Sender)
{
 ComponentsListView->Columns->Items[0]->Width=ComponentsListView->ClientWidth;
}
//---------------------------------------------------------------------------
void __fastcall TUClassRegistryFrame::ComponentsListViewDblClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 if(!CurrentRoot)
  CurrentRoot=Registry;

 if(!CurrentRoot)
  return;

 int itemindex=ComponentsListView->ItemIndex;
 if(itemindex<0)
 {
  return;
 }

 CurrentRoot=CurrentRoot->FindSubTree(AnsiString(ComponentsListView->Items->Item[itemindex]->Caption).c_str());

 std::vector<std::string> subtree;
 CurrentRoot->GetSubTrees(subtree);
 if(subtree.size())
 {
  CurrentRootName=CurrentRoot->GetFullName(Registry);
  CurrentSubTree=CurrentRoot->FindSubTree(subtree[0]);
  CurrentComponentName=CurrentSubTree->GetFullName(Registry);
 }
 else
 {
  CurrentRoot=CurrentRoot->GetRoot();
 }
 UpdateComponentList();
}
//---------------------------------------------------------------------------


void __fastcall TUClassRegistryFrame::ComponentsListViewColumnClick(TObject *Sender,
          TListColumn *Column)
{
 if(UpdateInterfaceFlag)
  return;

 if(!CurrentRoot)
  CurrentRoot=Registry;

 if(!CurrentRoot)
  return;

 RDK::UClassRegistry *root=CurrentRoot;
 CurrentRoot=CurrentRoot->GetRoot();

 if(!CurrentRoot)
 {
  CurrentRoot=root;
  return;
 }

 std::vector<std::string> subtree;
 CurrentRoot->GetSubTrees(subtree);
 CurrentRootName=CurrentRoot->GetFullName(Registry);
 CurrentSubTree=CurrentRoot->FindSubTree(root->GetName());
 CurrentComponentName=CurrentSubTree->GetFullName(Registry);
 UpdateComponentList();
}
//---------------------------------------------------------------------------


void __fastcall TUClassRegistryFrame::ComponentsListViewSelectItem(TObject *Sender,
          TListItem *Item, bool Selected)
{
 if(UpdateInterfaceFlag)
  return;

 if(!CurrentRoot)
  CurrentRoot=Registry;

 if(!CurrentRoot)
  return;

 int itemindex=ComponentsListView->ItemIndex;
 if(itemindex<0)
  return;

 CurrentSubTree=CurrentRoot->FindSubTree(AnsiString(ComponentsListView->Items->Item[itemindex]->Caption).c_str());

 if(!CurrentSubTree)
  return;

 CurrentComponentName=CurrentSubTree->GetFullName(Registry);
 UpdateComponentList();
}
//---------------------------------------------------------------------------


void __fastcall TUClassRegistryFrame::ComponentsListViewKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift)
{
 if(Key == VK_RETURN)
 {
  ComponentsListViewDblClick(Sender);
 }
}
//---------------------------------------------------------------------------

void __fastcall TUClassRegistryFrame::HeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
 int start=(CurrentPath.size())-2;
 int stop=Section->Index;
 for(int i=start;i>stop;i--)
 {
  ComponentsListViewColumnClick(0,0);
 }
}
//---------------------------------------------------------------------------

