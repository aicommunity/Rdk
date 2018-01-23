#ifndef UApplication_CPP
#define UApplication_CPP

#include "UApplication.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"

#ifndef __BORLANDC__
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#endif

using namespace std;


#ifdef __BORLANDC__
//#include "Bcb/Application.bcb.cpp"
#endif

extern void ExceptionHandler(int channel_index); // TODO: Потом ее куда то убрать

namespace RDK {

#ifndef __BORLANDC__
namespace po = boost::program_options;

po::options_description CmdLineDescription("Allowed options");
po::variables_map CmdVariablesMap;
#endif

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UApplication::UApplication(void)
{
 Name="Application";
 LastProjectsListMaxSize=10;
 ProjectOpenFlag=false;
 TestMode=false;
 CloseAfterTest=true;
 AppIsInit = false;
// DebugMode=false;
}

UApplication::~UApplication(void)
{

}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Имя файла приложения
const std::string& UApplication::GetApplicationFileName(void) const
{
 return ApplicationFileName;
}

bool UApplication::SetApplicationFileName(const std::string& value)
{
 if(ApplicationFileName == value)
  return true;
 ApplicationFileName=value;
 return true;
}

/// Рабочий каталог
const std::string& UApplication::GetWorkDirectory(void) const
{
 return WorkDirectory;
}

bool UApplication::SetWorkDirectory(const std::string& value)
{
 if(WorkDirectory == value)
  return true;
 WorkDirectory=value;
 UpdateLoggers();
 return true;
}

// Признак наличия открытого проекта
bool UApplication::GetProjectOpenFlag(void) const
{
 return ProjectOpenFlag;
}

bool UApplication::SetProjectOpenFlag(bool value)
{
 ProjectOpenFlag=value;
 CalcAppCaption();
 return true;
}

// Путь до папки проекта
const std::string& UApplication::GetProjectPath(void) const
{
 return ProjectPath;
}

bool UApplication::SetProjectPath(const std::string& value)
{
 if(ProjectPath == value)
  return true;
 ProjectPath=value;
 UpdateLoggers();
// EngineControl->GetEngineStateThread()->CloseEventsLogFile();
 CalcAppCaption();
 return true;
}

// Имя файла проекта
const std::string& UApplication::GetProjectFileName(void) const
{
 return ProjectFileName;
}

bool UApplication::SetProjectFileName(const std::string& value)
{
 if(ProjectFileName == value)
  return true;
 ProjectFileName=value;
 CalcAppCaption();
 return true;
}

/// Список последних открытых проектов
const std::list<std::string>& UApplication::GetLastProjectsList(void) const
{
 return LastProjectsList;
}

bool UApplication::SetLastProjectsList(const std::list<std::string>& value)
{
 if(LastProjectsList == value)
  return true;
 LastProjectsList=value;
 return true;
}

/// Размер истории последних открытых проектов
int UApplication::GetLastProjectsListMaxSize(void) const
{
 return LastProjectsListMaxSize;
}

bool UApplication::SetLastProjectsListMaxSize(int value)
{
 if(LastProjectsListMaxSize == value)
  return true;
 LastProjectsListMaxSize=value;
 return true;
}

/// Заголовок приложения
const std::string& UApplication::GetAppCaption(void) const
{
 return AppCaption;
}

// Файл настроек проекта
const RDK::USerStorageXML& UApplication::GetProjectXml(void) const
{
 return ProjectXml;
}

// Файл настроек интефрейса
const RDK::USerStorageXML& UApplication::GetInterfaceXml(void) const
{
 return InterfaceXml;
}


/// Каталог логов
std::string UApplication::GetLogDir(void) const
{
 return Core_GetLogDir();
}

bool UApplication::SetLogDir(const std::string& value)
{
 if(value == Core_GetLogDir())
  return true;

// LogDir=value;
 if(Core_SetLogDir(value.c_str()) == RDK_SUCCESS)
 {
  UpdateLoggers();
  return true;
 }
 return false;
}

/// Флаг включения отладочного режима логирования
bool UApplication::GetDebugMode(void) const
{
 return Core_GetDebugMode();
}

bool UApplication::SetDebugMode(bool value)
{
 if(value == Core_GetDebugMode())
  return true;
 if(Core_SetDebugMode(value) == RDK_SUCCESS)
  return true;
 return true;
}


/// Текущий каталог логов (с учетом переопределения в проекте)
std::string UApplication::CalcCurrentLogDir(void) const
{
 std::string log_dir;
 if(Project && Project->GetConfig().OverrideLogParameters && !ProjectPath.empty())
 {
  log_dir=ProjectPath+"EventsLog/";
 }
 else
 {
  log_dir=Core_GetLogDir();
  if(log_dir.empty())
   log_dir=WorkDirectory+"EventsLog/";
  if(!log_dir.empty() && log_dir.find_last_of("\\/") != log_dir.size()-1)
   log_dir+="/";
 }
 return log_dir;
}

/// Флаг, выставляется если включен режим тестирования
bool UApplication::IsTestMode(void) const
{
 return TestMode;
}

/// Имя файла с описанием тестов
const std::string& UApplication::GetTestsDescriptionFileName(void) const
{
 return TestsDescriptionFileName;
}

void UApplication::SetTestsDescriptionFileName(const std::string& value)
{
 if(TestsDescriptionFileName == value)
  return;
 TestsDescriptionFileName=value;
}

/// Признак требования завершить работу приложения после тестирования
bool UApplication::IsCloseAfterTest(void) const
{
 return CloseAfterTest;
}

/// Приложение инициализированно
bool UApplication::IsInit(void) const
{
 return AppIsInit;
}
// --------------------------

// --------------------------
// Методы инициализации
// --------------------------
/// Предоставляет доступ к диспетчеру команд
UEPtr<URpcDispatcher> UApplication::GetRpcDispatcher(void)
{
 return RpcDispatcher;
}

/// Устанавливает новый диспетчер команд
/// Ответственность за освобождение памяти диспетчера лежит на вызывающей стороне
bool UApplication::SetRpcDispatcher(const UEPtr<URpcDispatcher> &value)
{
 if(RpcDispatcher == value)
  return true;

 if(RpcDispatcher)
  RpcDispatcher->SetApplication(0);
 RpcDispatcher=value;
 RpcDispatcher->SetApplication(this);
 return true;
}

/// Предоставляет доступ к контроллеру движка
UEPtr<UEngineControl> UApplication::GetEngineControl(void)
{
 return EngineControl;
}

/// Устанавливает новый контроллер движка
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
bool UApplication::SetEngineControl(const UEPtr<UEngineControl> &value)
{
 if(EngineControl == value)
  return true;

 if(EngineControl)
 {
  EngineControl->PauseChannel(-1);
  EngineControl->SetApplication(0);
 }

 EngineControl=value;
 EngineControl->SetApplication(this);
 return true;
}

/// Предоставляет доступ к проекту
/*UEPtr<UProject> UApplication::GetProject(void)
{
 return Project;
} */

/// Устанавливает новый проект
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
bool UApplication::SetProject(const UEPtr<UProject> &value)
{
 if(Project == value)
  return true;

 // TODO: Здесь какие-то завершающие действия со старым проектом.
 Project=value;
 return true;
}

/// Возвращает конфигурацию проекта
const TProjectConfig& UApplication::GetProjectConfig(void) const
{
 return Project->GetConfig();
}

/// Устанавливает новую конфигурацию проекта
bool UApplication::SetProjectConfig(const TProjectConfig& value)
{
 if(!Project)
  return false;
 if(!Project->SetConfig(value))
  return false;

 GetCore()->GetLogger(RDK_GLOB_MESSAGE)->SetEventsLogMode(value.EventsLogFlag);
// EngineControl->GetEngineStateThread()->SetLogFlag(value.EventsLogFlag);
 return true;
}

/// Предоставляет доступ к контроллеру серверной части
UEPtr<UServerControl> UApplication::GetServerControl(void) const
{
 return ServerControl;
}

/// Устанавливает новый контроллер сервера
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
bool UApplication::SetServerControl(const UEPtr<UServerControl> &value)
{
 if(ServerControl == value)
  return true;

 // TODO: Здесь какие-то завершающие действия со старым сервером
 if(ServerControl)
  ServerControl->SetApplication(0);
 ServerControl=value;
 ServerControl->SetApplication(this);
 return true;
}

/// Менеджер тестов
/// Ответственность за освобождение памяти менеджера лежит на вызывающей стороне
UEPtr<UTestManager> UApplication::GetTestManager(void)
{
 return TestManager;
}

bool UApplication::SetTestManager(const UEPtr<UTestManager> &value)
{
 if(TestManager == value)
  return true;

 if(TestManager)
  TestManager->SetApplication(0);

 TestManager=value;
 TestManager->SetApplication(this);
 return true;
}

/// Инициализирует приложение
bool UApplication::Init(void)
{
 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Application initialization has been started.");
 Core_SetBufObjectsMode(1);

 std::string font_path=extract_file_path(ApplicationFileName);
 Core_SetSystemDir(font_path.c_str());
 SetWorkDirectory(font_path);
// SetLogDir(font_path);
 MLog_SetExceptionHandler(RDK_GLOB_MESSAGE,(void*)ExceptionHandler);
 MLog_SetExceptionHandler(RDK_SYS_MESSAGE,(void*)ExceptionHandler);
 Core_LoadFonts();

 EngineControl->Init();
 UApplication::SetNumChannels(1);
// MCore_ChannelInit(0,0,(void*)ExceptionHandler);

 LoadProjectsHistory();
 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Application initialization has been finished.");

 /*if(CommandLineArgs.size()<2)
  MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Command line parameters not found.");
 else
 {
  MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, (std::string("Parsing command line parameters: ")+concat_strings(CommandLineArgs,std::string(" "))).c_str());
  ProcessCommandLineArgs();
  MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Finished parsing command line parameters");
 }*/
 AppIsInit = true;
 return true;
}

/// Деинициализирует приложение
bool UApplication::UnInit(void)
{
 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Application uninitialization has been started.");
 RDK::UIVisualControllerStorage::ClearInterface();
 if(EngineControl)
 {
  EngineControl->PauseChannel(-1);
  EngineControl->StopEngineStateThread();
 }
 Sleep(10);
 CloseProject();
 EngineControl->UnInit();
 GetCore()->Destroy();

 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Application uninitialization has been finished.");
 AppIsInit = false;
 return true;
}


/// Проводит тестирование приложения, если менеджер тестов инициализирован и
/// тестовый режим включен
/// Возвращает код ошибки тестирования.
/// Если exit_request == true,
/// то по завершении метода приложение должно быть закрыто с возвращенным кодом ошибки
int UApplication::Test(bool &exit_request)
{
 exit_request=false;
 int test_result_code(0);
 if(IsTestMode())
 {
  exit_request=CloseAfterTest;
  if(TestManager)
  {
   if(TestManager->LoadTests(TestsDescriptionFileName) != RDK_SUCCESS)
   {
	MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Failed to load tests!");
	test_result_code=1000;
	ChangeTestModeState(false);
	return test_result_code;
   }

   MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Testing started");
   test_result_code=TestManager->ProcessTests();
   MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, (std::string("Testing finished with code: ")+sntoa(test_result_code)).c_str());
  }
 }
 ChangeTestModeState(false);
 return test_result_code;
}

