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

#ifdef __BORLANDC__
#include "Bcb/Application.bcb.h"
#endif

namespace RDK {

class UEngineControl;
class UServerControl;

class RDK_LIB_TYPE UApplication: public UAppController
{
protected: // ������ ����������
/// ��� ����� ����������
std::string ApplicationFileName;

/// ������� �������
std::string WorkDirectory;

// ������� ������� ��������� �������
bool ProjectOpenFlag;

// ���� �� ����� �������
std::string ProjectPath;

// ��� ����� �������
std::string ProjectFileName;

/// ������ ��������� �������� ��������
std::list<std::string> LastProjectsList;

/// ������ ������� ��������� �������� ��������
int LastProjectsListMaxSize;


protected: // ������ ����������
/// ��������� ������
UEPtr<URpcDispatcher> RpcDispatcher;

/// �������� ������
UEPtr<UProject> Project;

/// ���������� ������
UEPtr<UEngineControl> EngineControl;

/// ���������� ��������� �����
UEPtr<UServerControl> ServerControl;

protected: // ��������� ����������
/// ��������� ����������
std::string AppCaption;

// ���� �������� �������
RDK::USerStorageXML ProjectXml;

// ���� �������� ����������
RDK::USerStorageXML InterfaceXml;

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
/// ��� ����� ����������
const std::string& GetApplicationFileName(void) const;
bool SetApplicationFileName(const std::string& value);

/// ������� �������
const std::string& GetWorkDirectory(void) const;
bool SetWorkDirectory(const std::string& value);

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

/// ��������� ����������
const std::string& GetAppCaption(void) const;

// ���� �������� �������
const RDK::USerStorageXML& GetProjectXml(void) const;

// ���� �������� ����������
const RDK::USerStorageXML& GetInterfaceXml(void) const;
// --------------------------

// --------------------------
// ������ �������������
// --------------------------
/// ������������� ������ � ���������� ������
virtual UEPtr<URpcDispatcher> GetRpcDispatcher(void);

/// ������������� ����� ��������� ������
/// ��������������� �� ������������ ������ ���������� ����� �� ���������� �������
virtual bool SetRpcDispatcher(const UEPtr<URpcDispatcher> &value);

/// ������������� ������ � ����������� ������
virtual UEPtr<UEngineControl> GetEngineControl(void);

/// ������������� ����� ���������� ������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
virtual bool SetEngineControl(const UEPtr<UEngineControl> &value);

/// ������������� ������ � �������
virtual UEPtr<UProject> GetProject(void);

/// ������������� ����� ������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
virtual bool SetProject(const UEPtr<UProject> &value);

/// ���������� ������������ �������
virtual const TProjectConfig& GetProjectConfig(void) const;

/// ������������� ����� ������������ �������
virtual bool SetProjectConfig(const TProjectConfig& value);

/// ������������� ������ � ����������� ��������� �����
virtual UEPtr<UServerControl> GetServerControl(void) const;

/// ������������� ����� ���������� �������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
virtual bool SetServerControl(const UEPtr<UServerControl> &value);

/// �������������� ����������
virtual bool Init(void);

/// ���������������� ����������
virtual bool UnInit(void);
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
/// ������� ������ (����� ���������� � ��������)
virtual bool CreateProject(const std::string &file_name, RDK::TProjectConfig &project_config);

/// ��������� ������
virtual bool OpenProject(const std::string &filename);

/// ��������� ������
virtual bool SaveProject(void);
virtual bool SaveProjectAs(const std::string &filename);

/// ��������� ������
virtual bool CloseProject(void);

/// ��������� ������ � ����� ������������
virtual bool CloneProject(const std::string &filename);
virtual bool CloneProject(int source_id, int cloned_id);

virtual void ReloadParameters(void);

virtual bool CopyProject(const std::string &new_path);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
/// ���������� ������ �������
int GetNumEngines(void) const;
bool SetNumEngines(int num);
bool InsertEngine(int index);
bool DeleteEngine(int index);
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
/// ��������� ��������� ���������� ������, ��� ����, ���� engine_index == -1
virtual void StartEngine(int engine_index);

/// ������������� ��������� ���������� ������, ��� ����, ���� engine_index == -1
virtual void PauseEngine(int engine_index);

/// ���������� ��������� ���������� ������, ��� ����, ���� engine_index == -1
virtual void ResetEngine(int engine_index);

/// ������ ��� ������� ���������� ������, ��� ����, ���� engine_index == -1
virtual void StepEngine(int engine_index);
// --------------------------

protected:
// --------------------------
// ��������������� ������ ���������� ������
// --------------------------
/// ��������� ��������� ����������
void CalcAppCaption(void);

/// ��������� ���� � ������
bool LoadFile(const std::string file_name, std::string &buffer) const;

/// ��������� ���� �� ������
bool SaveFile(const std::string file_name, const std::string &buffer) const;

bool LoadModelFromFile(int channel_index, const std::string file_name);
bool SaveModelToFile(int channel_index, const std::string file_name);

bool LoadParametersFromFile(int channel_index, const std::string file_name);
bool SaveParametersToFile(int channel_index, const std::string file_name);

bool LoadStatesFromFile(int channel_index, const std::string file_name);
bool SaveStatesToFile(int channel_index, const std::string file_name);

bool LoadDescriptionFromFile(int channel_index, const std::string file_name);
bool SaveDescriptionToFile(int channel_index, const std::string file_name);

bool LoadClassesDescriptionsFromFile(const std::string file_name);
bool SaveClassesDescriptionsToFile(const std::string file_name);

bool LoadCommonClassesDescriptionsFromFile(const std::string file_name);
bool SaveCommonClassesDescriptionsToFile(const std::string file_name);

/// ��������� ������� �������� �� �����
void LoadProjectsHistory(void);

/// ��������� ������� �������� � ����
void SaveProjectsHistory(void);
// --------------------------
};

}

#endif

