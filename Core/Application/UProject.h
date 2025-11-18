#ifndef UAppProject_H
#define UAppProject_H

#include "UIVisualController.h"

namespace RDK
{
class UEnvironment; // Forward declaration

struct RDK_LIB_TYPE TProjectChannelConfig
{
/// ������� ��������� ������ ��� ������
/// 0 - �� ��������� (�� ��������)
/// 1 - ����������������� ������������
/// 2 - ������ �� �����
/// 3 - ������ �� ������ ���������
int ModelMode;

/// ������������� ����������������� ������������
int PredefinedStructure;

/// ��� ����� ������
std::string ModelFileName;

/// ��� ����� ����������
std::string ParametersFileName;

/// ��� ����� ���������
std::string StatesFileName;

/// ��� ������ ���������� ������
std::string ClassName;

/// ���������� ��� �����
int GlobalTimeStep;

/// ��� ����� �� ���������
int DefaultTimeStep;

/// ����� �������
int CalculationMode;

/// ����������� �������� ����� ������ �������, ��
int MinInterstepsInterval;

/// ���������������� ����� ����� ��������
bool InitAfterLoad;

/// �������� ������ ����� ��������
bool ResetAfterLoad;

/// ������������ ���������� �����
bool DebugMode;

/// ����� ��������� ������� ��� �����������
unsigned int DebugSysEventsMask;

/// ���� ��������� ������ ���� � ��������
bool DebuggerMessageFlag;

/// �������� ������ ������� � ��������� ���-���� ������
bool EventsLogMode;

/// ��� ������
std::string ChannelName;

/// ���������� ���������� ����� ������� ������
double MaxCalculationModelTime;

/// ���� ��������� ������������� ��������������� ���������� TimeStep ���
/// ������� ����������
bool UseIndTimeStepFlag;

TProjectChannelConfig(void);
TProjectChannelConfig(const TProjectChannelConfig& copy);

bool operator != (const TProjectChannelConfig& copy) const;
bool operator == (const TProjectChannelConfig& copy) const;

TProjectChannelConfig& operator = (const TProjectChannelConfig& copy);
};


struct RDK_LIB_TYPE TProjectConfig
{
/// ��� �������
std::string ProjectName;

/// �������� �������
std::string ProjectDescription;

/// ��� ����� �������� �������
std::string DescriptionFileName;

/// ��� ������������
std::string UserName;

/// Id ������������
int UserId;

/// ����� ����������
std::string CreationTime;

/// ���� ��������������� ���������� ������� ��� ��������
bool ProjectAutoSaveFlag;

/// ���� ��������������� ���������� ��������� ������� ��� ��������
bool ProjectAutoSaveStatesFlag;

/// ���� ���������� ���� ������� � ����
bool EventsLogFlag;

/// ����� �������
/// 0 - �������
/// 1 - ������
int ProjectMode;

/// ��� �������
/// 0 - �������������
/// 1 - ��������������
int ProjectType;

/// ����� ���������������
/// 0 - ������������ �����
/// 1 - ������ ����� �������� � ��������� ������
int MultiThreadingMode;

/// ����� �������� ������� ������
/// 0 - �������� ������� ������ � ���������� �������
/// 1 - �������� ������� ������ �� ������� ��������� ������
int CalcSourceTimeMode;

/// �������� ���������� ���������� � ������������� ������
int MTUpdateInterfaceInterval;


/// ����� ���������� ��������� GUI
/// 0 - � ������� ������, ���������� �� ������ �������
/// 1 - ����� �������� ���������� ������� ������� ����� ���������� GUI
/// (�� ����� ��������� ������ �� ����� ������, �������� ���������� ������ �����������)
int GuiUpdateMode;

/// ���� ����������� ��������� ������� �������
int ShowChannelsStateFlag;

bool ReflectionFlag;

/// ����� �������
int NumChannels;

/// ���� ������� �������� ���������� �����-������� ��� ��������� �������
bool DisableStopVideoSources;

/// ������������ ���������� �����
bool DebugMode;

/// ����� ��������� ������� ��� �����������
unsigned int DebugSysEventsMask;

/// ���� ��������� ������ ���� � ��������
bool DebuggerMessageFlag;

/// �������� ������ ������� � ��������� ���-���� ������
bool EventsLogMode;

/// �������������� ���������� ��������� �����
/// (���� false, �� ������������ ��������� ����� ����������,
/// ����� ��� ���� �� �����, ���� ��������� ����������� ������)
bool OverrideLogParameters;

/// ����� ���������� ���������� ��������
std::string ServerInterfaceAddress;

/// ���� ���������� ���������� ��������
int ServerInterfacePort;

/// ����� ���������� ���������� ��������
std::string HttpServerInterfaceAddress;

/// ���� ���������� ���������� ��������
int HttpServerInterfacePort;

/// ����� ��� ����������� � �������
std::string HttpServerLogin;

///  ������ ��� ����������� � �������
std::string HttpServerPassword;

// ���� ����������� ��������� �������
bool ProjectShowChannelsStates;

/// ��� ����� �������� ����������
std::string InterfaceFileName;

/// ������ ������������ �������
std::vector<TProjectChannelConfig> ChannelsConfig;


TProjectConfig(void);
TProjectConfig(const TProjectConfig& copy);

bool operator != (const TProjectConfig& copy) const;
bool operator == (const TProjectConfig& copy) const;


// --------------------------
/// ���������� ������ �������
// --------------------------
int GetNumChannels(void) const;
bool SetNumChannels(int num);
bool InsertChannel(int index);
bool DeleteChannel(int index);
// --------------------------
};

class RDK_LIB_TYPE UProject
{
protected: // ������
/// ������ �������
TProjectConfig Config;

/// ����� �������
std::string ProjectPath;

/// ���� ������� ����������� � �������
bool ModifiedFlag;

/// ���� ��������������� ���������� ������ ������������ � ������ ������� �����
bool ForceOldXmlFormat;

/// ���� ��������� ������ ������������� �������� ��������� ������������
/// (������ ��� ���������� ������ ������������ � ����� �������)
bool ForceNewConfigFilesStructure;

/// ������ ����� ������������
std::string ConfigVersion;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UProject(void);
virtual ~UProject(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// ������ �������
const TProjectConfig& GetConfig(void) const;
bool SetConfig(const TProjectConfig& value);
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
/// ��������� ������� ��������� � ������������ �������
bool IsModified(void) const;

/// ���������� ������� ������� ��������� � ������������ �������
bool SetModified(void);

/// ���������� ������� ������� ��������� � ������������ �������
bool ResetModified(void);

/// ���� ��������������� ���������� ������ ������������ � ������ ������� �����
bool GetForceOldXmlFormat(void) const;
bool SetForceOldXmlFormat(bool value);

/// ���� ��������� ������ ������������� �������� ��������� ������������
/// (������ ��� ���������� ������ ������������ � ����� �������)
bool GetForceNewConfigFilesStructure(void) const;
bool SetForceNewConfigFilesStructure(bool value);

/// ���������� ������������ ������� � ��������� �� ���������
/// ����� ����� ���������� ModifiedFlag
void ResetToDefault(void);

/// ������ � ���� �� �������
const std::string& GetProjectPath(void) const;
bool SetProjectPath(const std::string& value);

/// ��������� ������������ ������� �� xml
bool ReadFromXml(USerStorageXML &xml);

/// ��������� ������������ ������� � xml
bool WriteToXml(USerStorageXML &xml);

/// ���������� ������ � ���������� � ���� ������� �������
bool FixSavePoint(USerStorageXML &xml);

protected:
bool ReadFromXmlOld(USerStorageXML &xml);
bool ReadFromXmlNew(USerStorageXML &xml);
bool WriteToXmlOld(USerStorageXML &xml);
bool WriteToXmlNew(USerStorageXML &xml);

// --------------------------

// --------------------------
/// ���������� ������ �������
// --------------------------
public:
int GetNumChannels(void) const;
bool SetNumChannels(int num);
bool InsertChannel(int index);
bool DeleteChannel(int index);

/// ������������� Environment ��� ��������� ������
/// ���������� nullptr ���� ������ �� ��������� ��� ������ �� ��������
std::shared_ptr<UEnvironment> GetEnvironment(int channel_index);
// --------------------------
};


}

#endif

