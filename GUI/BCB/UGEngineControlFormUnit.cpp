//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include <Vcl.FileCtrl.hpp>
#include <System.IOUtils.hpp>
#include "UGEngineControlFormUnit.h"
#ifdef RDK_VIDEO
#include "TVideoGrabberControlFormUnit.h"
#endif
#include "UImagesFormUnit.h"
#include "UComponentsControlFormUnit.h"
#include "UComponentLinksFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#ifdef RDK_VIDEO
#include "VideoOutputFormUnit.h"
#endif
#include "UClassesListFormUnit.h"
#include "UComponentsPerformanceFormUnit.h"
#include "UFavoriteComponentInfoFormUnit.h"
#include "UDrawEngineFormUnit.h"
#include "UCreateProjectWizardFormUnit.h"
#include "UWatchFormUnit.h"
//#include "TUFileSystem.h"
#include "rdk_cpp_initdll.h"
#include "myrdk.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma link "UEngineMonitorFrameUnit"
#pragma link "UComponentsListFrameUnit"
#pragma link "UComponentsPerformanceFrameUnit"
#pragma link "UImagesFrameUnit"
#pragma link "UWatchFrameUnit"
#pragma link "UDrawEngineFrameUnit"
#pragma resource "*.dfm"
TUGEngineControlForm *UGEngineControlForm;
//---------------------------------------------------------------------------
__fastcall TUGEngineControlForm::TUGEngineControlForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 ProjectAutoSaveFlag=true;

 // Признак наличия открытого проекта
 ProjectOpenFlag=false;
}
//---------------------------------------------------------------------------
// Метод, вызываемый перед шагом расчета
void TUGEngineControlForm::ABeforeCalculate(void)
{
/*
 for(int i=0;i<Env_GetNumInputImages();i++)
 {
  if(!VideoOutputForm->GetVideoOutputFrame(i))
   continue;
  RDK::UBitmap &source=VideoOutputForm->GetVideoOutputFrame(i)->BmpSource;

  if(source.GetByteLength()>0)
  {
   Env_SetInputRes(i, InputEnvImageWidth, InputEnvImageHeight);
   Env_SetInputImage(i,(unsigned char*)source.GetData(),source.GetWidth(), source.GetHeight(),source.GetColorModel());
  }
 }
*/
}

// Метод, вызываемый после шага расчета
void TUGEngineControlForm::AAfterCalculate(void)
{
}

void TUGEngineControlForm::AUpdateInterface(void)
{
#ifdef RDK_VIDEO
 CaptureVideo1->Caption=String("Capture Video (")+IntToStr(VideoOutputForm->GetActiveSource())+")";
 OpenVideo1->Caption=String("Open Video File (")+IntToStr(VideoOutputForm->GetActiveSource())+")";
 OpenImage1->Caption=String("Open Image (")+IntToStr(VideoOutputForm->GetActiveSource())+")";
#endif

 Caption="Engine Control";
 if(ProjectOpenFlag)
 {
  Caption=Caption+String(" [")+ProjectFileName+"]";
 }
 StatusBar->SimpleText=UEngineMonitorForm->EngineMonitorFrame->StatusBar->SimpleText;
 StatusBar->Repaint();
 StatusBar->Update();
}

void __fastcall TUGEngineControlForm::FormShow(TObject *Sender)
{
 if(MainFormName.Length()>0)
 {
  TComponent *component=Application->FindComponent(MainFormName);
  TForm* form=dynamic_cast<TForm*>(component);
  if(form)
   form->Show();

  MainFormName="";
 }

 UpdateInterface();

 HideTimer->Enabled=true;

 UDrawEngineFrame1->ComponentsListFrame=UComponentsListFrame1;
 UComponentsListFrame1->DrawEngineFrame=UDrawEngineFrame1;
 UEngineMonitorForm->Parent=LogsTabSheet;
 UEngineMonitorForm->BorderStyle=bsNone;
 UEngineMonitorForm->EngineMonitorFrame->StatusBar->Visible=false;
 UEngineMonitorForm->Align=alClient;
 UEngineMonitorForm->Show();
}

