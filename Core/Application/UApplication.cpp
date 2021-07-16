#ifndef UApplication_CPP
#define UApplication_CPP

#include "UApplication.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

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
 ConfigsMainPath="../../Configs/";
 //ModelsMainPath="../../Models/";
 ModelsMainPath="../../../Models/";
 ChangeUseNewXmlFormatProjectFile(false);
 ChangeUseNewProjectFilesStructure(false);
 StorageBuildMode = 1;
 LogCreationMode=0;
 CalcStopLogLevel=RDK_EX_FATAL;
 UserName="";
 UserId=-1;
 //SetStandartXMLInCatalog();


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
 if(FixedLogPath.empty())
  UpdateLoggers();
 return true;
}

/// Относительный путь до папки с хранилищем конфигураций (обычно /Bin/Configs)
const std::string& UApplication::GetConfigsMainPath(void) const
{
 return ConfigsMainPath;
}

bool UApplication::SetConfigsMainPath(const std::string &value)
{
 if(ConfigsMainPath == value)
  return true;
 ConfigsMainPath=value;
 return true;
}

/// Относительный путь до папки с библиотеками (в данном пути сформируется две папки - MockLibs, RTlibs)
const std::string& UApplication::GetLibrariesPath(void) const
{
 return LibrariesPath;
}

bool UApplication::SetLibrariesPath(const std::string &value)
{
 if(LibrariesPath == value)
  return true;
 LibrariesPath=value;
 return true;
}

/// Относительный путь до папки с хранилищем конфигураций (обычно /Bin/Configs)
const std::string& UApplication::GetDatabaseMainPath(void) const
{
 return DatabaseMainPath;
}

bool UApplication::SetDatabaseMainPath(const std::string &value)
{
 if(DatabaseMainPath == value)
  return true;
 DatabaseMainPath=value;
 return true;
}

/// Относительный путь до папки с хранилищем конфигураций (обычно /Bin/Configs)
const std::string& UApplication::GetStorageMountPoint(void) const
{
 return StorageMountPoint;
}

bool UApplication::SetStorageMountPoint(const std::string &value)
{
 if(StorageMountPoint == value)
  return true;
 StorageMountPoint=value;
 return true;
}


/// Относительный путь до папки с хранилищем моделей  (обычно /Bin/Models)
const std::string& UApplication::GetModelsMainPath(void) const
{
 return ModelsMainPath;
}

bool UApplication::SetModelsMainPath(const std::string &value)
{
 if(ModelsMainPath == value)
  return true;
 ModelsMainPath=value;
 return true;
}

const std::string& UApplication::GetUserName(void) const
{
    return UserName;
}

bool UApplication::SetUserName(const std::string &value)
{
    if(UserName == value)
     return true;
    UserName=value;
    return true;
}

int UApplication::GetUserId(void) const
{
    return UserId;
}

