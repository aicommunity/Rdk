#ifndef UApplication_H
#define UApplication_H

#include "UAppCore.h"
#include "UProject.h"
#include "UIVisualController.h"
#include "URpcDispatcherQueues.h"
#include "URpcDispatcher.h"
#include "URpcDecoder.h"
#include "URpcDecoderInternal.h"
#include "URpcDecoderCommon.h"
#include "URpcCommand.h"
#include "URpcCommandInternal.h"
#include "UEngineControl.h"
#include "UEngineControlThread.h"
#include "UEngineStateThread.h"
#include "UBroadcasterInterface.h"
#include "UServerControl.h"
#include "UServerTransport.h"
#include "UProjectDeployer.h"
#include "UChannelProfiler.h"
#include "UTestManager.h"


namespace RDK {

class UEngineControl;
class UServerControl;

struct StandartXMLInCatalog
{
/// ��� xml �����
std::string XMLName;
/// ��� xml �����
std::string XMLDescription;
};

class RDK_LIB_TYPE UApplication: public UAppController
{
protected: // ������ ����������
/// ��� ����� ����������
std::string ApplicationFileName;

/// ������� �������
std::string WorkDirectory;

/// ������������� ���� �� ����� � ���������� ������������ (������ /Bin/Configs)
std::string ConfigsMainPath;

/// ������������� ���� �� ����� � ������������ (� ������ ���� ������������ ��� ����� - MockLibs, RTlibs)
std::string LibrariesPath;

/// ������������� ���� �� ����� � ���������� �������
std::string ClDescPath;

/// ������������� ���� �� ����� � ���������� �������� (���� ������ /Database)
std::string DatabaseMainPath;

/// ���������� ���� �� ����� ������������ ���������
std::string StorageMountPoint;

/// ������������� ���� �� ����� � ���������� ������� (������ /Bin/Models)
std::string ModelsMainPath;

/// ��� ������������
std::string UserName;

/// Id ������������
int UserId;

/// ������� ������� ��������� �������
bool ProjectOpenFlag;

/// ���� �� ����� �������
std::string ProjectPath;

/// ��� ����� �������
std::string ProjectFileName;

/// ���� ��������������� ���������� ������������ � ������ �������
bool UseNewXmlFormatProjectFile;

/// ���� ��������� ������ ������������� �������� ��������� ������������
/// (������ ��� ���������� ������ ������������ � ����� �������)
bool UseNewProjectFilesStructure;

/// ������ ��������� �������� ��������
std::list<std::string> LastProjectsList;

/// ������ ������� ��������� �������� ��������
int LastProjectsListMaxSize;

/// ����, ������������ ���� ������� ����� ������������
bool TestMode;

/// ��� ����� � ��������� ������
std::string TestsDescriptionFileName;

/// ������� ���������� ��������� ������ ���������� ����� ������������
bool CloseAfterTest;

/// ���� ��������������
bool AppIsInit;

///������ ���� � �������� xml ������ �� ����� � ���������� ������� (������ /Bin/Models)
std::list<StandartXMLInCatalog> xmlInCatalog;

/// ������ ������ ���������:
// 1 -  ������� ������. ������ ����������� ���������, ����� ������������. ��� ���������� ������� (�� ��������)
// 2 -  ����������� ������. ������ �����������, ����� ���������-��������, ����� ������������.
//      ��� ����������, ������� ������ ��������� �������� - �������.
//      ��������� (������� ���� � �����������-��������, �� ��� ������ � ���������) - ��������.
// 3 -  ������ ������ ��������. ������ ���������-��������, ����� ������������. ��� ���������� �������� (�� �������)
int StorageBuildMode;

/// ������������� ���� �� �����
/// ������������ ������ ���������� ���� ���� �����
std::string FixedLogPath;

/// ����� ������ �����
/// 0 - ������ �� ��������� (���� ��������� ������ ��� ������ ������ ResetChannel ��� StartChannel � ����� ������������)
/// 1 - ���� ���� ��������� ������ ������ ��� �������� ������ ����� ������������. � ����� ������������
/// 2 - ���� ���� ��������� ������ ������ ��� �������� ������ ����� ������������. � ��������� �����
/// 3 - ���� ���� ��������� �������� �� ���� ������ ������ ���������� � ��������� �����
int LogCreationMode;

/// ������� ��������� � ������� ��� ��������� �������� �������������� �������������� ������� �������
int CalcStopLogLevel;

/// ��������� ������ ��������� � cout
bool CoutLogMode;

protected: // ������ ����������
/// ��������� ������
std::shared_ptr<URpcDispatcher> RpcDispatcher;

/// �������� ������
std::shared_ptr<UProject> Project;

/// ���������� ������
std::shared_ptr<UEngineControl> EngineControl;

/// ���������� ��������� �����
std::shared_ptr<UServerControl> ServerControl;

/// �������� ������
std::shared_ptr<UTestManager> TestManager;

/// ������� �������
std::shared_ptr<UProjectDeployer> ProjectDeployer;

protected: // ��������� ����������
/// �������� ����������
std::string ProgramName;

/// ��������� ����������
std::string AppCaption;

/// ���� �������� �������
RDK::USerStorageXML ProjectXml;

/// ���� ������� �������
RDK::USerStorageXML HistoryXml;

/// ���� �������� ����������
RDK::USerStorageXML InterfaceXml;

/// ���������� ��� ���������� ������
std::string SaveBuffer;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UApplication(void);
virtual ~UApplication(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// �������� ����������
const std::string& GetProgramName(void) const;
void SetProgramName(const std::string &value);

/// ��� ����� ����������
const std::string& GetApplicationFileName(void) const;
bool SetApplicationFileName(const std::string& value);

/// ������� �������
const std::string& GetWorkDirectory(void) const;
bool SetWorkDirectory(const std::string& value);

/// ������������� ���� �� ����� � ���������� ������������ (������ /Bin/Configs)
const std::string& GetConfigsMainPath(void) const;
bool SetConfigsMainPath(const std::string &value);

const std::string& GetDatabaseMainPath(void) const;
bool SetDatabaseMainPath(const std::string &value);

const std::string& GetStorageMountPoint(void) const;
bool SetStorageMountPoint(const std::string &value);

/// ������������� ���� �� ����� � ������������ (� ������ ���� ������������ ��� ����� - MockLibs, RTLibs)
const std::string& GetLibrariesPath(void) const;
bool SetLibrariesPath(const std::string &value);

/// ������������� ���� �� ����� � ���������� �������
const std::string& GetClDescPath(void) const;
bool SetClDescPath(const std::string &value);

/// ������������� ���� �� ����� � ���������� ������� (������ /Bin/Models)
const std::string& GetModelsMainPath(void) const;
bool SetModelsMainPath(const std::string &value);

/// ��� ������������
const std::string& GetUserName(void) const;
bool SetUserName(const std::string &value);

/// Id ������������
int GetUserId(void) const;
bool SetUserId(int value);


// ������� ������� ��������� �������
bool GetProjectOpenFlag(void) const;
bool SetProjectOpenFlag(bool value);

// ���� �� ����� �������
const std::string& GetProjectPath(void) const;
bool SetProjectPath(const std::string& value);

// ��� ����� �������
const std::string& GetProjectFileName(void) const;
bool SetProjectFileName(const std::string& value);

/// ������ ��������� �������� ��������
const std::list<std::string>& GetLastProjectsList(void) const;
bool SetLastProjectsList(const std::list<std::string>& value);

/// ������ ������� ��������� �������� ��������
int GetLastProjectsListMaxSize(void) const;
bool SetLastProjectsListMaxSize(int value);

/// ���� ��������������� ���������� ������������ � ������ �������
bool IsUseNewXmlFormatProjectFile(void) const;
bool ChangeUseNewXmlFormatProjectFile(bool value);

/// ���� ��������� ������ ������������� �������� ��������� ������������
/// (������ ��� ���������� ������ ������������ � ����� �������)
bool IsUseNewProjectFilesStructure(void) const;
bool ChangeUseNewProjectFilesStructure(bool value);

///������ ���� � �������� xml ������ �� ����� � ���������� ������� (������ /Bin/Models)
const std::list<StandartXMLInCatalog>& GetStandartXMLInCatalog(void) const;
bool SetStandartXMLInCatalog(void);

/// ������������� ���� �� �����
const std::string& GetFixedLogPath(void) const;
bool SetFixedLogPath(const std::string& value);

/// ����� ������ �����
/// 0 - ������ �� ��������� (���� ��������� ������ ��� ������ ������ ResetChannel ��� StartChannel � ����� ������������)
/// 1 - ���� ���� ��������� ������ ������ ��� �������� ������ ����� ������������. � ����� ������������
/// 2 - ���� ���� ��������� ������ ������ ��� �������� ������ ����� ������������. � ��������� �����
/// 3 - ���� ���� ��������� �������� �� ���� ������ ������ ���������� � ��������� �����
int GetLogCreationMode(void) const;
bool SetLogCreationMode(int mode);

/// ������� ��������� � ������� ��� ��������� �������� �������������� �������������� ������� �������
int GetCalcStopLogLevel(void) const;
bool SetCalcStopLogLevel(int log_level);

/// ��������� ������ ��������� � cout
bool GetCoutLogMode(void) const;
bool SetCoutLogMode(bool value);

/// ��������� ����������
const std::string& GetAppCaption(void) const;

// ���� �������� �������
const RDK::USerStorageXML& GetProjectXml(void) const;

// ���� �������� ����������
const RDK::USerStorageXML& GetInterfaceXml(void) const;

/// ������� ����� (���������� ���������)
std::string GetLogDir(void) const;
bool SetLogDir(const std::string& value);

/// ���� ��������� ����������� ������ �����������
bool GetDebugMode(void) const;
bool SetDebugMode(bool value);

/// ������� ������� ����� (� ������ ��������������� � �������)
std::string CalcCurrentLogDir(void) const;

/// ����, ������������ ���� ������� ����� ������������
bool IsTestMode(void) const;

/// ��� ����� � ��������� ������
const std::string& GetTestsDescriptionFileName(void) const;
void SetTestsDescriptionFileName(const std::string& value);

/// ������� ���������� ��������� ������ ���������� ����� ������������
bool IsCloseAfterTest(void) const;

/// ���������� �����������������
bool IsInit(void) const;

/// ��������� ������������ ������ ������
void SetStorageBuildMode(int mode);

/// ��������� �������� ������ ������
int GetStorageBuildMode();
// --------------------------
/// �������� ���������-�������� �� ����������� ��������� � ����������� ������
void CreateSaveMockLibs();

// --------------------------
// ������ �������������
// --------------------------
/// ������������� ������ � ���������� ������
//virtual std::shared_ptr<URpcDispatcher> GetRpcDispatcher(void);

/// ������������� ����� ��������� ������
/// ��������������� �� ������������ ������ ���������� ����� �� ���������� �������
//virtual bool SetRpcDispatcher(const std::shared_ptr<URpcDispatcher> &value);

/// ������������� ������ � ����������� ������
virtual std::shared_ptr<UEngineControl> GetEngineControl(void);

/// ������������� ����� ���������� ������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
virtual bool SetEngineControl(const std::shared_ptr<UEngineControl> &value, const std::shared_ptr<UApplication> &self);

/// ������������� ������ � �������
//virtual std::shared_ptr<UProject> GetProject(void);

/// ������������� ����� ������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
virtual bool SetProject(const std::shared_ptr<UProject> &value);

/// ���������� ������������ �������
virtual const TProjectConfig& GetProjectConfig(void) const;

/// ������������� ����� ������������ �������
virtual bool SetProjectConfig(const TProjectConfig& value);

/// ������������� ������ � ����������� ��������� �����
virtual std::shared_ptr<UServerControl> GetServerControl(void) const;

/// ������������� ����� ���������� �������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
virtual bool SetServerControl(const std::shared_ptr<UServerControl> &value, const std::shared_ptr<UApplication> &self);

/// �������� ������
/// ��������������� �� ������������ ������ ��������� ����� �� ���������� �������
std::shared_ptr<UTestManager> GetTestManager(void);
virtual bool SetTestManager(const std::shared_ptr<UTestManager> &value, const std::shared_ptr<UApplication> &self);

/// ������� ������� (��� ���������� ������)
std::shared_ptr<UProjectDeployer> GetProjectDeployer(void);
virtual bool SetProjectDeployer(const std::shared_ptr<UProjectDeployer> &value, const std::shared_ptr<UApplication> &self);

/// �������������� ����������
virtual bool Init(void);

/// ���������������� ����������
virtual bool UnInit(void);

/// �������� ������������ ����������, ���� �������� ������ ��������������� �
/// �������� ����� �������
/// ���������� ��� ������ ������������.
/// ���� exit_request == true,
/// �� �� ���������� ������ ���������� ������ ���� ������� � ������������ ����� ������
virtual int Test(bool &exit_request);

/// ������������ ������� ��������� ������ � ��������������� ��������� ����������
void ProcessCommandLineArgs(std::vector<std::string> commandLineArgs);

#ifndef __BORLANDC__
/// ������������ ������� ��������� ������ � ��������������� ��������� ����������
void ProcessCommandLineArgs(int argc, char **argv);
#endif
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
/// ������� ������ (����� ���������� � ��������)
virtual bool CreateProject(const std::string &file_name, RDK::TProjectConfig &project_config);
virtual bool CreateProject(const std::string &file_name, const std::string &model_classname);

/// ��������� ������ �� ����� ������������
virtual bool UpdateProject(RDK::TProjectConfig &project_config);

/// ��������� ������
virtual bool OpenProject(const std::string &filename);

/// ��������� ������
virtual bool SaveProject(void);
virtual bool SaveProjectAs(const std::string &filename);

/// ��������� ������
virtual bool CloseProject(void);

/// ��������� ������ � ����� ������������
virtual bool CloneProject(const std::string &filename);

/// ��������������� ����� �������
virtual bool RenameProject(const std::string &filename);

virtual void ReloadParameters(void);

virtual bool CopyProject(const std::string &new_path);

/// ��������� ������ ���� �������� �������
virtual bool SaveProjectConfig(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
/// ���������� ������ �������
int GetNumChannels(void) const;
bool SetNumChannels(int num);
bool InsertChannel(int index);
bool DeleteChannel(int index);

/// ��������� ����� source_id � cloned_id
virtual bool CloneChannel(int source_id, int cloned_id);
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
/// ��������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
virtual void StartChannel(int channel_index);

/// ������������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
virtual void PauseChannel(int channel_index);

/// ���������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
virtual void ResetChannel(int channel_index);

/// ������ ��� ������� ���������� ������, ��� ����, ���� channel_index == -1
virtual void StepChannel(int channel_index);

/// ���������� true ���� ����� �������
virtual bool IsChannelStarted(int channel_index);

/// ��������� ��������� ������� �� id ������
virtual UEngineControl::UCalcState CheckCalcState(int channel_id) const;
// --------------------------

// --------------------------
// ������ �������� ���������� ������ � ����
// --------------------------
bool LoadModelFromFile(int channel_index, const std::string &file_name);
bool SaveModelToFile(int channel_index, const std::string &file_name);

bool LoadParametersFromFile(int channel_index, const std::string &file_name);
bool SaveParametersToFile(int channel_index, const std::string &file_name);

bool LoadStatesFromFile(int channel_index, const std::string &file_name);
bool SaveStatesToFile(int channel_index, const std::string &file_name);

bool LoadDescriptionFromFile(int channel_index, const std::string &file_name);
bool SaveDescriptionToFile(int channel_index, const std::string &file_name);

bool LoadClassesDescriptionsFromFile(const std::string &file_name);
bool SaveClassesDescriptionsToFile(const std::string &file_name);

bool LoadCommonClassesDescriptionsFromFile(const std::string &file_name);
bool SaveCommonClassesDescriptionsToFile(const std::string &file_name);

/// ��������� ������� �������� �� �����
void LoadProjectsHistory(void);

/// ��������� ������� �������� � ����
void SaveProjectsHistory(void);
// --------------------------

protected:
// --------------------------
// ��������������� ������ ���������� ������
// --------------------------
/// ��������� ����� � ������� ��������� ������������
bool FixSavePoint(USerStorageXML &xml);

/// �������� � ��������� �������� �����
void ChangeTestModeState(bool state);

/// ������������� ������� ��������� ������
void InitCmdParser(void);

/// ��������� ��������� ����������
void CalcAppCaption(void);

/// ��������� ��������� ������� ������������
void UpdateLoggers(void);

///  glog  
void InitializeGlogLogging();

/// ��������� ���� �� ������, ����� ��������� ����. ������ n_pass ������� ��������� � ������� ���������� � ����������� � ����������.
/// ���� ���������� �� �������, �� ������ ���� �������� ��� ���.
/// ���� ���������� �������, �� ��������� ���� �������� ������
bool SaveFileSafe(const std::string &file_name, const std::string &buffer, const std::string &temp_file_name, int n_pass=3);
// --------------------------
};



}

#endif

