#ifndef UTIME_H
#define UTIME_H

namespace RDK {

typedef int UTime;
typedef long long ULongTime;

extern UTime DefaultTimeStep;

class UTimeControl
{
private: // ��������� ����� � ������� ������ � ���� �� ������ ������
/// ����� ������ ������� � ����
double SourceStartGlobalTime;

/// ����� ������ ������� �������� ���� � ����
double SourceStepGlobalTime;

/// ����� ������ ������� � �������� �� ������ ��� ��� ������� ������� �������
double SourceStartLocalTime;

/// ����� ������ ������� �������� ���� � �������� �� ������ ��� ��� ������� ������� �������
double SourceStepLocalTime;

private: // ��������� ����� � �������� ������ � �������� �� ������ ��� ��� ��.
		 // ���������� �������
/// ����� ������ ������� � ��������
double ModelStartSecondTime;

/// ����� ������ ������� �������� ���� � ��������
double StepStartSecondTime;

private: // �������� ������� � �������� ������ � ������������� � �������� �� ������ �������
// ������� ����� ������ � �������������
ULongTime Time;

// ������� ����� ������ � ��������
double DoubleTime;

// �������� ����� � �������������
ULongTime RealTime;

// �������� ����� � ��������
double DoubleRealTime;

// ���������� ��� � �������� ������� � �������������
ULongTime RealTimeStep;

// ���������� ��� � �������� ������� � ��������
double DoubleRealTimeStep;

// ������� ����� ������� ���������� ������ � �������������
//ULongTime SourceTime;

// ������� ����� ������� ���������� ������ � ��������
double DoubleSourceTime;

// ���������� ��� �� ������� ������� ���������� ������ � �������������
//ULongTime SourceTimeStep;

// ���������� ��� �� ������� ������� ���������� ������ � ��������
double DoubleSourceTimeStep;


public: // �������� ������
// --------------------------
// ������������ � �����������
// --------------------------
UTimeControl(void);
virtual ~UTimeControl(void);
// --------------------------

// --------------------------
// ������ ���������� ����������� ����������
// --------------------------
/// ����� ������ ������� � ����
double GetSourceStartGlobalTime(void) const;
bool SetSourceStartGlobalTime(double value);

/// ����� ������ ������� � ����
double GetSourceStepGlobalTime(void) const;
bool SetSourceStepGlobalTime(double value);

/// ������� ����� � ����
double GetSourceStartLocalTime(void) const;
double SetSourceStartLocalTime(double value);

/// ������� ����� � ����
double GetSourceStepLocalTime(void) const;
double SetSourceStepLocalTime(double value);
// --------------------------

// --------------------------
// ������ ���������� ����������� ����������
// --------------------------
// ���������� ������� ����� ������
const ULongTime& GetTime(void) const;
const double& GetDoubleTime(void) const;

// ������������� ������� ����� ������
bool SetTime(ULongTime value);

// ����������� ����� ������ �� �������� ��������
bool IncreaseModelTime(ULongTime value);

// ����������� ����� ������ �� �������� �������� ��� �������� ���� ����������
// � �������� ����� �������
bool IncreaseModelTimeByStep(ULongTime step);

// ���������� �������� �����
const ULongTime& GetRealTime(void) const;
const double& GetDoubleRealTime(void) const;

// ������������� �������� �����
bool SetRealTime(ULongTime value);

// ����������� �������� ����� �� �������� ��������
bool IncreaseRealTime(ULongTime value);

// ���������� ���������� ��� � �������� �������
const ULongTime& GetRealTimeStep(void) const;
const double& GetDoubleRealTimeStep(void) const;
// --------------------------

// --------------------------
/// ������ ������� � �������� ������� ������� �� ������ ������ � ����
// --------------------------
/// ���������� ������� ����� �� ������ ������ � ����
/// ��������� ���������� ������� ��������� ����� � �������� ��� �������������
double CalcCurrentGlobalTime(double current_local_time);
double CalcCurrentGlobalTime(ULongTime current_local_time);
// --------------------------

};

}

#endif

