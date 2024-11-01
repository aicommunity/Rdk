#ifndef UTIME_H
#define UTIME_H

#include "../../Deploy/Include/initdll_defs.h"

namespace RDK {

typedef unsigned int UTime;
typedef unsigned long long ULongTime;

extern UTime DefaultTimeStep;

class RDK_LIB_TYPE UTimeControl
{
private: // ��������� ����� � ������� ������ � ���� �� ������ ������
/// ����� ������ ������� � ����
double SourceStartGlobalTime;

/// ����� ������ ������� �������� ���� � ����
double SourceCurrentGlobalTime;

/// ����� ������ ������� � �������� �� ������ ��� ��� ������� ������� �������
double SourceStartLocalTime;

/// ����� ������ ������� �������� ���� � �������� �� ������ ��� ��� ������� ������� �������
double SourceCurrentLocalTime;

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
/// ����� ������ ������� � ���� �� ������ ������ (�� ������� ��������� ������)
double GetSourceStartGlobalTime(void) const;
bool SetSourceStartGlobalTime(double value);

/// ������� ����� ��������� ������ � ���� �� ������ ������
double GetSourceCurrentGlobalTime(void) const;
bool SetSourceCurrentGlobalTime(double value);

/// ����� ������ ������� � ���� �� ������������ ����� (�� ������� ��������� ������)
double GetSourceStartLocalTime(void) const;
bool SetSourceStartLocalTime(double value);

/// ������� ����� ��������� � �������� �� ������������ ����� (�� ������� ��������� ������)
double GetSourceCurrentLocalTime(void) const;
bool SetSourceCurrentLocalTime(double value);

/// ������� ����� � ������������� �� ������������ ����� (�� ������� ��������� ������)
ULongTime GetSourceCurrentLocalTimeMs(void) const;
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
double CalcCurrentGlobalTime(double current_local_time) const;
double CalcCurrentGlobalTime(ULongTime current_local_time) const;
// --------------------------

};

}

#endif

