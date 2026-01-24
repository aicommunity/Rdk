# Rdk Core - Обзор

## RU

### Назначение

**Rdk (Core)** - это ядро системы Nmsdk, предоставляющее базовую инфраструктуру для компонентной архитектуры. Ядро реализует:

- Движок выполнения компонентов
- Компонентную систему
- Систему сериализации (XML, Binary)
- Графическую подсистему
- Прикладной уровень (RPC, сервер, управление проектами)
- Кроссплатформенные абстракции

### Структура модулей

#### Схема зависимостей модулей Rdk

```mermaid
flowchart TB
    subgraph "Core/Application"
        App[UApplication]
        EngineCtrl[UEngineControl]
        RPC[URpcDispatcher]
    end
    
    subgraph "Core/Engine"
        Engine[UEngine]
        Component[UComponent]
        Storage[UStorage]
        Env[UEnvironment]
    end
    
    subgraph "Core/Graphics"
        Graphics[UGraphics]
        DrawEngine[UDrawEngine]
    end
    
    subgraph "Core/Serialize"
        Serialize[USerStorage]
    end
    
    subgraph "Core/System"
        System[System Abstractions]
    end
    
    App --> EngineCtrl
    EngineCtrl --> Engine
    Engine --> Storage
    Engine --> Env
    Engine --> Component
    Component --> Graphics
    Component --> Serialize
    App --> RPC
    RPC --> EngineCtrl
    Graphics --> DrawEngine
    Engine --> System
```

#### Core/Application
Управление приложением, RPC, сервер, проекты.

**Основные классы:**
- `UApplication` - главный класс приложения
- `UEngineControl` - управление движком
- `URpcDispatcher` - диспетчер RPC команд
- `UServerTransport` - транспорт сервера (TCP, HTTP)
- `UProject` - проект

См. [Архитектура приложения](Application-Architecture.md)

#### Core/Engine
Движок и компонентная система.

**Основные классы:**
- `UEngine` - главный класс движка
- `UComponent` - базовый класс компонентов
- `UContainer` - контейнер компонентов
- `UNet` - сеть компонентов
- `UEnvironment` - окружение выполнения
- `UStorage` - хранилище компонентов
- `UProperty` - система свойств

См. [Архитектура движка](Engine-Architecture.md)

#### Core/Graphics
Система графики для визуализации компонентов и данных.

**Основные классы:**
- `UGraphics` - основной класс графики
- `UDrawEngine` - движок отрисовки
- `UBitmap` - растровое изображение
- `UFont` - шрифт

См. [Архитектура графики](Graphics-Architecture.md)

#### Core/Serialize
Система сериализации данных и компонентов.

**Основные классы:**
- `USerStorage` - хранилище данных
- `UXMLStdSerialize` - XML сериализация
- `UBinaryStdSerialize` - бинарная сериализация

См. [Архитектура сериализации](Serialize-Architecture.md)

#### Core/System
Кроссплатформенные системные абстракции.

**Основные классы:**
- `UGenericMutex` - универсальный мьютекс
- `UGenericEvent` - универсальное событие
- `UDllLoader` - загрузчик DLL/SO
- `USharedMemoryLoader` - загрузчик разделяемой памяти

См. [Системные абстракции](System-Platform-Abstraction.md)

#### Core/Utilities
Вспомогательные утилиты.

#### Core/Math
Математические утилиты (матрицы, векторы, математические операции).

#### Core/Console
Консольный движок для выполнения без GUI.

**Основные классы:**
- `UConsoleEngine` - консольный движок

### Зависимости

Rdk Core не зависит от других библиотек проекта, но использует:

- Qt5 (Core) - для базовой функциональности
- Стандартная библиотека C++
- Системные библиотеки (платформо-зависимые)

### Детальная документация

#### Архитектура (обзорные документы)

- [Архитектура приложения](Application-Architecture.md) - RPC, сервер, управление проектами
- [Архитектура движка](Engine-Architecture.md) - компоненты, контейнеры, свойства
- [Архитектура графики](Graphics-Architecture.md) - система графики и визуализации
- [Архитектура сериализации](Serialize-Architecture.md) - XML и бинарная сериализация
- [Системные абстракции](System-Platform-Abstraction.md) - кроссплатформенные абстракции

#### Детальная документация в Rdk/Docs

- [Обзор документации Rdk](../README.md) - главная страница документации Rdk
- [Архитектура](../Architecture.md) - детальное описание архитектуры подсистем
- [Архитектурные диаграммы](../Architecture-Diagrams.md) - обзорные диаграммы
- [API Справочник](../API-Reference.md) - полный справочник API

#### Детальная документация модулей

- [Движок](../Engine-Detailed.md) - компоненты, свойства, контроллеры
- [Приложение](../Application-Detailed.md) - RPC, проекты, сервер
- [Графика](../Graphics-Detailed.md) - графика, шрифты, отрисовка
- [Сериализация](../Serialize-Detailed.md) - XML, Binary сериализация
- [Система](../System-Detailed.md) - мьютексы, события, загрузка библиотек
- [Математика](../Math-Detailed.md) - математические утилиты
- [Утилиты](../Utilities-Detailed.md) - вспомогательные утилиты