// Сохраняет параметры интерфейса в xml
void TUGEngineControlForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.SelectNodeForce("Pages");
 xml.DelNodeInternalContent();
 int count=0;
 for(int i=1;i<PageControl1->PageCount;i++)
 {
  bool is_saved=false;
  for(int j=0;j<ComponentCount;j++)
  {
   TUVisualControllerFrame *frame=dynamic_cast<TUVisualControllerFrame*>(Components[j]);
   if(frame && frame->Parent == PageControl1->Pages[i])
   {
	xml.WriteString(string("Caption_")+RDK::sntoa(i+1),AnsiString(PageControl1->Pages[i]->Caption).c_str());
	xml.WriteString(string("Class_")+RDK::sntoa(i+1),AnsiString(Components[j]->ClassName()).c_str());
	xml.WriteString(string("Type_")+RDK::sntoa(i+1),AnsiString("Frame").c_str());
	xml.WriteString(string("ComponentControlName_")+RDK::sntoa(i+1),frame->GetComponentControlName());
	++count;
	is_saved=true;
	break;
   }
  }

  if(PageControl1->Pages[i]->ComponentCount>0)
  {
   TUVisualControllerForm *form=dynamic_cast<TUVisualControllerForm*>(PageControl1->Pages[i]->Components[0]);
   if(form)
   {
	std::map<std::string, TUVisualControllerForm*>::iterator I=UComponentsListFrame1->ComponentControllers.begin();
	for(;I != UComponentsListFrame1->ComponentControllers.end();++I)
	{
	 if(I->second->ClassNameIs(form->ClassName()))
	 {
	  xml.WriteString(string("Caption_")+RDK::sntoa(i+1),AnsiString(I->second->Caption).c_str());
	  xml.WriteString(string("Type_")+RDK::sntoa(i+1),"MultiForm");
	  xml.WriteString(string("Class_")+RDK::sntoa(i+1),I->first);
	  xml.WriteString(string("ComponentControlName_")+RDK::sntoa(i+1),I->second->GetComponentControlName());

	  ++count;
	  is_saved=true;
	  break;
	 }
	}
   }
  }
  if(!is_saved) // Делаем попытку сохранить данные как данные формы
  {
   std::map<std::string, TUVisualControllerForm*>::iterator I=UComponentsListFrame1->ComponentControllers.begin();
   for(;I != UComponentsListFrame1->ComponentControllers.end();++I)
   {
	if(I->second->Parent == PageControl1->Pages[i])
	{
	 xml.WriteString(string("Caption_")+RDK::sntoa(i+1),AnsiString(I->second->Caption).c_str());
	 xml.WriteString(string("Type_")+RDK::sntoa(i+1),"SingleForm");
	 xml.WriteString(string("Class_")+RDK::sntoa(i+1),I->first);
	 xml.WriteString(string("ComponentControlName_")+RDK::sntoa(i+1),I->second->GetComponentControlName());

	 ++count;
	}
   }
  }
 }
 xml.SelectUp();
 xml.WriteInteger("PageCount",count);
}

// Загружает параметры интерфейса из xml
void TUGEngineControlForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 int count=xml.ReadInteger("PageCount",0);

 ClearPages();
 xml.SelectNodeForce("Pages");
 for(int i=1;i<count+1;i++)
 {
  String caption=xml.ReadString(string("Caption_")+RDK::sntoa(i+1),std::string("Untitled")).c_str();
  String type=xml.ReadString(string("Type_")+RDK::sntoa(i+1),std::string("")).c_str();
  String str_class=xml.ReadString(string("Class_")+RDK::sntoa(i+1),std::string("")).c_str();
  string component_name=xml.ReadString(string("ComponentControlName_")+RDK::sntoa(i+1),std::string("")).c_str();

  bool is_loaded=false;
   if(type == "SingleForm" || type == "MultiForm")
   {
	TTabSheet* tab=AddComponentControlFormPage(component_name);
	tab->Caption=caption;
   }
   else
   if(type == "Frame")
    AddSpecialFramePage(str_class,caption);

 }
 xml.SelectUp();
}

// Создает новый проект
void TUGEngineControlForm::CreateProject(const String &FileName, const String &model_comp_name, const String &model_file_name)
{
 CloseProject();
 ProjectXml.Destroy();
 ProjectXml.SelectNodeRoot("Project/General");
 ProjectXml.WriteInteger("PredefinedStructure",PredefinedStructure);
 ProjectXml.WriteInteger("ProjectAutoSaveFlag",ProjectAutoSaveFlag);

 // Число входов среды
 ProjectXml.WriteInteger("NumEnvInputs",NumEnvInputs);

 // Число выходов среды
 ProjectXml.WriteInteger("NumEnvOutputs",NumEnvOutputs);

 ProjectXml.WriteInteger("InputEnvImageWidth",InputEnvImageWidth);
 ProjectXml.WriteInteger("InputEnvImageHeight",InputEnvImageHeight);

 // Шаг счета по умолчанию
 ProjectXml.WriteInteger("DefaultTimeStep",DefaultTimeStep);

 // Глобальный шаг счета модели
 ProjectXml.WriteInteger("GlobalTimeStep",GlobalTimeStep);

 ProjectXml.WriteBool("ReflectionFlag",ReflectionFlag);

 ProjectXml.WriteInteger("CalculationMode",CalculationMode);

 if(PredefinedStructure == 0 && model_file_name.Length()>0)
 {
  ProjectXml.WriteString("ModelFileName",AnsiString(model_file_name).c_str());
 }

 ProjectXml.WriteString("InterfaceFileName","Interface.xml");
 ProjectXml.WriteString("ParametersFileName","Parameters.xml");
 ProjectXml.WriteString("StatesFileName","States.xml");

 ProjectXml.SaveToFile(AnsiString(FileName).c_str());
 OpenProject(FileName);

 if(ProjectOpenFlag)
 {
  if(PredefinedStructure == 0 && model_comp_name.Length()>0)
  {
   Model_Create(AnsiString(model_comp_name).c_str());
  }
 }
}

