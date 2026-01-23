# Руководство по многопоточности

## RU

### Обзор

Руководство по работе с многопоточностью в Rdk Core.

### Использование мьютексов

```cpp
#include "Rdk/Core/System/UGenericMutex.h"

RDK::UGenericMutex* mutex = RDK::UCreateMutex();

// Блокировка
mutex->exclusive_lock();
// Критическая секция
mutex->exclusive_unlock();

// RAII обертка
{
    RDK::UGenericMutexExclusiveLocker locker(mutex);
    // Автоматическая разблокировка
}
```

### Использование событий

```cpp
#include "Rdk/Core/System/UGenericMutex.h"

RDK::UGenericEvent* event = RDK::UCreateEvent();

// В потоке 1
event->wait(1000); // ожидание

// В потоке 2
event->set(); // сигнал
```

### См. также

- [System-Detailed.md](../System-Detailed.md) - детальная документация системных абстракций

---

## EN

### Overview

Guide for working with multithreading in Rdk Core.

### See Also

- [System-Detailed.md](../System-Detailed.md) - system abstractions details
