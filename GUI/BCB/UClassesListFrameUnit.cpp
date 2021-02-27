//---------------------------------------------------------------------------

#include <vcl.h>
#include <vector>
#pragma hdrstop

#include "UClassesListFrameUnit.h"
#include <boost/algorithm/string.hpp>
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
 UpdateInterval=-1;
 UpdateInterfaceFlag=false;
 CheckModelFlag=false;
 RepaintNeeded=false;
}
//---------------------------------------------------------------------------

// Отрисовка фрейма
void TUClassesListFrame::AUpdateInterface(void)
{
 std::string search=AnsiString(Trim(SearchEdit->Text)).c_str();

 // Список RT библиотек
 RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock();
 std::string buff;
 storage->GetLibsNameListByType(buff,2);
 std::vector<std::string> RTlibsNames;
 RDK::separatestring(buff,RTlibsNames,',');

 std::vector<std::string> RTclassesNames;

 for(size_t i=0;i<RTlibsNames.size();i++)
 {
  const char * stringBuff;
  stringBuff = Storage_GetLibraryClassNames(RTlibsNames[i].c_str());

  // Если нет классов
  if((stringBuff[0] == '\0'))
  {
   Engine_FreeBufString(stringBuff);
   continue;
  }

  std::string buff2=stringBuff;

  std::vector<std::string> libClasses;
  RDK::separatestring(buff2,libClasses,',');
  Engine_FreeBufString(stringBuff);
  RTclassesNames.insert(RTclassesNames.end(),libClasses.begin(),libClasses.end());
 }

 if(PageControl->ActivePage == LibsTabSheet)
 {
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

   std::vector<std::string>::iterator I=TempLibraryNames.begin(),K;
   if(!search.empty())
	while(I != TempLibraryNames.end())
	{
	 bool f1 = boost::icontains(*I, search);
	 if(!f1)
	 {
	  TempLibraryNames.erase(I);
	 }
	 else
	  ++I;
	}
   sort(TempLibraryNames.begin(),TempLibraryNames.end());
   if(!TempLibraryNames.empty())
   {
	LibraryClassNames[NewLibraryNames[i]]=TempLibraryNames;
   }
   else
   {
	LibraryClassNames.erase(NewLibraryNames[i]);
   }
  }

  vector<string> v;
  for(std::map<std::string, std::vector<std::string> >::iterator it = LibraryClassNames.begin(); it != LibraryClassNames.end(); ++it)
  {
   v.push_back(it->first);
  }

  std::vector<std::string>::iterator libI=NewLibraryNames.begin();
  while(libI != NewLibraryNames.end())
  {
   if(find(v.begin(),v.end(),*libI) == v.end())
	NewLibraryNames.erase(libI);
   else
    ++libI;
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

  std::vector<std::string>::iterator I=ClassNames.begin(),K;
  if(!search.empty())
   while(I != ClassNames.end())
   {
	bool f1 = boost::icontains(*I, search);
	if(!f1)
	{
	 ClassNames.erase(I);
	}
 	else
	 ++I;
   }

  sort(ClassNames.begin(),ClassNames.end());
  numclasses=int(ClassNames.size());
  StringGrid->RowCount=numclasses+1;

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
//  std::vector<std::string> library_names;
//  library_names.resize(Storage_GetNumClassLibraries());
  LibsListStringGrid->RowCount=RTlibsNames.size()+1;
  LibsListStringGrid->ColCount=3;
  if(LibsListStringGrid->RowCount>1)
   LibsListStringGrid->FixedRows=1;
  for(size_t i=0;i<RTlibsNames.size();i++)
  {
//   const char *p=Storage_GetClassLibraryNameByIndex(i);
//   if(p)
//	library_names[i]=p;
//   Engine_FreeBufString(p);

   LibsListStringGrid->Cells[0][i+1]=i+1;
   LibsListStringGrid->Cells[1][i+1]=RTlibsNames[i].c_str();
//   LibsListStringGrid->Cells[2][i+1]=library_names[i].c_str();
  }
  LibsListStringGrid->ColWidths[0]=20;
  LibsListStringGrid->ColWidths[1]=LibsListStringGrid->ClientWidth-LibsListStringGrid->ColWidths[0];
  LibsListStringGrid->Cells[0][0]="#";
  LibsListStringGrid->Cells[1][0]="Library Name";

  DrawClassesList(AnsiString(LibsListStringGrid->Cells[1][LibsListStringGrid->Row]).c_str(), LibComponentListStringGrid);
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

void TUClassesListFrame::DrawClassesList(const std::string &library_name, TStringGrid *classes_string_grid)
{
  if(!library_name.empty())
  {
   const char* class_names=Storage_GetLibraryClassNames(library_name.c_str());
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

 DrawClassesList(AnsiString(LibsListStringGrid->Cells[1][LibsListStringGrid->Row]).c_str(), LibComponentListStringGrid);
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

void __fastcall TUClassesListFrame::SearchEditChange(TObject *Sender)
{
 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::CreateRuntimeLibrary1Click(TObject *Sender)
{
 // Storage
 RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();
 if(!storage)
  return;

 String lib_name = InputBox("Create Runtime Library", "Please enter library name", "");
 if(lib_name.Length()==0)
  return;

 if(!storage->CreateRuntimeCollection(AnsiString(lib_name).c_str()))
 {
  Application->MessageBox((String("An error occurred while creating library ")+lib_name).c_str(),L"Error",MB_OK);
 }

 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::DeleteRuntimeLibrary1Click(TObject *Sender)
{
 // Storage
 RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();
 if(!storage)
  return;

  std::string lib_name=AnsiString(LibsListStringGrid->Cells[1][LibsListStringGrid->Row]).c_str();

 if(Application->MessageBox((String("Are you sure to delete ")+lib_name.c_str()).c_str(),L"Error",MB_YESNO) != ID_YES)
  return;
 if(!storage->DeleteRuntimeCollection(lib_name.c_str()))
 {
  Application->MessageBox((String("An error occurred while creating library ")+lib_name.c_str()).c_str(),L"Error",MB_OK);
 }

 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::CreateNewClass1Click(TObject *Sender)
{
 RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();

 // Если нет модели
 if(!engine || !engine->GetModel() || !engine->GetModel()->GetStorage())
  return;

 // Выделенный компонент
 RDK::UEPtr<RDK::UContainer> container = engine->GetModel()
								->GetComponentL(NewComponentName, true);
 // Если компонент не выделен
 if(!container)
  return;

 // Имя текущей выбранной библиотеки (если выбрана)
 std::string lib_name=AnsiString(LibsListStringGrid->Cells[1][LibsListStringGrid->Row]).c_str();

 String class_name = InputBox("Class name", "Please enter new class name", "");
 if(class_name.Length()==0)
  return;

 String default_comp_name = InputBox("Default component name", "Please enter default component name", container->GetName().c_str());
 if(default_comp_name.Length()==0)
  return;


// std::string buff;
// engine->GetModel()->GetStorage()->GetLibsNameListByType(buff,2);
// std::vector<std::string> libs_names;
// RDK::separatestring(buff,libs_names,',');

 if(!engine->GetEnvironment()->
				GetStorage()->AddClassToCollection(AnsiString(class_name).c_str(), AnsiString(default_comp_name).c_str(),
											   false, container, lib_name))
 {
  Application->MessageBox((String("An error occurred while adding new class ")+class_name+ String(" to library ")+lib_name.c_str()).c_str(),L"Error",MB_OK);
  return;
 }

 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUClassesListFrame::DeleteClass1Click(TObject *Sender)
{
 //
}
//---------------------------------------------------------------------------