bool UApplication::SetUserId(int value)
{
    if(UserId == value)
     return true;
    UserId=value;
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
 if(LogCreationMode == 0 || LogCreationMode == 1)
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
 switch(LogCreationMode)
 {
 case 0:
 case 1:
 {
  if(Project && Project->GetConfig().OverrideLogParameters && !ProjectPath.empty())
  {
   log_dir=ProjectPath+"EventsLog/";
  }
  else
  {
   if(FixedLogPath.empty())
   {
    log_dir=Core_GetLogDir();
    if(log_dir.empty())
     log_dir=WorkDirectory+"EventsLog/";
   }
   else
   {
    log_dir=FixedLogPath;
   }
   if(!log_dir.empty() && log_dir.find_last_of("\\/") != log_dir.size()-1)
    log_dir+="/";
  }
 }
 break;

 case 2:
 case 3:
 {
  if(FixedLogPath.empty())
  {
   log_dir=Core_GetLogDir();
   if(log_dir.empty())
    log_dir=WorkDirectory+"EventsLog/";
  }
  else
  {
   log_dir=FixedLogPath;
  }
  if(!log_dir.empty() && log_dir.find_last_of("\\/") != log_dir.size()-1)
   log_dir+="/";
 }
 break;
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

/// Фиксированный путь до логов
const std::string& UApplication::GetFixedLogPath(void) const
{
 return FixedLogPath;
}

bool UApplication::SetFixedLogPath(const std::string& value)
{
 if(FixedLogPath == value)
  return true;

 FixedLogPath=value;
 return true;
}

/// Режим записи логов
/// 0 - запись по умолчанию (логи создаются заново при каждом вызове Reset в папке конфигурации)
/// 1 - файл лога создается заново только при открытии каждой новой конфигурации. В папке конфигурации
/// 2 - файл лога создается заново только при открытии каждой новой конфигурации. В системной папке
/// 3 - файл лога создается единожды на весь период работы приложения в системной папке
int UApplication::GetLogCreationMode(void) const
{
 return LogCreationMode;
}

bool UApplication::SetLogCreationMode(int mode)
{
 if(LogCreationMode == mode)
  return true;

 if(mode<0 || mode>3)
  return false;

 LogCreationMode=mode;
 return true;
}

/// Уровень сообщения в логгере при появлении которого осуществляется автоматический останов расчета
int UApplication::GetCalcStopLogLevel(void) const
{
 return CalcStopLogLevel;
}

bool UApplication::SetCalcStopLogLevel(int log_level)
{
    if(CalcStopLogLevel == log_level)
     return true;

    if(CalcStopLogLevel<-1)
     return false;

    CalcStopLogLevel=log_level;
    return true;
}


/// Установка необходимого режима сборки
void UApplication::SetStorageBuildMode(int mode)
{
 // пересборка не нужна
 if(StorageBuildMode == mode)
     return;

 StorageBuildMode = mode;
 CloseProject();
 RDK::GetCoreLock()->SetStorageBuildMode(StorageBuildMode);

 int size = GetNumChannels();

 for(int i = 0; i<size;i++)
 {
     MCore_ChannelInit(i,0,(void*)ExceptionHandler);
 }
}

/// Получение текущего режима сборки
int UApplication::GetStorageBuildMode()
{
 return StorageBuildMode;
}
// --------------------------
/// Создание библиотек-заглушек из статических библиотек с сохранением файлов
void UApplication::CreateSaveMockLibs()
{
    RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock();
    if(!storage->CreateMockLibs())
        return;
    storage->SaveMockLibs();
}
// --------------------------
// Методы инициализации
// --------------------------
/// Предоставляет доступ к диспетчеру команд
/*
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
*/
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

/// Деплоер проекта (под кончретную задачу)
UEPtr<UProjectDeployer> UApplication::GetProjectDeployer(void)
{
 return ProjectDeployer;
}

 bool UApplication::SetProjectDeployer(const UEPtr<UProjectDeployer> &value)
{
     if(ProjectDeployer == value)
      return true;

     if(ProjectDeployer)
      ProjectDeployer->SetApplication(0);

     ProjectDeployer=value;
     ProjectDeployer->SetApplication(this);

     return true;
}

const std::list<StandartXMLInCatalog>&  UApplication::GetStandartXMLInCatalog(void) const
{
    return xmlInCatalog;
}

bool UApplication::SetStandartXMLInCatalog(void)
{
    //std::string path = "D:/VideoAnalytics/Rtv-VideoAnalytics/Bin/Models/";
    std::string path=GetWorkDirectory() + GetModelsMainPath();
    std::string mask = "*.xml";
    std::vector<std::string> results;
    //int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results)
    int a = FindFilesList(path, mask, true, results);
    for (size_t i=0; i< results.size(); i++)
    {
        StandartXMLInCatalog newXMLType;
        RDK::USerStorageXML XmlStorage;
        std::string tmp=results[i];
        newXMLType.XMLName=tmp;

        tmp = path + tmp;
        XmlStorage.LoadFromFile(tmp,"Save");
        std::string deskr= XmlStorage.GetNodeAttribute("ModelDescription");
        std::string name= XmlStorage.GetNodeAttribute("ModelName");

        newXMLType.XMLDescription=deskr;
        xmlInCatalog.push_back(newXMLType);
    }

    if (a>0)
        return false;
    return true;
}

/// Инициализирует приложение
bool UApplication::Init(void)
{
 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Application initialization has been started.");
 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_INFO, (std::string("Version: ")+GetCoreVersion().ToStringFull()).c_str());
 Core_SetBufObjectsMode(1);

 std::string font_path=extract_file_path(ApplicationFileName);
 Core_SetSystemDir(font_path.c_str());
 SetWorkDirectory(font_path);
// SetLogDir(font_path);
 MLog_SetExceptionHandler(RDK_GLOB_MESSAGE,(void*)ExceptionHandler);
 MLog_SetExceptionHandler(RDK_SYS_MESSAGE,(void*)ExceptionHandler);
 Core_LoadFonts();

 EngineControl->Init();
 RDK::GetCoreLock()->SetLibrariesPath(LibrariesPath);
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
 SetStandartXMLInCatalog();

 AppIsInit = true;
 return true;
}

/// Деинициализирует приложение
bool UApplication::UnInit(void)
{
 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, "Application uninitialization has been started.");
 if(EngineControl)
 {
  EngineControl->PauseChannel(-1);
  EngineControl->StopEngineStateThread();
 }
 Sleep(10);
 CloseProject();
 EngineControl->UnInit();
 GetCoreLock()->Destroy();

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

  try
  {
   po::store(po::parse_command_line(argc, argv, CmdLineDescription), CmdVariablesMap);
   po::notify(CmdVariablesMap);
  }
  catch(po::unknown_option &ex)
  {
   MLog_LogMessage(RDK_GLOB_MESSAGE,RDK_EX_WARNING,ex.what());
   throw ex;
   return;
  }

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

  if(channel.PredefinedStructure == 0 && channel.ClassName.empty()
     && !channel.ModelFileName.empty())
  {
   std::string modelXml;
   if(LoadFile(channel.ModelFileName, modelXml))
   {
	 MModel_LoadComponent(i, "", modelXml.c_str());
	 if(Project->GetForceNewConfigFilesStructure())
	  channel.ModelFileName = std::string("Model_")+RDK::sntoa(i,2)+".xml";
	 else
	  channel.ModelFileName = (i == 0) ? std::string("model.xml") : std::string("model_")+RDK::sntoa(i)+".xml";
   }
  }
 }

 ProjectOpenFlag=true;
 ProjectPath=extract_file_path(file_name);
 Project->SetConfig(project_config);
 Project->SetForceNewConfigFilesStructure(true);
 Project->SetProjectPath(ProjectPath);
 ProjectFileName=extract_file_name(file_name);

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

