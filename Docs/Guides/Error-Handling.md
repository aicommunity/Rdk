# Руководство по обработке ошибок

## RU

### Обзор

Руководство по обработке ошибок и исключений в Rdk Core.

### Использование исключений

```cpp
#include "Rdk/Core/Utilities/UException.h"

try {
    // Код, который может выбросить исключение
    if (error_condition) {
        throw RDK::EStringError("Error message", 1);
    }
} catch (const RDK::UException& ex) {
    Logger->LogMessage(RDK_EX_ERROR, ex.what());
}
```

### Типы исключений

- `EFatal` - фатальная ошибка
- `EError` - исправимая ошибка
- `EWarning` - предупреждение
- `EInfo` - информационное сообщение
- `EDebug` - отладочное сообщение

### См. также

- [Utilities-Reference.md](../Utilities-Reference.md) - справочник по утилитам
- [Logging-System.md](../Logging-System.md) - система логирования

---

## EN

### Overview

Guide for error and exception handling in Rdk Core.

### See Also

- [Utilities-Reference.md](../Utilities-Reference.md) - utilities reference
- [Logging-System.md](../Logging-System.md) - logging system
