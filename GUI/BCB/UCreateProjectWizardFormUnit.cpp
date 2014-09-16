//---------------------------------------------------------------------------

#include <vcl.h>
#include <Vcl.FileCtrl.hpp>
#pragma hdrstop

#include "UCreateProjectWizardFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma link "UClassesListFrameUnit"
#pragma resource "*.dfm"
TUCreateProjectWizardForm *UCreateProjectWizardForm;
//---------------------------------------------------------------------------
__fastcall TUCreateProjectWizardForm::TUCreateProjectWizardForm(TComponent* Owner)
	: TForm(Owner)
{
 UpdateInterfaceFlag=false;
 ClearPredefinedModels();
}

void TUCreateProjectWizardForm::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

 if(PageControl->ActivePage == TabSheet1)
 {
  PrevButton->Enabled=false;
  NextButton->Enabled=true;
 }
 else
 if(PageControl->ActivePage == TabSheet2)
 {
  PrevButton->Enabled=true;
  NextButton->Enabled=true;
 }
 else
 if(PageControl->ActivePage == TabSheet3)
 {
  PrevButton->Enabled=true;
  NextButton->Enabled=false;
 }

 int index=PredefinedModelComboBox->ItemIndex;
 PredefinedModelComboBox->Clear();
 std::map<std::string, int>::iterator I,J;
 I=PredefinedModels.begin();
 J=PredefinedModels.end();
 for(;I!=J;++I)
  PredefinedModelComboBox->Items->Add(I->first.c_str());
 if(index >=0 && index < PredefinedModelComboBox->Items->Count)
  PredefinedModelComboBox->ItemIndex=index;
 else
  PredefinedModelComboBox->ItemIndex=0;

 UpdateInterfaceFlag=false;
}

// Очистка визарда
void TUCreateProjectWizardForm::ClearWizard(void)
{
 ProjectDirectoryLabeledEdit->Text="";
 ProjectNameLabeledEdit->Text="";
 ProjectDescriptionRichEdit->Text="";
 ProjectTypeRadioGroup->ItemIndex=0;
 ProjectTimeStepEdit->Text="30";
 ProjectTypeRadioGroupClick(this);
// ProjectCalculationModeRadioGroup->ItemIndex=2;
// PredefinedModelRadioButton->Checked=true;
 PredefinedModelRadioButtonClick(this);
 PredefinedModelComboBox->ItemIndex=0;
 PageControl->ActivePage = TabSheet1;
 UpdateInterface();
}

// Очистить список вариантов предустановленных моделей
void TUCreateProjectWizardForm::ClearPredefinedModels(void)
{
 PredefinedModels.clear();
 PredefinedModels["<None>"]=0;
}

// Добавить вариант в список предустановленных моделей
void TUCreateProjectWizardForm::AddPredefinedModel(const std::string &name, int index)
{
 PredefinedModels[name]=index;
}


// Отображает визард для создания проекта
int TUCreateProjectWizardForm::ShowCreateProject(void)
{
 ProjectDirectoryLabeledEdit->Enabled=true;
// PredefinedModelRadioButton->Enabled=true;
 PredefinedModelComboBox->Enabled=true;
 TabSheet3->Enabled=true;

 return ShowModal();
}

