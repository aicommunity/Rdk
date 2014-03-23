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
 if(!IsEngineInit())
  return;
 try
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
  const char *p_buf=Model_SaveComponent("");
  if(p_buf)
   RichEdit->Text=p_buf;
  Engine_FreeBufString(p_buf);
  RichEdit->Lines->SaveToFile(FileName);

  delete RichEdit;
 }
 catch(Exception &exception)
 {
  MEngine_LogMessage(GetSelectedEngineIndex(), RDK_EX_ERROR, (std::string("Save model Fail: ")+AnsiString(exception.Message).c_str()).c_str());
 }

}

// «агружает выбранную модель
// ≈сли filename == "", то открывает окно запроса диалога
void TUComponentsControlFrame::LoadModelFromFile(const String &filename)
{
 if(!IsEngineInit())
  return;

 try
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
 catch(Exception &exception)
 {
  MEngine_LogMessage(GetSelectedEngineIndex(), RDK_EX_ERROR, (std::string("Load model Fail: ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

// —охран€ет параметры выбранной модели
// ≈сли filename == "", то открывает окно запроса диалога
void TUComponentsControlFrame::SaveParametersToFile(const String &filename)
{
 if(!IsEngineInit())
  return;

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
 const char *p_buf=Model_SaveComponentParameters("");
 if(p_buf)
  RichEdit->Text=p_buf;
 Engine_FreeBufString(p_buf);
 RichEdit->Lines->SaveToFile(FileName);

 delete RichEdit;
}

// «агружает параметры выбранной модели
// ≈сли filename == "", то открывает окно запроса диалога
void TUComponentsControlFrame::LoadParametersFromFile(const String &filename)
{
 if(!IsEngineInit())
  return;

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
 if(!IsEngineInit())
  return;

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
 const char *p_buf=Model_SaveComponentState("");
 if(p_buf)
  RichEdit->Text=p_buf;
 Engine_FreeBufString(p_buf);

 RichEdit->Lines->SaveToFile(FileName);

 delete RichEdit;
}

// «агружает состо€ние выбранной модели
// ≈сли filename == "", то открывает окно запроса диалога
void TUComponentsControlFrame::LoadStatesFromFile(const String &filename)
{
 if(!IsEngineInit())
  return;

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
 std::string classid=AnsiString(ClassesListFrame->GetSelectedName()).c_str();
 std::string stringid=ComponentsListFrame->GetCurrentComponentId();
 const char* pname=Model_AddComponent(stringid.c_str(), classid.c_str());
 if(pname)
  Engine_FreeBufString(pname);

 ComponentsListFrame->UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsControlFrame::ReturnObjectButtonClick(TObject *Sender)
{
 std::string stringcompid=ComponentsListFrame->GetSelectedComponentName();
 if(stringcompid == "..")
  return;
// int compid=StrToInt(stringcompid.c_str());
 std::string stringid=ComponentsListFrame->GetCurrentComponentId();
 Model_DelComponent(stringid.c_str(), stringcompid.c_str());

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


