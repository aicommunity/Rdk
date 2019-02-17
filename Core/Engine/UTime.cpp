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
// SourceTime=0;

 // ������� ����� ������� ���������� ������ � ��������
 DoubleSourceTime=0;

 // ���������� ��� �� ������� ������� ���������� ������ � �������������
// SourceTimeStep=1;

 // ���������� ��� �� ������� ������� ���������� ������ � ��������
 DoubleSourceTimeStep=1.0e-6;

 SourceStartGlobalTime=0;
 SourceCurrentGlobalTime=0;
 SourceStartLocalTime=0;
 SourceCurrentLocalTime=0;
 ModelStartSecondTime=0.0;
 StepStartSecondTime=0.0;
}

UTimeControl::~UTimeControl(void)
{

}
// --------------------------

// --------------------------
// ������ ���������� ����������� ����������
// --------------------------
/// ����� ������ ������� � ���� (�� ������� ��������� ������)
double UTimeControl::GetSourceStartGlobalTime(void) const
{
 return SourceStartGlobalTime;
}

bool UTimeControl::SetSourceStartGlobalTime(double value)
{
 SourceStartGlobalTime=value;
 return true;
}

/// ������� ����� ��������� ������ � ����
double UTimeControl::GetSourceCurrentGlobalTime(void) const
{
 return SourceCurrentGlobalTime;
}

bool UTimeControl::SetSourceCurrentGlobalTime(double value)
{
 SourceCurrentGlobalTime=value;
 return true;
}

/// ������� ����� � ����
double UTimeControl::GetSourceStartLocalTime(void) const
{
 return SourceStartLocalTime;
}

bool UTimeControl::SetSourceStartLocalTime(double value)
{
 SourceStartLocalTime=value;
 return true;
}

/// ������� ����� � ����
double UTimeControl::GetSourceCurrentLocalTime(void) const
{
 return SourceCurrentLocalTime;
}

bool UTimeControl::SetSourceCurrentLocalTime(double value)
{
 SourceCurrentLocalTime=value;
 return SetRealTime(ULongTime((SourceCurrentLocalTime-SourceStartLocalTime)*1000000.0));
}

/// ������� ����� � �������������
ULongTime UTimeControl::GetSourceCurrentLocalTimeMs(void) const
{
 return ULongTime(SourceCurrentLocalTime*1000);
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
// --------------------------

// --------------------------
/// ������ ������� � �������� ������� ������� �� ������ ������ � ����
// --------------------------
/// ���������� ������� ����� �� ������ ������ � ����
/// ��������� ���������� ������� ��������� ����� � �������� ��� �������������
double UTimeControl::CalcCurrentGlobalTime(double current_local_time) const
{
 return SourceCurrentGlobalTime+(current_local_time-SourceCurrentLocalTime)/86400.0;
}

double UTimeControl::CalcCurrentGlobalTime(ULongTime current_local_time) const
{
 return SourceCurrentGlobalTime+(double(current_local_time)/1000.0-SourceCurrentLocalTime)/86400.0;
}
// --------------------------


}

#endif

