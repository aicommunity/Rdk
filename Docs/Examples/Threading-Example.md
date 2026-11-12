# Пример: Многопоточность

## RU

### Описание задачи

Реализовать потокобезопасный компонент, который обрабатывает данные из нескольких потоков.

### Решение

#### Шаг 1: Компонент с потокобезопасными свойствами

```cpp
#include "Rdk/Core/Engine/UContainer.h"
#include "Rdk/Core/System/UGenericMutex.h"

class UThreadSafeComponent : public RDK::UContainer {
public:
    UThreadSafeComponent() : RDK::UContainer() {}
    
protected:
    // Потокобезопасное свойство
    RDK::UProperty<double, UThreadSafeComponent, RDK::ptPubState> SharedData;
    
    // Мьютекс для защиты критических секций
    RDK::UGenericMutex DataMutex;
    
    virtual bool ADefault(void) override;
    
    // Потокобезопасные методы
    void ThreadSafeUpdate(double value);
    double ThreadSafeRead();
};
```

#### Шаг 2: Инициализация потокобезопасного свойства

```cpp
UThreadSafeComponent::UThreadSafeComponent()
    : RDK::UContainer()
    , SharedData("SharedData", this, 0.0, nullptr, true) // thread_safe=true
{
}

bool UThreadSafeComponent::ADefault(void) {
    if (!UContainer::ADefault())
        return false;
    
    SharedData = 0.0;
    return true;
}
```

#### Шаг 3: Потокобезопасные методы

```cpp
void UThreadSafeComponent::ThreadSafeUpdate(double value) {
    // Использование мьютекса для защиты критической секции
    RDK::UGenericMutexLock lock(DataMutex);
    
    // Обновление данных
    SharedData = value;
    
    // Дополнительные операции с данными
    ProcessData(value);
}

double UThreadSafeComponent::ThreadSafeRead() {
    // Использование мьютекса
    RDK::UGenericMutexLock lock(DataMutex);
    
    // Чтение данных
    return SharedData();
}
```

#### Шаг 4: Использование в многопоточной среде

```cpp
// Создание компонента
auto component = storage->CreateComponent<UThreadSafeComponent>("ThreadSafe");
component->Default();
component->Build();

// Запуск потоков
std::thread thread1([&component]() {
    for (int i = 0; i < 100; i++) {
        component->ThreadSafeUpdate(i * 1.5);
    }
});

std::thread thread2([&component]() {
    for (int i = 0; i < 100; i++) {
        double value = component->ThreadSafeRead();
        // Использование значения
    }
});

thread1.join();
thread2.join();
```

### Альтернативный подход: Потокобезопасные свойства

Если свойство объявлено как потокобезопасное (`thread_safe=true`), доступ к нему автоматически синхронизирован:

```cpp
// Прямой доступ к потокобезопасному свойству
component->SharedData = 10.0;  // Автоматически потокобезопасно
double value = component->SharedData();  // Автоматически потокобезопасно
```

### Объяснение ключевых моментов

1. **Потокобезопасные свойства** - использование `thread_safe=true` для автоматической синхронизации
2. **Мьютексы** - использование `UGenericMutex` для защиты критических секций
3. **RAII** - использование `UGenericMutexLock` для автоматической блокировки/разблокировки
4. **Избегание race conditions** - все операции с разделяемыми данными должны быть защищены

### Связанная документация

- [Guides/Threading-Guide.md](../Guides/Threading-Guide.md) - детальное руководство
- [System-Detailed.md](../System-Detailed.md) - детали системных абстракций

---

## EN

### Task Description

Implement a thread-safe component that processes data from multiple threads.

### Solution

#### Step 1: Component with Thread-Safe Properties

```cpp
#include "Rdk/Core/Engine/UContainer.h"
#include "Rdk/Core/System/UGenericMutex.h"

class UThreadSafeComponent : public RDK::UContainer {
public:
    UThreadSafeComponent() : RDK::UContainer() {}
    
protected:
    // Thread-safe property
    RDK::UProperty<double, UThreadSafeComponent, RDK::ptPubState> SharedData;
    
    // Mutex for protecting critical sections
    RDK::UGenericMutex DataMutex;
    
    virtual bool ADefault(void) override;
    
    // Thread-safe methods
    void ThreadSafeUpdate(double value);
    double ThreadSafeRead();
};
```

#### Step 2: Thread-Safe Property Initialization

```cpp
UThreadSafeComponent::UThreadSafeComponent()
    : RDK::UContainer()
    , SharedData("SharedData", this, 0.0, nullptr, true) // thread_safe=true
{
}

bool UThreadSafeComponent::ADefault(void) {
    if (!UContainer::ADefault())
        return false;
    
    SharedData = 0.0;
    return true;
}
```

#### Step 3: Thread-Safe Methods

```cpp
void UThreadSafeComponent::ThreadSafeUpdate(double value) {
    // Use mutex to protect critical section
    RDK::UGenericMutexLock lock(DataMutex);
    
    // Update data
    SharedData = value;
    
    // Additional data operations
    ProcessData(value);
}

double UThreadSafeComponent::ThreadSafeRead() {
    // Use mutex
    RDK::UGenericMutexLock lock(DataMutex);
    
    // Read data
    return SharedData();
}
```

#### Step 4: Usage in Multithreaded Environment

```cpp
// Create component
auto component = storage->CreateComponent<UThreadSafeComponent>("ThreadSafe");
component->Default();
component->Build();

// Start threads
std::thread thread1([&component]() {
    for (int i = 0; i < 100; i++) {
        component->ThreadSafeUpdate(i * 1.5);
    }
});

std::thread thread2([&component]() {
    for (int i = 0; i < 100; i++) {
        double value = component->ThreadSafeRead();
        // Use value
    }
});

thread1.join();
thread2.join();
```

### Alternative Approach: Thread-Safe Properties

If a property is declared as thread-safe (`thread_safe=true`), access to it is automatically synchronized:

```cpp
// Direct access to thread-safe property
component->SharedData = 10.0;  // Automatically thread-safe
double value = component->SharedData();  // Automatically thread-safe
```

### Key Points Explanation

1. **Thread-Safe Properties** - using `thread_safe=true` for automatic synchronization
2. **Mutexes** - using `UGenericMutex` to protect critical sections
3. **RAII** - using `UGenericMutexLock` for automatic lock/unlock
4. **Avoiding Race Conditions** - all operations with shared data must be protected

### Related Documentation

- [Guides/Threading-Guide.md](../Guides/Threading-Guide.md) - detailed guide
- [System-Detailed.md](../System-Detailed.md) - system abstractions details