bool UApplication::CreateProject(const std::string &file_name, const std::string &model_classname)
{
 RDK::TProjectConfig project_config;

 project_config.DebugMode=true;
 project_config.SetNumChannels(1);
 project_config.ProjectMode=0;
 project_config.ProjectName="Autocreated configuration";
 project_config.UserId = UserId;
 project_config.UserName = UserName;
 time_t time_data;
 time(&time_data);
 project_config.CreationTime = RDK::get_text_time(time_data, '.', '_');
 project_config.ProjectType=0;
 project_config.EventsLogFlag=true;
 project_config.CalcSourceTimeMode=0;
 project_config.MultiThreadingMode=1;
 project_config.MTUpdateInterfaceInterval=100;
 project_config.ChannelsConfig[0].CalculationMode=1;
 project_config.ChannelsConfig[0].ClassName=model_classname;
 project_config.ChannelsConfig[0].ModelMode=3;
 project_config.ChannelsConfig[0].InitAfterLoad=true;
 project_config.ChannelsConfig[0].GlobalTimeStep=2000;
 project_config.ChannelsConfig[0].ResetAfterLoad=true;
// project_config.ChannelsConfig[0].CalculationMode
 project_config.ChannelsConfig[0].DefaultTimeStep=2000;
 project_config.ChannelsConfig[0].MinInterstepsInterval=1;
 project_config.ChannelsConfig[0].MaxCalculationModelTime=1000;

 return CreateProject(file_name,project_config);
}

