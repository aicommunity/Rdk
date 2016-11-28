#ifndef UApplication_CPP
#define UApplication_CPP

#include "UApplication.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"

//#include "UAppCore.cpp"
//#include "UProject.cpp"
//#include "UIVisualController.cpp"
//#include "URpcDispatcherQueues.cpp"
//#include "URpcDispatcher.cpp"
//#include "URpcDecoder.cpp"
//#include "URpcDecoderInternal.cpp"
//#include "URpcCommand.cpp"
//#include "URpcCommandInternal.cpp"


#ifdef __BORLANDC__
//#include "Bcb/Application.bcb.cpp"
#endif

extern void ExceptionHandler(int channel_index); // TODO: Потом ее куда то убрать

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UApplication::UApplication(void)
{
 Name="Application";
 LastProjectsListMaxSize=10;
 ProjectOpenFlag=false;
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
 WorkDirectory=value;
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
 if(Project && Project->GetConfig().OverrideLogParameters)
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

 EngineControl->GetEngineStateThread()->SetLogFlag(value.EventsLogFlag);
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

/// Инициализирует приложение
bool UApplication::Init(void)
{
 Core_SetBufObjectsMode(1);

 std::string font_path=extract_file_path(ApplicationFileName);
 Core_SetSystemDir(font_path.c_str());
// SetLogDir(font_path);
 MLog_SetExceptionHandler(RDK_SYS_MESSAGE,(void*)ExceptionHandler);
 Core_LoadFonts();

 SetWorkDirectory(font_path);
 EngineControl->Init();
 UApplication::SetNumChannels(1);
 MCore_ChannelInit(0,0,(void*)ExceptionHandler);

 LoadProjectsHistory();
 return true;
}

/// Деинициализирует приложение
bool UApplication::UnInit(void)
{
 if(EngineControl)
 {
  EngineControl->PauseChannel(-1);
  EngineControl->StopEngineStateThread();
 }
 Sleep(10);
 CloseProject();
 return true;
}
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

 ProjectXml.LoadFromFile(filename,"");
 ProjectPath=extract_file_path(filename);
 ProjectFileName=extract_file_name(filename);
 Project->SetProjectPath(ProjectPath);
 Project->ReadFromXml(ProjectXml);
 UpdateLoggers();

 TProjectConfig config=Project->GetConfig();
 EngineControl->GetEngineStateThread()->SetLogFlag(config.EventsLogFlag);
 EngineControl->GetEngineStateThread()->CloseEventsLogFile();
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
 PauseChannel(-1);

 RDK::TProjectConfig config=GetProjectConfig();

 if(config.ProjectAutoSaveFlag)
  SaveProject();

// if(UServerControlForm)
//  UServerControlForm->ServerRestartTimer->Enabled=false;

 if(ProjectOpenFlag)
 {
  SetProjectFileName("");
  SetProjectPath("");
 }
 SetProjectOpenFlag(false);
 for(int i=GetNumChannels()-1;i>=0;i--)
 {
  Core_SelectChannel(i);
  if(GetEngine())
  {
   Env_UnInit();
   Model_Destroy();
  }
 }
 RDK::UIVisualControllerStorage::ClearInterface();
 EngineControl->StopEngineStateThread();
 return true;
}

/// Клонирует проект в новое расположение
bool UApplication::CloneProject(const std::string &filename)
{
 return true;
}

bool UApplication::CloneProject(int source_id, int cloned_id)
{
 if(source_id>=GetNumChannels() || cloned_id >= GetNumChannels())
  return false;
/*
 try {
 RDK::TProjectConfig config=GetProjectConfig();
 PredefinedStructure.resize(GetNumChannels());
 PredefinedStructure[cloned_id]=PredefinedStructure[source_id];

 // Шаг счета по умолчанию
 DefaultTimeStep.resize(GetNumChannels());
 DefaultTimeStep[cloned_id]=DefaultTimeStep[source_id];

 // Глобальный шаг счета модели
 GlobalTimeStep.resize(GetNumChannels());
 GlobalTimeStep[cloned_id]=GlobalTimeStep[source_id];

 if(!ProjectXml.SelectNodeRoot("Project/General"))
  return;

 ReflectionFlag=ProjectXml.ReadBool("ReflectionFlag",true);

 CalculationMode.resize(GetNumChannels());
 CalculationMode[cloned_id]=CalculationMode[source_id];

 InitAfterLoadFlag.resize(GetNumChannels());
 InitAfterLoadFlag[cloned_id]=InitAfterLoadFlag[source_id];

 ResetAfterLoadFlag.resize(GetNumChannels());
 ResetAfterLoadFlag[cloned_id]=ResetAfterLoadFlag[source_id];

 DebugModeFlag.resize(GetNumChannels());
 DebugModeFlag[cloned_id]=DebugModeFlag[source_id];

 MinInterstepsInterval.resize(GetNumChannels());
 MinInterstepsInterval[cloned_id]=MinInterstepsInterval[source_id];


 ProjectXml.WriteInteger("ProjectAutoSaveFlag",ProjectAutoSaveFlag);
 // Флаг автоматического сохранения проекта
 ProjectXml.WriteInteger("ProjectAutoSaveStateFlag",ProjectAutoSaveStateFlag);

 int selected_engine=GetSelectedChannelIndex();
 Core_SelectChannel(cloned_id);
 String modelfilename;

 if(source_id == 0)
   modelfilename=ProjectXml.ReadString("ModelFileName","").c_str();
  else
   modelfilename=ProjectXml.ReadString(std::string("ModelFileName_")+RDK::sntoa(source_id),"").c_str();


 if(!IsChannelInit())
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
 Core_SelectChannel(selected_engine);
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
   */
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

 for(size_t i=old_num;i<num;i++)
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
 return EngineControl->CheckCalcState(channel_index);
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
  EngineControl->GetEngineStateThread()->RecreateEventsLogFile();
}

/// Загружает файл в строку
bool UApplication::LoadFile(const std::string &file_name, std::string &buffer) const
{
 std::ifstream t(file_name.c_str(), ios::in);

 if(!t || t.fail() || t.bad())
 {
  buffer.clear();
  return false;
 }

 t.seekg(0, std::ios::end);
 if(t.fail() || t.bad())
  return false;
 buffer.reserve(t.tellg());
 t.seekg(0, std::ios::beg);
 if(t.fail() || t.bad())
  return false;

 buffer.assign((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
 if(t.fail() || t.bad())
  return false;
 return true;
}

/// Сохраняет файл из строки
bool UApplication::SaveFile(const std::string &file_name, const std::string &buffer) const
{
 std::ofstream t(file_name.c_str(), ios::trunc);

 if(!t || t.fail() || t.bad())
 {
  return false;
 }

 t<<buffer;
 if(t.fail() || t.bad())
  return false;
 return true;
}
// --------------------------

}

#endif

