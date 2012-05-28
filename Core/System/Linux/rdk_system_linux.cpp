#ifndef RDK_SYSTEM_LINUX_CPP
#define RDK_SYSTEM_LINUX_CPP

#include <ctime>
#include "../rdk_system.h"

namespace RDK {

// ¬озвращает текущее врем€ в миллисекундах от некоторого фиксированного момента
// (зависит от реализации)
unsigned long long GetCurrentStartupTime(void)
{
 time_t timedata;
 time(&timedata);
 return timedata*1000;
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

}
#endif

