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
}

UTimeControl::~UTimeControl(void)
{

}
// --------------------------

// --------------------------
// ������ ���������� ����������� ����������
// --------------------------
/// ����� ������ ������� � ����
double UTimeControl::GetSourceStartGlobalTime(void) const
{
 return SourceStartGlobalTime;
}

bool UTimeControl::SetSourceStartGlobalTime(double value)
{
 SourceStartGlobalTime=value;
 return true;
}

/// ����� ������ ������� � ����
double UTimeControl::GetSourceStepGlobalTime(void) const
{
 return SourceStepGlobalTime;
}

bool UTimeControl::SetSourceStepGlobalTime(double value)
{
 SourceStepGlobalTime=value;
 return true;
}

/// ������� ����� � ����
double UTimeControl::GetSourceStartLocalTime(void) const
{
 return SourceStartLocalTime;
}

double UTimeControl::SetSourceStartLocalTime(double value)
{
 SourceStartLocalTime=value;
 return true;
}

/// ������� ����� � ����
double UTimeControl::GetSourceStepLocalTime(void) const
{
 return SourceStepLocalTime;
}

double UTimeControl::SetSourceStepLocalTime(double value)
{
 SourceStepLocalTime=value;
 return SetRealTime((SourceStepLocalTime-SourceStartLocalTime)*1000.0);
}
// --------------------------

// --------------------------
// ��������� ����� � �������� ������ � �������� �� ������ ��� ��� ��.
// ���������� �������
// --------------------------
/*
/// ����� ������ ������� � ��������
double UTimeControl::GetModelStartSecondTime(void) const
{
 return ModelStartSecondTime;
}

bool UTimeControl::SetModelStartSecondTime(double value)
{
 ModelStartSecondTime=value;
 return true;
}

/// ����� ������ ������� �������� ���� � ��������
double UTimeControl::GetStepStartSecondTime(void) const
{
 return StepStartSecondTime;
}

bool UTimeControl::SetStepStartSecondTime(double value)
{
 StepStartSecondTime=value;
 return SetRealTime((StepStartSecondTime-ModelStartSecondTime)*1000);
}                            */
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
double UTimeControl::CalcCurrentGlobalTime(double current_local_time)
{
 return SourceStepGlobalTime+(current_local_time-SourceStepLocalTime)/86400.0;
}

double UTimeControl::CalcCurrentGlobalTime(ULongTime current_local_time)
{
 return SourceStepGlobalTime+(double(current_local_time)/1000.0-SourceStepLocalTime)/86400.0;
}
// --------------------------


}

#endif

