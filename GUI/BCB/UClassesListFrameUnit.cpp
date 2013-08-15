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

// Отрисовка фрейма
void TUClassesListFrame::AUpdateInterface(void)
{
 if(PageControl->ActivePage == LibsTabSheet)
 {
// const char* lib_list=Storage_GetClassLibrariesList();
// LibrariesListNames=lib_list;
// std::ve

  int num_classes=0;
  LibraryClassNames.clear();
  LibraryNames.resize(Storage_GetNumClassLibraries());
  for(size_t i=0;i<LibraryNames.size();i++)
  {
   LibraryNames[i]=Storage_GetClassLibraryNameByIndex(i);
   const char* class_names=Storage_GetLibraryClassNamesByIndex(i);
   num_classes+=RDK::separatestring(std::string(class_names),TempLibraryNames, ',');
   sort(TempLibraryNames.begin(),TempLibraryNames.end());
   LibraryClassNames[LibraryNames[i]]=TempLibraryNames;
  }

  TTreeNode* sel=TreeView->Selected;
  String SelectedName;

  if(sel)
   SelectedName=sel->Text;

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
 }
 else
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
}

// Возвращает имя выбранного класса
String TUClassesListFrame::GetSelectedName(void)
{
 if(PageControl->ActivePage == LibsTabSheet)
 {
  TTreeNode* sel=TreeView->Selected;
  if(sel && sel->Parent)
   return sel->Text;
 }
 else
  return StringGrid->Cells[0][StringGrid->Row];

 return String("");
}

void __fastcall TUClassesListFrame::PageControlChange(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------