/// Обновляет проект по новой конфигурации
bool UApplication::UpdateProject(RDK::TProjectConfig &project_config)
{
 if(!ProjectOpenFlag)
  return false;

 // Если необходима перезагрузка конфигурации, то выполняем перезагрузку
 bool is_reload_needed(false);

 const TProjectConfig old_project_config=Project->GetConfig();
 Project->SetConfig(project_config);

 // Первый проход. Определяем необходима ли перезагрузка конфигурации

 if(old_project_config.ProjectName != project_config.ProjectName)
 {
 }

 if(old_project_config.ProjectDescription != project_config.ProjectDescription)
 {
 }

 if(old_project_config.DescriptionFileName != project_config.DescriptionFileName)
 {
 }

 if(old_project_config.ProjectAutoSaveFlag != project_config.ProjectAutoSaveFlag)
 {
 }

 if(old_project_config.ProjectAutoSaveStatesFlag != project_config.ProjectAutoSaveStatesFlag)
 {
 }

 if(old_project_config.EventsLogFlag != project_config.EventsLogFlag)
 {
 }

 if(old_project_config.ProjectMode != project_config.ProjectMode)
 {
  is_reload_needed=true;
 }

 if(old_project_config.ProjectType != project_config.ProjectType)
 {
  is_reload_needed=true;
 }

 if(old_project_config.MultiThreadingMode != project_config.MultiThreadingMode)
 {
  is_reload_needed=true;
 }

 if(old_project_config.CalcSourceTimeMode != project_config.CalcSourceTimeMode)
 {

 }

 if(old_project_config.MTUpdateInterfaceInterval != project_config.MTUpdateInterfaceInterval)
 {

 }

 if(old_project_config.GuiUpdateMode != project_config.GuiUpdateMode)
 {

 }

 if(old_project_config.ShowChannelsStateFlag != project_config.ShowChannelsStateFlag)
 {

 }

 if(old_project_config.ReflectionFlag != project_config.ReflectionFlag)
 {

 }

 if(old_project_config.DisableStopVideoSources != project_config.DisableStopVideoSources)
 {

 }

 if(old_project_config.DebugMode != project_config.DebugMode)
 {

 }

 if(old_project_config.DebugSysEventsMask != project_config.DebugSysEventsMask)
 {

 }

 if(old_project_config.DebuggerMessageFlag != project_config.DebuggerMessageFlag)
 {

 }

 if(old_project_config.EventsLogMode != project_config.EventsLogMode)
 {
  is_reload_needed=true;
 }

 if(old_project_config.OverrideLogParameters != project_config.OverrideLogParameters)
 {
  is_reload_needed=true;
 }

 if(old_project_config.ServerInterfaceAddress != project_config.ServerInterfaceAddress)
 {

 }

 if(old_project_config.ServerInterfacePort != project_config.ServerInterfacePort)
 {

 }

 if(old_project_config.ProjectShowChannelsStates != project_config.ProjectShowChannelsStates)
 {

 }

 if(old_project_config.InterfaceFileName != project_config.InterfaceFileName)
 {
  is_reload_needed=true;
 }

 if(old_project_config.NumChannels != project_config.NumChannels)
 {
  is_reload_needed=true;
 }

 int min_num_channels=(project_config.NumChannels<old_project_config.NumChannels)?project_config.NumChannels:old_project_config.NumChannels;
 for(int i=0;i<min_num_channels;i++)
 {
  if(old_project_config.ChannelsConfig[i].ModelMode != old_project_config.ChannelsConfig[i].ModelMode)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].PredefinedStructure != old_project_config.ChannelsConfig[i].PredefinedStructure)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ModelFileName != old_project_config.ChannelsConfig[i].ModelFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ParametersFileName != old_project_config.ChannelsConfig[i].ParametersFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].StatesFileName != old_project_config.ChannelsConfig[i].StatesFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ClassName != old_project_config.ChannelsConfig[i].ClassName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].GlobalTimeStep != old_project_config.ChannelsConfig[i].GlobalTimeStep)
  {
  }

  if(old_project_config.ChannelsConfig[i].DefaultTimeStep != old_project_config.ChannelsConfig[i].DefaultTimeStep)
  {
  }

  if(old_project_config.ChannelsConfig[i].CalculationMode != old_project_config.ChannelsConfig[i].CalculationMode)
  {
  }

  if(old_project_config.ChannelsConfig[i].MinInterstepsInterval != old_project_config.ChannelsConfig[i].MinInterstepsInterval)
  {
  }

  if(old_project_config.ChannelsConfig[i].InitAfterLoad != old_project_config.ChannelsConfig[i].InitAfterLoad)
  {
  }

  if(old_project_config.ChannelsConfig[i].ResetAfterLoad != old_project_config.ChannelsConfig[i].ResetAfterLoad)
  {
  }

  if(old_project_config.ChannelsConfig[i].DebugMode != old_project_config.ChannelsConfig[i].DebugMode)
  {
  }

  if(old_project_config.ChannelsConfig[i].DebugSysEventsMask != old_project_config.ChannelsConfig[i].DebugSysEventsMask)
  {
  }

  if(old_project_config.ChannelsConfig[i].DebuggerMessageFlag != old_project_config.ChannelsConfig[i].DebuggerMessageFlag)
  {
  }

  if(old_project_config.ChannelsConfig[i].EventsLogMode != old_project_config.ChannelsConfig[i].EventsLogMode)
  {
  }

  if(old_project_config.ChannelsConfig[i].ChannelName != old_project_config.ChannelsConfig[i].ChannelName)
  {
  }

  if(old_project_config.ChannelsConfig[i].MaxCalculationModelTime != old_project_config.ChannelsConfig[i].MaxCalculationModelTime)
  {
  }

 }

 // Если необходима перезагрузка конфигурации то выполняем
 if(is_reload_needed)
 {
  if(!SaveProject())
   return false;

  if(!OpenProject(GetProjectPath()+GetProjectFileName()))
   return false;

  return true;
 }

 // ... иначе применяем отдельные настройки
 if(old_project_config.ProjectName != project_config.ProjectName)
 {
 }

 if(old_project_config.ProjectDescription != project_config.ProjectDescription)
 {
 }

 if(old_project_config.DescriptionFileName != project_config.DescriptionFileName)
 {
 }

 if(old_project_config.ProjectAutoSaveFlag != project_config.ProjectAutoSaveFlag)
 {
 }

 if(old_project_config.ProjectAutoSaveStatesFlag != project_config.ProjectAutoSaveStatesFlag)
 {
 }

 if(old_project_config.EventsLogFlag != project_config.EventsLogFlag)
 {
  GetCore()->GetLogger(RDK_GLOB_MESSAGE)->SetEventsLogMode(project_config.EventsLogFlag);
 }

 if(old_project_config.ProjectMode != project_config.ProjectMode)
 {
  // нет действий - приводит к повторному открытию конфигурации ранее
 }

 if(old_project_config.ProjectType != project_config.ProjectType)
 {
  // нет действий - приводит к повторному открытию конфигурации ранее
 }

 if(old_project_config.MultiThreadingMode != project_config.MultiThreadingMode)
 {
  // нет действий - приводит к повторному открытию конфигурации ранее
 }

 if(old_project_config.CalcSourceTimeMode != project_config.CalcSourceTimeMode)
 {
  for(int i=0;i<project_config.NumChannels;i++)
  {
   EngineControl->SetCalculationTimeSource(i, project_config.CalcSourceTimeMode);
  }
 }

 if(old_project_config.MTUpdateInterfaceInterval != project_config.MTUpdateInterfaceInterval)
 {

 }

 if(old_project_config.GuiUpdateMode != project_config.GuiUpdateMode)
 {
  EngineControl->SetGuiUpdateMode(project_config.GuiUpdateMode);
 }

 if(old_project_config.ShowChannelsStateFlag != project_config.ShowChannelsStateFlag)
 {

 }

 if(old_project_config.ReflectionFlag != project_config.ReflectionFlag)
 {

 }

 if(old_project_config.DisableStopVideoSources != project_config.DisableStopVideoSources)
 {

 }

 if(old_project_config.DebugMode != project_config.DebugMode)
 {
  for(int i=0;i<project_config.NumChannels;i++)
  {
   MLog_SetDebugMode(i,project_config.DebugMode);
  }
 }

 if(old_project_config.DebugSysEventsMask != project_config.DebugSysEventsMask)
 {
  for(int i=0;i<project_config.NumChannels;i++)
  {
   MLog_SetDebugSysEventsMask(i,project_config.DebugSysEventsMask);
  }
 }

 if(old_project_config.DebuggerMessageFlag != project_config.DebuggerMessageFlag)
 {
  for(int i=0;i<project_config.NumChannels;i++)
  {
   MLog_SetDebuggerMessageFlag(i,project_config.DebuggerMessageFlag);
  }
 }

 if(old_project_config.EventsLogMode != project_config.EventsLogMode)
 {
  is_reload_needed=true;
 }

 if(old_project_config.OverrideLogParameters != project_config.OverrideLogParameters)
 {
  is_reload_needed=true;
 }

 if(old_project_config.ServerInterfaceAddress != project_config.ServerInterfaceAddress)
 {

 }

 if(old_project_config.ServerInterfacePort != project_config.ServerInterfacePort)
 {

 }

  if(old_project_config.HttpServerInterfaceAddress != project_config.HttpServerInterfaceAddress)
 {

 }

 if(old_project_config.HttpServerInterfacePort != project_config.HttpServerInterfacePort)
 {

 }


  if(old_project_config.HttpServerLogin != project_config.HttpServerLogin)
 {

 }

 if(old_project_config.HttpServerPassword != project_config.HttpServerPassword)
 {

 }


 if(old_project_config.ProjectShowChannelsStates != project_config.ProjectShowChannelsStates)
 {

 }

 if(old_project_config.InterfaceFileName != project_config.InterfaceFileName)
 {
  // нет действий - приводит к повторному открытию конфигурации ранее
 }

 if(old_project_config.NumChannels != project_config.NumChannels)
 {
  // нет действий - приводит к повторному открытию конфигурации ранее
 }

 // обработка каналов
 for(int i=0;i<project_config.NumChannels;i++)
 {
  if(old_project_config.ChannelsConfig[i].ModelMode != project_config.ChannelsConfig[i].ModelMode)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].PredefinedStructure != project_config.ChannelsConfig[i].PredefinedStructure)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].ModelFileName != project_config.ChannelsConfig[i].ModelFileName)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].ParametersFileName != project_config.ChannelsConfig[i].ParametersFileName)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].StatesFileName != project_config.ChannelsConfig[i].StatesFileName)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].ClassName != project_config.ChannelsConfig[i].ClassName)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].GlobalTimeStep != project_config.ChannelsConfig[i].GlobalTimeStep)
  {
   if(MModel_Check(i))
   {
    MModel_SetGlobalTimeStep(i,"",project_config.ChannelsConfig[i].GlobalTimeStep);
   }
  }

  if(old_project_config.ChannelsConfig[i].DefaultTimeStep != project_config.ChannelsConfig[i].DefaultTimeStep)
  {
   if(MModel_Check(i))
   {
    MModel_SetDefaultTimeStep(i,project_config.ChannelsConfig[i].DefaultTimeStep);
   }
  }

  if(old_project_config.ChannelsConfig[i].CalculationMode != project_config.ChannelsConfig[i].CalculationMode)
  {
   EngineControl->SetCalculateMode(i, project_config.ChannelsConfig[i].CalculationMode);
  }

  if(old_project_config.ChannelsConfig[i].MinInterstepsInterval != project_config.ChannelsConfig[i].MinInterstepsInterval)
  {
   EngineControl->SetMinInterstepsInterval(i,project_config.ChannelsConfig[i].MinInterstepsInterval);
  }

  if(old_project_config.ChannelsConfig[i].InitAfterLoad != project_config.ChannelsConfig[i].InitAfterLoad)
  {
  }

  if(old_project_config.ChannelsConfig[i].ResetAfterLoad != project_config.ChannelsConfig[i].ResetAfterLoad)
  {
  }

  if(old_project_config.ChannelsConfig[i].DebugMode != project_config.ChannelsConfig[i].DebugMode)
  {
   MLog_SetDebugMode(i,project_config.ChannelsConfig[i].DebugMode);
  }

  if(old_project_config.ChannelsConfig[i].DebugSysEventsMask != project_config.ChannelsConfig[i].DebugSysEventsMask)
  {
   MLog_SetDebugSysEventsMask(i,project_config.ChannelsConfig[i].DebugSysEventsMask);
  }

  if(old_project_config.ChannelsConfig[i].DebuggerMessageFlag != project_config.ChannelsConfig[i].DebuggerMessageFlag)
  {
   MLog_SetDebuggerMessageFlag(i,project_config.ChannelsConfig[i].DebuggerMessageFlag);
  }

  if(old_project_config.ChannelsConfig[i].EventsLogMode != project_config.ChannelsConfig[i].EventsLogMode)
  {
   MLog_SetEventsLogMode(i,project_config.ChannelsConfig[i].EventsLogMode);
  }

  if(old_project_config.ChannelsConfig[i].ChannelName != project_config.ChannelsConfig[i].ChannelName)
  {
  }

  if(old_project_config.ChannelsConfig[i].MaxCalculationModelTime != project_config.ChannelsConfig[i].MaxCalculationModelTime)
  {
   GetEnvironmentLock()->SetMaxCalcTime(project_config.ChannelsConfig[i].MaxCalculationModelTime);
  }

  if(old_project_config.ChannelsConfig[i].UseIndTimeStepFlag != project_config.ChannelsConfig[i].UseIndTimeStepFlag)
  {
   GetEnvironmentLock()->SetUseIndTimeStepFlag(project_config.ChannelsConfig[i].UseIndTimeStepFlag);
  }

 }

 return SaveProject();
}

