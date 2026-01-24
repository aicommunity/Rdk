# Документация Rdk Core

## RU

### Назначение

Этот каталог содержит детальную документацию ядра Rdk - основного модуля системы Nmsdk.

### Быстрые ссылки

- [Архитектура](Architecture.md) - детальное описание архитектуры
- [API Справочник](API-Reference.md) - полный справочник API
- [Создание компонентов](Guides/Creating-Components.md) - руководство по созданию компонентов
- [Создание свойств](Guides/Creating-Properties.md) - руководство по созданию свойств
- [Движок](Engine-Detailed.md) - детальная документация движка
- [Приложение](Application-Detailed.md) - детальная документация приложения
- [Система логирования](Logging-System.md) - система логирования

### Структура документации

#### Обзорные документы

- [Architecture.md](Architecture.md) - детальное описание архитектуры подсистем Rdk
- [Architecture-Diagrams.md](Architecture-Diagrams.md) - обзорные диаграммы архитектуры (компоненты, развертывание, пакеты, активность)

#### Детальная документация модулей

- [Engine-Detailed.md](Engine-Detailed.md) - детальная документация модуля Core/Engine (компоненты, свойства, контроллеры)
- [Application-Detailed.md](Application-Detailed.md) - детальная документация модуля Core/Application (RPC, проекты, сервер)
- [Graphics-Detailed.md](Graphics-Detailed.md) - детальная документация модуля Core/Graphics (графика, шрифты, отрисовка)
- [Serialize-Detailed.md](Serialize-Detailed.md) - детальная документация модуля Core/Serialize (XML, Binary сериализация)
- [System-Detailed.md](System-Detailed.md) - детальная документация модуля Core/System (мьютексы, события, загрузка библиотек)

#### Справочники

- [Math-Libraries-Reference.md](Math-Libraries-Reference.md) - справочник по математическим библиотекам (матрицы, векторы, фильтры Калмана)
- [Math-Detailed.md](Math-Detailed.md) - детальная документация модуля Core/Math (расширенная версия)
- [Utilities-Reference.md](Utilities-Reference.md) - справочник по утилитам (исключения, файлы, временные метки)
- [Utilities-Detailed.md](Utilities-Detailed.md) - детальная документация модуля Core/Utilities (расширенная версия)
- [API-Reference.md](API-Reference.md) - справочник по всем публичным классам, методам, типам и константам
- [Controllers-System.md](Controllers-System.md) - система контроллеров
- [Console-Application.md](Console-Application.md) - консольное приложение
- [Logging-System.md](Logging-System.md) - система логирования
- [Tests.md](Tests.md) - устройство юнит- и интеграционных тестов

#### Диаграммы

- [Diagrams/README.md](Diagrams/README.md) - описание диаграмм
- [Diagrams/Component-Lifecycle.md](Diagrams/Component-Lifecycle.md) - жизненный цикл компонента
- [Diagrams/Property-System.md](Diagrams/Property-System.md) - система свойств
- [Diagrams/RPC-Flow.md](Diagrams/RPC-Flow.md) - поток обработки RPC команд

#### Руководства

- [Guides/Creating-Components.md](Guides/Creating-Components.md) - создание компонентов
- [Guides/Creating-Properties.md](Guides/Creating-Properties.md) - создание свойств
- [Guides/Creating-Controllers.md](Guides/Creating-Controllers.md) - создание контроллеров
- [Guides/Serialization-Guide.md](Guides/Serialization-Guide.md) - работа с сериализацией
- [Guides/RPC-Integration.md](Guides/RPC-Integration.md) - интеграция RPC
- [Guides/Threading-Guide.md](Guides/Threading-Guide.md) - многопоточность
- [Guides/Error-Handling.md](Guides/Error-Handling.md) - обработка ошибок

### Быстрый старт

- [Getting-Started.md](Getting-Started.md) - быстрый старт для разных ролей (разработчики компонентов, библиотек, интеграторы)

### Часто задаваемые вопросы

- [FAQ.md](FAQ.md) - часто задаваемые вопросы по Rdk Core

### Примеры

- [Examples/](Examples/) - практические примеры использования Rdk Core

### Связь с корневой документацией

Для обзорной информации см.:
- [Docs/Rdk-Core/Overview.md](../../Docs/Rdk-Core/Overview.md) - обзор Rdk Core
- [Docs/Rdk-Core/Application-Architecture.md](../../Docs/Rdk-Core/Application-Architecture.md) - архитектура приложения
- [Docs/Rdk-Core/Engine-Architecture.md](../../Docs/Rdk-Core/Engine-Architecture.md) - архитектура движка
- [Docs/Rdk-Core/Graphics-Architecture.md](../../Docs/Rdk-Core/Graphics-Architecture.md) - архитектура графики
- [Docs/Rdk-Core/Serialize-Architecture.md](../../Docs/Rdk-Core/Serialize-Architecture.md) - архитектура сериализации
- [Docs/Rdk-Core/System-Platform-Abstraction.md](../../Docs/Rdk-Core/System-Platform-Abstraction.md) - системные абстракции
- [Docs/Troubleshooting/Troubleshooting-Guide.md](../../Docs/Troubleshooting/Troubleshooting-Guide.md) - руководство по устранению неполадок

