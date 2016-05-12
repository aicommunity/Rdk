#ifndef UChannelProfilerH
#define UChannelProfilerH

#include "../../Deploy/Include/rdk.h"

namespace RDK {

/// �������� ������������ ������ � ������������������ ������
struct RDK_LIB_TYPE UIntegralPerfomanceResults
{
/// ����� ������, ���
double ModelTime;

/// �������� �����, ���
double RealTime;

/// �������� ����� ������� � ������ RT (Real Time)
double RtCalcDuration;

/// ��������� ����� ������� � ������ RT
double RtModelDuration;

/// ������ ������������������ ������ RT
double RtPerfomance;

/// ������ FPS (������� Gui)
double FullFps;

/// FPS ������ (����� ��� �� FPS ��� ���������� ��������� �������� �� Gui � ������)
double ModelFps;

UIntegralPerfomanceResults(void);
};

/// ���������� �������� ������������������ ��� ���������� ��� �������� ����������
struct RDK_LIB_TYPE UPerfomanceResults
{
/// ������� ����� �������, �
double AvgDuration;

/// ������� �� ������ ����������� ������� ������������ ����������
double Percentage;

UPerfomanceResults(void);
};

/// ������� ������������������ ���������� ��� �������� ����������
struct RDK_LIB_TYPE UPerfomanceData: public UPerfomanceResults
{
/// ������ ������������������, ��
std::vector<long long> CalcDurationHistory;

/// ��������� ������ � �������
void AddHistory(long long value, int max_values);

/// ������ ��������
void CalcAverage(void);

/// ������ �������� �� ������ ��������
void CalcPercentage(double full_time);
};

/// ������������ �������� ������ ������������������ � ������� �������� ������
class RDK_LIB_TYPE UChannelProfiler
{
protected: // ���������
/// ������ ������
int ChannelIndex;

/// ����� ����������
int AverageIterations;

/// ������ ������� ���� ����������� ���������
std::vector<std::string> ComponentsName;

/// ������ ���� ����������� �����������
std::vector<std::string> GuiNames;

protected: // ������
/// ������������������ ���������
std::vector<UPerfomanceData> ComponentsPerfomance;

/// ������������������ �����������
std::vector<UPerfomanceData> GuiPerfomance;

/// ������������������ ���� ������
UPerfomanceData ModelPerfomance;

/// ������������������ ����� ����� ������
UPerfomanceData OtherPerfomance;

/// ��������� ������������������ Gui
UPerfomanceData SummaryGuiPerfomance;

/// ������������ ������ � ������������������
UIntegralPerfomanceResults IntegralPerfomanceResults;

protected: // �������� ������ �������
/// ������ �������-����� � �������� ��� ��������� � ������ � �����
/// �������� ����������� ������ � ������������ � ComponentsName
/// ���� ComponentsName.size()>0 �� ����� ������ ComponentsName.size()+5
/// ��� 3 ��������� �������� ��� �����, �������������:
/// 1: ���������� ������������ ������
/// 2: ���� �������
/// 3: ���������� Gui
/// 4: ������� �� �������� ����� ������ �������� �����-������-��������� gui
/// 5: ������ ����� �����
/// ���� ComponentsName.size()==0 �� ����� ������ 4
/// ��� �������� - ��� �����, �������������:
/// 1: ���� �������
/// 2: ���������� Gui
/// 3: ������� �� �������� ����� ������ �������� �����-������-��������� gui
/// 4: ������ ����� �����
/// � ����� ������� �������� ����� 5-(2+3+4), ��� 4-(1+2+3) ����� ���� �� �����
/// ����, �.�. ������������ ����������� ������ �� ������ ��������
std::list<pair<std::string, UPerfomanceResults> > ComponentsProfilerOutputData;

/// ������ �������-����� � �������� ���������� Gui
/// �������� ����������� ������ � ������������ � GuiNames
std::list<pair<std::string, UPerfomanceResults> > GuiProfilerOutputData;

protected: // ��������� ����������
/// ������� ���������� ������� � ������
UGenericMutex* Mutex;

public: // ������
/// --------------------------
/// ������������ � �����������
/// --------------------------
UChannelProfiler(void);
virtual ~UChannelProfiler(void);
/// --------------------------

/// --------------------------
/// ������ ���������� �����������
/// --------------------------
/// ������ ������
int GetChannelIndex(void) const;
bool SetChannelIndex(int index);

/// ����� ����������
int GetAverageIterations(void) const;
bool SetAverageIterations(int num);

/// ������ ������� ���� ����������� ���������
std::vector<std::string> GetComponentsName(void) const;
bool SetComponentsName(const std::vector<std::string>& value);

/// ��������� ����� ��������� � ������
bool AddComponent(const std::string &name);

/// ��������� ��� �������� ���������� ������� ���������� � ������
bool AddAllComponents(const std::string &name);

/// ������� ���������
bool DelComponent(const std::string &name);
bool DelComponent(int index);

/// ������� ��� ����������
void DelAllComponents(void);

/// ������ ���� ����������� �����������
std::vector<std::string> GetGuiNames(void) const;
bool SetGuiNames(const std::vector<std::string>& value);

/// ��������� ��� ���������� � ������
bool AddAllGui(void);

/// ���������� ��� ��������� �� �������
std::string GetComponentName(int index) const;

/// ���� ������ �� ����� ����������
int FindComponentIndex(const std::string &name) const;

/// ���������� ��� gui �� �������
std::string GetGuiName(int index) const;
/// --------------------------

/// --------------------------
/// ������ ���������� �������
/// --------------------------
/// ������� ��� ����������� ������
void ClearPerfomanceData(void);

/// ��������� ������ ��� ���������� ����������
void AddComponentPerfomanceData(int index, long long value);
void AddComponentPerfomanceData(const std::string &name, long long value);

/// ��������� ������ ��� ���������� gui
void AddGuiPerfomanceData(int index, long long value);

/// ��������� ���������� ����� ������ ���� � ������������������
void LoadCorePerfomanceData(void);

/// ��������� ���������� ����� ������ gui � ������������������
void LoadGuiPerfomanceData(void);

/// ���������� ������ ������� ������������������ ���� �� ����� ������
void CalcCorePerfomance(void);

/// ���������� ������ ������� ������������������ gui �� ����� ������
void CalcGuiPerfomance(void);

/// ���������� ������ �������� ������ ����������
void CalcProfilerOutputData(void);

/// ���������� ������ ������ ������ ����������
void Calculate(void);
void CalculateCore(void);
void CalculateGui(void);
/// --------------------------

/// --------------------------
/// ������ ������ ����������
/// --------------------------
/// ����������� ������ ������������������ ��� ���������
UPerfomanceResults GetComponentPerfomance(const std::string &name) const;
UPerfomanceResults GetComponentPerfomance(int index) const;

/// ���������� ������ ������������������ ��� gui
UPerfomanceResults GetGuiPerfomance(int index) const;

/// ������������������ ���� ������
UPerfomanceData GetModelPerfomance(void) const;

/// ������������������ ����� ����� ������
UPerfomanceData GetOtherPerfomance(void) const;

/// ��������� ������������������ Gui
UPerfomanceData GetSummaryGuiPerfomance(void) const;

/// ���������� ������������ ������ � ������������������
UIntegralPerfomanceResults GetIntegralPerfomanceResults(void) const;
/// --------------------------

/// --------------------------
/// ������ ������ ������������ ����������
/// --------------------------
/// ���������� ������ � ������� �������� ��� ������ Rt
std::string CalcRtTimeText(void) const;

/// ���������� ������ � ������� �������� ��� �������� ������
std::string CalcNormalTimeText(void) const;

/// ���������� ������ � ������������������� ��� ������ Rt
std::string CalcRtPerfomanceText(void) const;

/// ���������� ������ � ������� ������������� ���� ������� ��� ����������� ������
std::string CalcNormalStepDurationText(void) const;

/// ���������� ������ � ������� FPS ��� ����������� ������
std::string CalcNormalFpsText(void) const;

/// ������ �������-����� � �������� ��� ��������� � ������ � �����
/// (�� �������� ���������������� ���������)
std::list<pair<std::string, UPerfomanceResults> > GetComponentsProfilerOutputData(void) const;

/// ������ �������-����� � �������� ���������� Gui
/// �������� ����������� ������ � ������������ � GuiNames
std::list<pair<std::string, UPerfomanceResults> > GetGuiProfilerOutputData(void) const;
/// --------------------------


// --------------------------
// ����� ������ ���������� ������������
// --------------------------
// ��������� ��������� ���������� � xml
virtual void SaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void LoadParameters(RDK::USerStorageXML &xml);
// --------------------------
};

}

#endif
