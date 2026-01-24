# Быстрый старт с Rdk Core

## RU

### Назначение

Это руководство поможет вам быстро начать работу с Rdk Core в зависимости от вашей роли и задач.

### Для разработчиков компонентов

#### Шаг 1: Изучите основы

1. Прочитайте [Architecture.md](Architecture.md) - общее понимание архитектуры
2. Изучите [Engine-Detailed.md](Engine-Detailed.md) - детали движка и компонентной системы
3. Ознакомьтесь с [Diagrams/Component-Lifecycle.md](Diagrams/Component-Lifecycle.md) - жизненный цикл компонента

#### Шаг 2: Создайте первый компонент

1. Следуйте руководству [Guides/Creating-Components.md](Guides/Creating-Components.md)
2. Изучите примеры в [Examples/](Examples/) (если доступны)
3. Создайте простой компонент, наследующий от `UContainer`

**Минимальный пример:**

```cpp
#include "Rdk/Core/Engine/UContainer.h"

class MyFirstComponent : public RDK::UContainer {
public:
    MyFirstComponent() : RDK::UContainer() {}
    
protected:
    virtual bool ADefault(void) override {
        if (!UContainer::ADefault())
            return false;
        // Инициализация по умолчанию
        return true;
    }
    
    virtual bool ABuild(void) override {
        if (!UContainer::ABuild())
            return false;
        // Построение компонента
        return true;
    }
    
    virtual bool ACalculate(void) override {
        if (!IsReady())
            return false;
        // Вычисления
        return true;
    }
};
```

#### Шаг 3: Добавьте свойства

1. Изучите [Guides/Creating-Properties.md](Guides/Creating-Properties.md)
2. Добавьте свойства к вашему компоненту
3. Изучите [Diagrams/Property-System.md](Diagrams/Property-System.md) - система свойств

#### Шаг 4: Зарегистрируйте компонент

```cpp
// В функции загрузки библиотеки
storage->AddClass<MyFirstComponent>("MyFirstComponent");
storage->BuildStorage();
```

#### Шаг 5: Используйте компонент

```cpp
auto comp = storage->CreateComponent<MyFirstComponent>("MyComp");
comp->Default();
comp->Build();
comp->Calculate();
```

### Для разработчиков библиотек

#### Шаг 1: Изучите архитектуру

1. [Architecture.md](Architecture.md) - общая архитектура
2. [Engine-Detailed.md](Engine-Detailed.md) - детали движка
3. [Application-Detailed.md](Application-Detailed.md) - прикладной уровень

#### Шаг 2: Создайте структуру библиотеки

1. Создайте функцию загрузки библиотеки
2. Регистрируйте все компоненты библиотеки в `UStorage`
3. Создайте описания классов (ClDesc) для каждого компонента

**Пример функции загрузки:**

```cpp
void LoadMyLibrary(RDK::UStorage* storage) {
    // Регистрация компонентов
    storage->AddClass<MyComponent1>("MyComponent1");
    storage->AddClass<MyComponent2>("MyComponent2");
    
    // Построение хранилища
    storage->BuildStorage();
}
```

#### Шаг 3: Реализуйте компоненты

1. Следуйте руководствам по созданию компонентов
2. Реализуйте сериализацию для всех компонентов
3. Добавьте документацию

#### Шаг 4: Тестирование

1. Создайте unit тесты для компонентов
2. См. [Tests.md](Tests.md) - структура тестов
3. Протестируйте интеграцию с другими библиотеками

### Для интеграторов

#### Шаг 1: Изучите API

1. [API-Reference.md](API-Reference.md) - полный справочник API
2. [Application-Detailed.md](Application-Detailed.md) - работа с приложением
3. [RPC Integration](Guides/RPC-Integration.md) - интеграция RPC

#### Шаг 2: Интеграция в приложение

1. Инициализируйте `UApplication`
2. Загрузите необходимые библиотеки
3. Создайте и настройте `UEngine`

**Пример инициализации:**

```cpp
#include "Rdk/Core/Application/UApplication.h"

RDK::UApplication app;
app.Initialize();

// Загрузка библиотек
RDK::UStorage* storage = app.GetStorage();
// ... загрузка библиотек

// Создание движка
RDK::UEngine* engine = app.GetEngine();
engine->SetStorage(storage);
```

#### Шаг 3: Работа с проектами

1. Изучите [Application-Detailed.md](Application-Detailed.md) - раздел о проектах
2. Используйте `UProject` для загрузки и сохранения проектов
3. Работайте с конфигурационными файлами

### Чеклист для начала работы

#### Для разработчиков компонентов

- [ ] Изучена архитектура Rdk Core
- [ ] Понятен жизненный цикл компонента
- [ ] Создан первый простой компонент
- [ ] Добавлены свойства к компоненту
- [ ] Компонент зарегистрирован в Storage
- [ ] Компонент протестирован

#### Для разработчиков библиотек

- [ ] Изучена архитектура Rdk Core
- [ ] Создана структура библиотеки
- [ ] Реализованы компоненты библиотеки
- [ ] Компоненты зарегистрированы
- [ ] Созданы ClDesc для компонентов
- [ ] Библиотека протестирована