/// Осуществляет парсинг командной строки и соответствующую настройку приложение
void UApplication::ProcessCommandLineArgs(std::vector<std::string> commandLineArgs)
{
  InitCmdParser();
  if(commandLineArgs.empty())
    return;

  std::vector<std::string>::iterator I=find(commandLineArgs.begin(),commandLineArgs.end(),"--test");
  if(I != commandLineArgs.end())
  {
    ++I;
    std::string configuration_name;
    if(I != commandLineArgs.end())
    {
      configuration_name = *I;
      SetTestsDescriptionFileName(configuration_name);
      ChangeTestModeState(true);
    }
    else
      ChangeTestModeState(false);
  }
  else
  {
    ChangeTestModeState(false);
  }

  I=find(commandLineArgs.begin(),commandLineArgs.end(),"--run");
  if(I != commandLineArgs.end())
    CloseAfterTest=true;
  else
    CloseAfterTest=false;
}

#ifndef __BORLANDC__
/// Осуществляет парсинг командной строки и записывает результаты в CommandLineArgs
void UApplication::ProcessCommandLineArgs(int argc, char **argv)
{
  InitCmdParser();

  po::store(po::parse_command_line(argc, argv, CmdLineDescription), CmdVariablesMap);
  po::notify(CmdVariablesMap);

  if(CmdVariablesMap.count("test"))
  {
	ChangeTestModeState(true);
	SetTestsDescriptionFileName(CmdVariablesMap["test"].as<std::string>());
  }
  else
	ChangeTestModeState(false);

  if(CmdVariablesMap.count("run"))
	CloseAfterTest=false;
  else
	CloseAfterTest=true;
}
#endif
// --------------------------

