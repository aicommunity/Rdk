# FAQ - Rdk Core

## RU

### Общие вопросы

#### Что такое Rdk Core?

Rdk Core - это ядро системы Nmsdk, предоставляющее базовую инфраструктуру для компонентной архитектуры. Оно включает движок выполнения, систему компонентов, сериализацию, графику и прикладной уровень.

#### Какие основные модули входят в Rdk Core?

- **Core/Engine** - движок и компонентная система
- **Core/Application** - управление приложением, RPC, сервер, проекты
- **Core/Graphics** - система графики для визуализации
- **Core/Serialize** - система сериализации (XML, Binary)
- **Core/System** - кроссплатформенные системные абстракции
- **Core/Math** - математические утилиты
- **Core/Utilities** - вспомогательные утилиты

#### Где найти обзорную информацию о Rdk Core?

См. [Architecture/Overview.md](Architecture/Overview.md) в документации Rdk.

### Компоненты

#### Как создать новый компонент?

См. подробное руководство: [Guides/Creating-Components.md](Guides/Creating-Components.md)

**Краткий ответ:**
1. Наследуйте класс от `UComponent` или его потомков (`UContainer`, `UNet`)
2. Реализуйте методы жизненного цикла: `ADefault()`, `ABuild()`, `AReset()`, `ACalculate()`
3. Зарегистрируйте компонент в `UStorage`
4. Используйте `UProperty` для создания свойств

#### Что такое жизненный цикл компонента?

Жизненный цикл компонента включает:
- **Default()** - инициализация значений по умолчанию
- **Build()** - построение компонента, создание подкомпонентов
- **Reset()** - сброс состояния
- **Calculate()** - выполнение вычислений

Подробнее см. [Diagrams/Component-Lifecycle.md](Diagrams/Component-Lifecycle.md)

#### Как создать свойство компонента?

См. подробное руководство: [Guides/Creating-Properties.md](Guides/Creating-Properties.md)

**Краткий ответ:**
Используйте шаблон `UProperty`:
```cpp
UProperty<double, MyComponent, ptPubParam> MyParam;
```

#### Что такое контроллер?

Контроллер - это механизм управления поведением компонента. См. [Controllers-System.md](Controllers-System.md) и [Guides/Creating-Controllers.md](Guides/Creating-Controllers.md)

### Свойства

#### Какие типы свойств существуют?

- **ptPubParam** - публичный параметр (настраивается пользователем)
- **ptPubState** - публичное состояние (видимо, но не настраивается)
- **ptPrivParam** - приватный параметр
- **ptPrivState** - приватное состояние
- **ptInput** - входное свойство
- **ptOutput** - выходное свойство

#### Как создать связь между свойствами?

Используйте метод `AttachTo()`:
```cpp
targetProperty.AttachTo(&sourceProperty);
```

Или через `CreateLink()` в контейнере:
```cpp
CreateLink(sourceOutput, targetInput);
```

### Сериализация

#### Как реализовать сериализацию компонента?

См. подробное руководство: [Guides/Serialization-Guide.md](Guides/Serialization-Guide.md)

**Краткий ответ:**
Переопределите методы `Save()` и `Load()`:
```cpp
virtual bool Save(USerStorageXML &xml) override;
virtual bool Load(USerStorageXML &xml) override;
```

#### Какие форматы сериализации поддерживаются?

- **XML** - текстовый формат, читаемый человеком
- **Binary** - бинарный формат, более компактный и быстрый

### RPC

#### Как интегрировать RPC в компонент?

См. подробное руководство: [Guides/RPC-Integration.md](Guides/RPC-Integration.md)

**Краткий ответ:**
1. Используйте `URpcDispatcher` для регистрации команд
2. Реализуйте обработчики команд
3. Используйте `URpcServer` для запуска сервера

### Многопоточность

#### Как обеспечить потокобезопасность компонента?

См. подробное руководство: [Guides/Threading-Guide.md](Guides/Threading-Guide.md)

**Краткий ответ:**
- Используйте `UGenericMutex` для защиты критических секций
- Используйте потокобезопасные свойства (`thread_safe=true`)
- Избегайте разделяемых изменяемых данных без синхронизации

### Обработка ошибок

#### Как правильно обрабатывать ошибки в компонентах?

См. подробное руководство: [Guides/Error-Handling.md](Guides/Error-Handling.md)

**Краткий ответ:**
- Используйте исключения `UException` и его потомков
- Логируйте ошибки через `Logger->LogMessageEx()`
- Возвращайте `false` из методов жизненного цикла при ошибках

### Производительность

#### Как оптимизировать производительность компонента?

- Избегайте выделения памяти в `ACalculate()`
- Используйте кэширование результатов
- Минимизируйте копирование данных
- Используйте прямой доступ к свойствам где возможно

Подробнее см. [Docs/Performance-And-Testing/Performance-Overview.md](../../Docs/Performance-And-Testing/Performance-Overview.md)

### Логирование

#### Как использовать систему логирования?

См. [Logging-System.md](Logging-System.md)

**Краткий ответ:**
```cpp
Logger->LogMessageEx(RDK_EX_INFO, GetName(), __FUNCTION__, "Message");
```

### Тестирование

#### Как писать тесты для компонентов?

См. [Tests.md](Tests.md) и [Docs/Performance-And-Testing/Testing-Strategy.md](../../Docs/Performance-And-Testing/Testing-Strategy.md)

