#ifndef UTIME_H
#define UTIME_H

namespace RDK {

typedef int UTime;
typedef long long ULongTime;

extern const UTime DefaultTimeStep;

class UTimeControl
{
private: // ���������� ��������
// ������� ����� ������ � �������������
static ULongTime Time;

// ������� ����� ������ � ��������
static double DoubleTime;

// �������� ����� � �������������
static ULongTime RealTime;

// �������� ����� � ��������
static double DoubleRealTime;

// ���������� ��� � �������� ������� � �������������
static ULongTime RealTimeStep;

// ���������� ��� � �������� ������� � ��������
static double DoubleRealTimeStep;

public: // �������� ������
// --------------------------
// ������ ���������� ����������� ����������
// --------------------------
// ���������� ������� ����� ������
static const ULongTime& GetTime(void);
static const double& GetDoubleTime(void);

// ������������� ������� ����� ������
static bool SetTime(ULongTime value);

// ���������� �������� �����
static const ULongTime& GetRealTime(void);
static const double& GetDoubleRealTime(void);

// ������������� �������� �����
static bool SetRealTime(ULongTime value);

// ����������� �������� ����� �� �������� ��������
static bool IncreaseRealTime(ULongTime value);

// ���������� ���������� ��� � �������� �������
static const ULongTime& GetRealTimeStep(void);
static const double& GetDoubleRealTimeStep(void);
// --------------------------


};

}

#endif

