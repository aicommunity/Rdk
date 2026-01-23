# Руководство по созданию контроллеров

## RU

### Обзор

Руководство по созданию контроллеров для связи компонентов с GUI.

### Создание контроллера

```cpp
#include "Rdk/Core/Engine/UController.h"

class MyController : public RDK::UController {
protected:
    virtual bool ALink(RDK::UContainer* component) override {
        if (!component) return false;
        // Инициализация
        return true;
    }
    
    virtual bool AUpdate(void) override {
        if (!Component) return false;
        // Обновление интерфейса
        return true;
    }
    
    virtual bool AUnLink(bool forchilds) override {
        // Очистка
        return true;
    }
};
```

### Использование контроллера

```cpp
MyController* controller = new MyController();
controller->Link(component);
controller->IsEnabled(true);
controller->Update();
```

### См. также

- [Controllers-System.md](../Controllers-System.md) - детальное описание системы контроллеров
- [Engine-Detailed.md](../Engine-Detailed.md) - документация движка

---

## EN

### Overview

Guide for creating controllers to connect components with GUI.

### See Also

- [Controllers-System.md](../Controllers-System.md) - controller system details
- [Engine-Detailed.md](../Engine-Detailed.md) - engine documentation