// --------------------------
// Методы управления проектом
// --------------------------
/// Создает проект (через сохранение и открытие)
bool UApplication::CreateProject(const std::string &file_name, RDK::TProjectConfig &project_config)
{
 CloseProject();

 ProjectOpenFlag=true;
 ProjectPath=extract_file_path(file_name);
 Project->SetConfig(project_config);
 Project->SetProjectPath(ProjectPath);
 ProjectFileName=extract_file_name(file_name);

 UApplication::SetNumChannels(project_config.NumChannels);

 for(int i=0;i<project_config.NumChannels;i++)
 {
  RDK::TProjectChannelConfig &channel=project_config.ChannelsConfig[i];

  if(!MCore_IsChannelInit(i))
   MCore_ChannelInit(i,channel.PredefinedStructure,(void*)ExceptionHandler);
  else
   MEnv_SetPredefinedStructure(i,channel.PredefinedStructure);

  if(channel.PredefinedStructure == 0 && !channel.ClassName.empty())
  {
   MModel_Create(i,channel.ClassName.c_str());
  }
 }

 if(SaveProject())
 {
  fstream file((ProjectPath+project_config.DescriptionFileName).c_str(),ios::out | ios::trunc);
  file<<project_config.ProjectDescription;
  file.flush();
  file.close();
  return OpenProject(ProjectPath+ProjectFileName);
 }
 else
  return false;
}

