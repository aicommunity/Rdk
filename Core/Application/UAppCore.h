#ifndef UAppCore_H
#define UAppCore_H

#include "../Utilities/USupport.h"
#include "../Utilities/UIniFile.h"
#include "../../Deploy/Include/rdk_application.h"

namespace RDK {

typedef void (*ProgressBarCallback)(int complete_percent);


///  ласс начальной инициализации
template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT, class ServerTransportT, class UProjectDeployerT>
class RDK_LIB_TYPE UAppCore
{
public:
 /// Ёкзепл€р прототипа декодера команд
 DecoderT rpcDecoder;

 /// Ёкзепл€р класса диспетчера команд
 DispatcherT rpcDispatcher;

 /// Ёкземпл€р класса контроллера сервера
 ServerControlT serverControl;

 /// Ёкземпл€р класса транспорта
 ServerTransportT serverTransport;

 DecoderCommonT rpcDecoderCommon;

 /// Ёкземпл€р класса контроллера расчета
 EngineControlT engineControl;

 /// Ёкзепл€р класса проекта
 ProjectT project;

 /// Ёкзепл€р класса приложени€
 ApplicationT application;

 /// Ёкземпл€р класса менеджера тестов
 TestManagerT rdkTestManager;

 /// Ёкземпл€р класса доставки конфигураций
 UProjectDeployerT projectDeployer;

public:
 std::string startProjectName;
 int autoStartProjectFlag;
 int autoexecLastProjectFlag;
 int hideAdminForm;
 int startMinimized;

 std::string mainFormName;
 int minimizeToTray;
 std::string programName;
 std::string configsMainPath;
 std::string databaseMainPath;
 std::string remoteFtpDatabasePath;
 int neverSleepOnMMThreadContention;
 std::string logDir;
 int startupDelay;
 std::string userName;
 int userId;

 int useNewXmlFormatProjectFile;
 int useNewProjectFilesStructure;

 int logDebugMode;

 int disableAdminForm;

 int serverPort;
 std::string serverAddress;
 int serverAutostartFlag;
 int serverStandaloneTask;
 int logCreationMode;
 int calcStopLogLevel;

 std::string librariesPath;
 std::string clDescPath;

 std::string temp_proj_path;

 std::string database_address;
 std::string database_name;
 std::string database_login;
 std::string database_password;

 std::string storageMountPoint;

 ProgressBarCallback FuncProgressBarCallback;

public:
 UAppCore(void);
 UAppCore(ProgressBarCallback func);
 ~UAppCore(void);

 /// »нициализаци€
 int Init(const std::string &application_file_name, const std::string &ini_file_name, const std::string &log_dir, int argc, char *argv[]);

