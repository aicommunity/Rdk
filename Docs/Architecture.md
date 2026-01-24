# Архитектура Rdk Core

## RU

### Обзор

Детальное описание архитектуры подсистем Rdk Core.

### Подсистемы

#### Core/Application

Управление приложением, RPC, сервер, проекты.

**Основные классы:**
- `UApplication` - главный класс приложения
- `UEngineControl` - управление движком
- `URpcDispatcher` - диспетчер RPC команд
- `UServerTransport` - транспорт сервера

См. [Architecture/Application-Architecture.md](Architecture/Application-Architecture.md)

#### Core/Engine

Движок и компонентная система.

**Основные классы:**
- `UEngine` - главный класс движка
- `UComponent` - базовый класс компонентов
- `UContainer` - контейнер компонентов
- `UEnvironment` - окружение выполнения
- `UStorage` - хранилище компонентов

**Взаимодействие классов:**

```mermaid
flowchart TB
    Engine[UEngine] --> Env[UEnvironment]
    Engine --> Storage[UStorage]
    Env --> RootContainer[UContainer]
    RootContainer --> Child1[UComponent]
    RootContainer --> Child2[UComponent]
    Storage -->|Factories| Child1
    Storage -->|Factories| Child2
```

В коде это соответствует тому, что:
- `UEngine` инициализирует `UStorage` и `UEnvironment` (см. `UEngine::Init` и связанные методы),
- `UStorage` загружает библиотеки (`ULibrary`) и регистрирует классы компонентов через фабрики,
- `UEnvironment` управляет жизненным циклом компонентов, вызывая `Reset/Calculate` у корневых контейнеров,
- `UContainer` и `UNet` группируют компоненты и делегируют вызовы их методам жизненного цикла.

См. [Architecture/Engine-Architecture.md](Architecture/Engine-Architecture.md)

#### Core/Graphics

Система графики для визуализации.

**Основные классы:**
- `UGraphics` - основной класс графики
- `UDrawEngine` - движок отрисовки
- `UBitmap` - растровое изображение

См. [Architecture/Graphics-Architecture.md](Architecture/Graphics-Architecture.md)

#### Core/Serialize

Система сериализации данных и компонентов.

**Основные классы:**
- `USerStorage` - хранилище данных
- `UXMLStdSerialize` - XML сериализация
- `UBinaryStdSerialize` - бинарная сериализация

См. [Architecture/Serialize-Architecture.md](Architecture/Serialize-Architecture.md)

#### Core/System

Кроссплатформенные системные абстракции.

**Основные классы:**
- `UGenericMutex` - универсальный мьютекс
- `UGenericEvent` - универсальное событие
- `UDllLoader` - загрузчик DLL/SO

См. [Architecture/System-Platform-Abstraction.md](Architecture/System-Platform-Abstraction.md)

### Диаграммы

Диаграммы классов и последовательностей для каждой подсистемы доступны в корневой документации:

- [Архитектура приложения](Architecture/Application-Architecture.md)
- [Архитектура движка](Architecture/Engine-Architecture.md)
- [Архитектура графики](Architecture/Graphics-Architecture.md)
- [Архитектура сериализации](Architecture/Serialize-Architecture.md)
- [Системные абстракции](Architecture/System-Platform-Abstraction.md)

---

## EN

### Overview

Detailed description of Rdk Core subsystem architecture.

### Subsystems

#### Core/Application

Application management, RPC, server, projects.

#### Core/Engine

Engine and component system.

**Class interaction:**

```mermaid
flowchart TB
    EngineEn[UEngine] --> EnvEn[UEnvironment]
    EngineEn --> StorageEn[UStorage]
    EnvEn --> RootContainerEn[UContainer]
    RootContainerEn --> Child1En[UComponent]
    RootContainerEn --> Child2En[UComponent]
    StorageEn -->|"Factories"| Child1En
    StorageEn -->|"Factories"| Child2En
```

This reflects the actual roles in code:
- `UEngine` initializes `UStorage` and `UEnvironment`,
- `UStorage` loads `ULibrary` instances and registers component classes via factories,
- `UEnvironment` drives the execution lifecycle (`Reset/Calculate`) for root containers,
- `UContainer` / `UNet` group components and propagate lifecycle calls to them.

#### Core/Graphics

Graphics system for visualization.

#### Core/Serialize

Data and component serialization system.

#### Core/System

Cross-platform system abstractions.

### Diagrams

Class and sequence diagrams for each subsystem are available in the root documentation:

- [Application Architecture](Architecture/Application-Architecture.md)
- [Engine Architecture](Architecture/Engine-Architecture.md)
- [Graphics Architecture](Architecture/Graphics-Architecture.md)
- [Serialization Architecture](Architecture/Serialize-Architecture.md)
- [System Abstractions](Architecture/System-Platform-Abstraction.md)