/// Открывает проект
bool UApplication::OpenProject(const std::string &filename)
{
 CloseProject();

 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, (std::string("Open project ")+filename+"...").c_str());
 ProjectXml.LoadFromFile(filename,"");
 ProjectPath=extract_file_path(filename);
 ProjectFileName=extract_file_name(filename);
 Project->SetProjectPath(ProjectPath);
 Project->ReadFromXml(ProjectXml);
 UpdateLoggers();

 TProjectConfig config=Project->GetConfig();
 GetCore()->GetLogger(RDK_GLOB_MESSAGE)->SetEventsLogMode(config.EventsLogFlag);
 GetCore()->GetLogger(RDK_GLOB_MESSAGE)->Clear();
// EngineControl->GetEngineStateThread()->SetLogFlag(config.EventsLogFlag);
// EngineControl->GetEngineStateThread()->CloseEventsLogFile();
// EngineControl->GetEngineStateThread()->SetLogDir(ProjectPath);

 EngineControl->SetThreadMode(config.MultiThreadingMode);
 CalcAppCaption();

try{

 EngineControl->SetNumChannels(config.NumChannels);

 ProjectXml.SelectNodeRoot("Project/MultiGeneral");
 int selected_channel_index=ProjectXml.ReadInteger("SelectedChannelIndex",0);

 ProjectXml.SelectNodeRoot("Project/General");

 if(LoadFile(ProjectPath+config.DescriptionFileName,config.ProjectDescription))
 {
  Project->SetConfig(config);
  Project->ResetModified();
 }

 for(int i=0;i<config.NumChannels;i++)
 {
  try
  {
   TProjectChannelConfig &channel_config=config.ChannelsConfig[i];
   EngineControl->SetCalculationTimeSource(i, config.CalcSourceTimeMode);
   EngineControl->SetMinInterstepsInterval(i,channel_config.MinInterstepsInterval);

   Core_SelectChannel(i);
   if(!Core_IsChannelInit())
	Core_ChannelInit(channel_config.PredefinedStructure,(void*)ExceptionHandler);
   else
	Env_SetPredefinedStructure(channel_config.PredefinedStructure);

   // TODO: Реалиовать загрузку описаний классов
   // Загрузка описаний классов
   Model_SetDefaultTimeStep(channel_config.DefaultTimeStep);
   Log_SetDebugMode(config.DebugMode);
   Log_SetDebugSysEventsMask(config.DebugSysEventsMask);
   Log_SetEventsLogMode(config.EventsLogMode);
   Log_SetDebuggerMessageFlag(config.DebuggerMessageFlag);
   Env_SetCurrentDataDir(ProjectPath.c_str());
   Env_CreateStructure();
   Env_Init();

   if(channel_config.PredefinedStructure == 0 && !channel_config.ModelFileName.empty())
   {
	if(extract_file_path(channel_config.ModelFileName).empty())
	{
	 LoadModelFromFile(i,ProjectPath+channel_config.ModelFileName);
	}
	else
	 LoadModelFromFile(i,channel_config.ModelFileName);
   }

   if(!channel_config.ParametersFileName.empty())
   {
	if(extract_file_path(channel_config.ParametersFileName).empty())
	 LoadParametersFromFile(i,ProjectPath+channel_config.ParametersFileName);
	else
	 LoadParametersFromFile(i,channel_config.ParametersFileName);
   }

   if(config.ProjectAutoSaveStatesFlag)
   {
	if(!channel_config.StatesFileName.empty())
	{
	 if(extract_file_path(channel_config.StatesFileName).empty())
	  LoadStatesFromFile(i,ProjectPath+channel_config.StatesFileName);
	 else
	  LoadStatesFromFile(i,channel_config.StatesFileName);
	}
   }

   if(Model_Check())
   {
	Model_SetGlobalTimeStep("",channel_config.GlobalTimeStep);
	if(channel_config.InitAfterLoad)
	 MEnv_Init(i);
	if(channel_config.ResetAfterLoad)
	 MEnv_Reset(i,0);
   }

   EngineControl->SetCalculateMode(i, channel_config.CalculationMode);
  }
  catch(RDK::UException &exception)
  {
   MLog_LogMessage(RDK_SYS_MESSAGE, exception.GetType(), (std::string("Core-OpenProject(Load Channel) Exception: (Name=")+std::string(Name.c_str())+std::string(") ")+exception.what()).c_str());
  }
  Sleep(0);
 }

 if(selected_channel_index>=GetNumChannels())
  selected_channel_index=0;

 Core_SelectChannel(selected_channel_index);
 InterfaceXml.Destroy();

 if(!config.InterfaceFileName.empty())
 {
  if(extract_file_path(config.InterfaceFileName).empty())
   InterfaceXml.LoadFromFile(ProjectPath+config.InterfaceFileName,"Interfaces");
  else
   InterfaceXml.LoadFromFile(config.InterfaceFileName,"Interfaces");

  InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 }
 ServerControl->SetNumChannels(config.NumChannels);

 RDK::UIVisualControllerStorage::LoadParameters(InterfaceXml);

 ProjectOpenFlag=true;
 EngineControl->StartEngineStateThread();

 RDK::UIVisualControllerStorage::UpdateInterface();
 RDK::UIVisualControllerStorage::AfterLoadProject();
}
catch(RDK::UException &exception)
{
// UShowProgressBarForm->Hide();
 MLog_LogMessage(RDK_SYS_MESSAGE, exception.GetType(), (std::string("Core-OpenProject Exception: (Name=")+Name+std::string(") ")+exception.what()).c_str());
}

 std::list<std::string> last_list=LastProjectsList;
 last_list.push_front(filename);
 while(int(last_list.size())>LastProjectsListMaxSize
  && !LastProjectsList.empty())
 {
  last_list.pop_back();
 }

 LastProjectsList=last_list;

 SaveProjectsHistory();

 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, (std::string("Project ")+filename+" has been opened successfully.").c_str());
 return true;
}

