#ifndef RDK_SYSTEM_ANSI_CPP
#define RDK_SYSTEM_ANSI_CPP

#include <ctime>
#include "../rdk_system.h"
#include "USharedMemoryLoader.ansi.cpp"
#include "UGenericMutex.ansi.cpp"

namespace RDK {

// ���������� ������� ����� � ������������� �� ���������� �������������� �������
// (������� �� ����������)
unsigned long long GetCurrentStartupTime(void)
{
 time_t timedata;
 time(&timedata);
 return timedata;
}

// ���������� � seconds � useconds ������� �������� ������ � �����������,
// ��������� � ���������� �������������� �������
void GetTimeOfDayInMicroseconds(unsigned long long &seconds, unsigned long long &useconds)
{
 unsigned long long currentTime = GetCurrentStartupTime();
 seconds = currentTime/1000;
 useconds = (currentTime % 1000) * 1000;
}

// ��������� ������� �� ������� � �������������
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 else
  return time2-time1;
}

/// ���������� ��������� ����� � ���� (� ��������� �� �����������) �� ������ ������
double GetVariantLocalTime(void)
{
//  TODO:
}


// �������� ������� �� �������� ����� �����������
void Sleep(int value)
{
}

// ������� �������
// ���������� 0 � ������ ������ ��� ���� ������� ��� ����������
// 1 - ���� ��� ���������� ���� � ����� ������
// 2 - ���� ����� ���� �� ����������
// 3 - ���� ��������� ������ ������
int CreateNewDirectory(const char* path)
{
 return 3;
}       

//---------------------------------------------------------------------------

// �������� ������ ������ ��� ��������� �� ��������� ����
int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results)
{
 return 0;
}

int CopyFile(const std::string &source_file, const std::string &dest_file)
{
 return 1;
}

int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask)
{
 return 0;
}

/// ������� ������������ ����� � ���������� ���, ���� ������ � �������
void RdkDebuggerMessage(const std::string &message)
{
}

}

#endif

