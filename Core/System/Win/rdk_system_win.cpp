#ifndef RDK_SYSTEM_WIN_CPP
#define RDK_SYSTEM_WIN_CPP

#include <windows.h>
#include "../rdk_system.h"

namespace RDK {

// Возвращает текущее время в миллисекундах от некоторого фиксированного момента
// (зависит от реализации)
unsigned long long GetCurrentStartupTime(void)
{
 return GetTickCount();
}

// Вычисляет разницу во времени в миллисекундах
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 else
  return time2-time1;
}

// Усыпляет процесс на заданное число миллисекунд
void Sleep(int value)
{
 ::Sleep(value);
}

}
#endif

