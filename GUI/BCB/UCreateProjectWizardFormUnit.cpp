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

 ProjectDirectoryLabeledEdit->Text=ProjectConfig.ProjectDirectory.c_str();
 ProjectNameLabeledEdit->Text=ProjectConfig.ProjectName.c_str();
 ProjectDescriptionRichEdit->Clear();
 ProjectDescriptionRichEdit->Lines->Add(ProjectConfig.ProjectDescription.c_str());
 ProjectAutoSaveFlagCheckBox->Checked=ProjectConfig.ProjectAutoSaveFlag;
 ProjectAutoSaveStatesFlagCheckBox->Checked=ProjectConfig.ProjectAutoSaveStatesFlag;
 EventsLogFlagCheckBox->Checked=ProjectConfig.EventsLogFlag;

 ProjectModeRadioGroup->ItemIndex=ProjectConfig.ProjectMode;
 ProjectTypeRadioGroup->ItemIndex=ProjectConfig.ProjectType;
 MultiThreadingModeRadioGroup->ItemIndex=ProjectConfig.MultiThreadingMode;
 CalculationSourceTimeModeRadioGroup->ItemIndex=ProjectConfig.CalcSourceTimeMode;
 ShowChannelsStateCheckBox->Checked=ProjectConfig.ShowChannelsStateFlag;

 // Настраиваем список каналов
 int num_channels=ProjectConfig.NumChannels;
 ChannelsNumberLabeledEdit->Text=IntToStr(num_channels);

 int channels_index=ChannelsStringGrid->Row;
 ChannelsStringGrid->RowCount=num_channels;
 ChannelsStringGrid->ColCount=1;

 for(int i=0;i<num_channels;i++)
 {
  ChannelsStringGrid->Cells[0][i]=IntToStr(i);
 }

 if(channels_index>=0 && channels_index<num_channels)
  ChannelsStringGrid->Row=channels_index;


 // Настраиваем состояние канала
 channels_index=ChannelsStringGrid->Row;
 if(channels_index>=0)
 {
  int index=0;
  PredefinedModelComboBox->Clear();
  std::map<std::string, int>::iterator I,J;
  I=PredefinedModels.begin();
  J=PredefinedModels.end();
  for(;I!=J;++I)
  {
   PredefinedModelComboBox->Items->Add(I->first.c_str());
   if(I->second == ProjectConfig.ChannelsConfig[channels_index].PredefinedStructure)
	index=PredefinedModelComboBox->Items->Count-1;
  }

  if(index >=0 && index < PredefinedModelComboBox->Items->Count)
   PredefinedModelComboBox->ItemIndex=index;
  else
   PredefinedModelComboBox->ItemIndex=0;

  ModelPageControl->ActivePageIndex=ProjectConfig.ChannelsConfig[channels_index].ModelMode;
  PredefinedModelComboBox->Text=ProjectConfig.ChannelsConfig[channels_index].PredefinedStructure;
  ProjectModelFileNameLabeledEdit->Text=ProjectConfig.ChannelsConfig[channels_index].ModelFileName.c_str();
  UClassesListFrame1->SetSelectedName(ProjectConfig.ChannelsConfig[channels_index].ClassName.c_str());

  ProjectTimeStepEdit->Text=IntToStr(ProjectConfig.ChannelsConfig[channels_index].DefaultTimeStep);
  GlobalTimeStepEdit->Text=IntToStr(ProjectConfig.ChannelsConfig[channels_index].GlobalTimeStep);

  CalculationModeRadioGroup->ItemIndex=ProjectConfig.ChannelsConfig[channels_index].CalculationMode;

  MinInterstepsIntervalEdit->Text=IntToStr(ProjectConfig.ChannelsConfig[channels_index].MinInterstepsInterval);

  InitAfterLoadCheckBox->Checked=ProjectConfig.ChannelsConfig[channels_index].InitAfterLoad;

  InitAfterLoadCheckBox->Checked=ProjectConfig.ChannelsConfig[channels_index].InitAfterLoad;
  ResetAfterLoadCheckBox->Checked=ProjectConfig.ChannelsConfig[channels_index].ResetAfterLoad;
  DebugModeCheckBox->Checked=ProjectConfig.ChannelsConfig[channels_index].DebugMode;

  if(ModelPageControl->ActivePage == DontChangeTabSheet)
  {
   ModelInfoRichEdit->Clear();
   String str="Model source: ";
   switch(ProjectConfig.ChannelsConfig[channels_index].ModelMode)
   {
   case 0:
//	str+=
   break;

   case 1:
   break;

   case 2:
   break;

   case 3:
   break;
   }
  }
 }
 else
  ModelInfoRichEdit->Clear();

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
// PredefinedModelRadioButtonClick(this);
// PredefinedModelComboBox->ItemIndex=0;
 PageControl->ActivePage = TabSheet1;
 ProjectConfig.NumChannels=1;
 ProjectConfig.ChannelsConfig.resize(1);
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