#### Справочники

- [Математические библиотеки](../Math-Libraries-Reference.md) - матрицы, векторы, фильтры Калмана
- [Утилиты](../Utilities-Reference.md) - исключения, файлы, временные метки
- [Система логирования](../Logging-System.md) - логирование
- [Система контроллеров](../Controllers-System.md) - контроллеры
- [Консольное приложение](../Console-Application.md) - консольный движок
- [Тесты](../Tests.md) - юнит и интеграционные тесты

#### Руководства

- [Создание компонентов](../Guides/Creating-Components.md)
- [Создание свойств](../Guides/Creating-Properties.md)
- [Создание контроллеров](../Guides/Creating-Controllers.md)
- [Сериализация](../Guides/Serialization-Guide.md)
- [RPC интеграция](../Guides/RPC-Integration.md)
- [Многопоточность](../Guides/Threading-Guide.md)
- [Обработка ошибок](../Guides/Error-Handling.md)
- [Управление конфигурациями](../Configuration-Management.md)

#### Диаграммы

- [Жизненный цикл компонента](../Diagrams/Component-Lifecycle.md)
- [Система свойств](../Diagrams/Property-System.md)
- [Поток RPC](../Diagrams/RPC-Flow.md)

#### Индексы

- [Полный индекс документации Rdk](../../Docs/Submodules/Rdk-Index.md) - структурированный индекс всей документации Rdk
- [Навигационная карта](../../Docs/Submodules/Navigation-Map.md) - визуальная карта документации

---

## EN

### Purpose

**Rdk (Core)** is the core of the Nmsdk system, providing the basic infrastructure for component-based architecture. The core implements:

- Component execution engine
- Component system
- Serialization system (XML, Binary)
- Graphics subsystem
- Application layer (RPC, server, project management)
- Cross-platform abstractions

### Module Structure

#### Core/Application
Application management, RPC, server, projects.

#### Core/Engine
Engine and component system.

#### Core/Graphics
Graphics system for visualizing components and data.

#### Core/Serialize
Data and component serialization system.

#### Core/System
Cross-platform system abstractions.

#### Core/Utilities
Helper utilities.

#### Core/Math
Mathematical utilities (matrices, vectors, mathematical operations).

#### Core/Console
Console engine for execution without GUI.

### Dependencies

Rdk Core does not depend on other project libraries but uses:

- Qt5 (Core) - for basic functionality
- C++ standard library
- System libraries (platform-dependent)

### Detailed Documentation

#### Architecture (Overview Documents)

- [Application Architecture](Application-Architecture.md) - RPC, server, project management
- [Engine Architecture](Engine-Architecture.md) - components, containers, properties
- [Graphics Architecture](Graphics-Architecture.md) - graphics and visualization system
- [Serialization Architecture](Serialize-Architecture.md) - XML and binary serialization
- [System Abstractions](System-Platform-Abstraction.md) - cross-platform abstractions

#### Detailed Documentation in Rdk/Docs

- [Rdk Documentation Overview](../README.md) - main Rdk documentation page
- [Architecture](../Architecture.md) - detailed subsystem architecture description
- [Architecture Diagrams](../Architecture-Diagrams.md) - overview diagrams
- [API Reference](../API-Reference.md) - complete API reference

#### Detailed Module Documentation

- [Engine](../Engine-Detailed.md) - components, properties, controllers
- [Application](../Application-Detailed.md) - RPC, projects, server
- [Graphics](../Graphics-Detailed.md) - graphics, fonts, rendering
- [Serialization](../Serialize-Detailed.md) - XML, Binary serialization
- [System](../System-Detailed.md) - mutexes, events, library loading
- [Math](../Math-Detailed.md) - mathematical utilities
- [Utilities](../Utilities-Detailed.md) - helper utilities

#### References

- [Math Libraries](../Math-Libraries-Reference.md) - matrices, vectors, Kalman filters
- [Utilities](../Utilities-Reference.md) - exceptions, files, timestamps
- [Logging System](../Logging-System.md) - logging
- [Controllers System](../Controllers-System.md) - controllers
- [Console Application](../Console-Application.md) - console engine
- [Tests](../Tests.md) - unit and integration tests

#### Guides

- [Creating Components](../Guides/Creating-Components.md)
- [Creating Properties](../Guides/Creating-Properties.md)
- [Creating Controllers](../Guides/Creating-Controllers.md)
- [Serialization](../Guides/Serialization-Guide.md)
- [RPC Integration](../Guides/RPC-Integration.md)
- [Threading](../Guides/Threading-Guide.md)
- [Error Handling](../Guides/Error-Handling.md)
- [Configuration Management](../Configuration-Management.md)

#### Diagrams

- [Component Lifecycle](../Diagrams/Component-Lifecycle.md)
- [Property System](../Diagrams/Property-System.md)
- [RPC Flow](../Diagrams/RPC-Flow.md)

#### Indexes

- [Complete Rdk Documentation Index](../../Docs/Submodules/Rdk-Index.md) - structured index of all Rdk documentation
- [Navigation Map](../../Docs/Submodules/Navigation-Map.md) - visual documentation map