// Закрывает существущий проект
void TUGEngineControlForm::CloseProject(void)
{
 if(ProjectAutoSaveFlag)
  SaveProject();

 if(ProjectOpenFlag)
 {
  ProjectFileName="";
  ProjectPath="";
 }
 ProjectOpenFlag=false;
 if(GetEngine())
  Env_UnInit();
 EngineUnInit();
 UpdateInterface();
}

// Открывает проект
void TUGEngineControlForm::OpenProject(const String &FileName)
{
 CloseProject();

 ProjectXml.LoadFromFile(AnsiString(FileName).c_str(),"");
 ProjectPath=ExtractFilePath(FileName);
 ProjectFileName=ExtractFileName(FileName);

 ProjectXml.SelectNodeRoot("Project/General");

 ProjectName=ProjectXml.ReadString("ProjectName","NoName").c_str();

 // Число входов среды
 NumEnvInputs=ProjectXml.ReadInteger("NumEnvInputs",1);

 // Число выходов среды
 NumEnvOutputs=ProjectXml.ReadInteger("NumEnvOutputs",1);

 InputEnvImageWidth=ProjectXml.ReadInteger("InputEnvImageWidth",360);
 InputEnvImageHeight=ProjectXml.ReadInteger("InputEnvImageHeight",240);

 PredefinedStructure=ProjectXml.ReadInteger("PredefinedStructure",0);

 // Флаг автоматического сохранения проекта
 ProjectAutoSaveFlag=ProjectXml.ReadInteger("ProjectAutoSaveFlag",1);

 // Флаг автоматического сохранения проекта
 ProjectAutoSaveStateFlag=ProjectXml.ReadInteger("ProjectAutoSaveStateFlag",1);

 // Шаг счета по умолчанию
 DefaultTimeStep=ProjectXml.ReadInteger("DefaultTimeStep",30);

 // Глобальный шаг счета модели
 GlobalTimeStep=ProjectXml.ReadInteger("GlobalTimeStep",30);

 ReflectionFlag=ProjectXml.ReadBool("ReflectionFlag",true);

 CalculationMode=ProjectXml.ReadInteger("CalculationMode",0);

 String descriptionfilename=ProjectXml.ReadString("ProjectDescriptionFileName","").c_str();

 if(descriptionfilename.Length() != 0 && FileExists(descriptionfilename))
 {
  TRichEdit* RichEdit=new TRichEdit(this);
  RichEdit->Parent=this;
  RichEdit->PlainText=true;
  RichEdit->Visible=false;

  if(ExtractFilePath(descriptionfilename).Length() == 0)
   RichEdit->Lines->LoadFromFile(ProjectPath+descriptionfilename);
  else
   RichEdit->Lines->LoadFromFile(descriptionfilename);
  ProjectDescription=RichEdit->Text;
  delete RichEdit;
 }

 String modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();

 //Env_SetPredefinedStructure(PredefinedStructure);

 GraphicalEngineInit(PredefinedStructure,NumEnvInputs,NumEnvOutputs,InputEnvImageWidth, InputEnvImageHeight ,ReflectionFlag,ExceptionHandler);
 Model_SetDefaultTimeStep(DefaultTimeStep);
 Env_SetCurrentDataDir(AnsiString(ProjectPath).c_str());
 #ifdef RDK_VIDEO
 for(int i=0;i<NumEnvInputs;i++)
  VideoOutputForm->AddSource();
 #endif

 if(PredefinedStructure == 0 && modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
 }

 String paramsfilename=ProjectXml.ReadString("ParametersFileName","").c_str();

 if(paramsfilename.Length() != 0)
 {
  if(ExtractFilePath(paramsfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(ProjectPath+paramsfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(paramsfilename);
 }

 if(ProjectAutoSaveStateFlag)
 {
  String statesfilename=ProjectXml.ReadString("StatesFileName","").c_str();

  if(statesfilename.Length() != 0)
  {
   if(ExtractFilePath(statesfilename).Length() == 0)
	UComponentsControlForm->ComponentsControlFrame->LoadStatesFromFile(ProjectPath+statesfilename);
   else
	UComponentsControlForm->ComponentsControlFrame->LoadStatesFromFile(statesfilename);
  }
 }
 if(Model_Check())
 {
  Model_SetGlobalTimeStep("",GlobalTimeStep);
 }

 InterfaceXml.Destroy();
 String interfacefilename=ProjectXml.ReadString("InterfaceFileName","").c_str();
 if(interfacefilename.Length() != 0)
 {
  if(ExtractFilePath(interfacefilename.c_str()).Length() == 0)
   InterfaceXml.LoadFromFile(AnsiString(ProjectPath+interfacefilename).c_str(),"Interfaces");
  else
   InterfaceXml.LoadFromFile(AnsiString(interfacefilename).c_str(),"Interfaces");

  InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 }
 RDK::UIVisualControllerStorage::LoadParameters(InterfaceXml);

 UEngineMonitorForm->EngineMonitorFrame->SetCalculateMode(CalculationMode);
 RDK::UIVisualControllerStorage::UpdateInterface();
 ProjectOpenFlag=true;
}

// Сохраняет проект
void TUGEngineControlForm::SaveProject(void)
{
 if(!ProjectOpenFlag)
  return;

 InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 RDK::UIVisualControllerStorage::SaveParameters(InterfaceXml);

 String interfacefilename=ProjectXml.ReadString("InterfaceFileName","").c_str();
 if(interfacefilename.Length() != 0)
 {
  if(ExtractFilePath(interfacefilename).Length() == 0)
   InterfaceXml.SaveToFile(AnsiString(ProjectPath+interfacefilename).c_str());
  else
   InterfaceXml.SaveToFile(AnsiString(interfacefilename).c_str());
 }
 else
 {
  ProjectXml.WriteString("InterfaceFileName","Interface.xml");
  InterfaceXml.SaveToFile(AnsiString(ProjectPath+interfacefilename).c_str());
 }

 ProjectXml.SelectNodeRoot("Project/General");

 String descriptionfilename=ProjectXml.ReadString("ProjectDescriptionFileName","").c_str();
 if(descriptionfilename.Length() == 0)
 {
  descriptionfilename="Description.rtf";
  ProjectXml.WriteString("ProjectDescriptionFileName",AnsiString(descriptionfilename).c_str());
 }

 if(descriptionfilename.Length() != 0)
 {
  TRichEdit* RichEdit=new TRichEdit(this);
  RichEdit->Parent=this;
  RichEdit->PlainText=true;
  RichEdit->Visible=false;
  RichEdit->Text=ProjectDescription;

  if(ExtractFilePath(descriptionfilename).Length() == 0)
   RichEdit->Lines->SaveToFile(ProjectPath+descriptionfilename);
  else
   RichEdit->Lines->SaveToFile(descriptionfilename);
  delete RichEdit;
 }

 String modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();
 if(modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(modelfilename);
 }
 else
 {
  ProjectXml.WriteString("ModelFileName","model.xml");
  UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(ProjectPath+"model.xml");
 }

 String paramsfilename=ProjectXml.ReadString("ParametersFileName","").c_str();
 if(paramsfilename.Length() == 0)
 {
  paramsfilename="Parameters.xml";
  ProjectXml.WriteString("ParametersFileName",AnsiString(paramsfilename).c_str());
 }

 if(paramsfilename.Length() != 0)
 {
  if(ExtractFilePath(paramsfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->SaveParametersToFile(ProjectPath+paramsfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->SaveParametersToFile(paramsfilename);
 }

 if(ProjectAutoSaveStateFlag)
 {
  String statesfilename=ProjectXml.ReadString("StatesFileName","").c_str();
  if(statesfilename.Length() == 0)
  {
   statesfilename="States.xml";
   ProjectXml.WriteString("StatesFileName",AnsiString(statesfilename).c_str());
  }

  if(statesfilename.Length() != 0)
  {
   if(ExtractFilePath(statesfilename).Length() == 0)
	UComponentsControlForm->ComponentsControlFrame->SaveStatesToFile(ProjectPath+statesfilename);
   else
	UComponentsControlForm->ComponentsControlFrame->SaveStatesToFile(statesfilename);
  }
 }

 ProjectXml.WriteInteger("PredefinedStructure",PredefinedStructure);
 ProjectXml.WriteInteger("ProjectAutoSaveFlag",ProjectAutoSaveFlag);

 // Флаг автоматического сохранения проекта
 ProjectXml.WriteInteger("ProjectAutoSaveStateFlag",ProjectAutoSaveStateFlag);

 // Число входов среды
 ProjectXml.WriteInteger("NumEnvInputs",NumEnvInputs);

 // Число выходов среды
 ProjectXml.WriteInteger("NumEnvOutputs",NumEnvOutputs);

 ProjectXml.WriteInteger("InputEnvImageWidth",InputEnvImageWidth);
 ProjectXml.WriteInteger("InputEnvImageHeight",InputEnvImageHeight);

 // Шаг счета по умолчанию
 ProjectXml.WriteInteger("DefaultTimeStep",DefaultTimeStep);

 // Глобальный шаг счета модели
 ProjectXml.WriteInteger("GlobalTimeStep",GlobalTimeStep);

 ProjectXml.WriteBool("ReflectionFlag",ReflectionFlag);

 ProjectXml.WriteInteger("CalculationMode",CalculationMode);

 ProjectXml.WriteString("ProjectName",AnsiString(ProjectName).c_str());

 ProjectXml.SaveToFile(AnsiString(ProjectPath+ProjectFileName).c_str());
}


// Добавляет заданный фрейм типа TUImagesFrame, TUWatchFrame и т.п. на заданную вкладку
bool TUGEngineControlForm::AddSpecialFrameToPage(TUVisualControllerFrame *frame, TTabSheet* tab, const String &caption)
{
 if(!frame|| !tab)
  return false;

 int count=1;
 for(int j=0;j<ComponentCount;j++)
 {
  TUVisualControllerFrame *checking_frame=dynamic_cast<TUVisualControllerFrame*>(Components[j]);
  if(checking_frame && checking_frame->ClassNameIs(frame->ClassName()))
  {
   ++count;
  }
 }

 frame->Name=frame->Name+String("_")+count;
 InsertComponent(frame);
 frame->Parent=tab;
 frame->Align=alClient;

 std::string str=AnsiString(caption).c_str();

 if(count>1 && str.find_last_of("_") == string::npos)
  tab->Caption=caption+String("_")+count;
 else
  tab->Caption=caption;

 return true;
}

// Добавляет страницу
TTabSheet* TUGEngineControlForm::AddSpecialFramePage(const String &type, const String &caption)
{
 TTabSheet* tab=new TTabSheet(PageControl1);
 tab->PageControl=PageControl1;
 TUVisualControllerFrame *frame=0;

 if(type == "TUImagesFrame")
 {
  frame=new TUImagesFrame(0);
 }
 else
 if(type == "TUWatchFrame")
 {
  frame=new TUWatchFrame(0);
 }
 else
 {
  tab->Caption=caption;
  return tab;
 }

 AddSpecialFrameToPage(frame, tab, caption);
 return tab;
}

// Создает новую вкладку с заданным именем для формы управления компонентом
// Если этим копонентом уже управляют, то возвращает указатель на такую вкладку
TTabSheet* TUGEngineControlForm::AddComponentControlFormPage(const string &component_name)
{
 int control_index=FindComponentControlPage(component_name);

 if(control_index>=0)
  return PageControl1->Pages[control_index];

 std::string name=Model_GetComponentClassName(component_name.c_str());

 std::map<std::string, TUVisualControllerForm*>::iterator I=UComponentsListFrame1->ComponentControllers.find(name);
 if(I != UComponentsListFrame1->ComponentControllers.end() && I->second)
 {
  TTabSheet* tab=new TTabSheet(PageControl1);
  tab->PageControl=PageControl1;
  if(!tab)
   return 0;

  TUVisualControllerForm *form=I->second->New(tab);
  if(!form)
   form=I->second;
  else
   tab->InsertComponent(form);
  form->SetComponentControlName(component_name);
  form->Parent=tab;
  form->Align=alClient;
  form->BorderStyle=bsNone;
  form->Show();
  form->UpdateInterface(true);
  tab->Caption=form->Caption;
  return tab;
 }
 return 0;
}


/// Ищет и возвращает указатель на форму или фрейм, соответствующий вкладке с заданным индексом
void TUGEngineControlForm::FindVisualController(int index, TUVisualControllerFrame* &frame, TUVisualControllerForm* &form)
{
 frame=0;
 form=0;

 if(index <0 || index>=PageControl1->PageCount)
  return;

 int i=index;
  bool is_saved=false;
  for(int j=0;j<ComponentCount;j++)
  {
   frame=dynamic_cast<TUVisualControllerFrame*>(Components[j]);
   if(frame && frame->Parent == PageControl1->Pages[i])
   {
	return;
   }
  }

 frame=0;
  if(PageControl1->Pages[i]->ComponentCount>0)
  {
   form=dynamic_cast<TUVisualControllerForm*>(PageControl1->Pages[i]->Components[0]);
   if(form)
   {
	std::map<std::string, TUVisualControllerForm*>::iterator I=UComponentsListFrame1->ComponentControllers.begin();
	for(;I != UComponentsListFrame1->ComponentControllers.end();++I)
	{
	 if(I->second->ClassNameIs(form->ClassName()))
	 {
	  return;
	 }
	}
   }
  }

  if(!is_saved) // Делаем попытку сохранить данные как данные формы
  {
   std::map<std::string, TUVisualControllerForm*>::iterator I=UComponentsListFrame1->ComponentControllers.begin();
   for(;I != UComponentsListFrame1->ComponentControllers.end();++I)
   {
	if(I->second->Parent == PageControl1->Pages[i])
	{
	 return;
	}
   }
  }

 form=0;
}



// Удаляет страницу
void TUGEngineControlForm::DelPage(int index)
{
 if(index < PageControl1->PageCount && index >= 1)
 {
  std::map<std::string, TUVisualControllerForm*>::iterator I=UComponentsListFrame1->ComponentControllers.begin();
  for(;I != UComponentsListFrame1->ComponentControllers.end();++I)
  {
   TUVisualControllerForm *form=dynamic_cast<TUVisualControllerForm*>(I->second);
   if(form && form->Parent == PageControl1->Pages[index])
   {
	form->BorderStyle=bsSizeable;
	form->Align=alNone;
	form->Hide();
	form->Parent=0;
	break;
   }
  }
  delete PageControl1->Pages[index];
 }

}

// Переименовывает заголовок страницы
void TUGEngineControlForm::RenamePage(int index, String new_name)
{
 if(index < PageControl1->PageCount && index >= 1)
  PageControl1->Pages[index]->Caption=new_name;
}

// Удаляет все лишние вкладки (оставляет 2 начальные)
void TUGEngineControlForm::ClearPages(void)
{
 while(PageControl1->PageCount > 1)
  DelPage(PageControl1->PageCount-1);
}

// Ищет, существует ли уже вкладка, управляющая компонентом с заданным именем
// Возвращает индекс вкладки или -1, если вкладка не найдена
int TUGEngineControlForm::FindComponentControlPage(const std::string &component_long_name)
{
 for(int i=1;i<PageControl1->PageCount;i++)
 {
  if(PageControl1->Pages[i]->ComponentCount != 0)
  {
   TUVisualControllerForm *form=dynamic_cast<TUVisualControllerForm*>(PageControl1->Pages[i]->Components[0]);
   if(form && form->GetComponentControlName() == component_long_name)
	return i;
  }
 }

 for(int i=1;i<PageControl1->PageCount;i++)
 {
  std::map<std::string, TUVisualControllerForm*>::iterator I=UComponentsListFrame1->ComponentControllers.begin();
  for(;I != UComponentsListFrame1->ComponentControllers.end();++I)
  {
   TUVisualControllerForm *form=dynamic_cast<TUVisualControllerForm*>(I->second);
   if(form && form->Parent == PageControl1->Pages[i] && form->GetComponentControlName() == component_long_name)
	return i;
  }
 }


 return -1;
}


//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Start1Click(TObject *Sender)
{
#ifdef RDK_VIDEO
 VideoOutputForm->Start();
#endif
 UEngineMonitorForm->EngineMonitorFrame->Start1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Pause1Click(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Pause1Click(Sender);
#ifdef RDK_VIDEO
 VideoOutputForm->StopOffline();
#endif
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::EngineMonitor1Click(TObject *Sender)
{
 UEngineMonitorForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::Images1Click(TObject *Sender)
{
 UImagesForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::VideoSource1Click(TObject *Sender)
{
#ifdef RDK_VIDEO
 VideoOutputForm->Show();
#endif
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::Step1Click(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Step1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::ComponentsControl1Click(TObject *Sender)
{
 UComponentsControlForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::ComponentsLinks1Click(TObject *Sender)
{
 UComponentLinksForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::LoadModel1Click(TObject *Sender)
{
 UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile("");
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::SaveModel1Click(TObject *Sender)
{
 UComponentsControlForm->ComponentsControlFrame->SaveModelToFile("");
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::OpenImage1Click(TObject *Sender)
{
#ifdef RDK_VIDEO
 if(!VideoOutputForm->GetActiveVideoOutputFrame())
  return;

 VideoOutputForm->GetActiveVideoOutputFrame()->MyVideoGrabberControlForm->VideoGrabberControlFrame->OpenImageFileButtonClick(Sender);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::OpenVideo1Click(TObject *Sender)
{
#ifdef RDK_VIDEO
 if(!VideoOutputForm->GetActiveVideoOutputFrame())
  return;

 VideoOutputForm->GetActiveVideoOutputFrame()->MyVideoGrabberControlForm->VideoGrabberControlFrame->VFBrowseButtonClick(Sender);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::CaptureVideo1Click(TObject *Sender)
{
#ifdef RDK_VIDEO
 if(!VideoOutputForm->GetActiveVideoOutputFrame())
  return;

 VideoOutputForm->GetActiveVideoOutputFrame()->MyVideoGrabberControlForm->VideoGrabberControlFrame->SelectMode(0);
 VideoOutputForm->GetActiveVideoOutputFrame()->MyVideoGrabberControlForm->Show();
#endif
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Reset1Click(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Reset1Click(Sender);
#ifdef RDK_VIDEO
 VideoOutputForm->StopOffline();
#endif
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Performance1Click(TObject *Sender)
{
 UComponentsPerformanceForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::LoadProjectItemClick(TObject *Sender)
{
 if(!OpenDialog->Execute())
  return;

 OpenProject(OpenDialog->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::SaveProjectItemClick(TObject *Sender)
{
 SaveProject();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::CreateProjectItemClick(TObject *Sender)
{
 UCreateProjectWizardForm->ClearWizard();
 UCreateProjectWizardForm->Caption="Create Project Wizard";
 if(UCreateProjectWizardForm->ShowModal() == mrOk)
 {
  CloseProject();
  PredefinedStructure=UCreateProjectWizardForm->PredefinedStructure;
  ProjectAutoSaveFlag=UCreateProjectWizardForm->ProjectAutoSaveFlagCheckBox->Checked;
  DefaultTimeStep=StrToInt(UCreateProjectWizardForm->ProjectTimeStepEdit->Text);
  GlobalTimeStep=DefaultTimeStep;

  NumEnvInputs=StrToInt(UCreateProjectWizardForm->NumInputsLabeledEdit->Text);
  NumEnvOutputs=StrToInt(UCreateProjectWizardForm->NumOutputsLabeledEdit->Text);
  InputEnvImageWidth=StrToInt(UCreateProjectWizardForm->ImageWidthLabeledEdit->Text);
  InputEnvImageHeight=StrToInt(UCreateProjectWizardForm->ImageHeightLabeledEdit->Text);
  ReflectionFlag=UCreateProjectWizardForm->UpendInputImageCheckBox->Checked;
  CalculationMode=UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex;

  CreateProject(UCreateProjectWizardForm->ProjectDirectoryLabeledEdit->Text+String("\\Project.ini"),UCreateProjectWizardForm->UClassesListFrame1->GetSelectedName(),UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text);

  ProjectName=UCreateProjectWizardForm->ProjectNameLabeledEdit->Text;
  ProjectDescription=UCreateProjectWizardForm->ProjectDescriptionRichEdit->Text;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::CreateModelClick(TObject *Sender)
{
 if(UClassesListForm->ShowModal() != mrOk)
  return;

 Model_Destroy();
 Model_Create(AnsiString(UClassesListForm->ClassesListFrame->GetSelectedName()).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::FavoriteInformation1Click(TObject *Sender)
{
 UFavoriteComponentInfoForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::DrawEngine1Click(TObject *Sender)
{
 UDrawEngineForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::ReloadParameters1Click(TObject *Sender)
{
 String paramsfilename=ProjectXml.ReadString("ParametersFileName","").c_str();
 if(paramsfilename.Length() == 0)
 {
  paramsfilename="Parameters.xml";
  ProjectXml.WriteString("ParametersFileName",AnsiString(paramsfilename).c_str());
 }

 if(paramsfilename.Length() != 0)
 {
  if(ExtractFilePath(paramsfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(ProjectPath+paramsfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(paramsfilename);
 }
}
//---------------------------------------------------------------------------



void __fastcall TUGEngineControlForm::CopyProject1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 const SELDIRHELP = 1000;
 String chosenDir=ExtractFilePath(Application->ExeName);

 if(SelectDirectory("Select project directory", ExtractFilePath(Application->ExeName), chosenDir,TSelectDirExtOpts() << sdNewFolder << sdNewUI))
 {
  SaveProject();
  RDK::CopyDir(AnsiString(ProjectPath).c_str(), AnsiString(chosenDir+"\\").c_str(), "*.*");
 }
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::ProjectOptions1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 UCreateProjectWizardForm->ProjectDirectoryLabeledEdit->Text=ProjectPath;
 UCreateProjectWizardForm->ProjectNameLabeledEdit->Text=ProjectName;
 UCreateProjectWizardForm->ProjectDescriptionRichEdit->Text=ProjectDescription;
 UCreateProjectWizardForm->ProjectTypeRadioGroup->ItemIndex=1;
 UCreateProjectWizardForm->ProjectAutoSaveFlagCheckBox->Checked=ProjectAutoSaveFlag;
 UCreateProjectWizardForm->ProjectTimeStepEdit->Text=IntToStr(DefaultTimeStep);
 UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex=CalculationMode;

 UCreateProjectWizardForm->PredefinedStructure=PredefinedStructure;
 if(PredefinedStructure)
 {
  UCreateProjectWizardForm->PredefinedModelRadioButton->Checked=true;
  UCreateProjectWizardForm->ModelFileNameRadioButton->Checked=false;
 }
 else
 {
  UCreateProjectWizardForm->PredefinedModelRadioButton->Checked=false;
  UCreateProjectWizardForm->ModelFileNameRadioButton->Checked=true;
  UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text=ProjectXml.ReadString("ModelFileName","").c_str();
 }

  UCreateProjectWizardForm->NumInputsLabeledEdit->Text=IntToStr(NumEnvInputs);
  UCreateProjectWizardForm->NumOutputsLabeledEdit->Text=IntToStr(NumEnvOutputs);
  UCreateProjectWizardForm->ImageWidthLabeledEdit->Text=IntToStr(InputEnvImageWidth);
  UCreateProjectWizardForm->ImageHeightLabeledEdit->Text=IntToStr(InputEnvImageHeight);
  UCreateProjectWizardForm->UpendInputImageCheckBox->Checked=ReflectionFlag;

 UCreateProjectWizardForm->Caption="Update Project Wizard";
 if(UCreateProjectWizardForm->ShowModal() == mrOk)
 {
  CloseProject();
  PredefinedStructure=UCreateProjectWizardForm->PredefinedStructure;
  ProjectAutoSaveFlag=UCreateProjectWizardForm->ProjectAutoSaveFlagCheckBox->Checked;
  DefaultTimeStep=StrToInt(UCreateProjectWizardForm->ProjectTimeStepEdit->Text);
  GlobalTimeStep=DefaultTimeStep;
  CalculationMode=UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex;

  CreateProject(UCreateProjectWizardForm->ProjectDirectoryLabeledEdit->Text+String("\\Project.ini"),UCreateProjectWizardForm->UClassesListFrame1->GetSelectedName(),UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text);

  ProjectName=UCreateProjectWizardForm->ProjectNameLabeledEdit->Text;
  ProjectDescription=UCreateProjectWizardForm->ProjectDescriptionRichEdit->Text;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::WatchWindow1Click(TObject *Sender)
{
 UWatchForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::FormCreate(TObject *Sender)
{
 DecimalSeparator = '.';

 // Грузим настройки приложения
 TMemIniFile *app_ini=new TMemIniFile("options.ini");
 MainFormName=app_ini->ReadString("General", "MainFormName", "");
 HideAdminFormFlag=app_ini->ReadBool("General", "HideAdminForm", false);
 AutoexecProjectFileName=app_ini->ReadString("General", "AutoexecProjectFileName", "");
 AutoStartProjectFlag=app_ini->ReadBool("General", "AutoStartProjectFlag", false);

 // Грузим шрифты
 std::vector<std::string> font_names;
 std::string font_path=AnsiString(ExtractFilePath(Application->ExeName)+"Fonts\\").c_str();
 RDK::FindFilesList(font_path, "*.fnt", true, font_names);

 RDK::ClearClobalFonts();
 RDK::UBitmapFont font;
 for(size_t i=0;i<font_names.size();i++)
 {
  RDK::AddGlobalFont(font_path+font_names[i]);
 }

 GraphicalEngineInit(0,1,1,320,240,1,ExceptionHandler);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::HideTimerTimer(TObject *Sender)
{
 HideTimer->Enabled=false;
 if(HideAdminFormFlag)
 {
  HideAdminFormFlag=false;
  Hide();
 }

 if(FileExists(AutoexecProjectFileName))
 {
  OpenProject(AutoexecProjectFileName);
  AutoexecProjectFileName="";
 }

 if(AutoStartProjectFlag)
 {
  AutoStartProjectFlag=false;
  Start1Click(Sender);
 }
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::FormCloseQuery(TObject *Sender, bool &CanClose)

{
 CloseProject();
 CanClose=true;
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Images2Click(TObject *Sender)
{
 AddSpecialFramePage("TUImagesFrame", "Images");
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Watches1Click(TObject *Sender)
{
 AddSpecialFramePage("TUWatchFrame", "Watches");
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::DeletePage1Click(TObject *Sender)
{
 if(PageControl1->ActivePageIndex != 0)
  DelPage(PageControl1->ActivePageIndex);
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::Images3Click(TObject *Sender)
{
 Images2Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Watches2Click(TObject *Sender)
{
 Watches1Click(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::UComponentsListFrame1GUI1Click(TObject *Sender)

{
 TTabSheet* tab=AddComponentControlFormPage(UComponentsListFrame1->GetSelectedComponentLongName());
 if(tab)
  tab->PageControl->ActivePageIndex=tab->TabIndex;
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::UDrawEngineFrame1GUI1Click(TObject *Sender)

{
 UComponentsListFrame1GUI1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::PageControl1Change(TObject *Sender)
{
 TUVisualControllerFrame* frame;
 TUVisualControllerForm* form;
 FindVisualController(PageControl1->ActivePageIndex, frame, form);
 if(frame)
  frame->UpdateInterface(true);
 if(form)
  form->UpdateInterface(true);
}
//---------------------------------------------------------------------------


