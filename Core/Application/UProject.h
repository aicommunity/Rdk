#ifndef UAppProject_H
#define UAppProject_H

#include "UIVisualController.h"

namespace RDK
{

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

TProjectChannelConfig(void);
TProjectChannelConfig(const TProjectChannelConfig& copy);

bool operator != (const TProjectChannelConfig& copy) const;
bool operator == (const TProjectChannelConfig& copy) const;
};


struct RDK_LIB_TYPE TProjectConfig
{
/// ��� �������
std::string ProjectName;

/// �������� �������
std::string ProjectDescription;

/// ��� ����� �������� �������
std::string DescriptionFileName;

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

/// ����� ���������� ���������� ��������
std::string ServerInterfaceAddress;

/// ���� ���������� ���������� ��������
int ServerInterfacePort;

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
// --------------------------
};


}

#endif

