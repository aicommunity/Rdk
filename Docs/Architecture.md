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

См. [Docs/Rdk-Core/Application-Architecture.md](../Docs/Rdk-Core/Application-Architecture.md)

#### Core/Engine

Движок и компонентная система.

**Основные классы:**
- `UEngine` - главный класс движка
- `UComponent` - базовый класс компонентов
- `UContainer` - контейнер компонентов
- `UEnvironment` - окружение выполнения
- `UStorage` - хранилище компонентов

См. [Docs/Rdk-Core/Engine-Architecture.md](../Docs/Rdk-Core/Engine-Architecture.md)

#### Core/Graphics

Система графики для визуализации.

**Основные классы:**
- `UGraphics` - основной класс графики
- `UDrawEngine` - движок отрисовки
- `UBitmap` - растровое изображение

См. [Docs/Rdk-Core/Graphics-Architecture.md](../Docs/Rdk-Core/Graphics-Architecture.md)

#### Core/Serialize

Система сериализации данных и компонентов.

**Основные классы:**
- `USerStorage` - хранилище данных
- `UXMLStdSerialize` - XML сериализация
- `UBinaryStdSerialize` - бинарная сериализация

См. [Docs/Rdk-Core/Serialize-Architecture.md](../Docs/Rdk-Core/Serialize-Architecture.md)

#### Core/System

Кроссплатформенные системные абстракции.

**Основные классы:**
- `UGenericMutex` - универсальный мьютекс
- `UGenericEvent` - универсальное событие
- `UDllLoader` - загрузчик DLL/SO

См. [Docs/Rdk-Core/System-Platform-Abstraction.md](../Docs/Rdk-Core/System-Platform-Abstraction.md)

### Диаграммы

Диаграммы классов и последовательностей доступны в соответствующих разделах корневой документации.

---

## EN

### Overview

Detailed description of Rdk Core subsystem architecture.

### Subsystems

#### Core/Application

Application management, RPC, server, projects.

#### Core/Engine

Engine and component system.

#### Core/Graphics

Graphics system for visualization.

#### Core/Serialize

Data and component serialization system.

#### Core/System

Cross-platform system abstractions.

### Diagrams

Class and sequence diagrams are available in the corresponding sections of the root documentation.
