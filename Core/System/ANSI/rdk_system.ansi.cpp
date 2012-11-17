#ifndef RDK_SYSTEM_ANSI_CPP
#define RDK_SYSTEM_ANSI_CPP

#include <ctime>
#include "../rdk_system.h"

namespace RDK {

// ¬озвращает текущее врем€ в миллисекундах от некоторого фиксированного момента
// (зависит от реализации)
unsigned long long GetCurrentStartupTime(void)
{
 time_t timedata;
 time(&timedata);
 return timedata;
}

// ¬ычисл€ет разницу во времени в миллисекундах
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 else
  return time2-time1;
}             

// ”сыпл€ет процесс на заданное число миллисекунд
void Sleep(int value)
{
}

// —оздает каталог
// ¬озвращает 0 в случае успеха или если каталог уже существует
// 1 - если уже существует файл с таким именем
// 2 - если такой путь не существует
// 3 - если произошла друга€ ошибка
int CreateNewDirectory(const char* path)
{
 return 3;
}

}

#endif