 /// ѕост-инициализаци€
 int PostInit(void);
};

template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT, class ServerTransportT, class ProjectDeployerT>
UAppCore<ApplicationT, EngineControlT, ProjectT, ServerControlT, TestManagerT, DispatcherT, DecoderT, DecoderCommonT, ServerTransportT, ProjectDeployerT>::UAppCore(void)
{
 FuncProgressBarCallback=0;
 rdkTestManager.SetApplication(&application);
 rpcDispatcher.SetApplication(&application);

 rpcDecoder.SetDispatcher(&rpcDispatcher);
 rpcDecoderCommon.SetDispatcher(&rpcDispatcher);

 application.SetTestManager(&rdkTestManager);

 rpcDispatcher.SetDecoderPrototype(&rpcDecoder);
 rpcDispatcher.SetCommonDecoder(&rpcDecoderCommon);

 serverControl.SetApplication(&application);
 serverControl.SetRpcDispatcher(&rpcDispatcher);

 serverTransport.SetApplication(&application);
 serverControl.SetServerTransport(&serverTransport);

 //¬нутрь прописываетс€ по идее само
 application.SetProjectDeployer(&projectDeployer);

 //application.SetRpcDispatcher(&rpcDispatcher);
 application.SetServerControl(&serverControl);
 application.SetEngineControl(&engineControl);
 application.SetProject(&project);

}

template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT, class ServerTransportT, class ProjectDeployerT>
UAppCore<ApplicationT, EngineControlT, ProjectT, ServerControlT, TestManagerT, DispatcherT, DecoderT, DecoderCommonT, ServerTransportT, ProjectDeployerT>::UAppCore(ProgressBarCallback func)
 : UAppCore()
{
FuncProgressBarCallback=func;
}


template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT, class ServerTransportT, class ProjectDeployerT>
UAppCore<ApplicationT, EngineControlT, ProjectT, ServerControlT, TestManagerT, DispatcherT, DecoderT, DecoderCommonT, ServerTransportT, ProjectDeployerT>::~UAppCore(void)
{
 application.PauseChannel(-1);
 application.CloseProject();
 application.UnInit();
}

/// »нициализаци€
template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT, class ServerTransportT, class ProjectDeployerT>
int UAppCore<ApplicationT, EngineControlT, ProjectT, ServerControlT, TestManagerT, DispatcherT, DecoderT, DecoderCommonT, ServerTransportT, ProjectDeployerT>::Init(const std::string &application_file_name, const std::string& ini_file_name, const std::string &log_dir, int argc, char *argv[])
{
 //std::cout<<"Test sout init"<<std::endl;

 // »нициализаци€ из стартового ini файла
 RDK::UIniFile<char> projectIniFile;


 projectIniFile.LoadFromFile(ini_file_name);
 startProjectName = projectIniFile("General", "AutoexecProjectFileName", "");
 autoStartProjectFlag = RDK::atoi(projectIniFile("General", "AutoStartProjectFlag", "0"));
 autoexecLastProjectFlag = RDK::atoi(projectIniFile("General", "AutoexecLastProjectFlag", "0"));
 hideAdminForm        = RDK::atoi(projectIniFile("General", "HideAdminForm", "0"));
 startMinimized       = RDK::atoi(projectIniFile("General", "StartMinimized", "0"));

 mainFormName=projectIniFile("General", "MainFormName", "");
 minimizeToTray=atoi(projectIniFile("General","MinimizeToTray","0"));
 programName=projectIniFile("General","ProgramName","Server");
 configsMainPath=projectIniFile("General", "ConfigsMainPath", "../../../Configs/");
 neverSleepOnMMThreadContention=atoi(projectIniFile("General","NeverSleepOnMMThreadContention","0"));
 logDir=projectIniFile("Log","Dir",""); // TODO: јналог Log/FixedLogPath
 if(logDir.empty())
  logDir=projectIniFile("Log","FixedLogPath",""); // TODO: јналог Log/Dir
 logCreationMode=atoi(projectIniFile("Log","LogCreationMode","0"));
 calcStopLogLevel=atoi(projectIniFile("Log","CalcStopLogLevel","1"));

// bool SetFixedLogPath(const std::string& value);

 startupDelay=atoi(projectIniFile("General","StartupDelay","0"));

 useNewXmlFormatProjectFile=atoi(projectIniFile("General","UseNewXmlFormatProjectFile","0"));
 useNewProjectFilesStructure=atoi(projectIniFile("General","UseNewProjectFilesStructure","0"));

 librariesPath=projectIniFile("General", "LibrariesPath", "../../../");
 clDescPath=projectIniFile("General", "ClDescPath", "../../../ClDesc/");

 databaseMainPath=projectIniFile("General","DatabaseMainPath","");
 remoteFtpDatabasePath=projectIniFile("General","RemoteFtpDatabasePath","");

 logDebugMode=atoi(projectIniFile("Log","DebugMode","0"));

 disableAdminForm=atoi(projectIniFile("General","DisableAdminForm","0"));
 temp_proj_path = projectIniFile("General", "TemporaryProjectPath", "");

 serverPort = atoi(projectIniFile("Server","BindPort","45545"));
 serverAddress = projectIniFile("Server","BindAddress","127.0.0.2");
 serverAutostartFlag = atoi(projectIniFile("Server","AutostartServer","0"));
 serverStandaloneTask = atoi(projectIniFile("Server","StandaloneTask","-1"));

 database_address = projectIniFile("PostgreSQL","DatabaseAddress","127.0.0.1");
 database_name = projectIniFile("PostgreSQL","DatabaseName","videoanalytics");
 database_login = projectIniFile("PostgreSQL","DatabaseLogin","");
 database_password = projectIniFile("PostgreSQL","DatabasePassword","");

 storageMountPoint = projectIniFile("Storage","StorageMountPoint","");

 if(startupDelay>0)
 {
  RDK::Sleep(startupDelay);
 }

 application.SetApplicationFileName(application_file_name);
 application.SetLogCreationMode(logCreationMode);
 application.SetCalcStopLogLevel(calcStopLogLevel);

 if(logDir.empty())
 {
  application.SetFixedLogPath(log_dir);
  application.SetLogDir(log_dir);
 }
 else
 {
  application.SetFixedLogPath(logDir);
  application.SetLogDir(logDir);
 }

 application.SetDebugMode(logDebugMode);

 application.SetLibrariesPath(librariesPath);
 application.SetClDescPath(clDescPath);
 application.SetConfigsMainPath(configsMainPath);
 application.ChangeUseNewXmlFormatProjectFile(useNewXmlFormatProjectFile);
 application.ChangeUseNewProjectFilesStructure(useNewProjectFilesStructure);

 try
 {
   if(argc > 1)
     application.ProcessCommandLineArgs(argc, argv);
 }
 catch(exception &ex)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE,RDK_EX_WARNING,ex.what());
  return 11711;
 }

 if(FuncProgressBarCallback)
  FuncProgressBarCallback(15);

 // »нициализаци€ из стартового ini файла
 RDK::UIniFile<char> userIniFile;


 if(userIniFile.LoadFromFile(extract_file_name_wo_ext(ini_file_name)+".user.ini"))
 {
  userName             = userIniFile("UserData", "UserName", "");
  userId               = RDK::atoi(userIniFile("UserData", "UserId", "-1"));

  if(!userName.empty())
   application.SetUserName(userName);

  if(userId>=0)
   application.SetUserId(userId);
 }

 application.Init();

 if(FuncProgressBarCallback)
  FuncProgressBarCallback(20);

 return 0;
}

