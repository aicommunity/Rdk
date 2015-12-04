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
 EngineControl->GetEngineStateThread()->SetLogPath(ProjectPath);
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
  EngineControl->PauseEngine(-1);
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
 Engine_SetBufObjectsMode(1);

 std::string font_path=extract_file_path(ApplicationFileName);
 SetSystemDir(font_path.c_str());
 GraphicalEngineInit(0,1,1,320,240,1,(void*)ExceptionHandler);
 Engine_LoadFonts();

 SetWorkDirectory(font_path);
 EngineControl->Init();

 LoadProjectsHistory();
 return true;
}

/// Деинициализирует приложение
bool UApplication::UnInit(void)
{
 if(EngineControl)
 {
  EngineControl->PauseEngine(-1);
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
 ProjectFileName=file_name;

 UApplication::SetNumEngines(project_config.NumChannels);

 for(size_t i=0;i<project_config.NumChannels;i++)
 {
  RDK::TProjectChannelConfig &channel=project_config.ChannelsConfig[i];

  if(!MIsEngineInit(i))
   MGraphicalEngineInit(i,channel.PredefinedStructure,1,1,640, 480 ,project_config.ReflectionFlag,(void*)ExceptionHandler);
  else
   MEnv_SetPredefinedStructure(i,channel.PredefinedStructure);

  if(channel.PredefinedStructure == 0 && !channel.ClassName.empty())
  {
   MModel_Create(i,channel.ClassName.c_str());
  }
 }

 if(SaveProject())
 {
  std::string file_name=ProjectFileName;
  return OpenProject(file_name);
 }
 else
  return false;
}

/// Открывает проект
bool UApplication::OpenProject(const std::string &filename)
{
 CloseProject();

// UShowProgressBarForm->SetWinTitle(Lang_LoadProjectTitle);

 ProjectXml.LoadFromFile(filename,"");
 ProjectPath=extract_file_path(filename);
 ProjectFileName=extract_file_name(filename);

 Project->SetProjectPath(ProjectPath);
 Project->ReadFromXml(ProjectXml);

 TProjectConfig config=Project->GetConfig();
 EngineControl->GetEngineStateThread()->SetLogFlag(config.EventsLogFlag);
 EngineControl->GetEngineStateThread()->SetLogPath(ProjectPath);

// ProjectXml.SelectNodeRoot("Project/MultiGeneral");
// int engines_mode=ProjectXml.ReadInteger("EnginesMode",0);
 EngineControl->SetThreadMode(config.MultiThreadingMode);
 CalcAppCaption();
 /*
 UShowProgressBarForm->SetBarHeader(1,Lang_LoadingData);
 UShowProgressBarForm->SetBarHeader(2,Lang_Total);
 UShowProgressBarForm->ResetBarStatus(1, 1, num_engines-1);
 UShowProgressBarForm->ResetBarStatus(2, 1, 2);

 if(AppWinState)
  UShowProgressBarForm->Show();
 UShowProgressBarForm->Update();
   */
try{

 EngineControl->SetNumEngines(config.NumChannels);

 ProjectXml.SelectNodeRoot("Project/MultiGeneral");
 int selected_engine_index=ProjectXml.ReadInteger("SelectedEngineIndex",0);

 ProjectXml.SelectNodeRoot("Project/General");

 for(int i=0;i<config.NumChannels;i++)
 {
  try
  {
   TProjectChannelConfig &channel_config=config.ChannelsConfig[i];
   EngineControl->SetCalculationTimeSource(i, config.CalcSourceTimeMode);
   EngineControl->SetMinInterstepsInterval(i,channel_config.MinInterstepsInterval);

   SelectEngine(i);
   if(!IsEngineInit())
	GraphicalEngineInit(channel_config.PredefinedStructure,1,1,640, 480 ,true,(void*)ExceptionHandler);
   else
	Env_SetPredefinedStructure(channel_config.PredefinedStructure);

   // TODO: Реалиовать загрузку описаний классов
   // Загрузка описаний классов
//   UComponentsControlForm->ComponentsControlFrame->LoadCommonClassesDescriptionFromFile("CommonClassesDescription.xml");
//   UComponentsControlForm->ComponentsControlFrame->LoadClassesDescriptionFromFile("ClassesDescription.xml");
   Model_SetDefaultTimeStep(channel_config.DefaultTimeStep);
   Env_SetCurrentDataDir(ProjectPath.c_str());

   Env_CreateStructure();
   Env_Init();

   Env_SetDebugMode(channel_config.DebugMode);

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
   Engine_LogMessage(exception.GetType(), (std::string("Core-OpenProject(Load Channel) Exception: (Name=")+std::string(Name.c_str())+std::string(") ")+exception.what()).c_str());
  }
  Sleep(0);
 }

 if(selected_engine_index>=GetNumEngines())
  selected_engine_index=0;

 SelectEngine(selected_engine_index);
 InterfaceXml.Destroy();

 if(!config.InterfaceFileName.empty())
 {
  if(extract_file_path(config.InterfaceFileName).empty())
   InterfaceXml.LoadFromFile(ProjectPath+config.InterfaceFileName,"Interfaces");
  else
   InterfaceXml.LoadFromFile(config.InterfaceFileName,"Interfaces");

  InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 }
 ServerControl->SetNumEngines(config.NumChannels);

 RDK::UIVisualControllerStorage::LoadParameters(InterfaceXml);

 ProjectOpenFlag=true;
 EngineControl->StartEngineStateThread();

 RDK::UIVisualControllerStorage::UpdateInterface();
 RDK::UIVisualControllerStorage::AfterLoadProject();
}
catch(RDK::UException &exception)
{
// UShowProgressBarForm->Hide();
 Engine_LogMessage(exception.GetType(), (std::string("Core-OpenProject Exception: (Name=")+Name+std::string(") ")+exception.what()).c_str());
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

 int selected_engine_index=GetSelectedEngineIndex();

 Project->WriteToXml(ProjectXml);

try
{

 InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 RDK::UIVisualControllerStorage::SaveParameters(InterfaceXml);

 TProjectConfig config=Project->GetConfig();

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

 ProjectXml.SelectNodeRoot("Project/General");

 /*
 if(!config.DescriptionFileName.empty())
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
 } */

 for(int i=0;i<config.NumChannels;i++)
 {
  SelectEngine(i);

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

 SelectEngine(selected_engine_index);


 ProjectXml.SaveToFile(ProjectPath+ProjectFileName);
}
catch(RDK::UException &exception)
{
 Engine_LogMessage(exception.GetType(), (std::string("Core-SaveProject Exception: (Name=")+Name+std::string(") ")+exception.what()).c_str());
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
 PauseEngine(-1);

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
 for(int i=GetNumEngines();i>=0;i--)
 {
  SelectEngine(i);
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
 if(source_id>=GetNumEngines() || cloned_id >= GetNumEngines())
  return false;
/*
 try {
 RDK::TProjectConfig config=GetProjectConfig();
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
   */
 return true;
}

void UApplication::ReloadParameters(void)
{
 if(!ProjectOpenFlag)
  return;

 int channel_index=GetSelectedEngineIndex();

 TProjectConfig config=Project->GetConfig();
 std::string params_file_name=config.ChannelsConfig[GetSelectedEngineIndex()].ParametersFileName;
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
// --------------------------


// --------------------------
// Методы управления движком
// --------------------------
/// Управление числом каналов
int UApplication::GetNumEngines(void) const
{
 return EngineControl->GetNumEngines();
}

bool UApplication::SetNumEngines(int num)
{
 int old_num=GetNumEngines();
 if(!EngineControl->SetNumEngines(num))
  return false;

 if(!ServerControl->SetNumEngines(old_num))
  return false;

 return true;
}

bool UApplication::InsertEngine(int index)
{
 if(!EngineControl->InsertEngine(index))
  return false;

 if(!ServerControl->InsertEngine(index))
  return false;

 return true;
}

bool UApplication::DeleteEngine(int index)
{
 if(!EngineControl->DeleteEngine(index))
  return false;

 if(!ServerControl->DeleteEngine(index))
  return false;

 return true;
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
/// Запускает аналитику выбранного канала, или всех, если engine_index == -1
void UApplication::StartEngine(int engine_index)
{
 EngineControl->StartEngine(engine_index);
}

/// Останавливает аналитику выбранного канала, или всех, если engine_index == -1
void UApplication::PauseEngine(int engine_index)
{
 EngineControl->PauseEngine(engine_index);
}

/// Сбрасывает аналитику выбранного канала, или всех, если engine_index == -1
void UApplication::ResetEngine(int engine_index)
{
 EngineControl->ResetEngine(engine_index);
}

/// Делает шаг расчета выбранного канала, или всех, если engine_index == -1
void UApplication::StepEngine(int engine_index)
{
 EngineControl->StepEngine(engine_index);
}
// --------------------------

// --------------------------
// Методы загрузки сохранения данных в файл
// --------------------------
bool UApplication::LoadModelFromFile(int channel_index, const std::string file_name)
{
 if(!IsEngineInit())
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

bool UApplication::SaveModelToFile(int channel_index, const std::string file_name)
{
 if(!IsEngineInit())
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

bool UApplication::LoadParametersFromFile(int channel_index, const std::string file_name)
{
 if(!IsEngineInit())
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

bool UApplication::SaveParametersToFile(int channel_index, const std::string file_name)
{
 if(!IsEngineInit())
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

bool UApplication::LoadStatesFromFile(int channel_index, const std::string file_name)
{
 if(!IsEngineInit())
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
  return false;

 if(!data.empty())
 {
  int i=GetSelectedEngineIndex();
  SelectEngine(channel_index);
  Model_LoadComponentState("",&data[0]);
  SelectEngine(i);
  return true;
 }
 return false;
}

bool UApplication::SaveStatesToFile(int channel_index, const std::string file_name)
{
 if(!IsEngineInit())
  return false;

 int i=GetSelectedEngineIndex();
 SelectEngine(channel_index);
 const char *p_buf=Model_SaveComponentState("");
 bool res=true;
 if(p_buf)
 {
  res=SaveFile(file_name,p_buf);
 }
 Engine_FreeBufString(p_buf);
 SelectEngine(i);
 return res;
}

bool UApplication::LoadDescriptionFromFile(int channel_index, const std::string file_name)
{
 return false;
}

bool UApplication::SaveDescriptionToFile(int channel_index, const std::string file_name)
{
 return false;
}

bool UApplication::LoadClassesDescriptionsFromFile(const std::string file_name)
{
 if(!IsEngineInit())
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

bool UApplication::SaveClassesDescriptionsToFile(const std::string file_name)
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

bool UApplication::LoadCommonClassesDescriptionsFromFile(const std::string file_name)
{
 if(!IsEngineInit())
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

bool UApplication::SaveCommonClassesDescriptionsToFile(const std::string file_name)
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

/// Загружает файл в строку
bool UApplication::LoadFile(const std::string file_name, std::string &buffer) const
{
 std::ifstream t(file_name.c_str(), ios::in);

 if(!t)
 {
  buffer.clear();
  return false;
 }

 t.seekg(0, std::ios::end);
 buffer.reserve(t.tellg());
 t.seekg(0, std::ios::beg);

 buffer.assign((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
 return true;
}

/// Сохраняет файл из строки
bool UApplication::SaveFile(const std::string file_name, const std::string &buffer) const
{
 std::ofstream t(file_name.c_str(), ios::trunc);

 if(!t)
 {
  return false;
 }

 t<<buffer;
 return true;
}
// --------------------------

}

#endif