#### Для интеграторов

- [ ] Изучен API Reference
- [ ] Понятна работа с Application
- [ ] Настроена интеграция RPC (если нужно)
- [ ] Реализована загрузка проектов
- [ ] Интеграция протестирована

### Следующие шаги

- [FAQ.md](FAQ.md) - ответы на часто задаваемые вопросы
- [Guides/](Guides/) - детальные руководства
- [Examples/](Examples/) - практические примеры
- [Docs/README.md](../../Docs/README.md) - главная документация проекта

---

## EN

### Purpose

This guide will help you quickly get started with Rdk Core depending on your role and tasks.

### For Component Developers

#### Step 1: Learn the Basics

1. Read [Architecture.md](Architecture.md) - general architecture understanding
2. Study [Engine-Detailed.md](Engine-Detailed.md) - engine and component system details
3. Review [Diagrams/Component-Lifecycle.md](Diagrams/Component-Lifecycle.md) - component lifecycle

#### Step 2: Create Your First Component

1. Follow the guide [Guides/Creating-Components.md](Guides/Creating-Components.md)
2. Study examples in [Examples/](Examples/) (if available)
3. Create a simple component inheriting from `UContainer`

**Minimal example:**

```cpp
#include "Rdk/Core/Engine/UContainer.h"

class MyFirstComponent : public RDK::UContainer {
public:
    MyFirstComponent() : RDK::UContainer() {}
    
protected:
    virtual bool ADefault(void) override {
        if (!UContainer::ADefault())
            return false;
        // Default initialization
        return true;
    }
    
    virtual bool ABuild(void) override {
        if (!UContainer::ABuild())
            return false;
        // Component building
        return true;
    }
    
    virtual bool ACalculate(void) override {
        if (!IsReady())
            return false;
        // Calculations
        return true;
    }
};
```

#### Step 3: Add Properties

1. Study [Guides/Creating-Properties.md](Guides/Creating-Properties.md)
2. Add properties to your component
3. Review [Diagrams/Property-System.md](Diagrams/Property-System.md) - property system

#### Step 4: Register the Component

```cpp
// In library loading function
storage->AddClass<MyFirstComponent>("MyFirstComponent");
storage->BuildStorage();
```

#### Step 5: Use the Component

```cpp
auto comp = storage->CreateComponent<MyFirstComponent>("MyComp");
comp->Default();
comp->Build();
comp->Calculate();
```

### For Library Developers

#### Step 1: Study Architecture

1. [Architecture.md](Architecture.md) - general architecture
2. [Engine-Detailed.md](Engine-Detailed.md) - engine details
3. [Application-Detailed.md](Application-Detailed.md) - application layer

#### Step 2: Create Library Structure

1. Create library loading function
2. Register all library components in `UStorage`
3. Create class descriptions (ClDesc) for each component

**Example loading function:**

```cpp
void LoadMyLibrary(RDK::UStorage* storage) {
    // Register components
    storage->AddClass<MyComponent1>("MyComponent1");
    storage->AddClass<MyComponent2>("MyComponent2");
    
    // Build storage
    storage->BuildStorage();
}
```

#### Step 3: Implement Components

1. Follow component creation guides
2. Implement serialization for all components
3. Add documentation

#### Step 4: Testing

1. Create unit tests for components
2. See [Tests.md](Tests.md) - test structure
3. Test integration with other libraries

### For Integrators

#### Step 1: Study API

1. [API-Reference.md](API-Reference.md) - complete API reference
2. [Application-Detailed.md](Application-Detailed.md) - working with application
3. [RPC Integration](Guides/RPC-Integration.md) - RPC integration

#### Step 2: Application Integration

1. Initialize `UApplication`
2. Load necessary libraries
3. Create and configure `UEngine`

**Example initialization:**

```cpp
#include "Rdk/Core/Application/UApplication.h"

RDK::UApplication app;
app.Initialize();

// Load libraries
RDK::UStorage* storage = app.GetStorage();
// ... load libraries

// Create engine
RDK::UEngine* engine = app.GetEngine();
engine->SetStorage(storage);
```

#### Step 3: Working with Projects

1. Study [Application-Detailed.md](Application-Detailed.md) - projects section
2. Use `UProject` to load and save projects
3. Work with configuration files

### Getting Started Checklist

#### For Component Developers

- [ ] Rdk Core architecture studied
- [ ] Component lifecycle understood
- [ ] First simple component created
- [ ] Properties added to component
- [ ] Component registered in Storage
- [ ] Component tested

#### For Library Developers

- [ ] Rdk Core architecture studied
- [ ] Library structure created
- [ ] Library components implemented
- [ ] Components registered
- [ ] ClDesc created for components
- [ ] Library tested

#### For Integrators

- [ ] API Reference studied
- [ ] Application usage understood
- [ ] RPC integration configured (if needed)
- [ ] Project loading implemented
- [ ] Integration tested

### Next Steps

- [FAQ.md](FAQ.md) - frequently asked questions
- [Guides/](Guides/) - detailed guides
- [Examples/](Examples/) - practical examples
- [Docs/README.md](../../Docs/README.md) - main project documentation