/// ѕост-инициализаци€
template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT, class ServerTransportT, class ProjectDeployerT>
int UAppCore<ApplicationT, EngineControlT, ProjectT, ServerControlT, TestManagerT, DispatcherT, DecoderT, DecoderCommonT, ServerTransportT, ProjectDeployerT>::PostInit(void)
{
 if(application.IsTestMode())
 {
   bool closeAfterTests = false;
   int returnCode = application.Test(closeAfterTests);
   if(closeAfterTests)
     return returnCode;
 }

 if(FuncProgressBarCallback)
  FuncProgressBarCallback(30);

 if(storageMountPoint!="")
     application.SetStorageMountPoint(storageMountPoint);

 if(databaseMainPath!="")
 {
     application.SetDatabaseMainPath(databaseMainPath);
 }

 if(temp_proj_path!="")
 {
    application.GetProjectDeployer()->SetTempProjectDeploymentPath(temp_proj_path);
 }

 application.GetServerControl()->GetServerTransport()->SetServerBinding(serverAddress, serverPort);
 cout<<"ServerAutoStartFlag: "<<serverAutostartFlag<<"\n";
 std::cout<<"Test cout "<<serverAddress.c_str()<<" "<<serverPort<<std::endl;
 if(serverAutostartFlag)
 {
   std::cout<<"Start TCP server on "<<serverAddress.c_str()<<" "<<serverPort<<std::endl;
   application.GetServerControl()->GetServerTransport()->ServerStart();
 }

 application.GetProjectDeployer()->SetStandaloneTask(serverStandaloneTask);

 if(remoteFtpDatabasePath!="")
 {
     application.GetProjectDeployer()->SetFtpRemotePath(remoteFtpDatabasePath);
 }

 if(FuncProgressBarCallback)
  FuncProgressBarCallback(40);

 if(database_login!="" && database_password!="")
 {
     application.GetProjectDeployer()->SetDatabaseAccess(database_address, database_name, database_login, database_password);
 }

 if(FuncProgressBarCallback)
  FuncProgressBarCallback(50);

 if(!startProjectName.empty())
  application.OpenProject(startProjectName);
 else
 if(autoexecLastProjectFlag)
 {
  const std::list<std::string> last_projects=application.GetLastProjectsList();
  if(!last_projects.empty())
  {
   std::string last_project_name=last_projects.front();
   application.OpenProject(last_project_name);
  }
 }

 if(FuncProgressBarCallback)
  FuncProgressBarCallback(80);

 if(autoStartProjectFlag)
 {
  if(!application.GetProjectOpenFlag())
   return -1;
  application.StartChannel(-1);
 }

 if(FuncProgressBarCallback)
  FuncProgressBarCallback(90);

 return 0;
}


}

#endif

