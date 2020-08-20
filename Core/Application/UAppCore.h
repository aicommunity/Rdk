#ifndef UAppCore_H
#define UAppCore_H

#include "../Utilities/USupport.h"
#include "../Utilities/UIniFile.h"
#include "../../Deploy/Include/rdk_application.h"


namespace RDK {

///  ласс начальной инициализации
template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT>
class RDK_LIB_TYPE UAppCore
{
public:
 /// Ёкзепл€р прототипа декодера команд
 DecoderT rpcDecoder;

 /// Ёкзепл€р класса диспетчера команд
 DispatcherT rpcDispatcher;

 /// Ёкземпл€р класса контроллера сервера
 ServerControlT serverControl;
 DecoderCommonT rpcDecoderCommon;

 /// Ёкземпл€р класса контроллера расчета
 EngineControlT engineControl;

 /// Ёкзепл€р класса проекта
 ProjectT project;

 /// Ёкзепл€р класса приложени€
 ApplicationT application;

 /// Ёкземпл€р класса менеджера тестов
 TestManagerT rdkTestManager;

public:
 std::string startProjectName;
 int autoStartProjectFlag;
 int hideAdminForm;
 int startMinimized;

 std::string mainFormName;
 int minimizeToTray;
 std::string programName;
 std::string configsMainPath;
 int neverSleepOnMMThreadContention;
 std::string logDir;
 int startupDelay;

 int useNewXmlFormatProjectFile;
 int useNewProjectFilesStructure;

 int logDebugMode;

 int disableAdminForm;


public:
 UAppCore(void);
 ~UAppCore(void);

 /// »нициализаци€
 int Init(const std::string &application_file_name, const std::string &log_dir, int argc, char *argv[]);
};

template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT>
UAppCore<ApplicationT, EngineControlT, ProjectT, ServerControlT, TestManagerT, DispatcherT, DecoderT, DecoderCommonT>::UAppCore(void)
{
 rdkTestManager.SetApplication(&application);
 application.SetTestManager(&rdkTestManager);

 rpcDispatcher.SetDecoderPrototype(&rpcDecoder);
 rpcDispatcher.SetCommonDecoder(&rpcDecoderCommon);

 application.SetRpcDispatcher(&rpcDispatcher);
 application.SetServerControl(&serverControl);
 application.SetEngineControl(&engineControl);
 application.SetProject(&project);
}

template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT>
UAppCore<ApplicationT, EngineControlT, ProjectT, ServerControlT, TestManagerT, DispatcherT, DecoderT, DecoderCommonT>::~UAppCore(void)
{
 application.PauseChannel(-1);
 application.CloseProject();
 application.UnInit();
}

/// »нициализаци€
template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT>
int UAppCore<ApplicationT, EngineControlT, ProjectT, ServerControlT, TestManagerT, DispatcherT, DecoderT, DecoderCommonT>::Init(const std::string &application_file_name, const std::string &log_dir, int argc, char *argv[])
{
 // »нициализаци€ из стартового ini файла
 RDK::UIniFile<char> projectIniFile;
 projectIniFile.LoadFromFile("VideoAnalytics.ini");
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

 if(startupDelay>0)
 {
  RDK::Sleep(startupDelay);
 }

 logDebugMode=atoi(projectIniFile("Log","DebugMode","1"));

 disableAdminForm=atoi(projectIniFile("General","DisableAdminForm","0"));

 application.SetApplicationFileName(application_file_name);
 if(logDir.empty())
  application.SetLogDir(log_dir);
 else
  application.SetLogDir(logDir);

 application.SetDebugMode(logDebugMode);

 application.SetConfigsMainPath(configsMainPath);
 application.ChangeUseNewXmlFormatProjectFile(useNewXmlFormatProjectFile);
 application.ChangeUseNewProjectFilesStructure(useNewProjectFilesStructure);


 if(argc > 1)
   application.ProcessCommandLineArgs(argc, argv);

 application.Init();

 if(application.IsTestMode())
 {
   bool closeAfterTests = false;
   int returnCode = application.Test(closeAfterTests);
   if(closeAfterTests)
     return returnCode;
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

