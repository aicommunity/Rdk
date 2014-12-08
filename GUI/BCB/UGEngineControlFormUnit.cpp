//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include <Vcl.FileCtrl.hpp>
#include <System.IOUtils.hpp>
#include "UGEngineControlFormUnit.h"
//#ifdef RDK_VIDEO
//#include "TVideoGrabberControlFormUnit.h"
//#endif
#include "UImagesFormUnit.h"
#include "UComponentsControlFormUnit.h"
#include "UComponentLinksFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#ifdef RDK_VIDEO
#include "VideoOutputFormUnit.h"
#include "TVideoRegistratorFormUnit.h"
#include "TVideoRegistratorFormUnit.h"
#endif
#include "UClassesListFormUnit.h"
#include "UComponentsPerformanceFormUnit.h"
#include "UFavoriteComponentInfoFormUnit.h"
//#include "UDrawEngineFormUnit.h"
#include "UCreateProjectWizardFormUnit.h"
#include "UWatchFormUnit.h"
#include "TIdHttpResultBroadcasterFormUnit.h"
#include "UServerControlFormUnit.h"
#include "UShowProgressBarUnit.h"
#include "TLoaderFormUnit.h"
#include "TApplicationOptionsFormUnit.h"
#include "UClassesDescriptionsFormUnit.h"

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

/// Экзепляр прототипа декодера команд
RDK::URpcDecoderInternal RdkRpcDecoder;

/// Экзепляр класса диспетчера команд
RDK::URpcDispatcher RdkRpcDispatcher;

/// Экзепляр класса пр	иложения
RDK::UApplication RdkApplication;

/// Глобальная переменная сигнализирующая о завершении инициализации приложения
bool ApplicationInitialized=false;

/// Режим работы визарда нового проекта
int CreateWizardMode=0;


HANDLE RdkLockStartapMutex;

String Lang_SaveProjectTitle("Saving Project...");
String Lang_LoadProjectTitle("Loading Project...");
String Lang_LoadingData("Loading data...");
String Lang_SavingData("Saving data...");
String Lang_Total("Total");
String Lang_SaveInterface("Saving Interface...");
String Lang_UpdateInterface("Update Interface...");
String Lang_Starting("Starting...");
String Lang_Stopping("Stopping...");

TUVisualControllerForm *RdkMainForm=0;

bool RdkIsApplicationRunning(void)
{
 std::string mutex_name="Global\\";
 std::string mutex_subname=AnsiString(Application->ExeName).c_str();
 for(size_t i=0;i<mutex_subname.size();i++)
 if(mutex_subname[i] == '\\')
  mutex_subname[i]='/';
 mutex_name+=mutex_subname;
 RdkLockStartapMutex=CreateMutex(0, false, mutex_name.c_str());
 int last_err=GetLastError();
 if(last_err == ERROR_ALREADY_EXISTS || last_err == ERROR_ACCESS_DENIED)
  return true;
 return false;
}

//---------------------------------------------------------------------------
__fastcall TUGEngineControlForm::TUGEngineControlForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 ProjectAutoSaveFlag=true;

 // Признак наличия открытого проекта
 ProjectOpenFlag=false;

 AppWinState=true;

 ProjectAutoSaveStateFlag=false;

 DisableAdminForm=false;

 EventsLogEnabled=true;
}

void __fastcall TUGEngineControlForm::WMSysCommand(TMessage &Msg)
{
unsigned int sys_code = Msg.WParam & 0xFFF0;
switch (sys_code)
{
 case SC_MINIMIZE:
 {
  if(RdkMainForm == this)
  {
   AppMinimize(this);
   Application->Minimize();
  }
  else
   WindowState=wsMinimized;
  Msg.Result = 0;
  return;
 break;
 }
			  /*
 case SC_MAXIMIZE:
 {
  // maximize command requested
  // TO TRAP: comment out the following two lines,
  // and remove the break line.
  // Application->Maximize();
  Msg.Result = 0;
  return;
  break;
 }              */

 case SC_RESTORE:
 {
//  if(WindowState != wsMinimized)
  {
   Application->Restore();
   WindowState=wsNormal;
  }
//  else
//   Application->Restore();
  AppRestore(this);
  Msg.Result = 0;
  return;
  break;
 }

 case SC_CLOSE:
 {
  if(RdkMainForm == this)
  {
   Close1Click(this);
  }
  else
   Hide();
  Msg.Result = 0;
  return;
  break;
 }
}
 TForm::Dispatch(&Msg);
}

void __fastcall TUGEngineControlForm::WMServerPing(TMessage &Msg)
{
/* static int ping_msg_index=0;
 std::string ping_msg="Ping Received from HWND=";
 ping_msg+=AnsiString(IntToHex(int(Msg.WParam),8)).c_str();
 ping_msg+=" Send message id=";
 ping_msg+=RDK::sntoa(WM_SERVER_PONG);
 ping_msg+=" Count=";
 ping_msg+=RDK::sntoa(++ping_msg_index);
 Engine_LogMessage(RDK_EX_INFO,ping_msg.c_str());*/
 PostMessage((HWND)Msg.WParam, WM_SERVER_PONG, 0, 0);
}


// Загружает данные положения формы из xml
void TUGEngineControlForm::LoadFormPosition(RDK::USerStorageXML &xml)
{
 xml.SelectNodeForce("FormPosition");
 int value=0;
 value=xml.ReadInteger("Left",Left);
 if(value<Screen->DesktopLeft || value>= Screen->DesktopLeft+Screen->DesktopWidth)
  value=Screen->DesktopLeft;
 Left=value;

 value=xml.ReadInteger("Top",Top);
 if(value<Screen->DesktopTop || value>= Screen->DesktopTop+Screen->DesktopHeight)
  value=Screen->DesktopTop;
 Top=value;

 Width=xml.ReadInteger("Width",Width);
 Height=xml.ReadInteger("Height",Height);
// Visible=xml.ReadBool("Visible",Visible);
// WindowState=xml.ReadInteger("WindowState",(int)WindowState);

 xml.SelectUp();
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

 Caption=ProgramName;
 if(ProjectOpenFlag)
 {
  Caption=Caption+String(" [")+ProjectName+": "+ProjectPath+ProjectFileName+"]";
 }
 StatusBar->SimpleText=UEngineMonitorForm->EngineMonitorFrame->StatusBar->SimpleText;
 StatusBar->Repaint();
 StatusBar->Update();

 if(GetNumEngines()<=1)
  ChannelsStringGrid->Visible=false;
 else
 {
  ChannelsStringGrid->RowCount=GetNumEngines();
  const TEngineMonitorThread * monitor=UEngineMonitorForm->EngineMonitorFrame->GetEngineMonitorThread();
  std::vector<int> CalcThreadStates;
  std::vector<int> VideoCaptureStates;

  if(monitor)
  {
   CalcThreadStates=monitor->ReadCalcThreadStates();
   VideoCaptureStates=monitor->ReadVideoCaptureStates();
  }

  if(ProjectShowChannelsStates)
  {
   ChannelsStringGrid->ColCount=3;
   ChannelsStringGrid->ColWidths[1]=20;
   ChannelsStringGrid->ColWidths[2]=20;
   ChannelsStringGrid->Width=ChannelsStringGrid->ColWidths[0]+
						ChannelsStringGrid->ColWidths[1]+
						ChannelsStringGrid->ColWidths[2]+20;
   for(int i=0;i<ChannelsStringGrid->RowCount;i++)
   {
	ChannelsStringGrid->Cells[0][i]=IntToStr(i);
	if(int(CalcThreadStates.size())>i)
	 ChannelsStringGrid->Cells[1][i]=CalcThreadStates[i];
	else
	 ChannelsStringGrid->Cells[1][i]="n/a";

	if(int(VideoCaptureStates.size())>i)
	 ChannelsStringGrid->Cells[2][i]=VideoCaptureStates[i];
	else
	 ChannelsStringGrid->Cells[2][i]="n/a";
   }
  }
  else
  {
   ChannelsStringGrid->ColCount=1;
   ChannelsStringGrid->Width=ChannelsStringGrid->ColWidths[0]+20;
   for(int i=0;i<ChannelsStringGrid->RowCount;i++)
   {
	ChannelsStringGrid->Cells[0][i]=IntToStr(i);
   }
  }

  ChannelsStringGrid->Row=GetSelectedEngineIndex();

  ChannelsStringGrid->Visible=true;
 }


}


// Возврат интерфейса в исходное состояние
void TUGEngineControlForm::AClearInterface(void)
{

}

