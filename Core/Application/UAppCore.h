#ifndef UAppCore_H
#define UAppCore_H

#include "../../../../Rdk/Core/Utilities/USupport.h"
#include "../../../../Rdk/Core/Utilities/UIniFile.h"
#include "../../../../Rdk/Deploy/Include/rdk_application.h"


namespace RDK {

/// ����� ��������� �������������
template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT>
class RDK_LIB_TYPE UAppCore
{
public:
 /// �������� ��������� �������� ������
 DecoderT rpcDecoder;

 /// �������� ������ ���������� ������
 DispatcherT rpcDispatcher;

 /// ��������� ������ ����������� �������
 ServerControlT serverControl;
 DecoderCommonT rpcDecoderCommon;

 /// ��������� ������ ����������� �������
 EngineControlT engineControl;

 /// �������� ������ �������
 ProjectT project;

 /// �������� ������ ����������
 ApplicationT application;

 /// ��������� ������ ��������� ������
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

 /// �������������
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

/// �������������
template<class ApplicationT, class EngineControlT, class ProjectT, class ServerControlT, class TestManagerT, class DispatcherT, class DecoderT, class DecoderCommonT>
int UAppCore<ApplicationT, EngineControlT, ProjectT, ServerControlT, TestManagerT, DispatcherT, DecoderT, DecoderCommonT>::Init(const std::string &application_file_name, const std::string &log_dir, int argc, char *argv[])
{
 // ������������� �� ���������� ini �����
 RDK::UIniFile<char> projectIniFile;
 projectIniFile.LoadFromFile("VideoAnalytics.ini");
 std::string startProjectName = projectIniFile("General", "AutoexecProjectFileName", "");
 int autoStartProjectFlag = RDK::atoi(projectIniFile("General", "autoStartProjectFlag", "0"));
 int hideAdminForm        = RDK::atoi(projectIniFile("General", "HideAdminForm", "0"));
 int startMinimized       = RDK::atoi(projectIniFile("General", "StartMinimized", "0"));

 std::string mainFormName=projectIniFile("General", "MainFormName", "");
 int minimizeToTray=atoi(projectIniFile("General","MinimizeToTray","0"));
 std::string programName=projectIniFile("General","ProgramName","Server");
 std::string configsMainPath=projectIniFile("General", "ConfigsMainPath", "../../../Configs/");
 int neverSleepOnMMThreadContention=atoi(projectIniFile("General","NeverSleepOnMMThreadContention","0"));
 std::string logDir=projectIniFile("Log","Dir","");
 int startupDelay=atoi(projectIniFile("General","StartupDelay","0"));

 int useNewXmlFormatProjectFile=atoi(projectIniFile("General","UseNewXmlFormatProjectFile","0"));
 int useNewProjectFilesStructure=atoi(projectIniFile("General","UseNewProjectFilesStructure","0"));

 if(startupDelay>0)
 {
  RDK::Sleep(startupDelay);
 }

 int logDebugMode=atoi(projectIniFile("Log","DebugMode","1"));

 int disableAdminForm=atoi(projectIniFile("General","DisableAdminForm","0"));

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