### Устранение неполадок

#### Где найти решения типичных проблем?

См. [Docs/Troubleshooting/Troubleshooting-Guide.md](../../Docs/Troubleshooting/Troubleshooting-Guide.md) в корневой документации.

### Дополнительные ресурсы

- [API Reference](API-Reference.md) - полный справочник API
- [Architecture](Architecture.md) - архитектура подсистем
- [Docs/README.md](../../Docs/README.md) - главная страница документации проекта

---

## EN

### General Questions

#### What is Rdk Core?

Rdk Core is the core of the Nmsdk system, providing the basic infrastructure for component-based architecture. It includes the execution engine, component system, serialization, graphics, and application layer.

#### What are the main modules in Rdk Core?

- **Core/Engine** - engine and component system
- **Core/Application** - application management, RPC, server, projects
- **Core/Graphics** - graphics system for visualization
- **Core/Serialize** - serialization system (XML, Binary)
- **Core/System** - cross-platform system abstractions
- **Core/Math** - mathematical utilities
- **Core/Utilities** - helper utilities

#### Where can I find overview information about Rdk Core?

See [Architecture/Overview.md](Architecture/Overview.md) in Rdk documentation.

### Components

#### How do I create a new component?

See detailed guide: [Guides/Creating-Components.md](Guides/Creating-Components.md)

**Quick answer:**
1. Inherit from `UComponent` or its descendants (`UContainer`, `UNet`)
2. Implement lifecycle methods: `ADefault()`, `ABuild()`, `AReset()`, `ACalculate()`
3. Register the component in `UStorage`
4. Use `UProperty` to create properties

#### What is component lifecycle?

Component lifecycle includes:
- **Default()** - initialization of default values
- **Build()** - component building, creating subcomponents
- **Reset()** - state reset
- **Calculate()** - execution of calculations

For more details see [Diagrams/Component-Lifecycle.md](Diagrams/Component-Lifecycle.md)

#### How do I create a component property?

See detailed guide: [Guides/Creating-Properties.md](Guides/Creating-Properties.md)

**Quick answer:**
Use the `UProperty` template:
```cpp
UProperty<double, MyComponent, ptPubParam> MyParam;
```

#### What is a controller?

A controller is a mechanism for managing component behavior. See [Controllers-System.md](Controllers-System.md) and [Guides/Creating-Controllers.md](Guides/Creating-Controllers.md)

### Properties

#### What property types exist?

- **ptPubParam** - public parameter (configurable by user)
- **ptPubState** - public state (visible but not configurable)
- **ptPrivParam** - private parameter
- **ptPrivState** - private state
- **ptInput** - input property
- **ptOutput** - output property

#### How do I create a link between properties?

Use the `AttachTo()` method:
```cpp
targetProperty.AttachTo(&sourceProperty);
```

Or through `CreateLink()` in a container:
```cpp
CreateLink(sourceOutput, targetInput);
```

### Serialization

#### How do I implement component serialization?

See detailed guide: [Guides/Serialization-Guide.md](Guides/Serialization-Guide.md)

**Quick answer:**
Override `Save()` and `Load()` methods:
```cpp
virtual bool Save(USerStorageXML &xml) override;
virtual bool Load(USerStorageXML &xml) override;
```

#### What serialization formats are supported?

- **XML** - human-readable text format
- **Binary** - binary format, more compact and faster

### RPC

#### How do I integrate RPC into a component?

See detailed guide: [Guides/RPC-Integration.md](Guides/RPC-Integration.md)

**Quick answer:**
1. Use `URpcDispatcher` to register commands
2. Implement command handlers
3. Use `URpcServer` to start the server

### Multithreading

#### How do I ensure component thread safety?

See detailed guide: [Guides/Threading-Guide.md](Guides/Threading-Guide.md)

**Quick answer:**
- Use `UGenericMutex` to protect critical sections
- Use thread-safe properties (`thread_safe=true`)
- Avoid shared mutable data without synchronization

### Error Handling

#### How do I properly handle errors in components?

See detailed guide: [Guides/Error-Handling.md](Guides/Error-Handling.md)

**Quick answer:**
- Use `UException` and its descendants
- Log errors through `Logger->LogMessageEx()`
- Return `false` from lifecycle methods on errors

### Performance

#### How do I optimize component performance?

- Avoid memory allocation in `ACalculate()`
- Use result caching
- Minimize data copying
- Use direct property access where possible

For more details see [Docs/Performance-And-Testing/Performance-Overview.md](../../Docs/Performance-And-Testing/Performance-Overview.md)

### Logging

#### How do I use the logging system?

See [Logging-System.md](Logging-System.md)

**Quick answer:**
```cpp
Logger->LogMessageEx(RDK_EX_INFO, GetName(), __FUNCTION__, "Message");
```

### Testing

#### How do I write tests for components?

See [Tests.md](Tests.md) and [Docs/Performance-And-Testing/Testing-Strategy.md](../../Docs/Performance-And-Testing/Testing-Strategy.md)

### Troubleshooting

#### Where can I find solutions to typical problems?

See [Docs/Troubleshooting/Troubleshooting-Guide.md](../../Docs/Troubleshooting/Troubleshooting-Guide.md) in the root documentation.

### Additional Resources

- [API Reference](API-Reference.md) - complete API reference
- [Architecture](Architecture.md) - subsystem architecture
- [Docs/README.md](../../Docs/README.md) - main project documentation page