/// Сохраняет проект
bool UApplication::SaveProject(void)
{
 if(!ProjectOpenFlag)
  return false;

 int selected_channel_index=Core_GetSelectedChannelIndex();

 Project->WriteToXml(ProjectXml);

try
{

 InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 RDK::UIVisualControllerStorage::SaveParameters(InterfaceXml);

 TProjectConfig config=Project->GetConfig();
 ProjectXml.SelectNodeRoot("Project/General");

 SaveFile(ProjectPath+config.DescriptionFileName,config.ProjectDescription);

 if(!config.InterfaceFileName.empty())
 {
  if(extract_file_path(config.InterfaceFileName).empty())
   InterfaceXml.SaveToFile(ProjectPath+config.InterfaceFileName);
  else
   InterfaceXml.SaveToFile(config.InterfaceFileName);
 }
 else
 {
  ProjectXml.WriteString("InterfaceFileName","Interface.xml");
  InterfaceXml.SaveToFile(ProjectPath+config.InterfaceFileName);
 }

 for(int i=0;i<config.NumChannels;i++)
 {
  Core_SelectChannel(i);

  TProjectChannelConfig &channel_config=config.ChannelsConfig[i];

  if(extract_file_path(channel_config.ModelFileName).empty())
   SaveModelToFile(i, ProjectPath+channel_config.ModelFileName);
  else
   SaveModelToFile(i, channel_config.ModelFileName);

  if(extract_file_path(channel_config.ParametersFileName).empty())
   SaveParametersToFile(i, ProjectPath+channel_config.ParametersFileName);
  else
   SaveParametersToFile(i,channel_config.ParametersFileName);

  if(config.ProjectAutoSaveStatesFlag)
  {
   if(extract_file_path(channel_config.StatesFileName).empty())
	SaveStatesToFile(i, ProjectPath+channel_config.StatesFileName);
   else
	SaveStatesToFile(i, channel_config.StatesFileName);
  }

  Sleep(0);
 }

 Core_SelectChannel(selected_channel_index);


 ProjectXml.SaveToFile(ProjectPath+ProjectFileName);
}
catch(RDK::UException &exception)
{
 MLog_LogMessage(RDK_SYS_MESSAGE, exception.GetType(), (std::string("Core-SaveProject Exception: (Name=")+Name+std::string(") ")+exception.what()).c_str());
}

 return true;
}

bool UApplication::SaveProjectAs(const std::string &filename)
{
 return true;
}

/// Закрывает проект
bool UApplication::CloseProject(void)
{
 if(!ProjectOpenFlag)
  return true;

 PauseChannel(-1);

 RDK::TProjectConfig config=GetProjectConfig();

 if(config.ProjectAutoSaveFlag)
  SaveProject();

 RDK::UIVisualControllerStorage::ClearInterface();
// if(UServerControlForm)
//  UServerControlForm->ServerRestartTimer->Enabled=false;

 if(ProjectOpenFlag)
 {
  SetProjectFileName("");
  SetProjectPath("");
 }
 SetProjectOpenFlag(false);
 EngineControl->StopEngineStateThread();
 for(int i=GetNumChannels()-1;i>=0;i--)
 {
  Core_SelectChannel(i);
  if(GetEngine())
  {
   Env_UnInit();
   Model_Destroy();
  }
  Storage_ClearObjectsStorage();
 }

 return true;
}

/// Клонирует проект в новое расположение
bool UApplication::CloneProject(const std::string &filename)
{
 return true;
}

void UApplication::ReloadParameters(void)
{
 if(!ProjectOpenFlag)
  return;

 int channel_index=Core_GetSelectedChannelIndex();

 TProjectConfig config=Project->GetConfig();
 std::string params_file_name=config.ChannelsConfig[Core_GetSelectedChannelIndex()].ParametersFileName;
 if(params_file_name.empty())
 {
  config.ChannelsConfig[channel_index].ParametersFileName="Parameters.xml";
  Project->SetConfig(config);
 }

 if(!params_file_name.empty())
 {
  if(extract_file_path(params_file_name).empty())
   LoadParametersFromFile(channel_index, ProjectPath+params_file_name);
  else
   LoadParametersFromFile(channel_index, params_file_name);
 }
}

