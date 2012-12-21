//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsControlFrameUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UClassesListFrameUnit"
#pragma link "UComponentsListFrameUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUComponentsControlFrame *UComponentsControlFrame;
//---------------------------------------------------------------------------
__fastcall TUComponentsControlFrame::TUComponentsControlFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 UpdateInterval=1000;
}
//---------------------------------------------------------------------------
void TUComponentsControlFrame::AUpdateInterface(void)
{
 ClassesListFrame->UpdateInterface();
 ComponentsListFrame->UpdateInterface();
}

// —охран€ет выбранную модель
// ≈сли filename == "", то открывает окно запроса диалога
void TUComponentsControlFrame::SaveModelToFile(const String &filename)
{
 String FileName=filename;
 if(filename == "")
 {
  if(!SaveTextFileDialog->Execute())
   return;
  FileName=SaveTextFileDialog->FileName;
 }

 TRichEdit* RichEdit=new TRichEdit(this);
 RichEdit->Visible=false;
 RichEdit->Parent=this;

 RichEdit->PlainText=true;
 RichEdit->Text=Model_SaveComponent("");
 RichEdit->Lines->SaveToFile(FileName);

 delete RichEdit;
}

// «агружает выбранную модель
// ≈сли filename == "", то открывает окно запроса диалога
void TUComponentsControlFrame::LoadModelFromFile(const String &filename)
{
 String FileName=filename;
 if(filename == "")
 {
  if(!OpenTextFileDialog->Execute())
   return;
  FileName=OpenTextFileDialog->FileName;
 }

 TRichEdit* RichEdit=new TRichEdit(this);
 RichEdit->Parent=this;
 RichEdit->PlainText=true;
 RichEdit->Visible=false;
 RichEdit->Lines->LoadFromFile(FileName);

 if(RichEdit->Text.Length() >0)
 {
  Model_Destroy();
  Model_LoadComponent("",AnsiString(RichEdit->Text).c_str());
 }

 delete RichEdit;
 ComponentsListFrame->UpdateInterface();
}

// —охран€ет параметры выбранной модели
// ≈сли filename == "", то открывает окно запроса диалога
void TUComponentsControlFrame::SaveParametersToFile(const String &filename)
{
 String FileName=filename;
 if(filename == "")
 {
  if(!SaveTextFileDialog->Execute())
   return;
  FileName=SaveTextFileDialog->FileName;
 }

 TRichEdit* RichEdit=new TRichEdit(this);
 RichEdit->Visible=false;
 RichEdit->Parent=this;

 RichEdit->PlainText=true;
 RichEdit->Text=Model_SaveComponentParameters("");
 RichEdit->Lines->SaveToFile(FileName);

 delete RichEdit;
}

// «агружает параметры выбранной модели
// ≈сли filename == "", то открывает окно запроса диалога
void TUComponentsControlFrame::LoadParametersFromFile(const String &filename)
{
 String FileName=filename;
 if(filename == "")
 {
  if(!OpenTextFileDialog->Execute())
   return;
  FileName=OpenTextFileDialog->FileName;
 }

 if(!FileExists(FileName))
  return;

 TRichEdit* RichEdit=new TRichEdit(this);
 RichEdit->Parent=this;
 RichEdit->PlainText=true;
 RichEdit->Visible=false;
 RichEdit->Lines->LoadFromFile(FileName);

 Model_LoadComponentParameters("",AnsiString(RichEdit->Text).c_str());

 delete RichEdit;
 ComponentsListFrame->UpdateInterface();
}

// —охран€ет состо€ние выбранной модели
// ≈сли filename == "", то открывает окно запроса диалога
void TUComponentsControlFrame::SaveStatesToFile(const String &filename)
{
 String FileName=filename;
 if(filename == "")
 {
  if(!SaveTextFileDialog->Execute())
   return;
  FileName=SaveTextFileDialog->FileName;
 }

 TRichEdit* RichEdit=new TRichEdit(this);
 RichEdit->Visible=false;
 RichEdit->Parent=this;

 RichEdit->PlainText=true;
 RichEdit->Text=Model_SaveComponentState("");
 RichEdit->Lines->SaveToFile(FileName);

 delete RichEdit;
}

// «агружает состо€ние выбранной модели
// ≈сли filename == "", то открывает окно запроса диалога
void TUComponentsControlFrame::LoadStatesFromFile(const String &filename)
{
 String FileName=filename;
 if(filename == "")
 {
  if(!OpenTextFileDialog->Execute())
   return;
  FileName=OpenTextFileDialog->FileName;
 }

 if(!FileExists(FileName))
  return;

 TRichEdit* RichEdit=new TRichEdit(this);
 RichEdit->Parent=this;
 RichEdit->PlainText=true;
 RichEdit->Visible=false;
 RichEdit->Lines->LoadFromFile(FileName);

 Model_LoadComponentState("",AnsiString(RichEdit->Text).c_str());

 delete RichEdit;
 ComponentsListFrame->UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsControlFrame::TakeObjectButtonClick(TObject *Sender)
{
 int classid=ClassesListFrame->GetSelectedId();
 std::string stringid=ComponentsListFrame->GetCurrentComponentId();
 Model_AddComponent(stringid.c_str(), classid);

 ComponentsListFrame->UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsControlFrame::ReturnObjectButtonClick(TObject *Sender)
{
 std::string stringcompid=ComponentsListFrame->GetSelectedComponentId();
 if(stringcompid == "..")
  return;
 int compid=StrToInt(stringcompid.c_str());
 std::string stringid=ComponentsListFrame->GetCurrentComponentId();
 Model_DelComponent(stringid.c_str(), compid);

 ComponentsListFrame->UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsControlFrame::ClassesListFrameStringGridKeyPress(TObject *Sender,
          System::WideChar &Key)
{
 if(Key == VK_RETURN)
  TakeObjectButtonClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsControlFrame::HeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
 if(Section->Index == 0)
 {
  Model_Destroy();
  Model_Create(AnsiString(ClassesListFrame->GetSelectedName()).c_str());
 }
 else
 if(Section->Index == 1)
 {
  Model_Destroy();
 }
 else
 if(Section->Index == 2)
 {
  LoadModelFromFile("");
 }
 else
 if(Section->Index == 3)
 {
  SaveModelToFile("");
 }
 else
 if(Section->Index == 4)
 {
  LoadParametersFromFile("");
 }
 else
 if(Section->Index == 5)
 {
  SaveParametersToFile("");
 }
 else
 if(Section->Index == 6)
 {
  LoadStatesFromFile("");
 }
 else
 if(Section->Index == 7)
 {
  SaveStatesToFile("");
 }
 ComponentsListFrame->UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsControlFrame::ClassesListFrameStringGridDblClick(TObject *Sender)

{
 TakeObjectButtonClick(Sender);
}
//---------------------------------------------------------------------------