/// Открывает проект
bool UApplication::OpenProject(const std::string &filename)
{
 CloseProject();

 bool is_loaded(false);
 if(!ProjectXml.LoadFromFile(filename,""))
 {
  MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_WARNING, (std::string("Can't read project file ")+filename).c_str());
  return false;
 }

 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_INFO, (std::string("Open configuration ")+filename+"...").c_str());
 ProjectPath=extract_file_path(filename);
 ProjectFileName=extract_file_name(filename);
 Project->SetProjectPath(ProjectPath);
 Project->ReadFromXml(ProjectXml);
 if(LogCreationMode != 3)
  UpdateLoggers();

 TProjectConfig config=Project->GetConfig();
 GetCore()->GetLogger(RDK_GLOB_MESSAGE)->SetEventsLogMode(config.EventsLogFlag);
 if(LogCreationMode != 3)
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
   {
	Env_SetPredefinedStructure(channel_config.PredefinedStructure);
	RDK_ASSERT_LOG(MEnv_CreateStructure(i));
	RDK_ASSERT_LOG(MEnv_Init(i));
   }

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
	 is_loaded=LoadModelFromFile(i,ProjectPath+channel_config.ModelFileName);
	}
	else
	 is_loaded=LoadModelFromFile(i,channel_config.ModelFileName);

	if(!is_loaded)
	 MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_ERROR, (std::string("Core-OpenProject: Can't open model file: ")+channel_config.ModelFileName).c_str());
   }


   if(!channel_config.ParametersFileName.empty())
   {
	if(extract_file_path(channel_config.ParametersFileName).empty())
	 is_loaded=LoadParametersFromFile(i,ProjectPath+channel_config.ParametersFileName);
	else
	 is_loaded=LoadParametersFromFile(i,channel_config.ParametersFileName);

 	if(!is_loaded)
	 MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_ERROR, (std::string("Core-OpenProject: Can't open parameters file: ")+channel_config.ParametersFileName).c_str());
   }

   if(config.ProjectAutoSaveStatesFlag)
   {
	if(!channel_config.StatesFileName.empty())
	{
	 if(extract_file_path(channel_config.StatesFileName).empty())
	  is_loaded=LoadStatesFromFile(i,ProjectPath+channel_config.StatesFileName);
	 else
	  is_loaded=LoadStatesFromFile(i,channel_config.StatesFileName);

	 if(!is_loaded)
	  MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_ERROR, (std::string("Core-OpenProject: Can't open states file: ")+channel_config.StatesFileName).c_str());
	}
   }

   if(Model_Check())
   {
	if(!channel_config.UseIndTimeStepFlag)
	 Model_SetGlobalTimeStep("",channel_config.GlobalTimeStep);
	if(channel_config.InitAfterLoad)
     MEnv_ModelInit(i,0);
	if(channel_config.ResetAfterLoad)
	 MEnv_Reset(i,0);
   }

   EngineControl->SetCalculateMode(i, channel_config.CalculationMode);
   GetEnvironmentLock()->SetMaxCalcTime(channel_config.MaxCalculationModelTime);
   GetEnvironmentLock()->SetUseIndTimeStepFlag(channel_config.UseIndTimeStepFlag);
  }
  catch(RDK::UException &exception)
  {
   MLog_LogMessage(RDK_SYS_MESSAGE, exception.GetType(), (std::string("Core-OpenProject(Load Channel) Exception: (Name=")+std::string(Name.c_str())+std::string(") ")+exception.what()).c_str());
  }
  Sleep(0);
 }

 EngineControl->SetGuiUpdateMode(config.GuiUpdateMode);

 if(selected_channel_index>=GetNumChannels())
  selected_channel_index=0;

 Core_SelectChannel(selected_channel_index);
 InterfaceXml.Destroy();

 if(!config.InterfaceFileName.empty())
 {
  if(extract_file_path(config.InterfaceFileName).empty())
   is_loaded=InterfaceXml.LoadFromFile(ProjectPath+config.InterfaceFileName,"Interfaces");
  else
   is_loaded=InterfaceXml.LoadFromFile(config.InterfaceFileName,"Interfaces");

  if(!is_loaded)
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_ERROR, (std::string("Core-OpenProject: Can't open interface file: ")+config.InterfaceFileName).c_str());

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

 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_INFO, (std::string("Configuration ")+filename+" has been opened.").c_str());
 return true;
}

