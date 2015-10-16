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

/// ��� ������
std::string ChannelName;

TProjectChannelConfig(void);
TProjectChannelConfig(const TProjectChannelConfig& copy);

bool operator != (const TProjectChannelConfig& copy) const;
bool operator == (const TProjectChannelConfig& copy) const;
};


struct RDK_LIB_TYPE TProjectConfig
{
/// ����� �������
std::string ProjectDirectory;

/// ��� �������
std::string ProjectName;

/// �������� �������
std::string ProjectDescription;

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
// --------------------------
};


}

#endif

