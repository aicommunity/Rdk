//---------------------------------------------------------------------------

#include <vcl.h>
#include <vector>
#pragma hdrstop

#include "UClassesListFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUClassesListFrame *UClassesListFrame;

using namespace RDK;
//---------------------------------------------------------------------------
__fastcall TUClassesListFrame::TUClassesListFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 UpdateInterfaceFlag=false;
 CheckModelFlag=false;
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

  RepaintNeeded=false;
  int num_classes=0;
  LibraryClassNames.clear();
  NewLibraryNames.resize(Storage_GetNumClassLibraries());
  for(size_t i=0;i<NewLibraryNames.size();i++)
  {
   const char *p=Storage_GetClassLibraryNameByIndex(i);
   if(p)
	NewLibraryNames[i]=p;
   Engine_FreeBufString(p);
   const char* class_names=Storage_GetLibraryClassNamesByIndex(i);
   if(class_names)
    num_classes+=RDK::separatestring(std::string(class_names),TempLibraryNames, ',');
   Engine_FreeBufString(class_names);
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

  TreeView->Perform(WM_SETREDRAW, 0, (NativeInt)0);
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
  TreeView->Perform(WM_SETREDRAW, 1, (NativeInt)0);
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
  {
   const char* pclass_name=Storage_GetClassName(ids[i]);
   if(pclass_name)
    ClassNames[i]=pclass_name;
   Engine_FreeBufString(pclass_name);
  }

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
   const char *p=Storage_GetClassLibraryNameByIndex(i);
   if(p)
    library_names[i]=p;
   Engine_FreeBufString(p);

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


/// Отрисовывает список классов в выбранной библиотеке
void TUClassesListFrame::DrawClassesList(int library_index, TStringGrid *classes_string_grid)
{
  if(library_index>=0)
  {
   const char* class_names=Storage_GetLibraryClassNamesByIndex(library_index);
   std::vector<std::string> classes_list;
   int num_classes=0;
   if(class_names)
    num_classes=RDK::separatestring(std::string(class_names),classes_list, ',');
   Engine_FreeBufString(class_names);
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

// Возврат интерфейса в исходное состояние
void TUClassesListFrame::AClearInterface(void)
{
 LibrariesListNames.clear();
 LibraryNames.clear();
 NewLibraryNames.clear();
 TempLibraryNames.clear();
 LibraryClassNames.clear();
 ClassNames.clear();
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
 if(PageControl->ActivePage == NameTabSheet)
  return StringGrid->Cells[0][StringGrid->Row];
 else
 if(PageControl->ActivePage == LibsControlTabSheet)
  return LibComponentListStringGrid->Cells[1][LibComponentListStringGrid->Row];

 return String("");
}

/// Устанавливает имя выбранного класса
void TUClassesListFrame::SetSelectedName(const String &name)
{
 std::string sname=AnsiString(name).c_str();

 if(PageControl->ActivePage == LibsTabSheet)
 {
/*  TTreeNode* sel=TreeView->Selected;
  if(sel && sel->Parent)
   return sel->Text; */
  // TODO: Реализовать!
 }
 else
 if(PageControl->ActivePage == NameTabSheet)
 {
//std::map<std::string, std::vector<std::string> > LibraryClassNames;
  for(size_t i=0;i<ClassNames.size();i++)
   if(ClassNames[i] == sname)
   {
	StringGrid->Row=i+1;
	break;
   }
 }
 else
 if(PageControl->ActivePage == LibsControlTabSheet)
 {
  bool found=false;
  for(size_t j=0;j<LibraryClassNames.size();j++)
  {
   for(size_t i=0;i<ClassNames.size();i++)
	if(ClassNames[i] == sname)
	{
	 LibComponentListStringGrid->Row=i+1;
	 found=true;
	 break;
	}
   if(found)
	break;
  }
 }

 UpdateInterface();
}

// Возвращает имя выбранной библиотеки
String TUClassesListFrame::GetSelectedLibraryName(void)
{
 if(PageControl->ActivePage == LibsTabSheet)
 {
/*  TTreeNode* sel=TreeView->Selected;
  if(sel && sel->Parent)
   return sel->Text;
   */
  return String("");
 }
 else
 if(PageControl->ActivePage == NameTabSheet)
  return String("");
 // return StringGrid->Cells[0][StringGrid->Row];
 else
 if(PageControl->ActivePage == LibsControlTabSheet)
  return LibsListStringGrid->Cells[1][LibsListStringGrid->Row];

 return String("");
}




/// Создает новый класс в выбранной Runtime library
bool TUClassesListFrame::AddClassToRuntimeLibrary(const std::string &object_prototype_name, const std::string &class_name, const std::string &library_name)
{
 Storage_CreateClass(object_prototype_name.c_str(),class_name.c_str(),library_name.c_str());

 return true;
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

void __fastcall TUClassesListFrame::CreateRuntimeLibraryButtonClick(TObject *Sender)

{
 Storage_CreateRuntimeCollection("RuntimeLibrary1");
 UpdateInterface();
 LibsListStringGrid->Row=LibsListStringGrid->RowCount-1;
}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::AddClassButtonClick(TObject *Sender)
{
 if(NewClassName.empty() || NewComponentName.empty())
  return;

 AddClassToRuntimeLibrary(NewComponentName,NewClassName,AnsiString(GetSelectedLibraryName()).c_str());

 NewClassName.clear();
 NewComponentName.clear();
 UpdateInterface();
}
//---------------------------------------------------------------------------


void __fastcall TUClassesListFrame::StringGridMouseMove(TObject *Sender, TShiftState Shift,
		  int X, int Y)
{
 int C,R;
 StringGrid->MouseToCell(X, Y, C, R);

 if(C >= StringGrid->ColCount || R >= StringGrid->RowCount || C <0 || R<1)
  return;

 if ((StringGrid->Row != R) || (StringGrid->Col != C))
 {
//  StringGrid->Row = R;
//  StringGrid->Col = C;
//  Application->CancelHint();

  std::string class_name=AnsiString(StringGrid->Cells[C][R]).c_str();
  RDK::UELockPtr<RDK::UStorage> storage=GetStorageLock();

  try
  {
   const UEPtr<UContainerDescription> descr=storage->GetClassDescription(class_name);

   if(!descr)
	return;

   StringGrid->Hint = descr->GetHeader().c_str();
   if(!descr->GetDescription().empty())
   {
	StringGrid->Hint = StringGrid->Hint + "\r\n";
	StringGrid->Hint = StringGrid->Hint + descr->GetDescription().c_str();
   }
  }
  catch(RDK::UStorage::EClassNameNotExist &ex)
  {

  }
 }
}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::LibComponentListStringGridMouseMove(TObject *Sender,
		  TShiftState Shift, int X, int Y)
{
 int C,R;
 LibComponentListStringGrid->MouseToCell(X, Y, C, R);

 if(C >= LibComponentListStringGrid->ColCount || R >= LibComponentListStringGrid->RowCount || C <0 || R<1)
  return;

 if ((LibComponentListStringGrid->Row != R) || (LibComponentListStringGrid->Col != C))
 {
//  LibComponentListStringGrid->Row = R;
//  LibComponentListStringGrid->Col = C;
//  Application->CancelHint();

  std::string class_name=AnsiString(LibComponentListStringGrid->Cells[1][R]).c_str();
  RDK::UELockPtr<RDK::UStorage> storage=GetStorageLock();

  try
  {
   const UEPtr<UContainerDescription> descr=storage->GetClassDescription(class_name);

   if(!descr)
	return;

   LibComponentListStringGrid->Hint = descr->GetHeader().c_str();
   if(!descr->GetDescription().empty())
   {
	LibComponentListStringGrid->Hint = LibComponentListStringGrid->Hint + "\r\n";
	LibComponentListStringGrid->Hint = LibComponentListStringGrid->Hint + descr->GetDescription().c_str();
/*
	TPoint aPoint(X,Y);
	aPoint = ClientToScreen(aPoint);
	Application->ActivateHint(aPoint);*/
   }
  }
  catch(RDK::UStorage::EClassNameNotExist &ex)
  {

  }
 }

}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::TreeViewMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y)
{
 int C,R;
 TTreeNode* node = TreeView->GetNodeAt(X, Y);

 if(!node)
  return;

 //Application->CancelHint();

 std::string class_name=AnsiString(node->Text).c_str();
  RDK::UELockPtr<RDK::UStorage> storage=GetStorageLock();

 try
 {
  const UEPtr<UContainerDescription> descr=storage->GetClassDescription(class_name);
  if(!descr)
   return;

  TreeView->Hint = descr->GetHeader().c_str();
  if(!descr->GetDescription().empty())
  {
   TreeView->Hint = TreeView->Hint + "\r\n";
   TreeView->Hint = TreeView->Hint + descr->GetDescription().c_str();
  }
 }
 catch(RDK::UStorage::EClassNameNotExist &ex)
 {

 }
}
//---------------------------------------------------------------------------