/// Сохраняет проект
bool UApplication::SaveProject(void)
{
 if(!ProjectOpenFlag)
  return false;

 int selected_channel_index=Core_GetSelectedChannelIndex();
 bool is_saved(false);

 ProjectXml.Create("Project");
 Project->WriteToXml(ProjectXml);
try
{
 InterfaceXml.Create(std::string("Interfaces"));
 InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 RDK::UIVisualControllerStorage::SaveParameters(InterfaceXml);

 TProjectConfig config=Project->GetConfig();
 ProjectXml.SelectNodeRoot("Project/General");

 SaveFileSafe(ProjectPath+config.DescriptionFileName,config.ProjectDescription,"save.tmp",3);

 if(!config.InterfaceFileName.empty())
 {
  if(extract_file_path(config.InterfaceFileName).empty())
   is_saved=InterfaceXml.SaveToFile(ProjectPath+config.InterfaceFileName);
  else
   is_saved=InterfaceXml.SaveToFile(config.InterfaceFileName);
 }
 else
 {
  ProjectXml.WriteString("InterfaceFileName","Interface.xml");
  is_saved=InterfaceXml.SaveToFile(ProjectPath+config.InterfaceFileName);
 }

 if(!is_saved)
  MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_ERROR, (std::string("Core-SaveProject: Can't save interface file: ")+config.InterfaceFileName).c_str());

 for(int i=0;i<config.NumChannels;i++)
 {
  Core_SelectChannel(i);

  TProjectChannelConfig &channel_config=config.ChannelsConfig[i];
  if(extract_file_path(channel_config.ModelFileName).empty())
   is_saved=SaveModelToFile(i, ProjectPath+channel_config.ModelFileName);
  else
   is_saved=SaveModelToFile(i, channel_config.ModelFileName);

  if(!is_saved)
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_ERROR, (std::string("Core-SaveProject: Can't save model file: ")+channel_config.ModelFileName).c_str());

  if(extract_file_path(channel_config.ParametersFileName).empty())
   is_saved=SaveParametersToFile(i, ProjectPath+channel_config.ParametersFileName);
  else
   is_saved=SaveParametersToFile(i,channel_config.ParametersFileName);

  if(!is_saved)
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_ERROR, (std::string("Core-SaveProject: Can't save parameters file: ")+channel_config.ParametersFileName).c_str());

  channel_config.UseIndTimeStepFlag=GetEnvironmentLock()->GetUseIndTimeStepFlag();

  if(config.ProjectAutoSaveStatesFlag)
  {
   if(extract_file_path(channel_config.StatesFileName).empty())
	is_saved=SaveStatesToFile(i, ProjectPath+channel_config.StatesFileName);
   else
	is_saved=SaveStatesToFile(i, channel_config.StatesFileName);

   if(!is_saved)
    MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_ERROR, (std::string("Core-SaveProject: Can't save states file: ")+channel_config.StatesFileName).c_str());
  }


  Sleep(0);
 }

 Core_SelectChannel(selected_channel_index);


 is_saved=ProjectXml.SaveToFile(ProjectPath+ProjectFileName);

 if(!is_saved)
  MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_ERROR, (std::string("Core-SaveProject: Can't save configuration: ")+ProjectFileName).c_str());
 else
 {
  std::string filename=ProjectPath+ProjectFileName;
  MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_INFO, (std::string("Configuration ")+filename+" has been saved.").c_str());
 }
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

 std::string filename=ProjectPath+ProjectFileName;


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
   Env_ModelUnInit(0);
   Env_DestroyStructure();
   Env_UnInit();
   Model_Destroy();
  }
  Storage_FreeObjectsStorage();
 }

 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_INFO, (std::string("Configuration ")+filename+" has been closed.").c_str());
 return true;
}