bool UApplication::CopyProject(const std::string &new_path)
{
 if(!ProjectOpenFlag)
  return false;

 if(new_path.empty())
  return false;

 if(new_path == ProjectPath)
  return true;

 SaveProject();

 RDK::CopyDir(ProjectPath, new_path, "*.*");
 return true;
}

/// Сохраняет только файл настроек проекта
bool UApplication::SaveProjectConfig(void)
{
 if(!ProjectOpenFlag)
  return false;

 Project->WriteToXml(ProjectXml);

 try
 {
  TProjectConfig config=Project->GetConfig();
  ProjectXml.SelectNodeRoot("Project/General");

  if(config.InterfaceFileName.empty())
  {
   ProjectXml.WriteString("InterfaceFileName","Interface.xml");
  }

  ProjectXml.SaveToFile(ProjectPath+ProjectFileName);
 }
 catch(RDK::UException &exception)
 {
  MLog_LogMessage(RDK_SYS_MESSAGE, exception.GetType(), (std::string("Core-SaveProjectConfig Exception: (Name=")+Name+std::string(") ")+exception.what()).c_str());
 }

 return true;
}
// --------------------------


// --------------------------
// Методы управления движком
// --------------------------
/// Управление числом каналов
int UApplication::GetNumChannels(void) const
{
 return EngineControl->GetNumChannels();
}

bool UApplication::SetNumChannels(int num)
{
 int old_num=GetNumChannels();
 if(!EngineControl->SetNumChannels(num))
  return false;

 if(!ServerControl->SetNumChannels(old_num))
  return false;

 Project->SetNumChannels(num);

 for(int i=old_num;i<num;i++)
 {
  if(!MCore_IsChannelInit(i))
   MCore_ChannelInit(i,0,(void*)ExceptionHandler);
 }

 return true;
}

bool UApplication::InsertChannel(int index)
{
 if(!EngineControl->InsertChannel(index))
  return false;

 if(!ServerControl->InsertChannel(index))
  return false;

 Project->InsertChannel(index);

 if(!MCore_IsChannelInit(index))
  MCore_ChannelInit(index,0,(void*)ExceptionHandler);
 return true;
}

bool UApplication::DeleteChannel(int index)
{
 if(!EngineControl->DeleteChannel(index))
  return false;

 if(!ServerControl->DeleteChannel(index))
  return false;

 Project->DeleteChannel(index);

 return true;
}