void __fastcall TUGEngineControlForm::FormShow(TObject *Sender)
{
// HideTimer->Enabled=true;

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
	  xml.WriteString(string("Caption_")+RDK::sntoa(i+1),AnsiString(form->Caption).c_str());
	  xml.WriteString(string("Type_")+RDK::sntoa(i+1),"MultiForm");
	  xml.WriteString(string("Class_")+RDK::sntoa(i+1),I->first);
	  xml.WriteString(string("ComponentControlName_")+RDK::sntoa(i+1),form->GetComponentControlName());

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
	 is_saved=true;
	 break;
	}
   }
  }
  if(!is_saved) // Делаем попытку сохранить данные как данные специальной формы
  {
   std::map<std::string, TUVisualControllerForm*>::iterator I=SpecialForms.begin();
   for(;I != SpecialForms.end();++I)
   {
	if(I->second->Parent == PageControl1->Pages[i])
	{
	 xml.WriteString(string("Caption_")+RDK::sntoa(i+1),AnsiString(I->second->Caption).c_str());
	 xml.WriteString(string("Type_")+RDK::sntoa(i+1),"SpecialForm");
	 xml.WriteString(string("Class_")+RDK::sntoa(i+1),I->first);

	 ++count;
	 is_saved=true;
	 break;
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

//  bool is_loaded=false;
   if(type == "SingleForm" || type == "MultiForm")
   {
	TTabSheet* tab=AddComponentControlFormPage(component_name);
	tab->Caption=caption;
   }
   else
   if(type == "Frame")
	AddSpecialFramePage(str_class,caption);
   else
   if(type == "SpecialForm")
    AddSpecialFormPage(str_class);

 }
 xml.SelectUp();
}

// Создает новый проект
void TUGEngineControlForm::CreateProject(TProjectConfig &project_config)
{
 CloseProject();

 std::string FileName=project_config.ProjectDirectory+"\\Project.ini";

 ProjectXml.Destroy();

 ProjectXml.SelectNodeRoot("Project/MultiGeneral");
 ProjectXml.WriteInteger("EnginesMode",project_config.MultiThreadingMode);

 ProjectXml.WriteInteger("CalculationTimeSourceMode",project_config.CalcSourceTimeMode);

 ProjectXml.WriteBool("EventsLogEnabled",project_config.EventsLogFlag);
 ProjectXml.WriteBool("ProjectShowChannelsStates",project_config.ShowChannelsStateFlag);

 ProjectXml.WriteInteger("NumEngines",project_config.NumChannels);

 ProjectXml.WriteInteger("SelectedEngineIndex",0);

 ProjectXml.SelectNodeRoot("Project/General");
 // Число входов среды
 ProjectXml.WriteInteger("NumEnvInputs",1);

 // Число выходов среды
 ProjectXml.WriteInteger("NumEnvOutputs",1);

 ProjectXml.WriteInteger("InputEnvImageWidth",640);
 ProjectXml.WriteInteger("InputEnvImageHeight",480);

 ProjectXml.WriteString("ProjectDescriptionFileName","Description.rtf");

 ProjectXml.WriteString("ProjectName",project_config.ProjectName);

 ProjectXml.WriteInteger("ProjectAutoSaveFlag",project_config.ProjectAutoSaveFlag);
 ProjectXml.WriteInteger("ProjectAutoSaveStateFlag",project_config.ProjectAutoSaveStatesFlag);

 ProjectXml.WriteInteger("ProjectMode",project_config.ProjectMode);

 ProjectXml.WriteBool(std::string("ReflectionFlag"),project_config.ReflectionFlag);


 UEngineMonitorForm->EngineMonitorFrame->SetNumChannels(project_config.NumChannels);
 try
 {
  TRichEdit* RichEdit=new TRichEdit(this);
  RichEdit->Parent=this;
  RichEdit->PlainText=true;
  RichEdit->Visible=false;
  RichEdit->Text=project_config.ProjectDescription.c_str();

  RichEdit->Lines->SaveToFile((project_config.ProjectDirectory+string("\\Description.rtf")).c_str());
  delete RichEdit;
 }
 catch(Exception &exception)
 {
  MEngine_LogMessage(GetSelectedEngineIndex(), RDK_EX_ERROR, (std::string("Save model Fail: ")+AnsiString(exception.Message).c_str()).c_str());
 }

 for(size_t i=0;i<project_config.NumChannels;i++)
 {
  TProjectChannelConfig &channel=project_config.ChannelsConfig[i];

  std::string suffix;
  if(i>0)
   suffix=std::string("_")+sntoa(i);

  ProjectXml.WriteInteger(std::string("PredefinedStructure")+suffix,channel.PredefinedStructure);

  // Шаг счета по умолчанию
  ProjectXml.WriteInteger(std::string("DefaultTimeStep")+suffix,channel.DefaultTimeStep);

  // Глобальный шаг счета модели
  ProjectXml.WriteInteger(std::string("GlobalTimeStep")+suffix,channel.GlobalTimeStep);

  ProjectXml.WriteInteger(std::string("CalculationMode")+suffix,channel.CalculationMode);

  ProjectXml.WriteBool(std::string("InitAfterLoadFlag")+suffix,channel.InitAfterLoad);
  ProjectXml.WriteBool(std::string("ResetAfterLoadFlag")+suffix,channel.ResetAfterLoad);
  ProjectXml.WriteBool(std::string("DebugModeFlag")+suffix,channel.DebugMode);

  string model_file_name=string("Model")+suffix+".xml";
  if(!channel.ModelFileName.empty())
   model_file_name=channel.ModelFileName;
  ProjectXml.WriteString(std::string("ModelFileName")+suffix,model_file_name);

  if(!MIsEngineInit(i))
   MGraphicalEngineInit(i,channel.PredefinedStructure,1,1,640, 480 ,project_config.ReflectionFlag,ExceptionHandler);
  else
   MEnv_SetPredefinedStructure(i,channel.PredefinedStructure);

  if(channel.PredefinedStructure == 0 && !channel.ClassName.empty())
  {
   MModel_Create(i,channel.ClassName.c_str());
  }

//  if(!channel.ClassName.empty())

  ProjectXml.WriteString(std::string("ParametersFileName")+suffix,string("Parameters")+suffix+".xml");
  ProjectXml.WriteString(std::string("StatesFileName")+suffix,string("States")+suffix+".xml");

//  UComponentsControlForm->ComponentsControlFrame->SaveModelToFile((project_config.ProjectDirectory+string("Model")+suffix+string(".xml")).c_str());
//  UComponentsControlForm->ComponentsControlFrame->SaveParametersToFile((project_config.ProjectDirectory+string("Parameters")+suffix+string(".xml")).c_str());

  try
  {
   if(channel.ModelFileName.empty())
   {
	TRichEdit* RichEdit=new TRichEdit(this);
	RichEdit->Visible=false;
	RichEdit->Parent=this;

	RichEdit->PlainText=true;
	const char *p_buf=MModel_SaveComponent(i,"");
	if(p_buf)
	 RichEdit->Text=p_buf;
	Engine_FreeBufString(p_buf);
	RichEdit->Lines->SaveToFile((project_config.ProjectDirectory+string("\\")+model_file_name).c_str());

	p_buf=MModel_SaveComponentParameters(i,"");
	if(p_buf)
	 RichEdit->Text=p_buf;
	Engine_FreeBufString(p_buf);
	RichEdit->Lines->SaveToFile((project_config.ProjectDirectory+string("\\Parameters")+suffix+string(".xml")).c_str());

    delete RichEdit;
   }
  }
  catch(Exception &exception)
  {
   MEngine_LogMessage(i, RDK_EX_ERROR, (std::string("Create Project: Save model or parameters Fail: ")+AnsiString(exception.Message).c_str()).c_str());
  }
 }

 ProjectXml.SaveToFile(FileName);
 OpenProject(FileName.c_str());
}

// Закрывает существущий проект
void TUGEngineControlForm::CloseProject(void)
{
 Pause1Click(this);

 if(ProjectAutoSaveFlag)
  SaveProject();

 UServerControlForm->ServerRestartTimer->Enabled=false;

 if(ProjectOpenFlag)
 {
  ProjectFileName="";
  ProjectPath="";
 }
 ProjectOpenFlag=false;
 for(int i=GetNumEngines();i>=0;i--)
 {
  SelectEngine(i);
  if(GetEngine())
  {
   Env_UnInit();
//  EngineUnInit();
   Model_Destroy();
  }
 }
 RDK::UIVisualControllerStorage::ClearInterface();
 UEngineMonitorForm->EngineMonitorFrame->StopEngineMonitorThread();
}

// Открывает проект
void TUGEngineControlForm::OpenProject(const String &FileName)
{
 CloseProject();

 UShowProgressBarForm->SetWinTitle(Lang_LoadProjectTitle);

 ProjectXml.LoadFromFile(AnsiString(FileName).c_str(),"");
 ProjectPath=ExtractFilePath(FileName);
 ProjectFileName=ExtractFileName(FileName);

 ProjectXml.SelectNodeRoot("Project/MultiGeneral");
 int engines_mode=ProjectXml.ReadInteger("EnginesMode",0);
 UEngineMonitorForm->EngineMonitorFrame->SetChannelsMode(engines_mode);

 int calc_time_mode=ProjectXml.ReadInteger("CalculationTimeSourceMode",0);
 UEngineMonitorForm->EngineMonitorFrame->SetCalculationTimeSourceMode(calc_time_mode);

 EventsLogEnabled=ProjectXml.ReadBool("EventsLogEnabled",true);
 ProjectShowChannelsStates=ProjectXml.ReadBool("ProjectShowChannelsStates",true);

 int num_engines=ProjectXml.ReadInteger("NumEngines",1);
 if(num_engines<=0)
  num_engines=1;

 UShowProgressBarForm->SetBarHeader(1,Lang_LoadingData);
 UShowProgressBarForm->SetBarHeader(2,Lang_Total);
 UShowProgressBarForm->ResetBarStatus(1, 1, num_engines-1);
 UShowProgressBarForm->ResetBarStatus(2, 1, 2);

 if(AppWinState)
  UShowProgressBarForm->Show();
 UShowProgressBarForm->Update();

try{

 UEngineMonitorForm->EngineMonitorFrame->SetNumChannels(num_engines);

 int selected_engine_index=ProjectXml.ReadInteger("SelectedEngineIndex",0);

 ProjectXml.SelectNodeRoot("Project/General");

 ProjectName=ProjectXml.ReadString("ProjectName","NoName").c_str();

 // Число входов среды
 NumEnvInputs=ProjectXml.ReadInteger("NumEnvInputs",1);

 // Число выходов среды
 NumEnvOutputs=ProjectXml.ReadInteger("NumEnvOutputs",1);

 InputEnvImageWidth=ProjectXml.ReadInteger("InputEnvImageWidth",360);
 InputEnvImageHeight=ProjectXml.ReadInteger("InputEnvImageHeight",240);

 ProjectMode=ProjectXml.ReadInteger("ProjectMode",1);

 PredefinedStructure.resize(GetNumEngines());
 PredefinedStructure[0]=ProjectXml.ReadInteger("PredefinedStructure",0);
 for(int i=1;i<GetNumEngines();i++)
 {
  PredefinedStructure[i]=ProjectXml.ReadInteger(std::string("PredefinedStructure_")+RDK::sntoa(i),0);
 }

 // Флаг автоматического сохранения проекта
 ProjectAutoSaveFlag=ProjectXml.ReadInteger("ProjectAutoSaveFlag",1);

 // Флаг автоматического сохранения проекта
 ProjectAutoSaveStateFlag=ProjectXml.ReadInteger("ProjectAutoSaveStateFlag",0);

 // Шаг счета по умолчанию
 DefaultTimeStep.resize(GetNumEngines());
 DefaultTimeStep[0]=ProjectXml.ReadInteger("DefaultTimeStep",30);
 for(int i=1;i<GetNumEngines();i++)
 {
  DefaultTimeStep[i]=ProjectXml.ReadInteger(std::string("DefaultTimeStep_")+RDK::sntoa(i),30);
 }

 // Глобальный шаг счета модели
 GlobalTimeStep.resize(GetNumEngines());
 GlobalTimeStep[0]=ProjectXml.ReadInteger("GlobalTimeStep",30);
 for(int i=1;i<GetNumEngines();i++)
 {
  GlobalTimeStep[i]=ProjectXml.ReadInteger(std::string("GlobalTimeStep_")+RDK::sntoa(i),30);
 }

 ReflectionFlag=ProjectXml.ReadBool("ReflectionFlag",true);

 CalculationMode.resize(GetNumEngines());
 CalculationMode[0]=ProjectXml.ReadInteger("CalculationMode",2);
 for(int i=1;i<GetNumEngines();i++)
 {
  CalculationMode[i]=ProjectXml.ReadInteger(std::string("CalculationMode_")+RDK::sntoa(i),2);
 }

 InitAfterLoadFlag.resize(GetNumEngines());
 InitAfterLoadFlag[0]=ProjectXml.ReadBool("InitAfterLoadFlag",1);
 for(int i=1;i<GetNumEngines();i++)
 {
  InitAfterLoadFlag[i]=ProjectXml.ReadBool(std::string("InitAfterLoadFlag_")+RDK::sntoa(i),1);
 }

 ResetAfterLoadFlag.resize(GetNumEngines());
 ResetAfterLoadFlag[0]=ProjectXml.ReadBool("ResetAfterLoadFlag",true);
 for(int i=1;i<GetNumEngines();i++)
 {
  ResetAfterLoadFlag[i]=ProjectXml.ReadBool(std::string("ResetAfterLoadFlag_")+RDK::sntoa(i),true);
 }

 DebugModeFlag.resize(GetNumEngines());
 DebugModeFlag[0]=ProjectXml.ReadBool("DebugModeFlag",false);
 for(int i=1;i<GetNumEngines();i++)
 {
  DebugModeFlag[i]=ProjectXml.ReadBool(std::string("DebugModeFlag_")+RDK::sntoa(i),false);
 }

 MinInterstepsInterval.resize(GetNumEngines());
 MinInterstepsInterval[0]=ProjectXml.ReadInteger("MinInterstepsInterval",20);
 UEngineMonitorForm->EngineMonitorFrame->SetMinInterstepsInterval(0,MinInterstepsInterval[0]);
 for(int i=1;i<GetNumEngines();i++)
 {
  MinInterstepsInterval[i]=ProjectXml.ReadInteger(std::string("MinInterstepsInterval_")+RDK::sntoa(i),20);
  UEngineMonitorForm->EngineMonitorFrame->SetMinInterstepsInterval(i,MinInterstepsInterval[i]);
 }


 String descriptionfilename=ProjectXml.ReadString("ProjectDescriptionFileName","").c_str();
 if(ExtractFilePath(descriptionfilename).Length() == 0)
  descriptionfilename=ProjectPath+descriptionfilename;

 if(descriptionfilename.Length() != 0 && FileExists(descriptionfilename))
 {
  TRichEdit* RichEdit=new TRichEdit(this);
  RichEdit->Parent=this;
  RichEdit->Visible=false;

  RichEdit->PlainText=true;

  RichEdit->Lines->LoadFromFile(descriptionfilename);
  ProjectDescription=RichEdit->Text;
  delete RichEdit;
 }

// #ifdef RDK_VIDEO
//  for(int j=0;j<NumEnvInputs;j++)
//   VideoOutputForm->AddSource();
// #endif

 for(int i=0;i<GetNumEngines();i++)
 {
  try
  {
  SelectEngine(i);
  String modelfilename;

  if(i == 0)
   modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();
  else
   modelfilename=ProjectXml.ReadString(std::string("ModelFileName_")+RDK::sntoa(i),"").c_str();


  if(!IsEngineInit())
   GraphicalEngineInit(PredefinedStructure[i],NumEnvInputs,NumEnvOutputs,InputEnvImageWidth, InputEnvImageHeight ,ReflectionFlag,ExceptionHandler);
  else
   Env_SetPredefinedStructure(PredefinedStructure[i]);

  // Загрузка описаний классов
  UComponentsControlForm->ComponentsControlFrame->LoadCommonClassesDescriptionFromFile("CommonClassesDescription.xml");
  UComponentsControlForm->ComponentsControlFrame->LoadClassesDescriptionFromFile("ClassesDescription.xml");
//  MStorage_LoadCommonClassesDescription(i,"CommonClassesDescription.xml");
//  MStorage_LoadClassesDescription(i,"ClassesDescription.xml");

  Model_SetDefaultTimeStep(DefaultTimeStep[i]);
  Env_SetCurrentDataDir(AnsiString(ProjectPath).c_str());

  Env_CreateStructure();
  Env_Init();

  Env_SetDebugMode(DebugModeFlag[i]);

  if(PredefinedStructure[i] == 0 && modelfilename.Length() != 0)
  {
   if(ExtractFilePath(modelfilename).Length() == 0)
	UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(ProjectPath+modelfilename);
   else
	UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
  }

  String paramsfilename;

  if(i == 0)
   paramsfilename=ProjectXml.ReadString("ParametersFileName","").c_str();
  else
   paramsfilename=ProjectXml.ReadString(std::string("ParametersFileName_")+RDK::sntoa(i),"").c_str();

  if(paramsfilename.Length() != 0)
  {
   if(ExtractFilePath(paramsfilename).Length() == 0)
	UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(ProjectPath+paramsfilename);
   else
	UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(paramsfilename);
  }

  if(ProjectAutoSaveStateFlag)
  {
   String statesfilename;
   if(i == 0)
	statesfilename=ProjectXml.ReadString("StatesFileName","").c_str();
   else
	statesfilename=ProjectXml.ReadString(std::string("StatesFileName_")+RDK::sntoa(i),"").c_str();

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
   Model_SetGlobalTimeStep("",GlobalTimeStep[i]);
   if(InitAfterLoadFlag[i])
    MEnv_Init(i);
   if(ResetAfterLoadFlag[i])
	MEnv_Reset(i,0);
  }
  UShowProgressBarForm->IncBarStatus(1);
  UShowProgressBarForm->Update();
  Sleep(0);
  }
  catch(RDK::UException &exception)
  {
   Engine_LogMessage(exception.GetType(), (std::string("Core-OpenProject(Load Channel) Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
  }
  catch(Exception &exception)
  {
   MEngine_LogMessage(GetSelectedEngineIndex(), RDK_EX_ERROR, (std::string("GUI-OpenProject(Load Channel) Exception: ")+AnsiString(exception.Message).c_str()).c_str());
  }
 }
 UShowProgressBarForm->IncBarStatus(1);
 UShowProgressBarForm->Update();
 Sleep(0);
 UShowProgressBarForm->IncBarStatus(2);
 UShowProgressBarForm->SetBarHeader(1,Lang_UpdateInterface);
 UShowProgressBarForm->ResetBarStatus(1, 1, 1);
 UShowProgressBarForm->Update();
 Sleep(0);

 if(selected_engine_index>=GetNumEngines())
  selected_engine_index=0;

 SelectEngine(selected_engine_index);
 InterfaceXml.Destroy();
 String interfacefilename;

// if(selected_engine_index == 0)
  interfacefilename=ProjectXml.ReadString("InterfaceFileName","").c_str();
// else
//  interfacefilename=ProjectXml.ReadString(std::string("InterfaceFileName_")+RDK::sntoa(selected_engine_index),"").c_str();

 if(interfacefilename.Length() != 0)
 {
  if(ExtractFilePath(interfacefilename.c_str()).Length() == 0)
   InterfaceXml.LoadFromFile(AnsiString(ProjectPath+interfacefilename).c_str(),"Interfaces");
  else
   InterfaceXml.LoadFromFile(AnsiString(interfacefilename).c_str(),"Interfaces");

  InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 }
 UServerControlForm->SetNumChannels(num_engines);
 RDK::UIVisualControllerStorage::LoadParameters(InterfaceXml);

 for(size_t i=0;i<CalculationMode.size();i++)
  UEngineMonitorForm->EngineMonitorFrame->SetCalculateMode(i, CalculationMode[i]);
 ProjectOpenFlag=true;
 UEngineMonitorForm->EngineMonitorFrame->StartEngineMonitorThread();

 RDK::UIVisualControllerStorage::UpdateInterface();
 RDK::UIVisualControllerStorage::AfterLoadProject();
 UServerControlForm->ServerRestartTimer->Enabled=true;
}
catch(RDK::UException &exception)
{
 UShowProgressBarForm->Hide();
 Engine_LogMessage(exception.GetType(), (std::string("Core-OpenProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
}
catch(Exception &exception)
{
 UShowProgressBarForm->Hide();
 MEngine_LogMessage(GetSelectedEngineIndex(), RDK_EX_ERROR, (std::string("Open project Fail: ")+AnsiString(exception.Message).c_str()).c_str());
}
catch(...)
{
 UShowProgressBarForm->Hide();
 throw;
}
 UShowProgressBarForm->IncBarStatus(1);
 UShowProgressBarForm->IncBarStatus(2);
 UShowProgressBarForm->Update();
 Sleep(0);
 UShowProgressBarForm->Hide();
}


// Загружает проект с индексом source_id, в движок с индексом cloned_id
void TUGEngineControlForm::CloneProject(int source_id, int cloned_id)
{
 if(source_id>=GetNumEngines() || cloned_id >= GetNumEngines())
  return;

 try {
 PredefinedStructure.resize(GetNumEngines());
 PredefinedStructure[cloned_id]=PredefinedStructure[source_id];

 // Шаг счета по умолчанию
 DefaultTimeStep.resize(GetNumEngines());
 DefaultTimeStep[cloned_id]=DefaultTimeStep[source_id];

 // Глобальный шаг счета модели
 GlobalTimeStep.resize(GetNumEngines());
 GlobalTimeStep[cloned_id]=GlobalTimeStep[source_id];

 if(!ProjectXml.SelectNodeRoot("Project/General"))
  return;

 ReflectionFlag=ProjectXml.ReadBool("ReflectionFlag",true);

 CalculationMode.resize(GetNumEngines());
 CalculationMode[cloned_id]=CalculationMode[source_id];

 InitAfterLoadFlag.resize(GetNumEngines());
 InitAfterLoadFlag[cloned_id]=InitAfterLoadFlag[source_id];

 ResetAfterLoadFlag.resize(GetNumEngines());
 ResetAfterLoadFlag[cloned_id]=ResetAfterLoadFlag[source_id];

 DebugModeFlag.resize(GetNumEngines());
 DebugModeFlag[cloned_id]=DebugModeFlag[source_id];

 MinInterstepsInterval.resize(GetNumEngines());
 MinInterstepsInterval[cloned_id]=MinInterstepsInterval[source_id];


 ProjectXml.WriteInteger("ProjectAutoSaveFlag",ProjectAutoSaveFlag);
 // Флаг автоматического сохранения проекта
 ProjectXml.WriteInteger("ProjectAutoSaveStateFlag",ProjectAutoSaveStateFlag);

 int selected_engine=GetSelectedEngineIndex();
 SelectEngine(cloned_id);
 String modelfilename;

 if(source_id == 0)
   modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();
  else
   modelfilename=ProjectXml.ReadString(std::string("ModelFileName_")+RDK::sntoa(source_id),"").c_str();


 if(!IsEngineInit())
  GraphicalEngineInit(PredefinedStructure[cloned_id],NumEnvInputs,NumEnvOutputs,InputEnvImageWidth, InputEnvImageHeight ,ReflectionFlag,ExceptionHandler);
 else
  Env_SetPredefinedStructure(PredefinedStructure[cloned_id]);

 Model_SetDefaultTimeStep(DefaultTimeStep[cloned_id]);
 Env_SetCurrentDataDir(AnsiString(ProjectPath).c_str());

 Env_CreateStructure();
 Env_Init();

 if(PredefinedStructure[cloned_id] == 0 && modelfilename.Length() != 0)
 {
  if(ExtractFilePath(modelfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(ProjectPath+modelfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile(modelfilename);
 }

 String paramsfilename;

 if(source_id == 0)
  paramsfilename=ProjectXml.ReadString("ParametersFileName","").c_str();
 else
  paramsfilename=ProjectXml.ReadString(std::string("ParametersFileName_")+RDK::sntoa(source_id),"").c_str();

 if(paramsfilename.Length() != 0)
 {
  if(ExtractFilePath(paramsfilename).Length() == 0)
   UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(ProjectPath+paramsfilename);
  else
   UComponentsControlForm->ComponentsControlFrame->LoadParametersFromFile(paramsfilename);
 }

 if(ProjectAutoSaveStateFlag)
 {
  String statesfilename;
  if(source_id == 0)
   statesfilename=ProjectXml.ReadString("StatesFileName","").c_str();
  else
   statesfilename=ProjectXml.ReadString(std::string("StatesFileName_")+RDK::sntoa(source_id),"").c_str();

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
  Model_SetGlobalTimeStep("",GlobalTimeStep[cloned_id]);
  if(InitAfterLoadFlag[cloned_id])
   MEnv_Init(cloned_id);
  if(ResetAfterLoadFlag[cloned_id])
   MEnv_Reset(cloned_id,0);
 }
 SelectEngine(selected_engine);
}
catch(RDK::UException &exception)
{
 UShowProgressBarForm->Hide();
 Engine_LogMessage(exception.GetType(), (std::string("Core-CloneProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
}
catch(Exception &exception)
{
 UShowProgressBarForm->Hide();
 MEngine_LogMessage(GetSelectedEngineIndex(), RDK_EX_ERROR, (std::string("GUI-CloneProject Exception: ")+AnsiString(exception.Message).c_str()).c_str());
}
catch(...)
{
 UShowProgressBarForm->Hide();
 throw;
}

}


// Сохраняет проект
void TUGEngineControlForm::SaveProject(void)
{
 if(!ProjectOpenFlag)
  return;

 UShowProgressBarForm->SetWinTitle(Lang_SaveProjectTitle);
 UShowProgressBarForm->SetBarHeader(1,Lang_SaveInterface);
 UShowProgressBarForm->SetBarHeader(2,Lang_Total);
 UShowProgressBarForm->ResetBarStatus(1, 1, 1);
 UShowProgressBarForm->ResetBarStatus(2, 1, 2);
 if(AppWinState)
  UShowProgressBarForm->Show();
 UShowProgressBarForm->Update();
try{

 InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 RDK::UIVisualControllerStorage::SaveParameters(InterfaceXml);

 String interfacefilename;
// if(GetSelectedEngineIndex() == 0)
  interfacefilename=ProjectXml.ReadString("InterfaceFileName","").c_str();
// else
 // interfacefilename=ProjectXml.ReadString(std::string("InterfaceFileName_")+RDK::sntoa(GetSelectedEngineIndex()),"").c_str();

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

 ProjectXml.WriteInteger("ProjectAutoSaveFlag",ProjectAutoSaveFlag);
 // Флаг автоматического сохранения проекта
 ProjectXml.WriteInteger("ProjectAutoSaveStateFlag",ProjectAutoSaveStateFlag);

 UShowProgressBarForm->IncBarStatus(2);
 UShowProgressBarForm->SetBarHeader(1,Lang_SavingData);
 UShowProgressBarForm->ResetBarStatus(1, 1, GetNumEngines()-1);
 UShowProgressBarForm->Update();
 Sleep(0);

 int selected_engine_index=GetSelectedEngineIndex();
 for(int i=0;i<GetNumEngines();i++)
 {
  SelectEngine(i);
  String modelfilename;
  if(i == 0)
   modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();
  else
   modelfilename=ProjectXml.ReadString(std::string("ModelFileName_")+RDK::sntoa(i),"").c_str();
  if(modelfilename.Length() != 0)
  {
   if(ExtractFilePath(modelfilename).Length() == 0)
	UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(ProjectPath+modelfilename);
   else
	UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(modelfilename);
  }
  else
  {
   std::string file_name;
   if(i == 0)
   {
	file_name="model.xml";
	ProjectXml.WriteString("ModelFileName",file_name);
   }
   else
   {
	file_name=std::string("model_")+RDK::sntoa(i)+".xml";
	ProjectXml.WriteString(std::string("ModelFileName_")+RDK::sntoa(i),file_name);
   }
   UComponentsControlForm->ComponentsControlFrame->SaveModelToFile(ProjectPath+file_name.c_str());
  }

  String paramsfilename;
  if(i == 0)
   paramsfilename=ProjectXml.ReadString("ParametersFileName","").c_str();
  else
   paramsfilename=ProjectXml.ReadString(std::string("ParametersFileName_")+RDK::sntoa(i),"").c_str();

  if(paramsfilename.Length() == 0)
  {
   if(i == 0)
   {
	paramsfilename="Parameters.xml";
    ProjectXml.WriteString("ParametersFileName",AnsiString(paramsfilename).c_str());
  }
   else
   {
	paramsfilename=String("Parameters_")+IntToStr(i)+".xml";
	ProjectXml.WriteString(std::string("ParametersFileName_")+RDK::sntoa(i),AnsiString(paramsfilename).c_str());
   }
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
	if(i == 0)
	{
	 statesfilename="States.xml";
	 ProjectXml.WriteString("StatesFileName",AnsiString(statesfilename).c_str());
	}
	else
	{
	 statesfilename=String("States_")+IntToStr(i)+".xml";
	 ProjectXml.WriteString(std::string("StatesFileName_")+RDK::sntoa(i),AnsiString(statesfilename).c_str());
	}
   }

   if(statesfilename.Length() != 0)
   {
	if(ExtractFilePath(statesfilename).Length() == 0)
	 UComponentsControlForm->ComponentsControlFrame->SaveStatesToFile(ProjectPath+statesfilename);
	else
	 UComponentsControlForm->ComponentsControlFrame->SaveStatesToFile(statesfilename);
   }
  }

  if(i == 0)
  {
   ProjectXml.WriteInteger("PredefinedStructure",PredefinedStructure[0]);
   // Шаг счета по умолчанию
   ProjectXml.WriteInteger("DefaultTimeStep",DefaultTimeStep[0]);

   // Глобальный шаг счета модели
   ProjectXml.WriteInteger("GlobalTimeStep",GlobalTimeStep[0]);

   ProjectXml.WriteInteger("CalculationMode",CalculationMode[0]);
   ProjectXml.WriteInteger("MinInterstepsInterval",MinInterstepsInterval[0]);
   ProjectXml.WriteBool("InitAfterLoadFlag",InitAfterLoadFlag[0]);
   ProjectXml.WriteBool("ResetAfterLoadFlag",ResetAfterLoadFlag[0]);
   ProjectXml.WriteBool("DebugModeFlag",DebugModeFlag[0]);
  }
  else
  {
   std::string suffix=RDK::sntoa(i);
   ProjectXml.WriteInteger(std::string("PredefinedStructure_")+suffix,PredefinedStructure[i]);
   // Шаг счета по умолчанию
   ProjectXml.WriteInteger(std::string("DefaultTimeStep_")+suffix,DefaultTimeStep[i]);

   // Глобальный шаг счета модели
   ProjectXml.WriteInteger(std::string("GlobalTimeStep_")+suffix,GlobalTimeStep[i]);

   ProjectXml.WriteInteger(std::string("CalculationMode_")+suffix,CalculationMode[i]);

   ProjectXml.WriteInteger(std::string("MinInterstepsInterval_")+suffix,MinInterstepsInterval[i]);
   ProjectXml.WriteBool(std::string("InitAfterLoadFlag_")+suffix,InitAfterLoadFlag[i]);
   ProjectXml.WriteBool(std::string("ResetAfterLoadFlag_")+suffix,ResetAfterLoadFlag[i]);
   ProjectXml.WriteBool(std::string("DebugModeFlag_")+suffix,DebugModeFlag[i]);
  }

  UShowProgressBarForm->IncBarStatus(1);
  UShowProgressBarForm->Update();
  Sleep(0);
 }

 SelectEngine(selected_engine_index);

 ProjectXml.WriteInteger("ProjectAutoSaveFlag",ProjectAutoSaveFlag);

 // Флаг автоматического сохранения проекта
 ProjectXml.WriteInteger("ProjectAutoSaveStateFlag",ProjectAutoSaveStateFlag);

 // Число входов среды
 ProjectXml.WriteInteger("NumEnvInputs",NumEnvInputs);

 // Число выходов среды
 ProjectXml.WriteInteger("NumEnvOutputs",NumEnvOutputs);

 ProjectXml.WriteInteger("ProjectMode",ProjectMode);

 ProjectXml.WriteInteger("InputEnvImageWidth",InputEnvImageWidth);
 ProjectXml.WriteInteger("InputEnvImageHeight",InputEnvImageHeight);

 ProjectXml.WriteBool("ReflectionFlag",ReflectionFlag);

 ProjectXml.WriteString("ProjectName",AnsiString(ProjectName).c_str());

 ProjectXml.SelectNodeRoot("Project/MultiGeneral");
 ProjectXml.WriteInteger("EnginesMode",UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode());
 ProjectXml.WriteInteger("CalculationTimeSourceMode",UEngineMonitorForm->EngineMonitorFrame->GetCalculationTimeSourceMode());

 ProjectXml.WriteInteger("NumEngines",GetNumEngines());
 ProjectXml.WriteInteger("SelectedEngineIndex",GetSelectedEngineIndex());

 ProjectXml.WriteBool("ProjectShowChannelsStates",ProjectShowChannelsStates);


 ProjectXml.SaveToFile(AnsiString(ProjectPath+ProjectFileName).c_str());
}
catch(RDK::UException &exception)
{
 UShowProgressBarForm->Hide();
 Engine_LogMessage(exception.GetType(), (std::string("Core-SaveProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
}
catch(Exception &exception)
{
 UShowProgressBarForm->Hide();
 MEngine_LogMessage(GetSelectedEngineIndex(), RDK_EX_ERROR, (std::string("GUI-SaveProject Exception: ")+AnsiString(exception.Message).c_str()).c_str());
}
catch(...)
{
 UShowProgressBarForm->Hide();
 throw;
}
 UShowProgressBarForm->IncBarStatus(1);
 UShowProgressBarForm->IncBarStatus(2);
 UShowProgressBarForm->Update();
 Sleep(0);
 UShowProgressBarForm->Hide();
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

// Добавляет заданную форму на заданную вкладку
bool TUGEngineControlForm::AddSpecialFormToPage(TUVisualControllerForm *form, TTabSheet* tab, const String &caption)
{
 if(!form|| !tab)
  return false;

 tab->InsertComponent(form);
 form->Parent=tab;
 form->Align=alClient;
 form->BorderStyle=bsNone;
 form->Show();
 form->UpdateInterface(true);

 std::string str=AnsiString(caption).c_str();

 tab->Caption=caption;

 return true;
}

// Добавляет страницу
TTabSheet* TUGEngineControlForm::AddSpecialFramePage(const String &type, const String &caption)
{
 TTabSheet* tab=new TTabSheet(PageControl1);
 tab->PageControl=PageControl1;
 TUVisualControllerFrame *frame=0;
// TUVisualControllerForm *form=0;

 if(type == "TUImagesFrame")
 {
  frame=new TUImagesFrame(0);
  AddSpecialFrameToPage(frame, tab, caption);
 }
 else
 if(type == "TUWatchFrame")
 {
  frame=new TUWatchFrame(0);
  AddSpecialFrameToPage(frame, tab, caption);
 }
 else
 {
  tab->Caption=caption;
  return tab;
 }

 return tab;
}

// Создает новую вкладку с заданным именем для специальных форм
TTabSheet* TUGEngineControlForm::AddSpecialFormPage(const String &type)
{
 TTabSheet* tab=0;
 TUVisualControllerForm *form=0;

 tab=FindSpecialFormPage(type, form);
 if(tab)
 {
  PageControl1->ActivePage=tab;
  return tab;
 }

 if(form && form->ClassNameIs(type))
 {
  tab=new TTabSheet(PageControl1);
  tab->PageControl=PageControl1;

  AddSpecialFormToPage(form, tab, form->Caption);
 }
 PageControl1->ActivePage=tab;
 return tab;
}

// Ищет вкладку с заданным именем для заданной специальной форме
TTabSheet* TUGEngineControlForm::FindSpecialFormPage(const String &type, TUVisualControllerForm* &form)
{
 TTabSheet* tab=0;

  std::map<std::string, TUVisualControllerForm*>::iterator I;
  I=SpecialForms.begin();
  for(;I != SpecialForms.end();++I)
  {
   form=dynamic_cast<TUVisualControllerForm*>(I->second);
   if(form && form->ClassNameIs(type))
   {
	for(int i=0;i<PageControl1->PageCount;i++)
	{
	 if(PageControl1->Pages[i]->ComponentCount>0)
	 {
	  TUVisualControllerForm* test_form=dynamic_cast<TUVisualControllerForm*>(PageControl1->Pages[i]->Components[0]);
	  if(form == test_form)
	  {
	   return PageControl1->Pages[i];
	  }
	 }
	}

	break;
   }
  }
 return tab;
}


// Создает новую вкладку с заданным именем для формы управления компонентом
// Если этим копонентом уже управляют, то возвращает указатель на такую вкладку
TTabSheet* TUGEngineControlForm::AddComponentControlFormPage(const string &component_name)
{
 int control_index=FindComponentControlPage(component_name);

 if(control_index>=0)
 {
  PageControl1->ActivePageIndex=control_index;
  return PageControl1->Pages[control_index];
 }

 const char *pname=Model_GetComponentClassName(component_name.c_str());
 std::string name;
 if(pname)
  name=pname;
 Engine_FreeBufString(pname);

 std::map<std::string, TUVisualControllerForm*>::iterator I=UComponentsListFrame1->ComponentControllers.find(name);
 if(I != UComponentsListFrame1->ComponentControllers.end() && I->second)
 {
  TTabSheet* tab=new TTabSheet(PageControl1);
  tab->PageControl=PageControl1;

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
  PageControl1->ActivePage=tab;
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

  {
   std::map<std::string, TUVisualControllerForm*>::iterator I=UComponentsListFrame1->ComponentControllers.begin();
   for(;I != UComponentsListFrame1->ComponentControllers.end();++I)
   {
	form=I->second;
	if(I->second->Parent == PageControl1->Pages[i])
	{
	 return;
	}
   }
  }

  {
   std::map<std::string, TUVisualControllerForm*>::iterator I=SpecialForms.begin();
   for(;I != SpecialForms.end();++I)
   {
	form=I->second;
	if(I->second->Parent == PageControl1->Pages[i])
	 return;
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

  I=SpecialForms.begin();
  for(;I != SpecialForms.end();++I)
  {
   TUVisualControllerForm *form=dynamic_cast<TUVisualControllerForm*>(I->second);
   if(form && form->Parent == PageControl1->Pages[index])
   {
	form->BorderStyle=bsSizeable;
	form->Align=alNone;
	form->Hide();
	form->Parent=0;
	PageControl1->Pages[index]->RemoveComponent(form);
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


/// Добавляет новый пункт в подменю сетевого вещания событиями
void TUGEngineControlForm::AddBroadcasterMenu(TMenuItem *item, TMenu *owner)
{
 owner->Items->Remove(item);
 Broadcasters1->Add(item);
}

/// Добавляет новый (глобальный) пункт в меню Window после пункта с именем after
void TUGEngineControlForm::AddGlobalWindowMenu(TMenuItem *item, TMenu *owner)
{
 owner->Items->Remove(item);
 Window1->Insert(10, item);
}

/// Загружает историю проектов из файла
void TUGEngineControlForm::LoadProjectsHistory(void)
{
 String opt_name=ExtractFileName(Application->ExeName);
 if(opt_name.Length()>4)
 opt_name=opt_name.SubString(0,opt_name.Length()-4);
 opt_name=opt_name+".projecthist";
 RDK::UIniFile<char> history_ini;
 history_ini.LoadFromFile(AnsiString(opt_name).c_str());
 std::vector<std::string> history;
 history_ini.GetVariableList("General",history);
 sort(history.begin(),history.end());

 LastProjectsList.clear();
 for(size_t i=0;i<history.size();i++)
 {
  LastProjectsList.push_back(history_ini("General",history[i],""));
  if(int(i)>=LastProjectsListMaxSize)
   break;
 }
}

/// Сохраняет историю проектов в файл
void TUGEngineControlForm::SaveProjectsHistory(void)
{
 RDK::UIniFile<char> history_ini;

 std::list<std::string>::iterator I=LastProjectsList.begin();
 int i=0;
 for(;I != LastProjectsList.end();I++)
 {
  history_ini("General",std::string("Hist")+RDK::sntoa(i++),*I);
 }

 String opt_name=ExtractFileName(Application->ExeName);
 if(opt_name.Length()>4)
 opt_name=opt_name.SubString(0,opt_name.Length()-4);
 opt_name=opt_name+".projecthist";
 history_ini.SaveToFile(AnsiString(opt_name).c_str());
}

/// Запуск отдельного канала
/// если channel_index == -1 то запускает все каналы
void TUGEngineControlForm::StartChannel(int channel_index)
{
 if(!ProjectOpenFlag)
  return;

 UShowProgressBarForm->SetWinTitle(Lang_Starting);
 UShowProgressBarForm->SetBarHeader(1,"");
 UShowProgressBarForm->SetBarHeader(2,Lang_Total);
 UShowProgressBarForm->ResetBarStatus(1, 1, 1);
 UShowProgressBarForm->ResetBarStatus(2, 1, 2);
 if(AppWinState)
  UShowProgressBarForm->Show();

 UEngineMonitorForm->EngineMonitorFrame->StartChannel(channel_index);
#ifdef RDK_VIDEO
 VideoOutputForm->Start(channel_index);
#endif
 UShowProgressBarForm->IncBarStatus(2);

 UShowProgressBarForm->Hide();
}

/// Останов отдельного канала
/// если channel_index == -1 то останавливает все каналы
void TUGEngineControlForm::PauseChannel(int channel_index)
{
 if(!ProjectOpenFlag)
  return;

 UShowProgressBarForm->SetWinTitle(Lang_Stopping);
 UShowProgressBarForm->SetBarHeader(1,"");
 UShowProgressBarForm->SetBarHeader(2,Lang_Total);
 UShowProgressBarForm->ResetBarStatus(1, 1, 1);
 UShowProgressBarForm->ResetBarStatus(2, 1, 2);
 if(AppWinState)
  UShowProgressBarForm->Show();

 UEngineMonitorForm->EngineMonitorFrame->PauseChannel(channel_index);
 UShowProgressBarForm->IncBarStatus(2);
#ifdef RDK_VIDEO
 VideoOutputForm->StopOffline(channel_index);
#endif
 UShowProgressBarForm->Hide();
}

/// Сброс отдельного канала
/// если channel_index == -1 то сбрасывает все каналы
void TUGEngineControlForm::ResetChannel(int channel_index)
{
 if(!ProjectOpenFlag)
  return;

 UEngineMonitorForm->EngineMonitorFrame->ResetChannel(channel_index);
#ifdef RDK_VIDEO
 VideoOutputForm->StopOffline(channel_index);
#endif
}

/// --------------------------
/// Методы управления каналами
/// --------------------------
/// Возвращает число каналов
int TUGEngineControlForm::GetNumChannels(void) const
{
 return GetNumEngines();
}

/// Устанавливает число каналов
/// также выставляет число источников видео
int TUGEngineControlForm::SetNumChannels(int value)
{
 Pause1Click(this);
 UEngineMonitorForm->EngineMonitorFrame->SetNumChannels(value);
 UServerControlForm->SetNumChannels(value);
 return 0;
}


/// Добавляет канал в позицию index
/// Если позиция не существует, то добавляет в конец
int TUGEngineControlForm::AddChannel(int index)
{
 Pause1Click(this);
 UEngineMonitorForm->EngineMonitorFrame->InsertChannel(index);
 UServerControlForm->SetNumChannels(GetNumChannels());
 return 0;
}

/// Удаляет канал из позиции index
int TUGEngineControlForm::DelChannel(int index)
{
 Pause1Click(this);
 UEngineMonitorForm->EngineMonitorFrame->DeleteChannel(index);
 UServerControlForm->SetNumChannels(GetNumChannels());
 return 0;
}

/// --------------------------



//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Start1Click(TObject *Sender)
{
 UEngineMonitorForm->RecreateEventsLogFile();
 StartChannel(-1);
 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Pause1Click(TObject *Sender)
{
 PauseChannel(-1);
 RDK::UIVisualControllerStorage::UpdateInterface();
 UEngineMonitorForm->RecreateEventsLogFile();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::EngineMonitor1Click(TObject *Sender)
{
 UEngineMonitorForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::Images1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 UImagesForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::VideoSource1Click(TObject *Sender)
{
#ifdef RDK_VIDEO
 AddSpecialFormPage("TVideoOutputForm");
 VideoOutputForm->Show();
#endif
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::Step1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 UEngineMonitorForm->EngineMonitorFrame->Step1Click(Sender);
 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::ComponentsControl1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 UComponentsControlForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::ComponentsLinks1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 UComponentLinksForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::LoadModel1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 UComponentsControlForm->ComponentsControlFrame->LoadModelFromFile("");
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::SaveModel1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 UComponentsControlForm->ComponentsControlFrame->SaveModelToFile("");
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::OpenImage1Click(TObject *Sender)
{
#ifdef RDK_VIDEO
 if(!VideoOutputForm->GetActiveVideoOutputFrame())
  return;

// VideoOutputForm->GetActiveVideoOutputFrame()->VideoCaptureOptionsForm->VideoGrabberControlFrame->OpenImageFileButtonClick(Sender);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::OpenVideo1Click(TObject *Sender)
{
#ifdef RDK_VIDEO
 if(!VideoOutputForm->GetActiveVideoOutputFrame())
  return;

// VideoOutputForm->GetActiveVideoOutputFrame()->VideoCaptureOptionsForm->VideoGrabberControlFrame->VFBrowseButtonClick(Sender);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::CaptureVideo1Click(TObject *Sender)
{
#ifdef RDK_VIDEO
 if(!VideoOutputForm->GetActiveVideoOutputFrame())
  return;

// VideoOutputForm->GetActiveVideoOutputFrame()->VideoCaptureOptionsForm->VideoGrabberControlFrame->SelectMode(0);
// VideoOutputForm->GetActiveVideoOutputFrame()->MyVideoGrabberControlForm->Show();
#endif
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Reset1Click(TObject *Sender)
{
 ResetChannel(-1);
 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Performance1Click(TObject *Sender)
{
 UComponentsPerformanceForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::LoadProjectItemClick(TObject *Sender)
{
 if(!LastProjectsList.empty())
  OpenDialog->InitialDir=ExtractFilePath(LastProjectsList.front().c_str());

 if(!OpenDialog->Execute())
  return;

 OpenProject(OpenDialog->FileName);
 LastProjectsList.push_front(AnsiString(OpenDialog->FileName).c_str());
 while(int(LastProjectsList.size())>LastProjectsListMaxSize && !LastProjectsList.empty())
 {
  LastProjectsList.pop_back();
 }


 SaveProjectsHistory();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::SaveProjectItemClick(TObject *Sender)
{
 SaveProject();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::CreateProjectItemClick(TObject *Sender)
{
 TProjectConfig clean_project;
 UCreateProjectWizardForm->ProjectConfig=clean_project;
 UCreateProjectWizardForm->ClearWizard();
 UCreateProjectWizardForm->Caption="Create Project Wizard";
 if(UCreateProjectWizardForm->ShowCreateProject(CreateWizardMode) == mrOk)
 {
  CloseProject();
  PredefinedStructure.resize(1);
  PredefinedStructure[0]=UCreateProjectWizardForm->ProjectConfig.ChannelsConfig[0].PredefinedStructure;
  ProjectAutoSaveFlag=UCreateProjectWizardForm->ProjectAutoSaveFlagCheckBox->Checked;

  DefaultTimeStep.resize(1);
  DefaultTimeStep[0]=StrToInt(UCreateProjectWizardForm->ProjectTimeStepEdit->Text);

  GlobalTimeStep=DefaultTimeStep;

//  NumEnvInputs=StrToInt(UCreateProjectWizardForm->NumInputsLabeledEdit->Text);
//  NumEnvOutputs=StrToInt(UCreateProjectWizardForm->NumOutputsLabeledEdit->Text);
//  InputEnvImageWidth=StrToInt(UCreateProjectWizardForm->ImageWidthLabeledEdit->Text);
//  InputEnvImageHeight=StrToInt(UCreateProjectWizardForm->ImageHeightLabeledEdit->Text);
//  ReflectionFlag=UCreateProjectWizardForm->UpendInputImageCheckBox->Checked;


  CalculationMode.resize(1,2);
  InitAfterLoadFlag.resize(1,1);
  ResetAfterLoadFlag.resize(1,1);
  DebugModeFlag.resize(1,false);
//  CalculationMode[0]=UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex;

  MinInterstepsInterval.resize(1);
  MinInterstepsInterval[0]=20;

  CreateProject(UCreateProjectWizardForm->ProjectConfig);

  ProjectName=UCreateProjectWizardForm->ProjectNameLabeledEdit->Text;
  ProjectDescription=UCreateProjectWizardForm->ProjectDescriptionRichEdit->Text;

  UEngineMonitorForm->EngineMonitorFrame->SetCalculationTimeSourceMode(UCreateProjectWizardForm->CalculationSourceTimeModeRadioGroup->ItemIndex);
 }
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::CreateModelClick(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

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
 if(!ProjectOpenFlag)
  return;

// UDrawEngineForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::ReloadParameters1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

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

// const SELDIRHELP = 1000;
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
 UCreateProjectWizardForm->ProjectTimeStepEdit->Text=IntToStr(DefaultTimeStep[GetSelectedEngineIndex()]);
// UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex=CalculationMode[GetSelectedEngineIndex()];
 UCreateProjectWizardForm->CalculationSourceTimeModeRadioGroup->ItemIndex=UEngineMonitorForm->EngineMonitorFrame->GetCalculationTimeSourceMode();

 UCreateProjectWizardForm->ProjectConfig.ChannelsConfig[0].PredefinedStructure=PredefinedStructure[GetSelectedEngineIndex()];
 if(PredefinedStructure[GetSelectedEngineIndex()])
 {
//  UCreateProjectWizardForm->PredefinedModelRadioButton->Checked=true;
//  UCreateProjectWizardForm->ModelFileNameRadioButton->Checked=false;
 }
 else
 {
//  UCreateProjectWizardForm->PredefinedModelRadioButton->Checked=false;
//  UCreateProjectWizardForm->ModelFileNameRadioButton->Checked=true;
  if(GetSelectedEngineIndex() == 0)
   UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text=ProjectXml.ReadString("ModelFileName","").c_str();
  else
   UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text=ProjectXml.ReadString(std::string("ModelFileName_")+RDK::sntoa(GetSelectedEngineIndex()),"").c_str();
 }

//  UCreateProjectWizardForm->NumInputsLabeledEdit->Text=IntToStr(NumEnvInputs);
//  UCreateProjectWizardForm->NumOutputsLabeledEdit->Text=IntToStr(NumEnvOutputs);
//  UCreateProjectWizardForm->ImageWidthLabeledEdit->Text=IntToStr(InputEnvImageWidth);
//  UCreateProjectWizardForm->ImageHeightLabeledEdit->Text=IntToStr(InputEnvImageHeight);
//  UCreateProjectWizardForm->UpendInputImageCheckBox->Checked=ReflectionFlag;

 UCreateProjectWizardForm->Caption="Update Project Wizard";
 if(UCreateProjectWizardForm->ShowProjectOptions() == mrOk)
 {
//  CloseProject();
  PredefinedStructure[GetSelectedEngineIndex()]=UCreateProjectWizardForm->ProjectConfig.ChannelsConfig[0].PredefinedStructure;
  ProjectAutoSaveFlag=UCreateProjectWizardForm->ProjectAutoSaveFlagCheckBox->Checked;
  DefaultTimeStep[GetSelectedEngineIndex()]=StrToInt(UCreateProjectWizardForm->ProjectTimeStepEdit->Text);
  GlobalTimeStep[GetSelectedEngineIndex()]=DefaultTimeStep[GetSelectedEngineIndex()];
//  CalculationMode[GetSelectedEngineIndex()]=UCreateProjectWizardForm->ProjectCalculationModeRadioGroup->ItemIndex;

//  CreateProject(UCreateProjectWizardForm->ProjectDirectoryLabeledEdit->Text+String("\\Project.ini"),UCreateProjectWizardForm->UClassesListFrame1->GetSelectedName(),UCreateProjectWizardForm->ProjectModelFileNameLabeledEdit->Text);

  ProjectName=UCreateProjectWizardForm->ProjectNameLabeledEdit->Text;
  ProjectDescription=UCreateProjectWizardForm->ProjectDescriptionRichEdit->Text;
  UEngineMonitorForm->EngineMonitorFrame->SetCalculationTimeSourceMode(UCreateProjectWizardForm->CalculationSourceTimeModeRadioGroup->ItemIndex);
  UpdateInterface(true);
 }
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::WatchWindow1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 UWatchForm->WindowState=wsNormal;
 UWatchForm->Show();
}
//---------------------------------------------------------------------------
void __fastcall TUGEngineControlForm::AppMinimize(TObject *Sender)
{
 if(!RdkMainForm)
  return;

 if(MinimizeToTray)
 {
  //Убираем с панели задач
  TrayIcon->Visible=true;
  ShowWindow(RdkMainForm->Handle,SW_HIDE);  // Скрываем программу
  ShowWindow(Application->Handle,SW_HIDE);  // Скрываем кнопку с TaskBar'а
  SetWindowLong(Application->Handle, GWL_EXSTYLE, GetWindowLong(Application->Handle, GWL_EXSTYLE) | !WS_EX_APPWINDOW);
  SetWindowLong(RdkMainForm->Handle, GWL_EXSTYLE, GetWindowLong(RdkMainForm->Handle, GWL_EXSTYLE) | !WS_EX_APPWINDOW);
 }
 else
 {
  RdkMainForm->WindowState=wsMinimized;
//  ShowWindow(RdkMainForm->Handle,SW_HIDE);  // Скрываем программу
 }
 AppWinState=false;
}

void __fastcall TUGEngineControlForm::AppRestore(TObject *Sender)
{
 if(!RdkMainForm)
  return;

 AppWinState=true;
 RdkMainForm->UpdateInterface();
}

void __fastcall TUGEngineControlForm::FormCreate(TObject *Sender)
{
 LastProjectsListMaxSize=10;
 FormatSettings.DecimalSeparator = '.';
 Saved8087CW = Default8087CW;
 System::Set8087CW(0x133f);

 TrayIcon->Icon->Assign(Application->Icon);

 // Грузим настройки приложения
 String opt_name=ExtractFileName(Application->ExeName);
 if(opt_name.Length()>4)
 opt_name=opt_name.SubString(0,opt_name.Length()-4);
 TMemIniFile *app_ini=new TMemIniFile(opt_name+".ini");
 MainFormName=app_ini->ReadString("General", "MainFormName", "");
 HideAdminFormFlag=app_ini->ReadBool("General", "HideAdminForm", false);
 AutoexecProjectFileName=app_ini->ReadString("General", "AutoexecProjectFileName", "");
 AutoStartProjectFlag=app_ini->ReadBool("General", "AutoStartProjectFlag", false);
 VideoGrabberLicenseString=app_ini->ReadString("General","VideoGrabberLicenseString","");
 MinimizeToTray=app_ini->ReadBool("General","MinimizeToTray",false);
 StartMinimized=app_ini->ReadBool("General","StartMinimized",false);
 ProgramName=app_ini->ReadString("General","ProgramName","Server");
 LastProjectsListMaxSize=app_ini->ReadInteger("General","LastProjectsListMaxSize",10);

 ServerInterfaceAddress=AnsiString(app_ini->ReadString("Server","BindAddress","127.0.0.1")).c_str();
 ServerInterfacePort=app_ini->ReadInteger("Server","BindPort",45045);

 DisableAdminForm=app_ini->ReadBool("General","DisableAdminForm",false);

 TrayIcon->Hint=ProgramName;

 Engine_SetBufObjectsMode(1);

 // Грузим шрифты
/* std::vector<std::string> font_names;
 std::string font_path=AnsiString(ExtractFilePath(Application->ExeName)+"Fonts\\").c_str();
 RDK::FindFilesList(font_path, "*.fnt", true, font_names);

 RDK::ClearClobalFonts();
 RDK::UBitmapFont font;
 for(size_t i=0;i<font_names.size();i++)
 {
  RDK::AddGlobalFont(font_path+font_names[i]);
 }
  */
 std::string font_path=AnsiString(ExtractFilePath(Application->ExeName)).c_str();
 SetSystemDir(font_path.c_str());
 GraphicalEngineInit(0,1,1,320,240,1,ExceptionHandler);
 Engine_LoadFonts();

 // Грузим историю проектов
 LoadProjectsHistory();

 RdkRpcDispatcher.SetDecoderPrototype(&RdkRpcDecoder);
 RdkApplication.SetRpcDispatcher(&RdkRpcDispatcher);
 RdkApplication.SetWorkDirectory(font_path);
 RdkApplication.Init();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::HideTimerTimer(TObject *Sender)
{
 if(!ApplicationInitialized)
  return;
 HideTimer->Enabled=false;

 UServerControlForm->SetServerBinding(ServerInterfaceAddress, ServerInterfacePort);

 UDrawEngineFrame1->ComponentsListFrame=UComponentsListFrame1;
 UComponentsListFrame1->DrawEngineFrame=UDrawEngineFrame1;
 UEngineMonitorForm->Parent=LogsTabSheet;
 UEngineMonitorForm->BorderStyle=bsNone;
 UEngineMonitorForm->EngineMonitorFrame->StatusBar->Visible=false;
 UEngineMonitorForm->Align=alClient;
 UEngineMonitorForm->Show();

 if(MainFormName.Length()>0)
 {
  TComponent *component=Application->FindComponent(MainFormName);
  RdkMainForm=dynamic_cast<TUVisualControllerForm*>(component);
 }

 if(!RdkMainForm)
  RdkMainForm=this;

 if(RdkMainForm)
 {
  SetWindowLong(RdkMainForm->Handle, GWL_EXSTYLE, GetWindowLong(RdkMainForm->Handle, GWL_EXSTYLE) | WS_EX_APPWINDOW);
  SetWindowLong(RdkMainForm->Handle, GWL_HWNDPARENT, 0);
 }

 if(StartMinimized)
 {
  if(!MinimizeToTray)
  {
   RdkMainForm->Show();
  }
  AppMinimize(this);
  Application->Minimize();
 }
 else
 {
  RdkMainForm->Show();
  RdkMainForm->UpdateInterface();
 }

 if(FileExists(AutoexecProjectFileName))
 {
  OpenProject(AutoexecProjectFileName);
  AutoexecProjectFileName="";
 }
/*
 bool hide_flag=HideAdminFormFlag;
 HideTimer->Enabled=false;
 if(HideAdminFormFlag)
 {
  HideAdminFormFlag=false;
  Hide();
 }

 UpdateInterface();
  */
 if(AutoStartProjectFlag)
 {
  AutoStartProjectFlag=false;
  Start1Click(Sender);
 }
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
 if(!RdkMainForm)
  return;

 if(RdkMainForm == this)
 {
  Pause1Click(Sender);
  Sleep(1000);
  CloseProject();
  CanClose=true;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Images2Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 AddSpecialFramePage("TUImagesFrame", "Images");
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Watches1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

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
/* TUVisualControllerFrame* frame;
 TUVisualControllerForm* form;
 FindVisualController(PageControl1->ActivePageIndex, frame, form);
 if(frame)
  frame->UpdateInterface(true);
 if(form)
  form->UpdateInterface(true);
  */
 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::DrawShow(TObject *Sender)
{
// UDrawEngineFrame1->ReloadNet();
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::Servercontrol1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 AddSpecialFormPage("TUServerControlForm");
}
//---------------------------------------------------------------------------





void __fastcall TUGEngineControlForm::ChannelsStringGridClick(TObject *Sender)
{
/* if(UpdateInterfaceFlag)
  return;

 SelectEngine(ChannelsStringGrid->Row);
 UDrawEngineFrame1->ReloadNet();
 RDK::UIVisualControllerStorage::UpdateInterface(true);
 */
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::ChannelsStringGridSelectCell(TObject *Sender,
		  int ACol, int ARow, bool &CanSelect)
{
 if(UpdateInterfaceFlag)
  return;

 if(ChannelsStringGrid->Row != ARow)
 {
  SelectEngine(ARow);
  UDrawEngineFrame1->ReloadNet();
  RDK::UIVisualControllerStorage::UpdateInterface(true);
 }
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::AddNew1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 SetNumChannels(GetNumEngines()+1);
 RDK::UIVisualControllerStorage::UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::DeleteLast1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 SetNumChannels(GetNumEngines()-1);
 RDK::UIVisualControllerStorage::UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::DeleteAll1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 SetNumChannels(1);
 RDK::UIVisualControllerStorage::UpdateInterface(true);
}
//---------------------------------------------------------------------------




void __fastcall TUGEngineControlForm::TrayIconDblClick(TObject *Sender)
{
 if(!RdkMainForm)
  return;

 if(DisableAdminForm)
 {
  if(RdkMainForm == this)
   return;
 }

 TrayIcon->ShowBalloonHint();

 if(RdkMainForm->Visible == false)
 {
  RdkMainForm->Show();
  RDK::UIVisualControllerStorage::UpdateInterface(true);
 }
 ShowWindow(RdkMainForm->Handle,SW_RESTORE);
 SetWindowLong(RdkMainForm->Handle, GWL_EXSTYLE, GetWindowLong(RdkMainForm->Handle, GWL_EXSTYLE) | WS_EX_APPWINDOW);
 SetForegroundWindow(RdkMainForm->Handle);
 AppWinState=true;
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::Open1Click(TObject *Sender)
{
 TrayIconDblClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Hide1Click(TObject *Sender)
{
 AppMinimize(this);
 Application->Minimize();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Close1Click(TObject *Sender)
{
 Pause1Click(Sender);
 Sleep(1000);
 CloseProject();
 Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Start2Click(TObject *Sender)
{
 Start1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Pause2Click(TObject *Sender)
{
 Pause1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Reset2Click(TObject *Sender)
{
 Reset1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::FormDestroy(TObject *Sender)
{
 // Грузим настройки приложения
 String opt_name=ExtractFileName(Application->ExeName);
 if(opt_name.Length()>4)
 opt_name=opt_name.SubString(0,opt_name.Length()-4);
 TMemIniFile *app_ini=new TMemIniFile(opt_name+".ini");

 app_ini->WriteString("Server","BindAddress",ServerInterfaceAddress.c_str());
 app_ini->WriteInteger("Server","BindPort",ServerInterfacePort);

 app_ini->UpdateFile();
 delete app_ini;

 System::Set8087CW(Saved8087CW);
}
//---------------------------------------------------------------------------




void __fastcall TUGEngineControlForm::AddChannel1Click(TObject *Sender)
{
 AddNew1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::DelChannel1Click(TObject *Sender)
{
 DeleteLast1Click(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::VideoRegistration1Click(TObject *Sender)
{
#ifdef RDK_VIDEO
 VideoRegistratorForm->Show();
#endif
}
//---------------------------------------------------------------------------




void __fastcall TUGEngineControlForm::SaveCommonDescriptions1Click(TObject *Sender)
{
 TRichEdit* RichEdit=new TRichEdit(this);
 RichEdit->Visible=false;
 RichEdit->Parent=this;

 RichEdit->PlainText=true;
 const char *p=Storage_SaveCommonClassesDescription();
 if(p)
  RichEdit->Text=p;
 Engine_FreeBufString(p);
 RichEdit->Lines->SaveToFile("CommonClassesDescription.xml");

 delete RichEdit;
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::SaveClassesDescriptions1Click(TObject *Sender)

{
 TRichEdit* RichEdit=new TRichEdit(this);
 RichEdit->Visible=false;
 RichEdit->Parent=this;

 RichEdit->PlainText=true;
 const char *p=Storage_SaveClassesDescription();
 if(p)
  RichEdit->Text=p;
 Engine_FreeBufString(p);
 RichEdit->Lines->SaveToFile("ClassesDescription.xml");

 delete RichEdit;
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::UComponentsListFrame1StringGridMouseEnter(TObject *Sender)

{
 if(Active)
  UComponentsListFrame1->StringGridMouseEnter(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::UComponentsListFrame1NiceParamValRichEditMouseEnter(TObject *Sender)

{
 if(Active)
  UComponentsListFrame1->NiceParamValRichEditMouseEnter(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::EnchancedSG1BasicStringGridMouseEnter(TObject *Sender)

{
 if(Active)
  UComponentsListFrame1->EnchancedSG1BasicStringGridMouseEnter(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::EnchancedSG2BasicStringGridMouseEnter(TObject *Sender)

{
 if(Active)
  UComponentsListFrame1->EnchancedSG2BasicStringGridMouseEnter(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::UComponentsListFrame1NiceStateValRichEditMouseEnter(TObject *Sender)

{
 if(Active)
  UComponentsListFrame1->NiceStateValRichEditMouseEnter(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::Insert1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 AddChannel(ChannelsStringGrid->Row);
 RDK::UIVisualControllerStorage::UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::DeleteSelected1Click(TObject *Sender)
{
 if(!ProjectOpenFlag)
  return;

 DelChannel(ChannelsStringGrid->Row);
 RDK::UIVisualControllerStorage::UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::InsertChannel1Click(TObject *Sender)
{
 Insert1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::DeleteSelectedChannel1Click(TObject *Sender)

{
 DeleteSelected1Click(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TUGEngineControlForm::ApplicationOptions1Click(TObject *Sender)
{
 ApplicationOptionsForm->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::ClassesDescription1Click(TObject *Sender)
{
 UClassesDescriptionsForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::StartChannel1Click(TObject *Sender)
{
 StartChannel(GetSelectedEngineIndex());
 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::PauseChannel1Click(TObject *Sender)
{
 PauseChannel(GetSelectedEngineIndex());
 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUGEngineControlForm::ResetChannel1Click(TObject *Sender)
{
 ResetChannel(GetSelectedEngineIndex());
 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------