/// Клонирует проект в новое расположение
bool UApplication::CloneProject(const std::string &filename)
{
 return true;
}

/// Переименовывает папку проекта
bool UApplication::RenameProject(const std::string &filename)
{
 if(!ProjectOpenFlag)
  return false;

 if(filename.empty())
  return false;

 PauseChannel(-1);
 bool events_log_mode=GetProjectConfig().EventsLogFlag;
  GetCore()->GetLogger(RDK_GLOB_MESSAGE)->SetEventsLogMode(false);

 std::string resfilename=filename;

 int res=RdkMoveFile(ProjectPath, resfilename);

 if(filename.find_last_of("\\/") != filename.size()-1)
  resfilename+="/";

 GetCore()->GetLogger(RDK_GLOB_MESSAGE)->SetEventsLogMode(events_log_mode);
 if(res == 0)
 {
  SetProjectPath(resfilename);

  std::list<std::string> last_list=LastProjectsList;
  last_list.push_front(resfilename+ProjectFileName);
  while(int(last_list.size())>LastProjectsListMaxSize
   && !LastProjectsList.empty())
  {
   last_list.pop_back();
  }

  LastProjectsList=last_list;

  SaveProjectsHistory();
  return true;
 }

 return false;
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

 if(RDK::CreateNewDirectory(new_path.c_str()) != 0)
  return false;

 RDK::CopyDir(ProjectPath, new_path, "*.*");
 return true;
}

/// Сохраняет только файл настроек проекта
bool UApplication::SaveProjectConfig(void)
{
 if(!ProjectOpenFlag)
  return false;

 bool is_saved(false);
 Project->WriteToXml(ProjectXml);

 try
 {
  TProjectConfig config=Project->GetConfig();
  ProjectXml.SelectNodeRoot("Project/General");

  if(config.InterfaceFileName.empty())
  {
   ProjectXml.WriteString("InterfaceFileName","Interface.xml");
  }

  is_saved=ProjectXml.SaveToFile(ProjectPath+ProjectFileName);
  if(!is_saved)
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_ERROR, (std::string("Core-SaveProject: Can't save project config file: ")+ProjectFileName).c_str());
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
if (index == 0)
    return false;

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
 if(source_id<0 || cloned_id <0)
  return false;

 if(cloned_id>=Core_GetNumChannels())
 {
  if(!SetNumChannels(cloned_id+1))
   return false;
 }

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
 {
  MLog_LogMessage(channel_index,RDK_EX_ERROR,(std::string("Failed to load model file: ")+file_name).c_str());
  return false;
 }

 if(!data.empty())
 {
  MModel_Destroy(channel_index);
  if(MModel_LoadComponent(channel_index, "",data.c_str()) == RDK_SUCCESS)
   return true;
 }
 return false;
}

bool UApplication::SaveModelToFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 const char *p_buf=MModel_SaveComponent(channel_index, "");
 bool res=false;
 if(p_buf)
 {
  SaveBuffer=p_buf;
  Engine_FreeBufString(p_buf);
  if(SaveBuffer.empty())
  {
   MLog_LogMessage(channel_index,RDK_EX_ERROR,(std::string("SaveModelToFile in")+file_name+" error: model size iz zero! File not changed.").c_str());
   return false;
  }

  if(SaveBuffer[0]!='<')
  {
   SaveBuffer[0]='<';
   MLog_LogMessage(channel_index,RDK_EX_WARNING,(std::string("SaveModelToFile in")+file_name+" warning: first symbol INVALID. Fixed.").c_str());
  }

  res=SaveFileSafe(file_name,SaveBuffer,"save.tmp",3);
 }
 return res;
}

