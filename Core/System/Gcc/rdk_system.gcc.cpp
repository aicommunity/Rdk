#ifndef RDK_SYSTEM_LINUX_CPP
#define RDK_SYSTEM_LINUX_CPP

#include <ctime>
#include <dirent.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../rdk_system.h"
#include "USharedMemoryLoader.gcc.cpp"
#include "UGenericMutex.gcc.cpp"

namespace RDK {

// ���������� ������� ����� � ������������� �� ���������� �������������� �������
// (������� �� ����������)
unsigned long long GetCurrentStartupTime(void)
{
/*
 time_t timedata;
 time(&timedata);
 return timedata*1000;
 */
 struct timespec tp;
 clock_gettime (CLOCK_REALTIME, &tp);

 unsigned long long result;
 unsigned long temp = tp.tv_sec/1000000;
 unsigned long temp_sec = tp.tv_sec - temp*1000000;
 unsigned long temp_msec = tp.tv_nsec/1000000;
 result = temp_sec*1000 + temp_msec;

 return result;
}

// ��������� ������� �� ������� � �������������
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 return time2-time1;
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
  if(!mkdirat(AT_FDCWD /*current working directory*/, path, S_IRWXU | S_IRWXG | S_IRWXO ))
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

int CopyFile(const std::string &source_file, const std::string &dest_file)
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

 output_fd = open(dest_file.c_str(), O_WRONLY | O_CREAT, st.st_mode);
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


}
#endif

