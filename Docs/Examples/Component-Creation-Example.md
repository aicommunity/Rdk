# Пример: Создание простого компонента

## RU

### Описание задачи

Создать простой компонент, который умножает входное значение на параметр и выдает результат.

### Решение

#### Шаг 1: Объявление класса

```cpp
#include "Rdk/Core/Engine/UContainer.h"

class UMultiplier : public RDK::UContainer {
public:
    UMultiplier() : RDK::UContainer() {}
    
protected:
    // Свойства компонента
    RDK::UProperty<double, UMultiplier, RDK::ptPubInput> Input;
    RDK::UProperty<double, UMultiplier, RDK::ptPubOutput> Output;
    RDK::UProperty<double, UMultiplier, RDK::ptPubParam> Multiplier;
    
    // Методы жизненного цикла
    virtual bool ADefault(void) override;
    virtual bool ABuild(void) override;
    virtual bool ACalculate(void) override;
};
```

#### Шаг 2: Инициализация свойств

```cpp
UMultiplier::UMultiplier()
    : RDK::UContainer()
    , Input("Input", this, 0.0, nullptr, false)
    , Output("Output", this, 0.0, nullptr, false)
    , Multiplier("Multiplier", this, 1.0, nullptr, false)
{
}
```

#### Шаг 3: Реализация методов жизненного цикла

```cpp
bool UMultiplier::ADefault(void) {
    if (!UContainer::ADefault())
        return false;
    
    // Установка значений по умолчанию
    Input = 0.0;
    Output = 0.0;
    Multiplier = 1.0;
    
    return true;
}

bool UMultiplier::ABuild(void) {
    if (!UContainer::ABuild())
        return false;
    
    // Проверка готовности входных данных
    if (!Input.IsConnected()) {
        Logger->LogMessageEx(RDK_EX_WARNING, GetName(), __FUNCTION__,
                            "Input is not connected");
    }
    
    return true;
}

bool UMultiplier::ACalculate(void) {
    if (!IsReady())
        return false;
    
    // Основная логика: умножение
    Output = Input() * Multiplier();
    
    return true;
}
```

#### Шаг 4: Регистрация компонента

```cpp
// В функции загрузки библиотеки
void LoadMyLibrary(RDK::UStorage* storage) {
    storage->AddClass<UMultiplier>("UMultiplier");
    storage->BuildStorage();
}
```

#### Шаг 5: Использование компонента

```cpp
// Создание компонента
auto multiplier = storage->CreateComponent<UMultiplier>("Multiplier");

// Инициализация
multiplier->Default();

// Настройка параметра
multiplier->Multiplier = 2.5;

// Построение
multiplier->Build();

// Выполнение вычислений
multiplier->Calculate();

// Получение результата
double result = multiplier->Output();
```

### Объяснение ключевых моментов

1. **Наследование от UContainer** - базовый класс для компонентов с контейнерами
2. **Свойства** - используются для входов, выходов и параметров
3. **Жизненный цикл** - ADefault, ABuild, ACalculate вызываются в определенном порядке
4. **IsReady()** - проверка готовности компонента перед вычислениями
5. **Регистрация** - компонент должен быть зарегистрирован в Storage

### Связанная документация

- [Guides/Creating-Components.md](../Guides/Creating-Components.md) - детальное руководство
- [Guides/Creating-Properties.md](../Guides/Creating-Properties.md) - создание свойств
- [Engine-Detailed.md](../Engine-Detailed.md) - детали движка

---

## EN

### Task Description

Create a simple component that multiplies an input value by a parameter and outputs the result.

### Solution

#### Step 1: Class Declaration

```cpp
#include "Rdk/Core/Engine/UContainer.h"

class UMultiplier : public RDK::UContainer {
public:
    UMultiplier() : RDK::UContainer() {}
    
protected:
    // Component properties
    RDK::UProperty<double, UMultiplier, RDK::ptPubInput> Input;
    RDK::UProperty<double, UMultiplier, RDK::ptPubOutput> Output;
    RDK::UProperty<double, UMultiplier, RDK::ptPubParam> Multiplier;
    
    // Lifecycle methods
    virtual bool ADefault(void) override;
    virtual bool ABuild(void) override;
    virtual bool ACalculate(void) override;
};
```

#### Step 2: Property Initialization

```cpp
UMultiplier::UMultiplier()
    : RDK::UContainer()
    , Input("Input", this, 0.0, nullptr, false)
    , Output("Output", this, 0.0, nullptr, false)
    , Multiplier("Multiplier", this, 1.0, nullptr, false)
{
}
```

#### Step 3: Lifecycle Methods Implementation

```cpp
bool UMultiplier::ADefault(void) {
    if (!UContainer::ADefault())
        return false;
    
    // Set default values
    Input = 0.0;
    Output = 0.0;
    Multiplier = 1.0;
    
    return true;
}

bool UMultiplier::ABuild(void) {
    if (!UContainer::ABuild())
        return false;
    
    // Check input readiness
    if (!Input.IsConnected()) {
        Logger->LogMessageEx(RDK_EX_WARNING, GetName(), __FUNCTION__,
                            "Input is not connected");
    }
    
    return true;
}

bool UMultiplier::ACalculate(void) {
    if (!IsReady())
        return false;
    
    // Main logic: multiplication
    Output = Input() * Multiplier();
    
    return true;
}
```

#### Step 4: Component Registration

```cpp
// In library loading function
void LoadMyLibrary(RDK::UStorage* storage) {
    storage->AddClass<UMultiplier>("UMultiplier");
    storage->BuildStorage();
}
```

#### Step 5: Using the Component

```cpp
// Create component
auto multiplier = storage->CreateComponent<UMultiplier>("Multiplier");

// Initialize
multiplier->Default();

// Configure parameter
multiplier->Multiplier = 2.5;

// Build
multiplier->Build();

// Execute calculations
multiplier->Calculate();

// Get result
double result = multiplier->Output();
```

### Key Points Explanation

1. **Inheriting from UContainer** - base class for components with containers
2. **Properties** - used for inputs, outputs, and parameters
3. **Lifecycle** - ADefault, ABuild, ACalculate called in specific order
4. **IsReady()** - check component readiness before calculations
5. **Registration** - component must be registered in Storage

### Related Documentation

- [Guides/Creating-Components.md](../Guides/Creating-Components.md) - detailed guide
- [Guides/Creating-Properties.md](../Guides/Creating-Properties.md) - creating properties
- [Engine-Detailed.md](../Engine-Detailed.md) - engine details