bool UApplication::LoadParametersFromFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
 {
  MLog_LogMessage(channel_index,RDK_EX_ERROR,(std::string("Failed to load parameters file: ")+file_name).c_str());
  return false;
 }

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
 bool res=false;
 if(p_buf)
 {
  SaveBuffer=p_buf;
  Engine_FreeBufString(p_buf);
  if(SaveBuffer.empty())
  {
   MLog_LogMessage(channel_index,RDK_EX_ERROR,(std::string("SaveParametersToFile in")+file_name+" error: model size iz zero! File not changed.").c_str());
   return false;
  }

  if(SaveBuffer[0]!='<')
  {
   SaveBuffer[0]='<';
   MLog_LogMessage(channel_index,RDK_EX_WARNING,(std::string("SaveParametersToFile in")+file_name+" warning: first symbol INVALID. Fixed.").c_str());
  }

  res=SaveFileSafe(file_name,SaveBuffer,"save.tmp",3);
 }
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
  res=SaveFileSafe(file_name,p_buf,"save.tmp",3);
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
  res=SaveFileSafe(file_name,p,"save.tmp",3);
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
  res=SaveFileSafe(file_name,p,"save.tmp",3);
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
 history_ini.LoadFromFile((WorkDirectory+opt_name).c_str());
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
 history_ini.SaveToFile(WorkDirectory+opt_name);
}

/// Флаг принудительного сохранения конфигураций в старом формате
bool UApplication::IsUseNewXmlFormatProjectFile(void) const
{
 return UseNewXmlFormatProjectFile;
}

bool UApplication::ChangeUseNewXmlFormatProjectFile(bool value)
{
 if(UseNewXmlFormatProjectFile == value)
  return true;

 UseNewXmlFormatProjectFile=value;
 if(Project)
  Project->SetForceOldXmlFormat(!value);
 return true;
}

/// Флаг включения нового представления файловой структуры конфигурации
/// (только при сохранении данных конфигурации в новом формате)
bool UApplication::IsUseNewProjectFilesStructure(void) const
{
 return UseNewProjectFilesStructure;
}

bool UApplication::ChangeUseNewProjectFilesStructure(bool value)
{
 if(UseNewProjectFilesStructure == value)
  return true;

 UseNewProjectFilesStructure=value;
 if(Project)
  Project->SetForceNewConfigFilesStructure(value);
 return true;
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
    ("standalone", "standalone vesrion of server without network")
    ("conf", po::value<string>(), "Configuration file name")
    ("ctime", po::value<double>(), "Calculation time interval, in seconds")
    ("info", po::value<string>(), "Information about core, possible: CollectionsList, ClassesList, CollectionClassesList, ClassProperties")
    ("class", po::value<string>(), "Class name")
    ("collection", po::value<string>(), "collection name")
    ("mask", po::value<unsigned>(), "Property mask")
    ("save_model_bmp", po::value<string>(), "Component name")
    ("session", po::value<unsigned>(), "Session Id")
;
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




/// Сохраняет файл из строки, через временный файл. Делает n_pass попыток сохранить с чтением результата и сразвнением с оригиналом.
/// Если сохранение не удалось, то старый файл остается как был.
/// Если сохранение удалось, то временный файл заменяет старый
bool UApplication::SaveFileSafe(const std::string &file_name, const std::string &buffer, const std::string &temp_file_name, int n_pass)
{
 if(temp_file_name.empty())
  return false;

 if(n_pass<1)
  return false;

 if(temp_file_name == file_name)
  return false;

 bool is_temp_saved(false);

 for(int i=0;i<n_pass;i++)
 {
  bool is_saved=SaveFile(temp_file_name,buffer);
  if(!is_saved)
  {
   MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_ERROR, std::string(std::string("SaveFileSafe file: ")+file_name+std::string(" as ")+temp_file_name+std::string(" save attepmt #")+sntoa(i+1)+" FAILED.").c_str());
   continue;
  }

  std::string temp_buffer;
  bool is_loaded=LoadFile(temp_file_name,temp_buffer);
  if(!is_loaded)
  {
   MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_ERROR, std::string(std::string("SaveFileSafe file: ")+file_name+std::string(" as ")+temp_file_name+std::string(" test load attepmt #")+sntoa(i+1)+" FAILED.").c_str());
   continue;
  }

  if(buffer != temp_buffer)
  {
   MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_ERROR, std::string(std::string("SaveFileSafe file: ")+file_name+std::string(" as ")+temp_file_name+std::string(" compare attepmt #")+sntoa(i+1)+" FAILED.").c_str());
   continue;
  }
  is_temp_saved=true;
  break;
 }

 if(!is_temp_saved)
 {
  MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_ERROR, std::string(std::string("SaveFileSafe file: ")+file_name+std::string(" as ")+temp_file_name+std::string(" all ")+sntoa(n_pass)+" attepmts FAILED.").c_str());
  return false;
 }

 int copy_error=RdkCopyFile(temp_file_name, file_name);

 if(!copy_error)
  return true;

 MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_ERROR, std::string(std::string("SaveFileSafe file: ")+temp_file_name+std::string(" copy to ")+file_name+std::string(" FAILED with error code ")+sntoa(copy_error)).c_str());
 return false;
}
// --------------------------

}

#endif