// std::map<std::string, int>::iterator I=PredefinedModels.find(AnsiString(PredefinedModelComboBox->Text).c_str());

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
 if(UpdateInterfaceFlag)
  return;

 ProjectConfig.ProjectType=ProjectTypeRadioGroup->ItemIndex;

 if(ProjectConfig.ProjectType == 0)
 {
  for(int i=0;i<ProjectConfig.NumChannels;i++)
  {
   int project_ts=ProjectConfig.ChannelsConfig[i].DefaultTimeStep;
   int global_ts=ProjectConfig.ChannelsConfig[i].GlobalTimeStep;

   if(project_ts == 2000 || project_ts == 30)
	ProjectConfig.ChannelsConfig[i].DefaultTimeStep=2000;

   if(global_ts == 2000 || global_ts == 30)
	ProjectConfig.ChannelsConfig[i].GlobalTimeStep=2000;
  }
 }
 else
 if(ProjectConfig.ProjectType == 1)
 {
  for(int i=0;i<ProjectConfig.NumChannels;i++)
  {
   int project_ts=ProjectConfig.ChannelsConfig[i].DefaultTimeStep;
   int global_ts=ProjectConfig.ChannelsConfig[i].GlobalTimeStep;

   if(project_ts == 2000 || project_ts == 30)
	ProjectConfig.ChannelsConfig[i].DefaultTimeStep=30;

   if(global_ts == 2000 || global_ts == 30)
	ProjectConfig.ChannelsConfig[i].GlobalTimeStep=30;
  }
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
 int channels_index=ChannelsStringGrid->Row;
 if(channels_index>=0)
 {
  ProjectConfig.ChannelsConfig[channels_index].ModelFileName=AnsiString(ProjectModelFileNameLabeledEdit->Text).c_str();
 }
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


void __fastcall TUCreateProjectWizardForm::ChannelsNumberLabeledEditChange(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 int num_channels=ProjectConfig.NumChannels;
 try
 {
  num_channels=StrToInt(ChannelsNumberLabeledEdit->Text);
 }
 catch(EConvertError &ex)
 {
 }

 if(num_channels == ProjectConfig.NumChannels)
  return;

 ProjectConfig.NumChannels=num_channels;
 ProjectConfig.ChannelsConfig.resize(ProjectConfig.NumChannels);
 UpdateInterface();
}
//---------------------------------------------------------------------------


void __fastcall TUCreateProjectWizardForm::ModelPageControlChange(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 int channels_index=ChannelsStringGrid->Row;
 if(channels_index>=0)
 {
  ProjectConfig.ChannelsConfig[channels_index].ModelMode=ModelPageControl->ActivePageIndex;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::ChannelsStringGridClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::PredefinedModelComboBoxChange(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 int channels_index=ChannelsStringGrid->Row;
 if(channels_index>=0)
 {
  ProjectConfig.ChannelsConfig[channels_index].PredefinedStructure=PredefinedModels[AnsiString(PredefinedModelComboBox->Text).c_str()];
 }
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::UClassesListFrame1StringGridClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 int channels_index=ChannelsStringGrid->Row;
 if(channels_index>=0)
 {
  ProjectConfig.ChannelsConfig[channels_index].ClassName=AnsiString(UClassesListFrame1->GetSelectedName()).c_str();
 }
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::UClassesListFrame1LibComponentListStringGridClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 int channels_index=ChannelsStringGrid->Row;
 if(channels_index>=0)
 {
  ProjectConfig.ChannelsConfig[channels_index].ClassName=AnsiString(UClassesListFrame1->GetSelectedName()).c_str();
 }
}
//---------------------------------------------------------------------------


void __fastcall TUCreateProjectWizardForm::ProjectTimeStepEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
 if(UpdateInterfaceFlag)
  return;

 int channels_index=ChannelsStringGrid->Row;
 if(channels_index<0)
  return;

 if(Key == VK_RETURN)
 {
  try
  {
   ProjectConfig.ChannelsConfig[channels_index].DefaultTimeStep=StrToInt(ProjectTimeStepEdit->Text);
   UpdateInterface();
  }
  catch(EConvertError &ex)
  {

  }
 }
}
//---------------------------------------------------------------------------
void __fastcall TUCreateProjectWizardForm::GlobalTimeStepEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
 if(UpdateInterfaceFlag)
  return;

 int channels_index=ChannelsStringGrid->Row;
 if(channels_index<0)
  return;

 if(Key == VK_RETURN)
 {
  try
  {
   ProjectConfig.ChannelsConfig[channels_index].DefaultTimeStep=StrToInt(GlobalTimeStepEdit->Text);
   UpdateInterface();
  }
  catch(EConvertError &ex)
  {

  }
 }
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::CalculationModeRadioGroupClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 int channels_index=ChannelsStringGrid->Row;
 if(channels_index<0)
  return;

 ProjectConfig.ChannelsConfig[channels_index].CalculationMode=CalculationModeRadioGroup->ItemIndex;
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::MinInterstepsIntervalEditKeyPress(TObject *Sender,
		  System::WideChar &Key)
{
 if(UpdateInterfaceFlag)
  return;

 int channels_index=ChannelsStringGrid->Row;
 if(channels_index<0)
  return;

 if(Key == VK_RETURN)
 {
  try
  {
   ProjectConfig.ChannelsConfig[channels_index].MinInterstepsInterval=StrToInt(MinInterstepsIntervalEdit->Text);
   UpdateInterface();
  }
  catch(EConvertError &ex)
  {

  }
 }
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::InitAfterLoadCheckBoxClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 int channels_index=ChannelsStringGrid->Row;
 if(channels_index<0)
  return;

 ProjectConfig.ChannelsConfig[channels_index].InitAfterLoad=InitAfterLoadCheckBox->Checked;
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::ResetAfterLoadCheckBoxClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 int channels_index=ChannelsStringGrid->Row;
 if(channels_index<0)
  return;

 ProjectConfig.ChannelsConfig[channels_index].ResetAfterLoad=ResetAfterLoadCheckBox->Checked;
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::DebugModeCheckBoxClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 int channels_index=ChannelsStringGrid->Row;
 if(channels_index<0)
  return;

 ProjectConfig.ChannelsConfig[channels_index].DebugMode=DebugModeCheckBox->Checked;
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::ProjectDirectoryLabeledEditChange(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 ProjectConfig.ProjectDirectory=AnsiString(ProjectDirectoryLabeledEdit->Text).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::ProjectNameLabeledEditChange(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 ProjectConfig.ProjectName=AnsiString(ProjectNameLabeledEdit->Text).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::ProjectDescriptionRichEditChange(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 std::string str;
 for(int i=0;i<ProjectDescriptionRichEdit->Lines->Count;i++)
 {
  str+=AnsiString(ProjectDescriptionRichEdit->Lines->Strings[i]).c_str();
  if(i<ProjectDescriptionRichEdit->Lines->Count-1)
   str+="/r/n";
 }

 ProjectConfig.ProjectName=str;
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::ProjectAutoSaveFlagCheckBoxClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 ProjectConfig.ProjectAutoSaveFlag=ProjectAutoSaveFlagCheckBox->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::ProjectAutoSaveStatesFlagCheckBoxClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 ProjectConfig.ProjectAutoSaveStatesFlag=ProjectAutoSaveStatesFlagCheckBox->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::EventsLogFlagCheckBoxClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 ProjectConfig.EventsLogFlag=EventsLogFlagCheckBox->Checked;
}
//---------------------------------------------------------------------------



void __fastcall TUCreateProjectWizardForm::ProjectModeRadioGroupClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 ProjectConfig.ProjectMode=ProjectModeRadioGroup->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::MultiThreadingModeRadioGroupClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 ProjectConfig.MultiThreadingMode=MultiThreadingModeRadioGroup->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TUCreateProjectWizardForm::CalculationSourceTimeModeRadioGroupClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 ProjectConfig.CalcSourceTimeMode=CalculationSourceTimeModeRadioGroup->ItemIndex;
}
//---------------------------------------------------------------------------

