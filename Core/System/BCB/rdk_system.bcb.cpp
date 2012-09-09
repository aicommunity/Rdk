#ifndef RDK_SYSTEM_BUILDER_CPP
#define RDK_SYSTEM_BUILDER_CPP

#include <windows.h>
#include "../rdk_system.h"
#include <System.SysUtils.hpp>

namespace RDK {

// ���������� ������� ����� � ������������� �� ���������� �������������� �������
// (������� �� ����������)
unsigned long long GetCurrentStartupTime(void)
{
 return GetTickCount();
}

// ��������� ������� �� ������� � �������������
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 else
  return time2-time1;
}

// �������� ������� �� �������� ����� �����������
void Sleep(int value)
{
 ::Sleep(value);
}

}
#endif

