//---------------------------------------------------------------------------

#include <vcl.h>
#include <vector>
#pragma hdrstop

#include "UClassesListFrameUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUClassesListFrame *UClassesListFrame;
//---------------------------------------------------------------------------
__fastcall TUClassesListFrame::TUClassesListFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 UpdateInterfaceFlag=false;
}
//---------------------------------------------------------------------------

// ��������� ������
void TUClassesListFrame::AUpdateInterface(void)
{
 if(PageControl->ActivePage == LibsTabSheet)
 {
// const char* lib_list=Storage_GetClassLibrariesList();
// LibrariesListNames=lib_list;
// std::ve

  RepaintNeeded=false;
  int num_classes=0;
  LibraryClassNames.clear();
  NewLibraryNames.resize(Storage_GetNumClassLibraries());
  for(size_t i=0;i<NewLibraryNames.size();i++)
  {
   NewLibraryNames[i]=Storage_GetClassLibraryNameByIndex(i);
   const char* class_names=Storage_GetLibraryClassNamesByIndex(i);
   num_classes+=RDK::separatestring(std::string(class_names),TempLibraryNames, ',');
   sort(TempLibraryNames.begin(),TempLibraryNames.end());
   LibraryClassNames[NewLibraryNames[i]]=TempLibraryNames;
  }

  if(NewLibraryNames == LibraryNames)
   return;

  LibraryNames=NewLibraryNames;

  TTreeNode* sel=TreeView->Selected;
  String SelectedName;

  if(sel)
   SelectedName=sel->Text;

  TreeView->Perform(WM_SETREDRAW, 0, 0);
  TreeView->Items->Clear();
  for(size_t i=0;i<LibraryNames.size();i++)
  {
   TTreeNode* tn;
   tn=TreeView->Items->AddChild(0,LibraryNames[i].c_str());
   std::vector<std::string> &class_names=LibraryClassNames[LibraryNames[i]];
   for(size_t j=0;j<class_names.size();j++)
   {
	TTreeNode* node=0;
	node=TreeView->Items->AddChild(tn,class_names[j].c_str());
	if(SelectedName == class_names[j].c_str())
	 TreeView->Selected=node;
   }

   tn->Expand(true);
  }
  TreeView->Perform(WM_SETREDRAW, 1, 0);
  TreeView->Update();
 }
 else
 if(PageControl->ActivePage == NameTabSheet)
 {
  int numclasses=Storage_GetNumClasses();

  int row=StringGrid->Row;
  StringGrid->ColCount=1;
  StringGrid->RowCount=numclasses+1;

  std::vector<int> ids;
  ids.resize(numclasses);
  Storage_GetClassesList(&ids[0]);
  ClassNames.resize(numclasses);
  for(int i=0;i<numclasses;i++)
   ClassNames[i]=Storage_GetClassName(ids[i]);

  sort(ClassNames.begin(),ClassNames.end());
  for(int i=0;i<numclasses;i++)
  {
   StringGrid->Cells[0][i+1]=ClassNames[i].c_str();
  }

  StringGrid->ColWidths[0]=Width;

  StringGrid->Cells[0][0]="Class name";

  if(row<StringGrid->RowCount)
   StringGrid->Row=row;
 }
 else
 if(PageControl->ActivePage == LibsControlTabSheet)
 {
  std::vector<std::string> library_names;
  library_names.resize(Storage_GetNumClassLibraries());
  LibsListStringGrid->RowCount=library_names.size()+1;
  LibsListStringGrid->ColCount=3;
  if(LibsListStringGrid->RowCount>0)
   LibsListStringGrid->FixedRows=1;
  for(size_t i=0;i<library_names.size();i++)
  {
   library_names[i]=Storage_GetClassLibraryNameByIndex(i);
   LibsListStringGrid->Cells[0][i+1]=i+1;
   LibsListStringGrid->Cells[1][i+1]=library_names[i].c_str();
//   LibsListStringGrid->Cells[2][i+1]=library_names[i].c_str();
  }
  LibsListStringGrid->ColWidths[0]=20;
  LibsListStringGrid->ColWidths[1]=LibsListStringGrid->ClientWidth-LibsListStringGrid->ColWidths[0];
  LibsListStringGrid->Cells[0][0]="#";
  LibsListStringGrid->Cells[1][0]="Library Name";

  DrawClassesList(LibsListStringGrid->Row-1, LibComponentListStringGrid);
 }
}


/// ������������ ������ ������� � ��������� ����������
void TUClassesListFrame::DrawClassesList(int library_index, TStringGrid *classes_string_grid)
{
  if(library_index>=0)
  {
   const char* class_names=Storage_GetLibraryClassNamesByIndex(library_index);
   std::vector<std::string> classes_list;
   int num_classes;
   num_classes=RDK::separatestring(std::string(class_names),classes_list, ',');
   sort(classes_list.begin(),classes_list.end());
//   LibraryClassNames[NewLibraryNames[i]]=TempLibraryNames;

   classes_string_grid->RowCount=num_classes+1;
   classes_string_grid->ColCount=2;
   if(classes_string_grid->RowCount>1)
	classes_string_grid->FixedRows=1;

   for(size_t i=0;i<classes_list.size();i++)
   {
	classes_string_grid->Cells[0][i+1]=i+1;
	classes_string_grid->Cells[1][i+1]=classes_list[i].c_str();
   }
  }
  else
  {
   classes_string_grid->RowCount=0;
  }
  classes_string_grid->Cells[0][0]="#";
  classes_string_grid->Cells[1][0]="Class Name";
  classes_string_grid->ColWidths[0]=20;
  classes_string_grid->ColWidths[1]=classes_string_grid->ClientWidth-classes_string_grid->ColWidths[0];
}

// ������� ���������� � �������� ���������
void TUClassesListFrame::AClearInterface(void)
{
 LibrariesListNames.clear();
 LibraryNames.clear();
 NewLibraryNames.clear();
 TempLibraryNames.clear();
 LibraryClassNames.clear();
 ClassNames.clear();
}

// ���������� ��� ���������� ������
String TUClassesListFrame::GetSelectedName(void)
{
 if(PageControl->ActivePage == LibsTabSheet)
 {
  TTreeNode* sel=TreeView->Selected;
  if(sel && sel->Parent)
   return sel->Text;
 }
 else
 if(PageControl->ActivePage == NameTabSheet)
  return StringGrid->Cells[0][StringGrid->Row];
 else
 if(PageControl->ActivePage == LibsControlTabSheet)
  return LibComponentListStringGrid->Cells[1][LibComponentListStringGrid->Row];

 return String("");
}

void __fastcall TUClassesListFrame::PageControlChange(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::StringGridMouseEnter(TObject *Sender)
{
 StringGrid->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::TreeViewMouseEnter(TObject *Sender)
{
 TreeView->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::LibsListStringGridSelectCell(TObject *Sender,
          int ACol, int ARow, bool &CanSelect)
{
 if(UpdateInterfaceFlag)
  return;

 DrawClassesList(LibsListStringGrid->Row-1, LibComponentListStringGrid);
}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::LibsListStringGridMouseEnter(TObject *Sender)

{
 LibsListStringGrid->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::LibComponentListStringGridMouseEnter(TObject *Sender)

{
 LibComponentListStringGrid->SetFocus();
}
//---------------------------------------------------------------------------

