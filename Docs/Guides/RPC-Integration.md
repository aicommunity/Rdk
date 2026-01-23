# Руководство по интеграции RPC

## RU

### Обзор

Руководство по интеграции RPC команд в приложение.

### Создание RPC команды

```cpp
#include "Rdk/Core/Application/URpcCommand.h"

RDK::UEPtr<RDK::URpcCommand> command(new RDK::URpcCommandInternal());
command->SetCommandName("MyCommand");
// ... установка параметров ...
```

### Обработка RPC команды

```cpp
class MyRpcDecoder : public RDK::URpcDecoder {
protected:
    virtual bool AProcessCommand(const RDK::UEPtr<RDK::URpcCommand>& command) override {
        if (command->GetCommandName() == "MyCommand") {
            // Обработка команды
            return true;
        }
        return false;
    }
};
```

### См. также

- [Application-Detailed.md](../Application-Detailed.md) - детальная документация RPC системы
- [Diagrams/RPC-Flow.md](../Diagrams/RPC-Flow.md) - поток обработки RPC команд

---

## EN

### Overview

Guide for integrating RPC commands into application.

### See Also

- [Application-Detailed.md](../Application-Detailed.md) - RPC system details
- [Diagrams/RPC-Flow.md](../Diagrams/RPC-Flow.md) - RPC command processing flow
