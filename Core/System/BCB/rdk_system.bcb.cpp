#ifndef RDK_SYSTEM_BUILDER_CPP
#define RDK_SYSTEM_BUILDER_CPP

#include <windows.h>
#include "../rdk_system.h"
#include "../UGenericMutex.h"
//#include "../win/USharedMemoryLoader.win.cpp"
//#include "UGenericMutex.bcb.cpp"
#include <System.SysUtils.hpp>  
#include <windows.h>
#include <vcl.h>
#include <vector>
#include <string>


namespace RDK {

// ���������� ������� ����� � ������������� �� ���������� �������������� �������
// (������� �� ����������)
unsigned long long GetCurrentStartupTime(void)
{
 return GetTickCount();
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

// �������� ������� �� �������� ����� �����������
void Sleep(int value)
{
 ::Sleep(value);
}

// ������� �������
// ���������� 0 � ������ ������ ��� ���� ������� ��� ����������
// 1 - ���� ��� ���������� ���� � ����� ������
// 2 - ���� ����� ���� �� ����������
// 3 - ���� ��������� ������ ������
int CreateNewDirectory(const char* path)
{
 DWORD dwFileAttributes = GetFileAttributes(path);
 if(dwFileAttributes == INVALID_FILE_ATTRIBUTES || (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
 {
  if(!::CreateDirectory(path, 0))
  {
   if(GetLastError() == ERROR_PATH_NOT_FOUND)
    return 2;

   return 0;
  }
 }
 else
  return 1;

 return 0;
}


//---------------------------------------------------------------------------

// �������� ������ ������ ��� ��������� �� ��������� ����
int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results)
{
   results.clear();
   HANDLE findhandle;
 std::string filemask=path+std::string("/")+mask;
 WIN32_FIND_DATA finddata;
 std::string samplefilename;

 findhandle=FindFirstFile(
    filemask.c_str(),    // pointer to name of file to search for
    &finddata     // pointer to returned information
    );

 if(findhandle != INVALID_HANDLE_VALUE)
 {
  do
  {
   samplefilename=AnsiString(ExtractFileName(finddata.cFileName)).c_str();
//   PathStripPath(finddata.cFileName);
//   samplefilename=finddata.cFileName;

   if(samplefilename != "." && samplefilename != ".." && ((!isfile && (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || isfile)))
   {
    results.push_back(samplefilename);
   }

  } while(FindNextFile(findhandle,&finddata));
 }
 FindClose(findhandle);
 return 0;
}

int RdkCopyFile(const std::string &source_file, const std::string &dest_file)
{
 if(CopyFileEx(source_file.c_str(), dest_file.c_str(),0,0,
  false, COPY_FILE_OPEN_SOURCE_FOR_WRITE))
  return 0;

 return 1;
}

int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask)
{
 std::vector<std::string> results;

 int res=FindFilesList(source_dir, mask, true, results);
 if(!res)
 {
  for(size_t i=0;i<results.size();i++)
   if(RdkCopyFile(source_dir+results[i],dest_dir+results[i]))
    return 1;
 }
 return 0;
}


}
#endif

