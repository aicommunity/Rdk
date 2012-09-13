#ifndef RDK_SYSTEM_LINUX_CPP
#define RDK_SYSTEM_LINUX_CPP

#include <ctime>
#include <sys/time.h>
#include "../rdk_system.h"

namespace RDK {

// ¬озвращает текущее врем€ в миллисекундах от некоторого фиксированного момента
// (зависит от реализации)
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

// ¬ычисл€ет разницу во времени в миллисекундах
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 return time2-time1;
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
int CreateDirectory(const char* path)
{
 return 3;
}



}
#endif

