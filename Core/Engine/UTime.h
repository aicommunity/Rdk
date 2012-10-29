#ifndef UTIME_H
#define UTIME_H

namespace RDK {

typedef int UTime;
typedef long long ULongTime;

extern UTime DefaultTimeStep;

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

// ������� ����� ������� ���������� ������ � �������������
static ULongTime SourceTime;

// ������� ����� ������� ���������� ������ � ��������
static double DoubleSourceTime;

// ���������� ��� �� ������� ������� ���������� ������ � �������������
static ULongTime SourceTimeStep;

// ���������� ��� �� ������� ������� ���������� ������ � ��������
static double DoubleSourceTimeStep;


public: // �������� ������
// --------------------------
// ������ ���������� ����������� ����������
// --------------------------
// ���������� ������� ����� ������
static const ULongTime& GetTime(void);
static const double& GetDoubleTime(void);

// ������������� ������� ����� ������
static bool SetTime(ULongTime value);

// ����������� ����� ������ �� �������� ��������
static bool IncreaseModelTime(ULongTime value);

// ����������� ����� ������ �� �������� �������� ��� �������� ���� ����������
// � �������� ����� �������
static bool IncreaseModelTimeByStep(ULongTime step);

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


// ������� ����� ������� ���������� ������ � �������������
static const ULongTime& GetSourceTime(void);
static const double& GetDoubleSourceTime(void);

// ������������� ����� ������� ���������� ������
static bool SetSourceTime(ULongTime value);

// ����������� ����� ������� ���������� ������ �� �������� ��������
static bool IncreaseSourceTime(ULongTime value);

// ���������� ��� �� ������� ������� ���������� ������ � �������������
static const ULongTime& GetSourceTimeStep(void);
static const double& GetDoubleSourceTimeStep(void);
// --------------------------


};

}

#endif

