#ifndef RDK_SYSTEM_LINUX_CPP
#define RDK_SYSTEM_LINUX_CPP

#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <stdlib.h>
#include <cstdio>
#include <iostream>

#include "../rdk_system.h"
#include "USharedMemoryLoader.gcc.cpp"
#include "UGenericMutex.gcc.cpp"

#define RDK_ENABLE_DEBUG_OUTPUT

namespace RDK {

// ���������� ������� ����� � ������������� �� ���������� �������������� �������
// (������� �� ����������)
unsigned long long GetCurrentStartupTime(void)
{
 // ������������� ������ - 1970-01-01 00:00:00 +0000 (UTC).

 timeval currentTime;
 gettimeofday(&currentTime, NULL);
 unsigned long long result = currentTime.tv_sec*1000 + currentTime.tv_usec/1000;

 return result;
}

// ���������� � seconds � useconds ������� �������� ������ � �����������,
// ��������� � ���������� �������������� �������
void GetTimeOfDayInMicroseconds(unsigned long long &seconds, unsigned long long &useconds)
{
 // ������������� ������ - 1970-01-01 00:00:00 +0000 (UTC).

 timeval currentTime;
 gettimeofday(&currentTime, NULL);
 seconds = static_cast<unsigned long long>(currentTime.tv_sec);
 useconds = static_cast<unsigned long long>(currentTime.tv_usec);
}

// ��������� ������� �� ������� � �������������
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 return time2-time1;
}

/// ���������� ��������� ����� � ���� (� ��������� �� �����������) �� ������ ������
double GetVariantLocalTime(void)
{
 timespec currentime;
 clock_gettime(CLOCK_REALTIME, &currentime);

 // Initialize 'timezone' variable from the info from environment.
 // But 'timezone' is the number of seconds that the local time zone is earlier than UTC
 // so to get local time we have to subtract the offset from the UTC time.
 tzset();
 currentime.tv_sec = currentime.tv_sec - timezone;

 int numberOfDays = currentime.tv_sec/(24*60*60);
 return numberOfDays +
        static_cast<double>(currentime.tv_sec-numberOfDays*(24*60*60)) / (24*60*60)  +      // part of days in seconds
        static_cast<double>(currentime.tv_nsec)/(static_cast<double>(24*60*60)*1000000000); // part of day in nanoseconds
}


// �������� ������� �� �������� ����� �����������
void Sleep(int value)
{
 usleep(value*1000);
}

// ������� �������
// ���������� 0 � ������ ������ ��� ���� ������� ��� ����������
// 1 - ���� ��� ���������� ���� � ����� ������
// 2 - ���� ����� ���� �� ����������
// 3 - ���� ��������� ������ ������
int CreateNewDirectory(const char* path)
{
 struct stat dirStat;
 int statResult = stat(path, &dirStat);

 if (statResult == 0)
 {
  if (S_ISDIR(dirStat.st_mode))
   return 0; // A directory with the requested name already exists
  else
   return 1; // If it's not a directory then it's a file
 }
 else
 {
  if(mkdirat(AT_FDCWD /*current working directory*/, path, S_IRWXU | S_IRWXG | S_IRGRP | S_IXGRP) != 0)
  {
   if(errno == ENOENT)
    return 2; // No such file or directory
   else
    return 3; // Something else happened
  }
  else
  {
   return 0;
  }
 }

 return 3;
}

// �������� ������ ������ ��� ��������� �� ��������� ����
int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results)
{
 results.clear();

 return 0;
}

int RdkCopyFile(const std::string &source_file, const std::string &dest_file)
{
 int input_fd, output_fd;

 input_fd = open(source_file.c_str(), O_RDONLY);
 if (input_fd == -1)
  return 1;

 struct stat st;
 //size_t  fileSize;
 if (stat(source_file.c_str(), &st) == 0)
 {
  if (S_ISDIR(st.st_mode))
   return 1;

  //fileSize = st.st_size;
 }
 else
 {
  return 1;
 }

 output_fd = open(dest_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, st.st_mode);
 if (output_fd == -1)
  return 1;

 unsigned char buf[10240];
 ssize_t readResult;

 while(true)
 {
  readResult = read(input_fd, &buf[0], sizeof(buf));
  if (readResult == -1)
   return 1;
  if (readResult == 0) // End of the file
   break;

  if (write(output_fd, &buf[0], readResult) == -1)
   return 1;
 }

 // check the number of the bytes written?

 return 0;
}

int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask)
{
 // TODO: rewrite it

 // It's just a shell command but it works.
 // If there's no such directory it creates a file called like dest_dir
 // and copies one of the files from the source_dir there.
 std::string copyCommand("cp -r ");
 copyCommand.append(source_dir);
 copyCommand.push_back('/');
 copyCommand.append(mask);
 copyCommand.push_back(' ');
 copyCommand.append(dest_dir);

 system(copyCommand.c_str());

 return 0;
}


/// ������� ������������ ����� � ���������� ���, ���� ������ � �������
void RdkDebuggerMessage(const std::string &message)
{
#ifdef _DEBUG
 std::cout<<message<<endl;
#endif
}

}
#endif

