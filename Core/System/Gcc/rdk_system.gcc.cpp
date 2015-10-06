#ifndef RDK_SYSTEM_LINUX_CPP
#define RDK_SYSTEM_LINUX_CPP

#include <ctime>
#include <dirent.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <stdlib.h>

#include "../rdk_system.h"
#include "USharedMemoryLoader.gcc.cpp"
#include "UGenericMutex.gcc.cpp"

namespace RDK {

// ¬озвращает текущее врем€ в миллисекундах от некоторого фиксированного момента
// (зависит от реализации)
unsigned long long GetCurrentStartupTime(void)
{
 // ‘иксированный момент - 1970-01-01 00:00:00 +0000 (UTC).

 timeval currentTime;
 gettimeofday(&currentTime, NULL);
 unsigned long long result = currentTime.tv_sec*1000 + currentTime.tv_usec/1000;

 return result;
}

// «аписывает в seconds и useconds текущие значени€ секунд и микросекунд,
// прошедших с некоторого фиксированного момента
void GetTimeOfDayInMicroseconds(unsigned long long &seconds, unsigned long long &useconds)
{
 // ‘иксированный момент - 1970-01-01 00:00:00 +0000 (UTC).

 timeval currentTime;
 gettimeofday(&currentTime, NULL);
 seconds = static_cast<unsigned long long>(currentTime.tv_sec);
 useconds = static_cast<unsigned long long>(currentTime.tv_usec);
}

// ¬ычисл€ет разницу во времени в миллисекундах
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 return time2-time1;
}

/// ¬озвращает локальное врем€ в дн€х (с точностью до миллисекунд) от начала времен
double GetVariantLocalTime(void)
{
// TODO:
}


// ”сыпл€ет процесс на заданное число миллисекунд
void Sleep(int value)
{
 usleep(value*1000);
}

// —оздает каталог
// ¬озвращает 0 в случае успеха или если каталог уже существует
// 1 - если уже существует файл с таким именем
// 2 - если такой путь не существует
// 3 - если произошла друга€ ошибка
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

// ѕолучает список файлов или каталогов по заданному пути
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

