#ifndef RDK_SYSTEM_ANSI_CPP
#define RDK_SYSTEM_ANSI_CPP

#include <ctime>
#include "../rdk_system.h"
#include "USharedMemoryLoader.ansi.cpp"
#include "UGenericMutex.ansi.cpp"

namespace RDK {

// Возвращает текущее время в миллисекундах от некоторого фиксированного момента
// (зависит от реализации)
unsigned long long GetCurrentStartupTime(void)
{
 time_t timedata;
 time(&timedata);
 return timedata;
}

// Записывает в seconds и useconds текущие значения секунд и микросекунд,
// прошедших с некоторого фиксированного момента
void GetTimeOfDayInMicroseconds(unsigned long long &seconds, unsigned long long &useconds)
{
 unsigned long long currentTime = GetCurrentStartupTime();
 seconds = currentTime/1000;
 useconds = (currentTime % 1000) * 1000;
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
//  TODO:
}


// Усыпляет процесс на заданное число миллисекунд
void Sleep(int value)
{
}

// Создает каталог
// Возвращает 0 в случае успеха или если каталог уже существует
// 1 - если уже существует файл с таким именем
// 2 - если такой путь не существует
// 3 - если произошла другая ошибка
int CreateNewDirectory(const char* path)
{
 return 3;
}       

//---------------------------------------------------------------------------

// Получает список файлов или каталогов по заданному пути
int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results)
{
 return 0;
}

int RdkCopyFile(const std::string &source_file, const std::string &dest_file)
{
 return 1;
}

/// Перемещает файл
int RdkMoveFile(const std::string &source_file, const std::string &dest_file)
{
 return 1;
}

int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask)
{
 return 0;
}

/// Функция осуществляет вывод в отладочный лог, если сборка в отладке
void RdkDebuggerMessage(const std::string &message)
{
}

/// Возвращает объем используемой приложением памяти
/// Если не удалось определить то возвращает false
bool ReadUsedMemoryInfo(unsigned long long &total_used_memory, unsigned long long &largest_free_block)
{
 return false;
}

}

#endif

