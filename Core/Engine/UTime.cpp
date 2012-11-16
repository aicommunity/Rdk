#ifndef UTIME_CPP
#define UTIME_CPP

#include "UTime.h"

namespace RDK {

UTime DefaultTimeStep=(UTime)2000;

// --------------------------
// ���������� ��������
// --------------------------
// ������� ����� ������
ULongTime UTimeControl::Time=0;
double UTimeControl::DoubleTime=0;

// �������� �����
ULongTime UTimeControl::RealTime=0;
double UTimeControl::DoubleRealTime=0;

// ���������� ��� � �������� �������
ULongTime UTimeControl::RealTimeStep=1;
double UTimeControl::DoubleRealTimeStep=1.0e-6;


// ������� ����� ������� ���������� ������ � �������������
ULongTime UTimeControl::SourceTime=0;

// ������� ����� ������� ���������� ������ � ��������
double UTimeControl::DoubleSourceTime=0;

// ���������� ��� �� ������� ������� ���������� ������ � �������������
ULongTime UTimeControl::SourceTimeStep=1;

// ���������� ��� �� ������� ������� ���������� ������ � ��������
double UTimeControl::DoubleSourceTimeStep=1.0e-6;
// --------------------------

// --------------------------
// ������ ���������� ����������� ����������
// --------------------------
// ���������� ������� ����� ������
const ULongTime& UTimeControl::GetTime(void)
{
 return Time;
}

const double& UTimeControl::GetDoubleTime(void)
{
 return DoubleTime;
}

// ������������� ������� ����� ������
bool UTimeControl::SetTime(ULongTime value)
{
 if(Time == value)
  return true;

 Time=value;
 DoubleTime=Time/1000000.0;
 return true;
}

// ����������� ����� ������ �� �������� ��������
bool UTimeControl::IncreaseModelTime(ULongTime value)
{
 return UTimeControl::SetTime(UTimeControl::GetTime()+value);
}

// ����������� ����� ������ �� �������� �������� ��� �������� ���� ����������
// � �������� ����� �������
bool UTimeControl::IncreaseModelTimeByStep(ULongTime step)
{
 return UTimeControl::IncreaseModelTime(1000000/step);
}

// ���������� �������� �����
const ULongTime& UTimeControl::GetRealTime(void)
{
 return RealTime;
}

const double& UTimeControl::GetDoubleRealTime(void)
{
 return DoubleRealTime;
}

// ������������� �������� �����
bool UTimeControl::SetRealTime(ULongTime value)
{
 RealTimeStep=value-RealTime;
 if(RealTimeStep == 0)
  RealTimeStep=1;

 DoubleRealTimeStep=RealTimeStep/1.0e6;

 RealTime=value;
 DoubleRealTime=RealTime/1000000.0;
 return true;
}

// ����������� �������� ����� �� �������� ��������
bool UTimeControl::IncreaseRealTime(ULongTime value)
{
 return SetRealTime(RealTime+value);
}

// ���������� ���������� ��� � �������� �������
const ULongTime& UTimeControl::GetRealTimeStep(void)
{
 return RealTimeStep;
}

const double& UTimeControl::GetDoubleRealTimeStep(void)
{
 return DoubleRealTimeStep;
}

// ������� ����� ������� ���������� ������ � �������������
const ULongTime& UTimeControl::GetSourceTime(void)
{
 return SourceTime;
}

const double& UTimeControl::GetDoubleSourceTime(void)
{
 return DoubleSourceTime;
}

// ������������� ����� ������� ���������� ������
bool UTimeControl::SetSourceTime(ULongTime value)
{
 SourceTimeStep=value-SourceTime;
 if(SourceTimeStep == 0)
  SourceTimeStep=1;

 DoubleSourceTimeStep=SourceTimeStep/1.0e6;

 SourceTime=value;
 DoubleSourceTime=SourceTime/1000000.0;
 return true;
}

// ����������� ����� ������� ���������� ������ �� �������� ��������
bool UTimeControl::IncreaseSourceTime(ULongTime value)
{
 return SetSourceTime(SourceTime+value);
}

// ���������� ��� �� ������� ������� ���������� ������ � �������������
const ULongTime& UTimeControl::GetSourceTimeStep(void)
{
 return SourceTimeStep;
}

const double& UTimeControl::GetDoubleSourceTimeStep(void)
{
 return DoubleSourceTimeStep;
}
// --------------------------


}

#endif

