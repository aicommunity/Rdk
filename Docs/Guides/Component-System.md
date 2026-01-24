# Компонентная система (Component System)

## RU

### Обзор

Компонентная система является основой архитектуры Nmsdk. Все функциональные единицы реализованы как компоненты, которые могут быть соединены в сети для выполнения вычислений.

### Базовые концепции

#### Компонент (UComponent)

**UComponent** - базовый класс для всех компонентов в системе. Каждый компонент:

- Наследуется от `UComponent` или `UNet`
- Имеет свойства (Properties) - параметры, состояния, входы, выходы
- Может быть соединен с другими компонентами
- Имеет жизненный цикл: Default → Build → Reset → Calculate

#### Свойства (Properties)

Свойства компонентов могут быть следующих типов:

- **ptParameter** - параметр компонента (настраивается пользователем)
- **ptState** - состояние компонента (изменяется во время выполнения)
- **ptTemp** - временное свойство (используется для промежуточных вычислений)
- **ptInput** - входное свойство (получает данные от других компонентов)
- **ptOutput** - выходное свойство (передает данные другим компонентам)

#### Контейнер (UContainer)

**UContainer** - контейнер для группировки компонентов. Позволяет:

- Организовывать компоненты в иерархические структуры
- Управлять жизненным циклом группы компонентов
- Изолировать компоненты друг от друга

#### Сеть (UNet)

**UNet** - сеть компонентов, наследник `UItem`. Сеть:

- Содержит компоненты и их соединения
- Управляет выполнением компонентов
- Обеспечивает передачу данных между компонентами

### Жизненный цикл компонента

1. **Default (ADefault)** - инициализация значений по умолчанию
2. **Build (ABuild)** - построение структуры компонента
3. **Reset (AReset)** - сброс состояния перед вычислениями
4. **Calculate (ACalculate)** - выполнение вычислений

**Пример создания компонента:**

```cpp
// Создание компонента через хранилище
auto component = storage->CreateComponent<MyComponent>("ComponentName");

// Настройка параметров
component->MyParameter = 42.0;
component->MyState = initialValue;

// Построение структуры (создание подкомпонентов, связей)
component->Build();

// Перед вычислениями - сброс состояния
component->Reset();

// Выполнение вычислений (вызывается каждый шаг времени)
component->Calculate();
```

**Best practices для разработки компонентов:**

1. **Инициализация в ADefault()**: устанавливайте все параметры и состояния в значения по умолчанию
2. **Создание структуры в ABuild()**: создавайте подкомпоненты, настраивайте связи, регистрируйте свойства
3. **Сброс в AReset()**: очищайте временные данные, сбрасывайте счётчики, готовьте компонент к новому циклу вычислений
4. **Вычисления в ACalculate()**: выполняйте основную логику компонента, обновляйте выходные свойства
5. **Использование свойств**: используйте типизированные свойства (`UProperty<T>`) для параметров, состояний, входов и выходов
6. **Потокобезопасность**: для компонентов, используемых в многопоточном режиме, используйте `thread_safe=true` при создании свойств

### См. также

- [Engine Architecture](../Architecture/Engine-Architecture.md) - детальная архитектура движка
- [Creating Components](Creating-Components.md) - практическое руководство по созданию компонентов
- [Configuration Management](../Configuration-Management.md) - управление конфигурациями

---

## EN

### Overview

The component system is the foundation of the Nmsdk architecture. All functional units are implemented as components that can be connected into networks for computation.

### Basic Concepts

#### Component (UComponent)

**UComponent** - base class for all components in the system.

#### Properties

Component properties can be of the following types:

- **ptParameter** - component parameter (configured by user)
- **ptState** - component state (changes during execution)
- **ptTemp** - temporary property (used for intermediate calculations)
- **ptInput** - input property (receives data from other components)
- **ptOutput** - output property (sends data to other components)

#### Container (UContainer)

**UContainer** - container for grouping components.

#### Net (UNet)

**UNet** - component network, inherits from `UItem`.

### Component Lifecycle

1. **Default (ADefault)** - default value initialization
2. **Build (ABuild)** - component structure building
3. **Reset (AReset)** - state reset before calculations
4. **Calculate (ACalculate)** - calculation execution

### See Also

- [Engine Architecture](../Architecture/Engine-Architecture.md) - detailed engine architecture
- [Creating Components](Creating-Components.md) - practical guide for creating components
- [Configuration Management](../Configuration-Management.md) - configuration management
