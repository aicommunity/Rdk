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
#include <dirent.h>
#include <string.h>

#include "../rdk_system.h"
//#include "USharedMemoryLoader.gcc.cpp"
//#include "UGenericMutex.gcc.cpp"
#include "UDllLoader.gcc.cpp"

#define RDK_ENABLE_DEBUG_OUTPUT

namespace RDK {

// Возвращает текущее время в миллисекундах от некоторого фиксированного момента
// (зависит от реализации)
unsigned long long GetCurrentStartupTime(void)
{
 // Фиксированный момент - 1970-01-01 00:00:00 +0000 (UTC).

 timeval currentTime;
 gettimeofday(&currentTime, NULL);
 unsigned long long result = ((unsigned long long)currentTime.tv_sec)*1000 + currentTime.tv_usec/1000;

 return result;
}

// Записывает в seconds и useconds текущие значения секунд и микросекунд,
// прошедших с некоторого фиксированного момента
void GetTimeOfDayInMicroseconds(unsigned long long &seconds, unsigned long long &useconds)
{
 // Фиксированный момент - 1970-01-01 00:00:00 +0000 (UTC).

 timeval currentTime;
 gettimeofday(&currentTime, NULL);
 seconds = static_cast<unsigned long long>(currentTime.tv_sec);
 useconds = static_cast<unsigned long long>(currentTime.tv_usec);
}

// Вычисляет разницу во времени в миллисекундах
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 return time2-time1;
}

/// Возвращает локальное время в днях (с точностью до миллисекунд) от начала времен
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


// Усыпляет процесс на заданное число миллисекунд
void Sleep(int value)
{
 usleep(value*1000);
}

// Создает каталог
// Возвращает 0 в случае успеха или если каталог уже существует
// 1 - если уже существует файл с таким именем
// 2 - если такой путь не существует
// 3 - если произошла другая ошибка
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

// Получает список файлов или каталогов по заданному пути
int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results)
{
 results.clear();
 DIR *dp;
 struct dirent *dirp;
 if((dp  = opendir(path.c_str())) == NULL)
  return errno;
 std::string mask4cmp_left;
 std::string mask4cmp_middle;
 std::string mask4cmp_right;
 std::string* dst_ptr = &mask4cmp_left;
 std::string::const_iterator mask_it = mask.begin();
 short int break_count = 0;
 for(;mask_it != mask.end();mask_it++)
 {
   if(*mask_it == '*')
   {
    break_count++;
    if(mask4cmp_middle.empty())
     dst_ptr = &mask4cmp_middle;
    else
     dst_ptr = &mask4cmp_right;
    continue;
   }
   else
    dst_ptr->push_back(*mask_it);
 }
 if((break_count == 1) && mask4cmp_right.empty() && (!mask4cmp_middle.empty()))
 {
  mask4cmp_right = mask4cmp_middle;
  mask4cmp_middle.clear();
 }
 // Размер dirp->d_name = 256
 if((mask4cmp_left.size() + mask4cmp_right.size() + mask4cmp_middle.size())>255)
  return 1;

#define STRIT std::string::iterator
 while ((dirp = readdir(dp)) != NULL)
 {
  std::string cut_name = dirp->d_name;
  if((strcmp(cut_name.c_str(), ".") == 0) || (strcmp(cut_name.c_str(), "..") == 0))
   continue;
  if((dirp->d_type != DT_DIR) && (!isfile))
   continue;
  if((dirp->d_type == DT_DIR) && (isfile))
   continue;
  if(cut_name.size() < (mask4cmp_left.size()+mask4cmp_right.size()))
   continue;
  bool break_ctrl = false;
  if(!mask4cmp_left.empty())
  {
   STRIT it_left = mask4cmp_left.begin();
   STRIT it_name = cut_name.begin();
   for(; it_left != mask4cmp_left.end(); it_left++, it_name++)
   {
    if(*it_left != *it_name)
    {
     break_ctrl = true;
     break;
    }
   }
   if(break_ctrl)
    continue;
  }
  if(!mask4cmp_middle.empty())
  {
   STRIT it_midle = mask4cmp_middle.begin();
   STRIT it_name = cut_name.begin() + mask4cmp_left.size();
   STRIT it_name_end = cut_name.end() - mask4cmp_right.size();
   while (it_name != it_name_end)
   {
    if (*it_midle == *it_name)
    {
     bool equale = true;
     STRIT it2_midle = it_midle;
     STRIT it2_name = it_name;
     for (; it2_midle != mask4cmp_middle.end(); it2_midle++, it2_name++)
     {
      if (*it2_midle != *it2_name)
      {
       equale = false;
       break_ctrl = true;
       break;
      }
     }
     if(equale)
     {
      break_ctrl = false;
      break;
     }
    }
    else
     break_ctrl = true;
    it_name++;
   }

   if (break_ctrl)
    continue;
  }
   if(!mask4cmp_right.empty())
  {
   STRIT it_right = mask4cmp_right.end();
   STRIT it_name = cut_name.end();
   while(it_right != mask4cmp_right.begin())
   {
    if(*(--it_right) != *(--it_name))
    {
     break_ctrl = true;
     break;
    }
   }
   if(break_ctrl)
    continue;
  }
  results.push_back(std::string(dirp->d_name));
 }
#undef STRIT
 closedir(dp);
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


/// Функция осуществляет вывод в отладочный лог, если сборка в отладке
void RdkDebuggerMessage(const std::string &message)
{
#ifndef NDEBUG
 std::cout<<message<<std::endl;
#endif
}

/// Функция создает загрузчика динамических библиотек и вызывает для него Load(dll_name)
RDK_LIB_TYPE UDllLoader* UCreateAndLoadDllLoader(const std::string dll_name)
{
    UDllLoader *loader = new UDllLoaderGcc(dll_name);
    loader->Load();
    return loader;
}

/// Функция разрушения объекта загрузчика динамических бибилиотек, НЕ выгружает библиотеку
RDK_LIB_TYPE void UDestroyDllLoader(UDllLoader *handle)
{
    if(handle)
        delete handle;
}

}
#endif