/// Клонирует канал source_id в cloned_id
bool UApplication::CloneChannel(int source_id, int cloned_id)
{
 if(source_id<0 || cloned_id <0 || source_id>=GetNumChannels() || cloned_id >=GetNumChannels())
  return false;

 try
 {
  RDK::TProjectConfig config=GetProjectConfig();
  config.ChannelsConfig[cloned_id]=config.ChannelsConfig[source_id];

  TProjectChannelConfig &source_channel=config.ChannelsConfig[source_id];
  TProjectChannelConfig &cloned_channel=config.ChannelsConfig[cloned_id];

  // Меняем номера индексов в именах файлов модели, параметров и состояний
  if(cloned_id>0)
   cloned_channel.ModelFileName=std::string("model_")+sntoa(cloned_id+1)+".xml";
  else
   cloned_channel.ModelFileName=std::string("model")+".xml";

  if(cloned_id>0)
   cloned_channel.ParametersFileName=std::string("parameters_")+sntoa(cloned_id+1)+".xml";
  else
   cloned_channel.ParametersFileName=std::string("parameters")+".xml";

  if(cloned_id>0)
   cloned_channel.StatesFileName=std::string("states_")+sntoa(cloned_id+1)+".xml";
  else
   cloned_channel.StatesFileName=std::string("states")+".xml";

  int selected_engine=Core_GetSelectedChannelIndex();
  Core_SelectChannel(cloned_id);

  if(!Core_IsChannelInit())
   Core_ChannelInit(cloned_channel.PredefinedStructure,reinterpret_cast<void*>(ExceptionHandler));
  else
   Env_SetPredefinedStructure(cloned_channel.PredefinedStructure);

   Model_SetDefaultTimeStep(cloned_channel.DefaultTimeStep);
   Log_SetDebugMode(config.DebugMode);
   Log_SetDebugSysEventsMask(config.DebugSysEventsMask);
   Log_SetEventsLogMode(config.EventsLogMode);
   Log_SetDebuggerMessageFlag(config.DebuggerMessageFlag);
   Env_SetCurrentDataDir(ProjectPath.c_str());
   Env_CreateStructure();
   Env_Init();

 if(cloned_channel.PredefinedStructure == 0 && !cloned_channel.ModelFileName.empty())
 {
  if(extract_file_path(cloned_channel.ModelFileName).empty())
  {
   RdkCopyFile(ProjectPath+source_channel.ModelFileName, ProjectPath+cloned_channel.ModelFileName);
   LoadModelFromFile(cloned_id, ProjectPath+cloned_channel.ModelFileName);
  }
  else
  {
   RdkCopyFile(source_channel.ModelFileName, ProjectPath+cloned_channel.ModelFileName);
   LoadModelFromFile(cloned_id, cloned_channel.ModelFileName);
  }
 }

 if(!cloned_channel.ParametersFileName.empty())
 {
  if(extract_file_path(cloned_channel.ParametersFileName).empty())
  {
   RdkCopyFile(ProjectPath+source_channel.ParametersFileName, ProjectPath+cloned_channel.ParametersFileName);
   LoadParametersFromFile(cloned_id, ProjectPath+cloned_channel.ParametersFileName);
  }
  else
  {
   RdkCopyFile(source_channel.ParametersFileName, ProjectPath+cloned_channel.ParametersFileName);
   LoadParametersFromFile(cloned_id, cloned_channel.ParametersFileName);
  }
 }

 if(config.ProjectAutoSaveStatesFlag)
 {
  if(!cloned_channel.StatesFileName.empty())
  {
   if(extract_file_path(cloned_channel.StatesFileName).empty())
   {
	RdkCopyFile(ProjectPath+source_channel.StatesFileName, ProjectPath+cloned_channel.StatesFileName);
	LoadStatesFromFile(cloned_id, ProjectPath+cloned_channel.StatesFileName);
   }
   else
   {
	RdkCopyFile(source_channel.StatesFileName, ProjectPath+cloned_channel.StatesFileName);
	LoadStatesFromFile(cloned_id, cloned_channel.StatesFileName);
   }
  }
 }

 if(Model_Check())
 {
  Model_SetGlobalTimeStep("",cloned_channel.GlobalTimeStep);
  if(cloned_channel.InitAfterLoad)
   MEnv_Init(cloned_id);
  if(cloned_channel.ResetAfterLoad)
   MEnv_Reset(cloned_id,0);
 }
 EngineControl->SetCalculateMode(cloned_id, cloned_channel.CalculationMode);

 Core_SelectChannel(selected_engine);
 SetProjectConfig(config);
}
catch(RDK::UException &exception)
{
 MLog_LogMessage(RDK_SYS_MESSAGE, exception.GetType(), (std::string("Core-OpenCloneChannel Exception: (Name=")+Name+std::string(") ")+exception.what()).c_str());
}
catch(...)
{
 throw;
}

 return true;
}


// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
/// Запускает аналитику выбранного канала, или всех, если channel_index == -1
void UApplication::StartChannel(int channel_index)
{
 EngineControl->StartChannel(channel_index);
}

/// Останавливает аналитику выбранного канала, или всех, если channel_index == -1
void UApplication::PauseChannel(int channel_index)
{
 EngineControl->PauseChannel(channel_index);
}

/// Сбрасывает аналитику выбранного канала, или всех, если channel_index == -1
void UApplication::ResetChannel(int channel_index)
{
 EngineControl->ResetChannel(channel_index);
}

/// Делает шаг расчета выбранного канала, или всех, если channel_index == -1
void UApplication::StepChannel(int channel_index)
{
 EngineControl->StepChannel(channel_index);
}

/// Возвращает true если канал запущен
bool UApplication::IsChannelStarted(int channel_index)
{
 if(!EngineControl)
  return false;
 return (EngineControl->CheckCalcState(channel_index) == UEngineControl::csRunning);
}

/// Проверяет состояние расчета по id канала
UEngineControl::UCalcState UApplication::CheckCalcState(int channel_id) const
{
 if(!EngineControl)
  return UEngineControl::csUnknown;
 return EngineControl->CheckCalcState(channel_id);
}
// --------------------------

// --------------------------
// Методы загрузки сохранения данных в файл
// --------------------------
bool UApplication::LoadModelFromFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
  return false;

 if(!data.empty())
 {
  MModel_Destroy(channel_index);
  MModel_LoadComponent(channel_index, "",data.c_str());
  return true;
 }
 return false;
}

bool UApplication::SaveModelToFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 const char *p_buf=MModel_SaveComponent(channel_index, "");
 bool res=true;
 if(p_buf)
 {
  res=SaveFile(file_name,p_buf);
 }
 Engine_FreeBufString(p_buf);
 return res;
}

bool UApplication::LoadParametersFromFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
  return false;

 if(!data.empty())
 {
  MModel_LoadComponentParameters(channel_index, "",data.c_str());
  return true;
 }
 return false;
}

bool UApplication::SaveParametersToFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 const char *p_buf=MModel_SaveComponentParameters(channel_index, "");
 bool res=true;
 if(p_buf)
 {
  res=SaveFile(file_name,p_buf);
 }
 Engine_FreeBufString(p_buf);
 return res;
}

