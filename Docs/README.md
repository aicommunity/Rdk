# Документация Rdk Core

## RU

### Назначение

Этот каталог содержит детальную документацию ядра Rdk - основного модуля системы Nmsdk.

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

### Связь с корневой документацией

Для обзорной информации см.:
- [Docs/Rdk-Core/Overview.md](../../Docs/Rdk-Core/Overview.md) - обзор Rdk Core
- [Docs/Rdk-Core/Application-Architecture.md](../../Docs/Rdk-Core/Application-Architecture.md) - архитектура приложения
- [Docs/Rdk-Core/Engine-Architecture.md](../../Docs/Rdk-Core/Engine-Architecture.md) - архитектура движка
- [Docs/Rdk-Core/Graphics-Architecture.md](../../Docs/Rdk-Core/Graphics-Architecture.md) - архитектура графики
- [Docs/Rdk-Core/Serialize-Architecture.md](../../Docs/Rdk-Core/Serialize-Architecture.md) - архитектура сериализации
- [Docs/Rdk-Core/System-Platform-Abstraction.md](../../Docs/Rdk-Core/System-Platform-Abstraction.md) - системные абстракции

### Старая документация

Старая документация (`.doc`, `.vsd` файлы) сохранена в `Rdk/Docs.old/`.

---

## EN

### Purpose

This directory contains detailed documentation of the Rdk core - the main module of the Nmsdk system.

### Documentation Structure

- [Architecture.md](Architecture.md) - detailed description of Rdk subsystem architecture
- [Tests.md](Tests.md) - unit and integration test structure

### Link to Root Documentation

For overview information see:
- [Docs/Rdk-Core/Overview.md](../../Docs/Rdk-Core/Overview.md) - Rdk Core overview
- [Docs/Rdk-Core/Application-Architecture.md](../../Docs/Rdk-Core/Application-Architecture.md) - application architecture
- [Docs/Rdk-Core/Engine-Architecture.md](../../Docs/Rdk-Core/Engine-Architecture.md) - engine architecture
- [Docs/Rdk-Core/Graphics-Architecture.md](../../Docs/Rdk-Core/Graphics-Architecture.md) - graphics architecture
- [Docs/Rdk-Core/Serialize-Architecture.md](../../Docs/Rdk-Core/Serialize-Architecture.md) - serialization architecture
- [Docs/Rdk-Core/System-Platform-Abstraction.md](../../Docs/Rdk-Core/System-Platform-Abstraction.md) - system abstractions

### Old Documentation

Old documentation (`.doc`, `.vsd` files) is preserved in `Rdk/Docs.old/`.
