#ifndef RDK_SYSTEM_H
#define RDK_SYSTEM_H

namespace RDK {

// Возвращает текущее время в миллисекундах от некоторого фиксированного момента
// (зависит от реализации)
unsigned long long GetCurrentStartupTime(void);

// Вычисляет разницу во времени в миллисекундах
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2);

// Усыпляет процесс на заданное число миллисекунд
void Sleep(int value);

}

#endif

