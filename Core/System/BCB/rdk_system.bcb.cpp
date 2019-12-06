#ifndef RDK_SYSTEM_BUILDER_CPP
#define RDK_SYSTEM_BUILDER_CPP

#include <windows.h>
#include <psapi.h>
#include "../rdk_system.h"
#include "../UGenericMutex.h"
#include "../Win/UDllLoader.win.cpp"
//#include "../win/USharedMemoryLoader.win.cpp"
//#include "UGenericMutex.bcb.cpp"
#include <System.SysUtils.hpp>  
#include <windows.h>
#include <vcl.h>
#include <vector>
#include <string>
#include <time.h>

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
 return (unsigned long long)(TDateTime::CurrentDateTime().operator double()*86400000.0);
}

// Записывает в seconds и useconds текущие значения секунд и микросекунд,
// прошедших с некоторого фиксированного момента
void GetTimeOfDayInMicroseconds(unsigned long long &seconds, unsigned long long &useconds)
{
/*
 double currentTime = TDateTime::CurrentDateTime().operator double()*86400.0;
 seconds = (unsigned long long)currentTime;
 useconds = (unsigned long long)((double(currentTime)-seconds)) * 1000;*/
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
 return TDateTime::CurrentDateTime().operator double();
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

// Получает список файлов или каталогов по заданному пути
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

   if(samplefilename != "." && samplefilename != ".." && ((!isfile && (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
   														   (isfile && !(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))))
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
 DWORD error=0;
 if(!CopyFileEx(source_file.c_str(), dest_file.c_str(),0,0,
  false, COPY_FILE_ALLOW_DECRYPTED_DESTINATION | COPY_FILE_OPEN_SOURCE_FOR_WRITE))
 {
  error=GetLastError();
  return error;
 }

 return 0;
}

int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask)
{
 std::vector<std::string> results;

 int res=FindFilesList(source_dir, mask, true, results);
 DWORD error=0;
 if(!res)
 {
  for(size_t i=0;i<results.size();i++)
  {
   DWORD err=RdkCopyFile(source_dir+results[i],dest_dir+results[i]);
   if(err != 0)
	error=err;
  }
 }
 return error;
}

/// Функция осуществляет вывод в отладочный лог, если сборка в отладке
void RdkDebuggerMessage(const std::string &message)
{
#if defined(_DEBUG) && defined(RDK_ENABLE_DEBUG_OUTPUT)
 OutputDebugString(message.c_str());
#endif
}

/// Функция создает загрузчика динамических библиотек и вызывает для него Load(dll_name)
RDK_LIB_TYPE UDllLoader* UCreateAndLoadDllLoader(const std::string &dll_name)
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


unsigned long long GetMemoryUsedInfo(void)
{
 unsigned long long	result(0);
 PROCESS_MEMORY_COUNTERS mc;
 int cb=sizeof(PROCESS_MEMORY_COUNTERS);

 if(GetProcessMemoryInfo(GetCurrentProcess(), &mc, cb))
  result= mc.WorkingSetSize;
 return result;
}


unsigned long long GetLargestFreeMemRegion(void* &AAddressOfLargest)
{
 TSystemInfo Si;
 LongWord P, dwRet;
 TMemoryBasicInformation Mbi;

 unsigned long long Result = 0;
 AAddressOfLargest = 0;
 GetSystemInfo(&Si);
 P = 0;
 while (P < LongWord(Si.lpMaximumApplicationAddress))
 {
  dwRet = VirtualQuery((void*)(P), &Mbi, sizeof(Mbi));
  if( (dwRet > 0) && (Mbi.State && MEM_FREE != 0))
  {
	  if (Result < Mbi.RegionSize)
	  {
		Result = Mbi.RegionSize;
		AAddressOfLargest = Mbi.BaseAddress;
	  }
	  P+=Mbi.RegionSize;
  }
  else
   P+=Si.dwPageSize;
 }
 return Result;
}


/// Возвращает объем используемой приложением памяти
/// Если не удалось определить то возвращает false
bool ReadUsedMemoryInfo(unsigned long long &total_used_memory, unsigned long long &largest_free_block)
{
 total_used_memory=GetMemoryUsedInfo();
 void* AAddressOfLargest(0);
 largest_free_block=GetLargestFreeMemRegion(AAddressOfLargest);
 return true;
}


/// Преобразует широкую строку в обычную
std::string& narrow(const std::wstring& wstr, std::string &result, unsigned codepage)
{
 unsigned real_codepage=(codepage==0)?CP_ACP:codepage;

	int slength = (int)wstr.length() + 1;
	int len = WideCharToMultiByte(real_codepage, 0, wstr.c_str(), slength, 0, 0, 0, 0);
	result.resize(len, '\0');
	WideCharToMultiByte(real_codepage, 0, wstr.c_str(), slength, &result[0], len, 0, 0);

//	int r_size1=r.size();
//	int r_length1=r.length();
	if(!result.empty())
	 result.resize(len-1);

//	int r_size2=r.size();
//	int r_length2=r.length();
	return result;
}

/// Deprecated
std::string& narrow(const std::wstring& wstr, const std::locale& loc, std::string &result)
{
 return narrow(wstr, result);
}


/// Преобразует обычную строку в широкую
std::wstring& widen(const std::string& str, std::wstring &result, unsigned codepage)
{
 unsigned real_codepage=(codepage==0)?CP_ACP:codepage;
	int slength = (int)str.length() + 1;
	int len = MultiByteToWideChar(real_codepage, 0, str.c_str(), slength, 0, 0);
	result.resize(len, L'\0');
	MultiByteToWideChar(real_codepage, 0, str.c_str(), slength, &result[0], len);

 //	int r_size1=r.size();
 //	int r_length1=r.length();
	if(!result.empty())
	 result.resize(len-1);
 //	int r_size2=r.size();
 //	int r_length2=r.length();
	return result;
}


/// Deprecated
std::wstring& widen(const std::string& str, const std::locale& loc, std::wstring &result)
{
 return widen(str, result);
}


/*

// Копипаста из https://codereview.stackexchange.com/questions/419/converting-between-stdwstring-and-stdstring
std::wstring s2ws(const std::string& s)
{

	int slength = (int)s.length() + 1;
	int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);

	int r_size1=r.size();
	int r_length1=r.length();
	if(!r.empty())
	 r.resize(len-1);
	int r_size2=r.size();
	int r_length2=r.length();
	return r;

}

std::string ws2s(const std::wstring& s)
{
	int slength = (int)s.length() + 1;
	int len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
    std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);

	int r_size1=r.size();
	int r_length1=r.length();
  	if(!r.empty())
	 r.resize(len-1);

	int r_size2=r.size();
	int r_length2=r.length();
	return r;

}
*/


}
#endif

