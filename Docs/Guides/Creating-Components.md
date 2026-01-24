# Руководство по созданию компонентов

## RU

### Обзор

Это руководство описывает процесс создания новых компонентов для системы Rdk Core.

### Шаг 1: Наследование от базового класса

```cpp
#include "Rdk/Core/Engine/UContainer.h"

class MyComponent : public RDK::UContainer {
public:
    MyComponent() : RDK::UContainer() {
    }
    
    virtual ~MyComponent() {
    }
};
```

### Шаг 2: Реализация виртуальных методов

```cpp
protected:
    // Установка значений по умолчанию
    virtual bool ADefault(void) override {
        // Инициализация свойств
        return true;
    }
    
    // Построение внутренней структуры
    virtual bool ABuild(void) override {
        // Построение компонента
        return true;
    }
    
    // Сброс состояния
    virtual bool AReset(void) override {
        // Сброс перед вычислениями
        return true;
    }
    
    // Вычисления
    virtual bool ACalculate(void) override {
        // Основная логика компонента
        return true;
    }
```

### Шаг 3: Регистрация свойств

```cpp
protected:
    virtual bool ABuild(void) override {
        // Регистрация свойств
        RegisterProperty<double>("Input", ptPubInput);
        RegisterProperty<double>("Output", ptPubOutput);
        RegisterProperty<double>("Parameter", ptPubParameter);
        
        return true;
    }
```

### Шаг 4: Регистрация в библиотеке

```cpp
// В файле библиотеки
extern "C" RDK_LIB_TYPE bool RDK_CALL LoadLibrary(RDK::UStorage* storage) {
    // Регистрация компонента
    storage->AddClass("MyComponent", 
        new RDK::UVirtualMethodFactory(
            RDK::UEPtr<RDK::UContainer>(new MyComponent())
        )
    );
    return true;
}
```

### Детальное руководство

Для более подробной информации см. [Component-Development.md](Component-Development.md) - полное руководство по разработке компонентов с примерами, best practices и паттернами.

### Примеры

См. также:
- [Engine-Detailed.md](../Engine-Detailed.md) - детальные примеры
- [Component-Development.md](Component-Development.md) - полное руководство

---

## EN

### Overview

This guide describes the process of creating new components for Rdk Core system.

### Step 1: Inherit from Base Class

Inherit from `UContainer` or `UComponent`.

### Step 2: Implement Virtual Methods

Implement `ADefault()`, `ABuild()`, `AReset()`, `ACalculate()`.

### Step 3: Register Properties

Use `RegisterProperty()` in `ABuild()`.

### Step 4: Register in Library

Register component in library's `LoadLibrary()` function.

### Detailed Guide

For more detailed information, see [Component-Development.md](Component-Development.md) - complete component development guide with examples, best practices, and patterns.