// Отображает визард для модификации проекта
int TUCreateProjectWizardForm::ShowProjectOptions(void)
{
 ProjectDirectoryLabeledEdit->Enabled=false;
// PredefinedModelRadioButton->Enabled=false;
 PredefinedModelComboBox->Enabled=false;
 TabSheet3->Enabled=false;

 return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::FinishButtonClick(TObject *Sender)
{
 if(ProjectDirectoryLabeledEdit->Text.Length() != 0)
 {
  try
  {
   if(!DirectoryExists(ProjectDirectoryLabeledEdit->Text))
	ForceDirectories(ProjectDirectoryLabeledEdit->Text);
  }
  catch(EInOutError &exception)
  {
   Application->MessageBox(L"Please select project directory correctly", L"Error", MB_OK);
   return;
  }
 }
 else
 {
  Application->MessageBox(L"Please select project directory", L"Error", MB_OK);
  return;
 }

 std::map<std::string, int>::iterator I=PredefinedModels.find(AnsiString(PredefinedModelComboBox->Text).c_str());

 if(I != PredefinedModels.end())
  PredefinedStructure=I->second;
 else
  PredefinedStructure=0;
 ModalResult=mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::Button1Click(TObject *Sender)
{
 String chosenDir=ExtractFilePath(Application->ExeName);

// if(SelectDirectory(chosenDir,TSelectDirOpts() << sdAllowCreate << sdPerformCreate << sdPrompt,SELDIRHELP))
 if(SelectDirectory("Select project directory", ExtractFilePath(Application->ExeName), chosenDir,TSelectDirExtOpts() << sdNewFolder << sdNewUI))
 {
  ProjectDirectoryLabeledEdit->Text=chosenDir;
 }
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::ProjectTypeRadioGroupClick(TObject *Sender)

{
 if(ProjectTypeRadioGroup->ItemIndex == 0)
 {
//  ProjectCalculationModeRadioGroup->ItemIndex=1;
  if(ProjectTimeStepEdit->Text == "2000" || ProjectTimeStepEdit->Text == "30")
   ProjectTimeStepEdit->Text="2000";

//  VideoAnalysisGroupBox->Enabled=false;
 }
 else
 if(ProjectTypeRadioGroup->ItemIndex == 1)
 {
//  ProjectCalculationModeRadioGroup->ItemIndex=0;
  if(ProjectTimeStepEdit->Text == "2000" || ProjectTimeStepEdit->Text == "30")
   ProjectTimeStepEdit->Text="30";

//  VideoAnalysisGroupBox->Enabled=true;
 }
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::PredefinedModelRadioButtonClick(TObject *Sender)
{
// PredefinedModelRadioButton->Checked=true;
 PredefinedModelComboBox->Enabled=true;
// ModelFileNameRadioButton->Checked=false;
 ProjectModelFileNameLabeledEdit->Enabled=false;
 OpenModelButton->Enabled=false;
// RootModelComponentNameRadioButton->Checked=false;
 UClassesListFrame1->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::RootModelComponentNameRadioButtonClick(TObject *Sender)
{
// PredefinedModelRadioButton->Checked=false;
 PredefinedModelComboBox->Enabled=false;
// ModelFileNameRadioButton->Checked=false;
 ProjectModelFileNameLabeledEdit->Enabled=false;
 OpenModelButton->Enabled=false;
// RootModelComponentNameRadioButton->Checked=true;
 UClassesListFrame1->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::PageControlChange(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::PrevButtonClick(TObject *Sender)
{
 if(PageControl->ActivePageIndex > 0)
 {
  PageControl->ActivePageIndex=PageControl->ActivePageIndex-1;
  UpdateInterface();
 }
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::NextButtonClick(TObject *Sender)
{
 if(PageControl->ActivePageIndex < PageControl->PageCount-1)
 {
  PageControl->ActivePageIndex=PageControl->ActivePageIndex+1;
  UpdateInterface();
 }
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::OpenModelButtonClick(TObject *Sender)
{
 OpenTextFileDialog->InitialDir=ProjectDirectoryLabeledEdit->Text;
 if(!OpenTextFileDialog->Execute())
  return;

 ProjectModelFileNameLabeledEdit->Text=OpenTextFileDialog->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::ModelFileNameRadioButtonClick(TObject *Sender)

{
// PredefinedModelRadioButton->Checked=false;
 PredefinedModelComboBox->Enabled=false;
// ModelFileNameRadioButton->Checked=true;
 ProjectModelFileNameLabeledEdit->Enabled=true;
 OpenModelButton->Enabled=true;
// RootModelComponentNameRadioButton->Checked=false;
 UClassesListFrame1->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::FormShow(TObject *Sender)
{
 UClassesListFrame1->UpdateInterface(true);
}
//---------------------------------------------------------------------------

