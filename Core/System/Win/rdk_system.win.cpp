#ifndef RDK_SYSTEM_WIN_CPP
#define RDK_SYSTEM_WIN_CPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <OleAuto.h>
#include "../rdk_system.h"
//#include "USharedMemoryLoader.win.cpp"
//#include "UGenericMutex.win.cpp"
#include "UDllLoader.win.cpp"

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  116444736000000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  116444736000000000ULL
#endif

namespace RDK {

// Возвращает текущее время в миллисекундах от некоторого фиксированного момента
// (зависит от реализации)
unsigned long long GetCurrentStartupTime(void)
{
 return GetTickCount();
}

// Записывает в seconds и useconds текущие значения секунд и микросекунд,
// прошедших с некоторого фиксированного момента
void GetTimeOfDayInMicroseconds(unsigned long long &seconds, unsigned long long &useconds)
{
 FILETIME ft;
 long long tmpres = 0;

 GetSystemTimeAsFileTime(&ft);

 tmpres |= ft.dwHighDateTime;
 tmpres <<= 32;
 tmpres |= ft.dwLowDateTime;

 // converting file time to unix epoch
 tmpres -= DELTA_EPOCH_IN_MICROSECS;
 tmpres /= 10;  /*convert into microseconds*/
 time_t secs=(int)(tmpres / 1000000UL);
 seconds = (unsigned long long)(tmpres / 1000000UL);
 useconds = (unsigned long long)(tmpres % 1000000UL);
}

// Вычисляет разницу во времени в миллисекундах
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 else
  return time2-time1;
}

/// Возвращает локальное время в днях (с точностью до миллисекунд) от начала времен
double GetVariantLocalTime(void)
{
 SYSTEMTIME loc_time;
 GetLocalTime(&loc_time);
 double res=0;
 SystemTimeToVariantTime(&loc_time,&res);
 return res +  ((ONETHOUSANDMILLISECONDS/1000.0) * loc_time.wMilliseconds);
}


// Усыпляет процесс на заданное число миллисекунд
void Sleep(int value)
{
 ::Sleep(value);
}                                  

// Создает каталог
// Возвращает 0 в случае успеха или если каталог уже существует
// 1 - если уже существует файл с таким именем
// 2 - если такой путь не существует
// 3 - если произошла другая ошибка
int CreateNewDirectory(const char* path)
{
 DWORD dwFileAttributes = GetFileAttributesA(path);
 if(dwFileAttributes == INVALID_FILE_ATTRIBUTES || (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
 {
  if(!::CreateDirectoryA(path, 0))
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


// Получает список файлов или каталогов по заданному пути
int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results)
{
   results.clear();
   HANDLE findhandle;
 std::string filemask=path+mask;
 WIN32_FIND_DATAA finddata;
 std::string samplefilename;

 findhandle=FindFirstFileA(
    filemask.c_str(),    // pointer to name of file to search for
    &finddata     // pointer to returned information
    );

 if(findhandle != INVALID_HANDLE_VALUE)
 {
  do
  {      
    samplefilename=finddata.cFileName;
 //  samplefilename=AnsiString(ExtractFileName(finddata.cFileName)).c_str();
   std::string::size_type i=samplefilename.find_last_of("\\/");
   if(i != std::string::npos)
    samplefilename.erase(0,i+1);

   if(samplefilename != "." && samplefilename != ".." && ((!isfile && (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || isfile)))
   {
    results.push_back(samplefilename);
   }

  } while(FindNextFileA(findhandle,&finddata));
 }
 FindClose(findhandle);
 return 0;
}

int CopyFile(const std::string &source_file, const std::string &dest_file)
{
 if(CopyFileExA(source_file.c_str(), dest_file.c_str(),0,0,
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
   if(CopyFile(source_dir+results[i],dest_dir+results[i]))
    return 1;
 }
 return 0;
}

/// Функция осуществляет вывод в отладочный лог, если сборка в отладке
void RdkDebuggerMessage(const std::string &message)
{
}

/// Функция создает загрузчика динамических библиотек и вызывает для него Load(dll_name)
RDK_LIB_TYPE UDllLoader* UCreateAndLoadDllLoader(const std::string dll_name)
{
    UDllLoader * loader = new UDllLoaderWin(dll_name);
    loader->Load();
	return loader;
}

/// Функция разрушения объекта загрузчика динамических бибилиотек, НЕ выгружает библиотеку
RDK_LIB_TYPE void UDestroyDllLoader(UDllLoader *handle)
{
    if (handle)
      delete handle;
}


}
#endif

