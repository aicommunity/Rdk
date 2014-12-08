#ifndef UAppProject_H
#define UAppProject_H

#include "UIVisualController.h"

namespace RDK
{

class RDK_LIB_TYPE UProject
{

};

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

TProjectChannelConfig(void);
TProjectChannelConfig(const TProjectChannelConfig& copy);
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

/// ������ ������������ �������
std::vector<TProjectChannelConfig> ChannelsConfig;


TProjectConfig(void);
TProjectConfig(const TProjectConfig& copy);
};


}

#endif

