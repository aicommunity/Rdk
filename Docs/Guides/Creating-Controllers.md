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

Guide for creating controllers to connect components with the GUI.

### Creating a Controller

```cpp
#include "Rdk/Core/Engine/UController.h"

class MyController : public RDK::UController {
protected:
    virtual bool ALink(RDK::UContainer* component) override {
        if (!component) return false;
        // Initialization
        return true;
    }
    
    virtual bool AUpdate(void) override {
        if (!Component) return false;
        // Update interface
        return true;
    }
    
    virtual bool AUnLink(bool forchilds) override {
        // Cleanup
        return true;
    }
};
```

### Using a Controller

```cpp
MyController* controller = new MyController();
controller->Link(component);
controller->IsEnabled(true);
controller->Update();
```

### See Also

- [Controllers-System.md](../Controllers-System.md) - detailed description of the controller system
- [Engine-Detailed.md](../Engine-Detailed.md) - engine documentation