bool UApplication::LoadStatesFromFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
  return false;

 if(!data.empty())
 {
  int i=Core_GetSelectedChannelIndex();
  Core_SelectChannel(channel_index);
  Model_LoadComponentState("",&data[0]);
  Core_SelectChannel(i);
  return true;
 }
 return false;
}

bool UApplication::SaveStatesToFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 int i=Core_GetSelectedChannelIndex();
 Core_SelectChannel(channel_index);
 const char *p_buf=Model_SaveComponentState("");
 bool res=true;
 if(p_buf)
 {
  res=SaveFile(file_name,p_buf);
 }
 Engine_FreeBufString(p_buf);
 Core_SelectChannel(i);
 return res;
}

bool UApplication::LoadDescriptionFromFile(int channel_index, const std::string &file_name)
{
 return false;
}

bool UApplication::SaveDescriptionToFile(int channel_index, const std::string &file_name)
{
 return false;
}

bool UApplication::LoadClassesDescriptionsFromFile(const std::string &file_name)
{
 if(!Core_IsChannelInit())
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
  return false;

 if(!data.empty())
 {
  Storage_LoadClassesDescription(data.c_str());
  return true;
 }
 return false;
}

bool UApplication::SaveClassesDescriptionsToFile(const std::string &file_name)
{
 const char *p=Storage_SaveClassesDescription();
 bool res=true;
 if(p)
 {
  res=SaveFile(file_name,p);
  Engine_FreeBufString(p);
 }
 return res;
}

bool UApplication::LoadCommonClassesDescriptionsFromFile(const std::string &file_name)
{
 if(!Core_IsChannelInit())
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
  return false;

 if(!data.empty())
 {
  Storage_LoadCommonClassesDescription(data.c_str());
  return true;
 }
 return false;
}

bool UApplication::SaveCommonClassesDescriptionsToFile(const std::string &file_name)
{
 const char *p=Storage_SaveCommonClassesDescription();
 bool res=true;
 if(p)
 {
  res=SaveFile(file_name,p);
  Engine_FreeBufString(p);
 }

 return res;
}

/// Загружает историю проектов из файла
void UApplication::LoadProjectsHistory(void)
{
 std::string opt_name=extract_file_name(ApplicationFileName);
 if(opt_name.size()>4)
 opt_name=opt_name.substr(0,opt_name.size()-4);
 opt_name=opt_name+".projecthist";
 RDK::UIniFile<char> history_ini;
 history_ini.LoadFromFile(opt_name.c_str());
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
void UApplication::SaveProjectsHistory(void)
{
 RDK::UIniFile<char> history_ini;

 std::list<std::string>::iterator I=LastProjectsList.begin();
 int i=0;
 for(;I != LastProjectsList.end();I++)
 {
  history_ini("General",std::string("Hist")+RDK::sntoa(i++),*I);
 }

 std::string opt_name=extract_file_name(ApplicationFileName);
 if(opt_name.size()>4)
 opt_name=opt_name.substr(0,opt_name.size()-4);
 opt_name=opt_name+".projecthist";
 history_ini.SaveToFile(opt_name);
}
// --------------------------

// --------------------------
// Вспомогательные методы управления счетом
// --------------------------
/// Включает и выключает тестовый режим
void UApplication::ChangeTestModeState(bool state)
{
 if(TestMode == state)
  return;

 TestMode=state;
 if(TestMode == true)
  MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Test mode is ON.");
 else
  MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Test mode is OFF.");
}


/// Инициализация парсера командной строки
void UApplication::InitCmdParser(void)
{
#ifndef __BORLANDC__
 CmdLineDescription.add_options()
	("help", "produce help message")
	("test", po::value<string>(), "Test file name")
	("run", "Run application after test");
#endif
}
/*
int UApplication::ParseArgs(const std::vector<std::string> &args, std::map<std::string,std::string> &parsed_args)
{
 if(args.empty())
  return RDK_SUCCESS;

 parsed_args["Application"]=args[0];
 size_t index=1;
 for(size_t i=1;i<args.size()-1;i++)
 {
  parsed_args[args[i]]=args[i+1];
  index+=2;
 }

 return RDK_SUCCESS;
} */


/// Вычисляет заголовок приложения
void UApplication::CalcAppCaption(void)
{
 AppCaption=std::string("[")+Project->GetConfig().ProjectName+std::string(": ")+ProjectPath+ProjectFileName+"]";
}

/// Обновляет состояние средств логгирования
void UApplication::UpdateLoggers(void)
{
 RdkCoreManager.SetLogDir(CalcCurrentLogDir().c_str());
 if(EngineControl && EngineControl->GetEngineStateThread())
 {
  GetCore()->GetLogger(RDK_GLOB_MESSAGE)->RecreateEventsLogFile();
 }
//  EngineControl->GetEngineStateThread()->RecreateEventsLogFile();
}
// --------------------------

}

#endif

