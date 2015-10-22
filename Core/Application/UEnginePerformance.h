#ifndef UEnginePerformanceH
#define UEnginePerformanceH

#include "../../Deploy/Include/rdk.h"

namespace RDK {

/// �������� ���������� ������ � �������� �������
struct RDK_LIB_TYPE UEngineTimeData
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
double RtPerformance;
};

/// ���������� �������� ������������������ ��� ���������� ��� �������� ����������
struct RDK_LIB_TYPE UPerformanceResults
{
/// �������
long long AvgCalcDuration;

/// ������� �� ������ ����������� ������� ������������ ����������
double CalcPrecent;
};

/// ������� ������������������ ���������� ��� �������� ����������
struct RDK_LIB_TYPE UPerformanceData: public UPerformanceResults
{
/// ������ ������������������
std::vector<long long> CalcDurationHistory;

};

/// ������������ �������� ������ ������������������ � ������� �������� ������
class RDK_LIB_TYPE UEnginePrerformanceCalc
{
protected: // ���������
/// ����� ����������
int AverageIterations;

/// ������ ������� ���� ����������� ���������
std::vector<std::string> ComponentsName;

/// ������ ���� ����������� �����������
std::vector<std::string> GuiNames;

protected: // ������
/// ������������������ ���������
std::vector<UPerformanceData> ComponentsPerformance;

/// ������������������ �����������
std::vector<UPerformanceData> GuiPerformance;

/// ������������������ ���� ������
UPerformanceData ModelPerformance;

/// ����� ������
UEngineTimeData EngineTimeData;

public: // ������
/// --------------------------
/// ������������ � �����������
/// --------------------------
UEnginePrerformanceCalc(void);
virtual ~UEnginePrerformanceCalc(void);
/// --------------------------

/// --------------------------
/// ������ ���������� �����������
/// --------------------------
/// ����� ����������
int GetAverageIterations(void) const;
bool SetAverageIterations(int num);

/// ������ ������� ���� ����������� ���������
const std::vector<std::string>& GetComponentsName(void) const;
bool SetComponentsName(const std::vector<std::string>& value);

/// ������ ���� ����������� �����������
const std::vector<std::string>& GetGuiNames(void) const;
bool SetGuiNames(const std::vector<std::string>& value);

/// ���������� ��� ��������� �� �������
std::string GetComponentName(int index) const;

/// ���� ������ �� ����� ����������
std::string FindComponentIndex(const std::string &name) const;

/// ���������� ��� gui �� �������
std::string GetGuiName(int index) const;
/// --------------------------

/// --------------------------
/// ������ ���������� �������
/// --------------------------
/// ������� ��� ����������� ������
void ClearPerformanceData(void);

/// ��������� ������ ��� ���������� ����������
void AddComponentPerformanceData(int index, long long value);
void AddComponentPerformanceData(const std::string &name, long long value);

/// ��������� ������ ��� ���������� gui
void AddGuiPerformanceData(int index, long long value);

/// ���������� ������ ������ ������������������
void Calculate(void);
/// --------------------------

/// --------------------------
/// ������ ������ ����������
/// --------------------------
/// ����������� ������ ������������������ ��� ���������
UPerformanceResults GetComponentPerformance(const std::string &name) const;
UPerformanceResults GetComponentPerformance(int index) const;

/// ����������� ������ ������������������ ��� gui
UPerformanceResults GetGuiPerformance(int index) const;

/// ������������������ ���� ������
UPerformanceData GetModelPerformance(void) const;

/// ����� ������
UEngineTimeData GetEngineTimeData(void) const;
/// --------------------------
};

}

#endif