### Старая документация

Старая документация (`.doc`, `.vsd` файлы) сохранена в `Rdk/Docs.old/`.

---

## EN

### Purpose

This directory contains detailed documentation of the Rdk core - the main module of the Nmsdk system.

### Quick Links

- [Architecture](Architecture.md) - detailed architecture description
- [API Reference](API-Reference.md) - complete API reference
- [Creating Components](Guides/Creating-Components.md) - guide for creating components
- [Creating Properties](Guides/Creating-Properties.md) - guide for creating properties
- [Engine](Engine-Detailed.md) - detailed engine documentation
- [Application](Application-Detailed.md) - detailed application documentation
- [Logging System](Logging-System.md) - logging system

### Documentation Structure

#### Overview Documents

- [Architecture.md](Architecture.md) - detailed description of Rdk subsystem architecture
- [Architecture-Diagrams.md](Architecture-Diagrams.md) - overview architecture diagrams (components, deployment, packages, activity)

#### Detailed Module Documentation

- [Engine-Detailed.md](Engine-Detailed.md) - detailed documentation of Core/Engine module (components, properties, controllers)
- [Application-Detailed.md](Application-Detailed.md) - detailed documentation of Core/Application module (RPC, projects, server)
- [Graphics-Detailed.md](Graphics-Detailed.md) - detailed documentation of Core/Graphics module (graphics, fonts, rendering)
- [Serialize-Detailed.md](Serialize-Detailed.md) - detailed documentation of Core/Serialize module (XML, Binary serialization)
- [System-Detailed.md](System-Detailed.md) - detailed documentation of Core/System module (mutexes, events, library loading)

#### References

- [Math-Libraries-Reference.md](Math-Libraries-Reference.md) - reference for mathematical libraries (matrices, vectors, Kalman filters)
- [Math-Detailed.md](Math-Detailed.md) - detailed documentation of Core/Math module (extended version)
- [Utilities-Reference.md](Utilities-Reference.md) - reference for utilities (exceptions, files, timestamps)
- [Utilities-Detailed.md](Utilities-Detailed.md) - detailed documentation of Core/Utilities module (extended version)
- [API-Reference.md](API-Reference.md) - reference for all public classes, methods, types, and constants
- [Controllers-System.md](Controllers-System.md) - controllers system
- [Console-Application.md](Console-Application.md) - console application
- [Logging-System.md](Logging-System.md) - logging system
- [Tests.md](Tests.md) - unit and integration test structure

#### Diagrams

- [Diagrams/README.md](Diagrams/README.md) - diagrams description
- [Diagrams/Component-Lifecycle.md](Diagrams/Component-Lifecycle.md) - component lifecycle
- [Diagrams/Property-System.md](Diagrams/Property-System.md) - property system
- [Diagrams/RPC-Flow.md](Diagrams/RPC-Flow.md) - RPC command processing flow

#### Guides

- [Guides/Creating-Components.md](Guides/Creating-Components.md) - creating components
- [Guides/Creating-Properties.md](Guides/Creating-Properties.md) - creating properties
- [Guides/Creating-Controllers.md](Guides/Creating-Controllers.md) - creating controllers
- [Guides/Serialization-Guide.md](Guides/Serialization-Guide.md) - working with serialization
- [Guides/RPC-Integration.md](Guides/RPC-Integration.md) - RPC integration
- [Guides/Threading-Guide.md](Guides/Threading-Guide.md) - multithreading
- [Guides/Error-Handling.md](Guides/Error-Handling.md) - error handling

### Quick Start

- [Getting-Started.md](Getting-Started.md) - quick start for different roles (component developers, library developers, integrators)

### Frequently Asked Questions

- [FAQ.md](FAQ.md) - frequently asked questions about Rdk Core

### Examples

- [Examples/](Examples/) - practical examples of using Rdk Core

### Link to Root Documentation

For overview information see:
- [Docs/Rdk-Core/Overview.md](../../Docs/Rdk-Core/Overview.md) - Rdk Core overview
- [Docs/Rdk-Core/Application-Architecture.md](../../Docs/Rdk-Core/Application-Architecture.md) - application architecture
- [Docs/Rdk-Core/Engine-Architecture.md](../../Docs/Rdk-Core/Engine-Architecture.md) - engine architecture
- [Docs/Rdk-Core/Graphics-Architecture.md](../../Docs/Rdk-Core/Graphics-Architecture.md) - graphics architecture
- [Docs/Rdk-Core/Serialize-Architecture.md](../../Docs/Rdk-Core/Serialize-Architecture.md) - serialization architecture
- [Docs/Rdk-Core/System-Platform-Abstraction.md](../../Docs/Rdk-Core/System-Platform-Abstraction.md) - system abstractions
- [Docs/Troubleshooting/Troubleshooting-Guide.md](../../Docs/Troubleshooting/Troubleshooting-Guide.md) - troubleshooting guide

### Old Documentation

Old documentation (`.doc`, `.vsd` files) is preserved in `Rdk/Docs.old/`.
