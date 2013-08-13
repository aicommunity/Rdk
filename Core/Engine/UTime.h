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
ULongTime SourceTime;

// ������� ����� ������� ���������� ������ � ��������
double DoubleSourceTime;

// ���������� ��� �� ������� ������� ���������� ������ � �������������
ULongTime SourceTimeStep;

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


// ������� ����� ������� ���������� ������ � �������������
const ULongTime& GetSourceTime(void) const;
const double& GetDoubleSourceTime(void) const;

// ������������� ����� ������� ���������� ������
bool SetSourceTime(ULongTime value);

// ����������� ����� ������� ���������� ������ �� �������� ��������
bool IncreaseSourceTime(ULongTime value);

// ���������� ��� �� ������� ������� ���������� ������ � �������������
const ULongTime& GetSourceTimeStep(void) const;
const double& GetDoubleSourceTimeStep(void) const;
// --------------------------


};

}

#endif

