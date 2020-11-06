#ifndef UAppCore_H
#define UAppCore_H

#include "../Utilities/USupport.h"
#include "../Utilities/UIniFile.h"
#include "../../Deploy/Include/rdk_application.h"

namespace RDK {

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

 int useNewXmlFormatProjectFile;
 int useNewProjectFilesStructure;

 int logDebugMode;

 int disableAdminForm;

 int serverPort;
 std::string serverAddress;
 int serverAutostartFlag;


public:
 UAppCore(void);
 ~UAppCore(void);

 /// »нициализаци€
 int Init(const std::string &application_file_name, const std::string &ini_file_name, const std::string &log_dir, int argc, char *argv[]);
};

template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT, class ServerTransportT, class ProjectDeployerT>
UAppCore<ApplicationT, EngineControlT, ProjectT, ServerControlT, TestManagerT, DispatcherT, DecoderT, DecoderCommonT, ServerTransportT, ProjectDeployerT>::UAppCore(void)
{
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
 autoStartProjectFlag = RDK::atoi(projectIniFile("General", "autoStartProjectFlag", "0"));
 hideAdminForm        = RDK::atoi(projectIniFile("General", "HideAdminForm", "0"));
 startMinimized       = RDK::atoi(projectIniFile("General", "StartMinimized", "0"));

 mainFormName=projectIniFile("General", "MainFormName", "");
 minimizeToTray=atoi(projectIniFile("General","MinimizeToTray","0"));
 programName=projectIniFile("General","ProgramName","Server");
 configsMainPath=projectIniFile("General", "ConfigsMainPath", "../../../Configs/");
 neverSleepOnMMThreadContention=atoi(projectIniFile("General","NeverSleepOnMMThreadContention","0"));
 logDir=projectIniFile("Log","Dir","");
 startupDelay=atoi(projectIniFile("General","StartupDelay","0"));

 useNewXmlFormatProjectFile=atoi(projectIniFile("General","UseNewXmlFormatProjectFile","0"));
 useNewProjectFilesStructure=atoi(projectIniFile("General","UseNewProjectFilesStructure","0"));

 databaseMainPath=projectIniFile("General","DatabaseMainPath","");
 remoteFtpDatabasePath=projectIniFile("General","RemoteFtpDatabasePath","");

 if(startupDelay>0)
 {
  RDK::Sleep(startupDelay);
 }

 logDebugMode=atoi(projectIniFile("Log","DebugMode","1"));

 disableAdminForm=atoi(projectIniFile("General","DisableAdminForm","0"));
 std::string temp_proj_path = projectIniFile("General", "TemporaryProjectPath", "");

 serverPort = atoi(projectIniFile("Server","BindPort","45545"));
 serverAddress = projectIniFile("Server","BindAddress","127.0.0.2");
 serverAutostartFlag = atoi(projectIniFile("Server","AutostartServer","0"));

 std::string database_address = projectIniFile("PostgreSQL","DatabaseAddress","127.0.0.1");
 std::string database_name = projectIniFile("PostgreSQL","DatabaseName","videoanalytics");
 std::string database_login = projectIniFile("PostgreSQL","DatabaseLogin","");
 std::string database_password = projectIniFile("PostgreSQL","DatabasePassword","");


 application.SetApplicationFileName(application_file_name);
 if(logDir.empty())
  application.SetLogDir(log_dir);
 else
  application.SetLogDir(logDir);

 application.SetDebugMode(logDebugMode);

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

 application.Init();

 if(application.IsTestMode())
 {
   bool closeAfterTests = false;
   int returnCode = application.Test(closeAfterTests);
   if(closeAfterTests)
     return returnCode;
 }

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

 if(remoteFtpDatabasePath!="")
 {
     application.GetProjectDeployer()->SetFtpRemotePath(remoteFtpDatabasePath);
 }

 if(database_login!="" && database_password!="")
 {
     application.GetProjectDeployer()->SetDatabaseAccess(database_address, database_name, database_login, database_password);
 }

 if(!startProjectName.empty())
  application.OpenProject(startProjectName);

 if(autoStartProjectFlag)
 {
  if(!application.GetProjectOpenFlag())
   return -1;
  application.StartChannel(-1);
 }
 return 0;
}

}

#endif

