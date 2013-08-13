#ifndef UTIME_CPP
#define UTIME_CPP

#include "UTime.h"

namespace RDK {

UTime DefaultTimeStep=(UTime)2000;

// --------------------------
// ������������ � �����������
// --------------------------
UTimeControl::UTimeControl(void)
{
 // ������� ����� ������
 Time=0;
 DoubleTime=0;

 // �������� �����
 RealTime=0;
 DoubleRealTime=0;

 // ���������� ��� � �������� �������
 RealTimeStep=1;
 DoubleRealTimeStep=1.0e-6;

 // ������� ����� ������� ���������� ������ � �������������
 SourceTime=0;

 // ������� ����� ������� ���������� ������ � ��������
 DoubleSourceTime=0;

 // ���������� ��� �� ������� ������� ���������� ������ � �������������
 SourceTimeStep=1;

 // ���������� ��� �� ������� ������� ���������� ������ � ��������
 DoubleSourceTimeStep=1.0e-6;
}

UTimeControl::~UTimeControl(void)
{

}
// --------------------------

// --------------------------
// ������ ���������� ����������� ����������
// --------------------------
// ���������� ������� ����� ������
const ULongTime& UTimeControl::GetTime(void) const
{
 return Time;
}

const double& UTimeControl::GetDoubleTime(void) const
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
 return SetTime(UTimeControl::GetTime()+value);
}

// ����������� ����� ������ �� �������� �������� ��� �������� ���� ����������
// � �������� ����� �������
bool UTimeControl::IncreaseModelTimeByStep(ULongTime step)
{
 return IncreaseModelTime(1000000/step);
}

// ���������� �������� �����
const ULongTime& UTimeControl::GetRealTime(void) const
{
 return RealTime;
}

const double& UTimeControl::GetDoubleRealTime(void) const
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
const ULongTime& UTimeControl::GetRealTimeStep(void) const
{
 return RealTimeStep;
}

const double& UTimeControl::GetDoubleRealTimeStep(void) const
{
 return DoubleRealTimeStep;
}

// ������� ����� ������� ���������� ������ � �������������
const ULongTime& UTimeControl::GetSourceTime(void) const
{
 return SourceTime;
}

const double& UTimeControl::GetDoubleSourceTime(void) const
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
const ULongTime& UTimeControl::GetSourceTimeStep(void) const
{
 return SourceTimeStep;
}

const double& UTimeControl::GetDoubleSourceTimeStep(void) const
{
 return DoubleSourceTimeStep;
}
// --------------------------


}

#endif

