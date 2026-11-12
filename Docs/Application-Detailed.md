# Детальная документация модуля Core/Application

## RU

### Обзор

Модуль `Core/Application` реализует уровень приложения Rdk Core, включая систему RPC (Remote Procedure Call), управление проектами, серверную часть, управление движком и интеграцию с GUI. Этот модуль обеспечивает высокоуровневый интерфейс для работы с системой.

### UML диаграмма классов RPC системы

```mermaid
classDiagram
    class UAppController {
        <<abstract>>
        #UEPtr~UApplication~ Application
        +GetApplication() UApplication*
        +SetApplication(UApplication) bool
    }
    
    class UApplication {
        #UEPtr~URpcDispatcher~ RpcDispatcher
        #UEPtr~UProject~ Project
        #UEPtr~UEngineControl~ EngineControl
        #UEPtr~UServerControl~ ServerControl
        #UEPtr~UProjectDeployer~ ProjectDeployer
        #UEPtr~UTestManager~ TestManager
        +OpenProject(string) bool
        +CloseProject() bool
        +SaveProject() bool
        +GetProject() UProject*
        +GetEngineControl() UEngineControl*
        +GetServerControl() UServerControl*
    }
    
    class URpcDispatcherQueues {
        #queue~URpcCommand~ CommandQueue
        #queue~URpcCommand~ ProcessedQueue
        +PushCommand(URpcCommand) bool
        +PopCommand() URpcCommand
        +PushToProcessedQueue(URpcCommand) void
    }
    
    class URpcDispatcher {
        #vector~UEPtr~URpcDecoder~~ Decoders
        #UEPtr~URpcDecoder~ CommonDecoder
        #UEPtr~URpcDecoder~ DecoderPrototype
        #UEPtr~UApplication~ Application
        #boost::mutex DispatchMutex
        #boost::thread DispatcherThread
        +IsCmdSupported(URpcCommand) bool
        +SetDecoderPrototype(URpcDecoder) void
        +SetCommonDecoder(URpcDecoder) bool
        +Dispatch() void
        +StopDispatch() void
        +SyncDispatchCommand(URpcCommand, timeout) bool
        #DispatchCommand(URpcCommand) void
    }
    
    class URpcDecoder {
        <<abstract>>
        #URpcDispatcher* Dispatcher
        #boost::mutex DispatchMutex
        #boost::thread DecoderThread
        +SetDispatcher(URpcDispatcher) void
        +New() URpcDecoder*
        +IsCmdSupported(URpcCommand) bool*
        +Process() void
        +ProcessCommand(URpcCommand) bool
        +AProcessCommand(URpcCommand) bool*
    }
    
    class URpcDecoderInternal {
        +AProcessCommand(URpcCommand) bool
    }
    
    class URpcDecoderCommon {
        +AProcessCommand(URpcCommand) bool
    }
    
    class URpcCommand {
        #string CommandName
        #int ChannelIndex
        +GetCommandName() string
        +GetChannelIndex() int
    }
    
    class URpcCommandInternal {
        +GetCommandName() string
    }
    
    class UServerTransport {
        #UEPtr~UApplication~ Application
        #map~string,UTransferReader~ PacketReaders
        +SetServerBinding(string, int) void
        +GetServerBindingPort() int
        +ServerIsActive() bool
        +ProcessIncomingData() void*
    }
    
    class UServerTransportTcp {
        +ProcessIncomingData() void
    }
    
    class UServerTransportHttp {
        +ProcessIncomingData() void
    }
    
    class UServerControl {
        #UEPtr~UApplication~ Application
        #UEPtr~UServerTransport~ ServerTransport
        #UEPtr~UServerTransport~ ServerTransportHttp
        #UEPtr~URpcDispatcher~ RpcDispatcher
        +StartServer() bool
        +StopServer() bool
        +IsServerActive() bool
    }
    
    UAppController <|-- UApplication
    UAppController <|-- UServerControl
    UAppController <|-- UServerTransport
    URpcDispatcherQueues <|-- URpcDispatcher
    URpcDispatcherQueues <|-- URpcDecoder
    URpcDecoder <|-- URpcDecoderInternal
    URpcDecoder <|-- URpcDecoderCommon
    URpcDispatcher "1" o-- "*" URpcDecoder
    UApplication "1" o-- "1" URpcDispatcher
    UApplication "1" o-- "1" UServerControl
    UServerControl "1" o-- "1" UServerTransport
    UServerTransport <|-- UServerTransportTcp
    UServerTransport <|-- UServerTransportHttp
```

### UML диаграмма классов управления проектами

```mermaid
classDiagram
    class UProject {
        #string ProjectName
        #string ProjectPath
        #TProjectConfig Config
        #vector~TProjectChannelConfig~ ChannelsConfig
        #vector~UEPtr~UEnvironment~~ Environments
        +GetProjectName() string
        +GetProjectPath() string
        +GetConfig() TProjectConfig&
        +GetChannelConfig(int) TProjectChannelConfig&
        +LoadProject(string) bool
        +SaveProject(string) bool
        +CreateChannel(int) bool
        +DeleteChannel(int) bool
        +GetEnvironment(int) UEnvironment*
    }
    
    class TProjectConfig {
        +string ProjectName
        +string ProjectDescription
        +string UserName
        +int UserId
        +int ProjectMode
        +int ProjectType
        +int MultiThreadingMode
        +int NumChannels
    }
    
    class TProjectChannelConfig {
        +int ModelMode
        +int PredefinedStructure
        +string ModelFileName
        +string ParametersFileName
        +string StatesFileName
        +string ClassName
        +int GlobalTimeStep
        +int CalculationMode
        +bool InitAfterLoad
        +bool ResetAfterLoad
    }
    
    class UProjectDeployer {
        #UEPtr~UApplication~ Application
        +DeployProject(UProject, string) bool
        +UploadResults(string) bool
        +GetDeploymentStatus() int
    }
    
    class UProjectDeployerQt {
        +DeployProject(UProject, string) bool
        +UploadResults(string) bool
    }
    
    class UAppCore~ApplicationT,EngineControlT,ProjectT,ServerControlT,TestManagerT,DispatcherT,DecoderT,DecoderCommonT,ServerTransportT,ProjectDeployerT~ {
        +DecoderT rpcDecoder
        +DispatcherT rpcDispatcher
        +ServerControlT serverControl
        +ServerTransportT serverTransport
        +DecoderCommonT rpcDecoderCommon
        +EngineControlT engineControl
        +ProjectT project
        +ApplicationT application
        +TestManagerT rdkTestManager
        +UProjectDeployerT projectDeployer
        +Init(string, string, string, string, int, char**) int
        +PostInit() int
    }
    
    UApplication "1" o-- "1" UProject
    UProject "1" o-- "*" TProjectChannelConfig
    UProject "1" o-- "1" TProjectConfig
    UProject "1" o-- "*" UEnvironment
    UApplication "1" o-- "1" UProjectDeployer
    UProjectDeployer <|-- UProjectDeployerQt
    UAppCore --> UApplication
```

### UML диаграмма классов управления движком

```mermaid
classDiagram
    class UEngineControl {
        #UEPtr~UApplication~ Application
        #vector~UEngineControlThread*~ EngineControlThreads
        #UEngineStateThread* EngineStateThread
        #vector~UEPtr~UBroadcasterInterface~~ BroadcastersList
        #UELockVar~int~ ThreadMode
        #UELockVar~int~ UpdateInterval
        #UELockVar~int~ GuiUpdateMode
        +GetThreadMode() int
        +SetThreadMode(int) void
        +GetUpdateInterval() int
        +SetUpdateInterval(int) void
        +StartCalculation(int) bool
        +StopCalculation(int) bool
        +ResetCalculation(int) bool
        +GetCalcState(int) UCalcState
        +UpdateInterface() void
    }
    
    class UEngineControlThread {
        #int ChannelIndex
        #UEPtr~UApplication~ Application
        #UEPtr~UEnvironment~ Environment
        #boost::thread Thread
        #bool ThreadRunning
        +Run() void
        +Stop() void
        +GetChannelIndex() int
    }
    
    class UEngineStateThread {
        #UEPtr~UApplication~ Application
        #boost::thread Thread
        #bool ThreadRunning
        +Run() void
        +Stop() void
        +CheckChannelsState() void
    }
    
    class UBroadcasterInterface {
        <<interface>>
        +Broadcast(string, data) void
    }
    
    class UChannelProfiler {
        #int ChannelIndex
        #double LastDuration
        #double AverageDuration
        +GetLastDuration() double
        +GetAverageDuration() double
        +ProfileCalculation() void
    }
    
    UAppController <|-- UEngineControl
    UApplication "1" o-- "1" UEngineControl
    UEngineControl "1" o-- "*" UEngineControlThread
    UEngineControl "1" o-- "1" UEngineStateThread
    UEngineControl "1" o-- "*" UBroadcasterInterface
    UEngineControl "1" o-- "*" UChannelProfiler
    UEngineControlThread --> UEnvironment
```

### UML диаграмма классов визуальных контроллеров

```mermaid
classDiagram
    class UIController {
        <<abstract>>
        #string ControllerName
        +GetControllerName() string
        +SetControllerName(string) void
    }
    
    class UIControllerStorage {
        #map~string,UIController*~ Controllers
        +AddController(string, UIController) bool
        +GetController(string) UIController*
        +RemoveController(string) bool
    }
    
    class UIVisualController {
        #UEPtr~UApplication~ Application
        #int UpdateInterval
        +GetApplication() UApplication*
        +SetApplication(UApplication) bool
        +GetUpdateInterval() int
        +SetUpdateInterval(int) void
        +UpdateInterface() void
        #AUpdateInterface() void*
    }
    
    class UIVisualControllerStorage {
        #map~string,UIVisualController*~ VisualControllers
        +AddVisualController(string, UIVisualController) bool
        +GetVisualController(string) UIVisualController*
        +UpdateAllInterfaces() void
    }
    
    class UAppController {
        #UEPtr~UApplication~ Application
        +GetApplication() UApplication*
        +SetApplication(UApplication) bool
    }
    
    UIController <|-- UIVisualController
    UIController <|-- UAppController
    UIVisualControllerStorage "1" o-- "*" UIVisualController
    UIControllerStorage "1" o-- "*" UIController
    UApplication "1" o-- "*" UAppController
```

### Диаграмма последовательности обработки RPC команды (расширенная)

```mermaid
sequenceDiagram
    participant Client as Клиент
    participant Transport as UServerTransport
    participant Dispatcher as URpcDispatcher
    participant Decoder as URpcDecoder
    participant App as UApplication
    participant Engine as UEngine
    participant Project as UProject
    
    Client->>Transport: Отправка команды (TCP/HTTP)
    Transport->>Transport: ProcessIncomingData()
    Transport->>Transport: ParseCommand()
    Transport->>Dispatcher: PushCommand(command)
    Dispatcher->>Dispatcher: DispatchCommand()
    Dispatcher->>Decoder: IsCmdSupported(command)
    
    alt Команда поддерживается
        Decoder->>Decoder: AProcessCommand(command)
        Decoder->>App: ExecuteCommand(command)
        
        alt Внутренняя команда
            App->>App: ProcessInternalCommand()
            App->>Project: Project operations
            Project->>Engine: Engine operations
        else Общая команда
            App->>App: ProcessCommonCommand()
            App->>Engine: Engine operations
        end
        
        Engine-->>App: Результат
        App-->>Decoder: Результат выполнения
        Decoder->>Dispatcher: PushToProcessedQueue(result)
        Dispatcher->>Transport: SendResponse(result)
        Transport-->>Client: Ответ клиенту
    else Команда не поддерживается
        Decoder-->>Dispatcher: false
        Dispatcher->>Transport: SendError("Command not supported")
        Transport-->>Client: Ошибка
    end
```

### Диаграмма последовательности инициализации приложения

```mermaid
sequenceDiagram
    participant Main as main()
    participant AppCore as UAppCore
    participant App as UApplication
    participant Storage as UStorage
    participant Engine as UEngine
    participant Server as UServerControl
    participant Dispatcher as URpcDispatcher
    
    Main->>AppCore: Init(application_file, ini_file, log_dir, user_name, argc, argv)
    AppCore->>AppCore: Initialize components
    AppCore->>App: SetApplicationFileName()
    AppCore->>App: SetWorkDirectory()
    AppCore->>App: SetConfigsMainPath()
    AppCore->>App: SetLibrariesPath()
    AppCore->>Storage: LoadLibraries()
    Storage->>Storage: Build()
    AppCore->>Dispatcher: SetApplication(app)
    AppCore->>Server: SetApplication(app)
    AppCore->>Server: SetRpcDispatcher(dispatcher)
    AppCore->>App: PostInit()
    App->>App: Initialize logging
    App->>App: Load configuration
    App->>App: Initialize GUI controllers
    App-->>AppCore: Initialization complete
    AppCore-->>Main: Success
```

### Диаграмма последовательности открытия проекта

```mermaid
sequenceDiagram
    participant App as UApplication
    participant Project as UProject
    participant Storage as UStorage
    participant Engine as UEngine
    participant Env as UEnvironment
    
    App->>Project: LoadProject(project_path)
    Project->>Project: LoadConfig()
    Project->>Project: LoadChannelsConfig()
    
    loop Для каждого канала
        Project->>Storage: CreateComponent(channel_class_name)
        Storage-->>Project: UEPtr~UContainer~
        Project->>Env: CreateEnvironment()
        Project->>Env: SetModel(component)
        Project->>Env: LoadParameters(parameters_file)
        Project->>Env: LoadStates(states_file)
        Project->>Env: Build()
        Project->>Project: AddEnvironment(env)
    end
    
    Project->>Project: ProjectOpenFlag = true
    Project-->>App: true
    App->>App: UpdateInterface()
```

### Описание основных классов

#### UApplication

Главный класс приложения. Управляет всеми подсистемами: проектами, движком, сервером, RPC диспетчером.

**Основные свойства:**
- `RpcDispatcher` - диспетчер RPC команд
- `Project` - активный проект
- `EngineControl` - контроллер движка
- `ServerControl` - контроллер сервера
- `ProjectDeployer` - деплоер проектов
- `TestManager` - менеджер тестов

**Основные методы:**
- `OpenProject(path)` - открытие проекта
- `CloseProject()` - закрытие проекта
- `SaveProject()` - сохранение проекта
- `GetProject()` - получение активного проекта
- `GetEngineControl()` - получение контроллера движка
- `GetServerControl()` - получение контроллера сервера

#### URpcDispatcher

Диспетчер RPC команд. Управляет очередями команд и распределяет их по декодерам.

**Основные свойства:**
- `Decoders` - массив декодеров для каналов
- `CommonDecoder` - общий декодер сервера
- `DecoderPrototype` - прототип для создания новых декодеров
- `Application` - указатель на приложение

**Основные методы:**
- `IsCmdSupported(command)` - проверка поддержки команды
- `SetDecoderPrototype(decoder)` - установка прототипа декодера
- `Dispatch()` - диспетчеризация команд из очереди
- `StopDispatch()` - остановка диспетчеризации
- `SyncDispatchCommand(command, timeout)` - синхронная диспетчеризация команды

#### URpcDecoder

Абстрактный класс декодера RPC команд. Реализует декодирование и выполнение команд.

**Основные методы:**
- `New()` - создание копии декодера
- `IsCmdSupported(command)` - проверка поддержки команды
- `Process()` - обработка команд из очереди
- `ProcessCommand(command)` - обработка одной команды
- `AProcessCommand(command)` - абстрактный метод обработки команды (переопределяется)

#### URpcDecoderInternal

Декодер внутренних команд приложения. Обрабатывает команды управления проектами, движком и т.д.

#### URpcDecoderCommon

Декодер общих команд сервера. Обрабатывает команды, доступные всем клиентам.

#### URpcCommand

Базовый класс RPC команды.

**Основные свойства:**
- `CommandName` - имя команды
- `ChannelIndex` - индекс канала

#### UServerTransport

Абстрактный класс транспорта сервера. Определяет интерфейс для приема и отправки RPC команд.

**Основные методы:**
- `SetServerBinding(address, port)` - установка адреса и порта
- `GetServerBindingPort()` - получение порта
- `ServerIsActive()` - проверка активности сервера
- `ProcessIncomingData()` - обработка входящих данных

#### UServerTransportTcp

Реализация транспорта через TCP.

#### UServerTransportHttp

Реализация транспорта через HTTP.

#### UServerControl

Контроллер серверной части. Управляет транспортом и RPC диспетчером.

**Основные методы:**
- `StartServer()` - запуск сервера
- `StopServer()` - остановка сервера
- `IsServerActive()` - проверка активности сервера

#### UProject

Класс проекта. Управляет конфигурацией проекта и каналами.

**Основные свойства:**
- `Config` - конфигурация проекта
- `ChannelsConfig` - конфигурации каналов
- `Environments` - окружения выполнения для каналов

**Основные методы:**
- `LoadProject(path)` - загрузка проекта
- `SaveProject(path)` - сохранение проекта
- `CreateChannel(index)` - создание канала
- `DeleteChannel(index)` - удаление канала
- `GetEnvironment(index)` - получение окружения канала

#### UProjectDeployer

Класс для развертывания проектов. Управляет деплоем конфигураций и загрузкой результатов.

**Основные методы:**
- `DeployProject(project, target)` - развертывание проекта
- `UploadResults(path)` - загрузка результатов
- `GetDeploymentStatus()` - получение статуса развертывания

#### UEngineControl

Контроллер движка. Управляет выполнением расчетов в однопоточном и многопоточном режимах.

**Основные свойства:**
- `ThreadMode` - режим работы (однопоточный/многопоточный)
- `UpdateInterval` - интервал обновления интерфейса
- `GuiUpdateMode` - режим обновления GUI
- `EngineControlThreads` - потоки управления каналами
- `EngineStateThread` - поток мониторинга состояния

**Основные методы:**
- `StartCalculation(channel)` - запуск расчета канала
- `StopCalculation(channel)` - остановка расчета канала
- `ResetCalculation(channel)` - сброс расчета канала
- `GetCalcState(channel)` - получение состояния расчета
- `UpdateInterface()` - обновление интерфейса

#### UEngineControlThread

Поток управления расчетом одного канала.

**Основные методы:**
- `Run()` - выполнение потока
- `Stop()` - остановка потока
- `GetChannelIndex()` - получение индекса канала

#### UEngineStateThread

Поток мониторинга состояния расчета всех каналов.

**Основные методы:**
- `Run()` - выполнение потока
- `Stop()` - остановка потока
- `CheckChannelsState()` - проверка состояния каналов

#### UAppCore

Шаблонный класс для инициализации приложения. Создает и настраивает все компоненты приложения.

**Основные свойства:**
- `application` - экземпляр приложения
- `engineControl` - контроллер движка
- `project` - проект
- `serverControl` - контроллер сервера
- `rpcDispatcher` - RPC диспетчер
- `rpcDecoder` - RPC декодер

**Основные методы:**
- `Init(...)` - инициализация приложения
- `PostInit()` - пост-инициализация

#### UIVisualController

Базовый класс визуальных контроллеров для связи с GUI.

**Основные методы:**
- `UpdateInterface()` - обновление интерфейса
- `AUpdateInterface()` - абстрактный метод обновления (переопределяется)

### Примеры использования

#### Инициализация приложения

```cpp
#include "Rdk/Core/Application/UAppCore.h"
#include "Rdk/Core/Application/UApplication.h"

// Создание ядра приложения
RDK::UAppCore<
    RDK::UApplication,
    RDK::UEngineControl,
    RDK::UProject,
    RDK::UServerControl,
    RDK::UTestManager,
    RDK::URpcDispatcher,
    RDK::URpcDecoderInternal,
    RDK::URpcDecoderCommon,
    RDK::UServerTransportTcp,
    RDK::UProjectDeployer
> app_core;

// Инициализация
int result = app_core.Init(
    "application.exe",
    "config.ini",
    "/path/to/logs",
    "username",
    argc,
    argv
);

if (result == 0) {
    // Пост-инициализация
    app_core.PostInit();
    
    // Получение приложения
    RDK::UApplication* app = &app_core.application;
}
```

#### Открытие проекта

```cpp
// Открытие проекта
if (app->OpenProject("/path/to/project.xml")) {
    RDK::UProject* project = app->GetProject();
    
    // Получение конфигурации
    const RDK::TProjectConfig& config = project->GetConfig();
    std::cout << "Project: " << config.ProjectName << std::endl;
    std::cout << "Channels: " << config.NumChannels << std::endl;
}
```

#### Управление расчетом

```cpp
RDK::UEngineControl* engine_control = app->GetEngineControl();

// Запуск расчета канала 0
engine_control->StartCalculation(0);

// Проверка состояния
RDK::UEngineControl::UCalcState state = engine_control->GetCalcState(0);
if (state == RDK::UEngineControl::csRunning) {
    std::cout << "Calculation is running" << std::endl;
}

// Остановка расчета
engine_control->StopCalculation(0);
```

#### Работа с RPC

```cpp
// Получение RPC диспетчера
RDK::URpcDispatcher* dispatcher = app->GetRpcDispatcher();

// Создание команды
RDK::UEPtr<RDK::URpcCommand> command = /* ... */;

// Синхронная отправка команды
bool success = dispatcher->SyncDispatchCommand(command, 5000); // timeout 5 сек
```

### См. также

- [Architecture.md](Architecture.md) - общая архитектура
- [Engine-Detailed.md](Engine-Detailed.md) - детальная документация движка
- [Diagrams/RPC-Flow.md](Diagrams/RPC-Flow.md) - поток обработки RPC команд
- [Console-Application.md](Console-Application.md) - консольное приложение

---

## EN

### Overview

The `Core/Application` module implements the application layer of Rdk Core, including the RPC (Remote Procedure Call) system, project management, server components, engine control, and GUI integration. This module provides a high-level interface for working with the system.

### RPC System

The RPC system allows remote control of the application:
- `URpcDispatcher` - command dispatcher
- `URpcDecoder` - command decoder
- `UServerTransport` - transport layer (TCP/HTTP)
- `UServerControl` - server controller

### Project Management

Project management provides:
- `UProject` - project class
- `UProjectDeployer` - project deployment
- Multi-channel support
- Configuration management

### Engine Control

Engine control manages calculation execution:
- `UEngineControl` - engine controller
- `UEngineControlThread` - calculation thread
- `UEngineStateThread` - state monitoring thread

### See Also

- [Architecture.md](Architecture.md) - general architecture
- [Engine-Detailed.md](Engine-Detailed.md) - engine detailed documentation
- [Diagrams/RPC-Flow.md](Diagrams/RPC-Flow.md) - RPC command processing flow
- [Console-Application.md](Console-Application.md) - console application

```mermaid
classDiagram
    class UAppController {
        <<abstract>>
        #UEPtr~UApplication~ Application
        +GetApplication() UApplication*
        +SetApplication(UApplication) bool
    }
    
    class UApplication {
        #UEPtr~URpcDispatcher~ RpcDispatcher
        #UEPtr~UProject~ Project
        #UEPtr~UEngineControl~ EngineControl
        #UEPtr~UServerControl~ ServerControl
        #UEPtr~UProjectDeployer~ ProjectDeployer
        #UEPtr~UTestManager~ TestManager
        +OpenProject(string) bool
        +CloseProject() bool
        +SaveProject() bool
        +GetProject() UProject*
        +GetEngineControl() UEngineControl*
        +GetServerControl() UServerControl*
    }
    
    class URpcDispatcherQueues {
        #queue~URpcCommand~ CommandQueue
        #queue~URpcCommand~ ProcessedQueue
        +PushCommand(URpcCommand) bool
        +PopCommand() URpcCommand
        +PushToProcessedQueue(URpcCommand) void
    }
    
    class URpcDispatcher {
        #vector~UEPtr~URpcDecoder~~ Decoders
        #UEPtr~URpcDecoder~ CommonDecoder
        #UEPtr~URpcDecoder~ DecoderPrototype
        #UEPtr~UApplication~ Application
        #boost::mutex DispatchMutex
        #boost::thread DispatcherThread
        +IsCmdSupported(URpcCommand) bool
        +SetDecoderPrototype(URpcDecoder) void
        +SetCommonDecoder(URpcDecoder) bool
        +Dispatch() void
        +StopDispatch() void
        +SyncDispatchCommand(URpcCommand, timeout) bool
        #DispatchCommand(URpcCommand) void
    }
    
    class URpcDecoder {
        <<abstract>>
        #URpcDispatcher* Dispatcher
        #boost::mutex DispatchMutex
        #boost::thread DecoderThread
        +SetDispatcher(URpcDispatcher) void
        +New() URpcDecoder*
        +IsCmdSupported(URpcCommand) bool*
        +Process() void
        +ProcessCommand(URpcCommand) bool
        +AProcessCommand(URpcCommand) bool*
    }
    
    class URpcDecoderInternal {
        +AProcessCommand(URpcCommand) bool
    }
    
    class URpcDecoderCommon {
        +AProcessCommand(URpcCommand) bool
    }
    
    class URpcCommand {
        #string CommandName
        #int ChannelIndex
        +GetCommandName() string
        +GetChannelIndex() int
    }
    
    class URpcCommandInternal {
        +GetCommandName() string
    }
    
    class UServerTransport {
        #UEPtr~UApplication~ Application
        #map~string,UTransferReader~ PacketReaders
        +SetServerBinding(string, int) void
        +GetServerBindingPort() int
        +ServerIsActive() bool
        +ProcessIncomingData() void*
    }
    
    class UServerTransportTcp {
        +ProcessIncomingData() void
    }
    
    class UServerTransportHttp {
        +ProcessIncomingData() void
    }
    
    class UServerControl {
        #UEPtr~UApplication~ Application
        #UEPtr~UServerTransport~ ServerTransport
        #UEPtr~UServerTransport~ ServerTransportHttp
        #UEPtr~URpcDispatcher~ RpcDispatcher
        +StartServer() bool
        +StopServer() bool
        +IsServerActive() bool
    }
    
    UAppController <|-- UApplication
    UAppController <|-- UServerControl
    UAppController <|-- UServerTransport
    URpcDispatcherQueues <|-- URpcDispatcher
    URpcDispatcherQueues <|-- URpcDecoder
    URpcDecoder <|-- URpcDecoderInternal
    URpcDecoder <|-- URpcDecoderCommon
    URpcDispatcher "1" o-- "*" URpcDecoder
    UApplication "1" o-- "1" URpcDispatcher
    UApplication "1" o-- "1" UServerControl
    UServerControl "1" o-- "1" UServerTransport
    UServerTransport <|-- UServerTransportTcp
    UServerTransport <|-- UServerTransportHttp
```

```mermaid
classDiagram
    class UProject {
        #string ProjectName
        #string ProjectPath
        #TProjectConfig Config
        #vector~TProjectChannelConfig~ ChannelsConfig
        #vector~UEPtr~UEnvironment~~ Environments
        +GetProjectName() string
        +GetProjectPath() string
        +GetConfig() TProjectConfig&
        +GetChannelConfig(int) TProjectChannelConfig&
        +LoadProject(string) bool
        +SaveProject(string) bool
        +CreateChannel(int) bool
        +DeleteChannel(int) bool
        +GetEnvironment(int) UEnvironment*
    }
    
    class TProjectConfig {
        +string ProjectName
        +string ProjectDescription
        +string UserName
        +int UserId
        +int ProjectMode
        +int ProjectType
        +int MultiThreadingMode
        +int NumChannels
    }
    
    class TProjectChannelConfig {
        +int ModelMode
        +int PredefinedStructure
        +string ModelFileName
        +string ParametersFileName
        +string StatesFileName
        +string ClassName
        +int GlobalTimeStep
        +int CalculationMode
        +bool InitAfterLoad
        +bool ResetAfterLoad
    }
    
    class UProjectDeployer {
        #UEPtr~UApplication~ Application
        +DeployProject(UProject, string) bool
        +UploadResults(string) bool
        +GetDeploymentStatus() int
    }
    
    class UProjectDeployerQt {
        +DeployProject(UProject, string) bool
        +UploadResults(string) bool
    }
    
    class UAppCore~ApplicationT,EngineControlT,ProjectT,ServerControlT,TestManagerT,DispatcherT,DecoderT,DecoderCommonT,ServerTransportT,ProjectDeployerT~ {
        +DecoderT rpcDecoder
        +DispatcherT rpcDispatcher
        +ServerControlT serverControl
        +ServerTransportT serverTransport
        +DecoderCommonT rpcDecoderCommon
        +EngineControlT engineControl
        +ProjectT project
        +ApplicationT application
        +TestManagerT rdkTestManager
        +UProjectDeployerT projectDeployer
        +Init(string, string, string, string, int, char**) int
        +PostInit() int
    }
    
    UApplication "1" o-- "1" UProject
    UProject "1" o-- "*" TProjectChannelConfig
    UProject "1" o-- "1" TProjectConfig
    UProject "1" o-- "*" UEnvironment
    UApplication "1" o-- "1" UProjectDeployer
    UProjectDeployer <|-- UProjectDeployerQt
    UAppCore --> UApplication
```

```mermaid
classDiagram
    class UEngineControl {
        #UEPtr~UApplication~ Application
        #vector~UEngineControlThread*~ EngineControlThreads
        #UEngineStateThread* EngineStateThread
        #vector~UEPtr~UBroadcasterInterface~~ BroadcastersList
        #UELockVar~int~ ThreadMode
        #UELockVar~int~ UpdateInterval
        #UELockVar~int~ GuiUpdateMode
        +GetThreadMode() int
        +SetThreadMode(int) void
        +GetUpdateInterval() int
        +SetUpdateInterval(int) void
        +StartCalculation(int) bool
        +StopCalculation(int) bool
        +ResetCalculation(int) bool
        +GetCalcState(int) UCalcState
        +UpdateInterface() void
    }
    
    class UEngineControlThread {
        #int ChannelIndex
        #UEPtr~UApplication~ Application
        #UEPtr~UEnvironment~ Environment
        #boost::thread Thread
        #bool ThreadRunning
        +Run() void
        +Stop() void
        +GetChannelIndex() int
    }
    
    class UEngineStateThread {
        #UEPtr~UApplication~ Application
        #boost::thread Thread
        #bool ThreadRunning
        +Run() void
        +Stop() void
        +CheckChannelsState() void
    }
    
    class UBroadcasterInterface {
        <<interface>>
        +Broadcast(string, data) void
    }
    
    class UChannelProfiler {
        #int ChannelIndex
        #double LastDuration
        #double AverageDuration
        +GetLastDuration() double
        +GetAverageDuration() double
        +ProfileCalculation() void
    }
    
    UAppController <|-- UEngineControl
    UApplication "1" o-- "1" UEngineControl
    UEngineControl "1" o-- "*" UEngineControlThread
    UEngineControl "1" o-- "1" UEngineStateThread
    UEngineControl "1" o-- "*" UBroadcasterInterface
    UEngineControl "1" o-- "*" UChannelProfiler
    UEngineControlThread --> UEnvironment
```

```mermaid
classDiagram
    class UIController {
        <<abstract>>
        #string ControllerName
        +GetControllerName() string
        +SetControllerName(string) void
    }
    
    class UIControllerStorage {
        #map~string,UIController*~ Controllers
        +AddController(string, UIController) bool
        +GetController(string) UIController*
        +RemoveController(string) bool
    }
    
    class UIVisualController {
        #UEPtr~UApplication~ Application
        #int UpdateInterval
        +GetApplication() UApplication*
        +SetApplication(UApplication) bool
        +GetUpdateInterval() int
        +SetUpdateInterval(int) void
        +UpdateInterface() void
        #AUpdateInterface() void*
    }
    
    class UIVisualControllerStorage {
        #map~string,UIVisualController*~ VisualControllers
        +AddVisualController(string, UIVisualController) bool
        +GetVisualController(string) UIVisualController*
        +UpdateAllInterfaces() void
    }
    
    class UAppController {
        #UEPtr~UApplication~ Application
        +GetApplication() UApplication*
        +SetApplication(UApplication) bool
    }
    
    UIController <|-- UIVisualController
    UIController <|-- UAppController
    UIVisualControllerStorage "1" o-- "*" UIVisualController
    UIControllerStorage "1" o-- "*" UIController
    UApplication "1" o-- "*" UAppController
```

```mermaid
sequenceDiagram
    participant Client as Клиент
    participant Transport as UServerTransport
    participant Dispatcher as URpcDispatcher
    participant Decoder as URpcDecoder
    participant App as UApplication
    participant Engine as UEngine
    participant Project as UProject
    
    Client->>Transport: Отправка команды (TCP/HTTP)
    Transport->>Transport: ProcessIncomingData()
    Transport->>Transport: ParseCommand()
    Transport->>Dispatcher: PushCommand(command)
    Dispatcher->>Dispatcher: DispatchCommand()
    Dispatcher->>Decoder: IsCmdSupported(command)
    
    alt Команда поддерживается
        Decoder->>Decoder: AProcessCommand(command)
        Decoder->>App: ExecuteCommand(command)
        
        alt Внутренняя команда
            App->>App: ProcessInternalCommand()
            App->>Project: Project operations
            Project->>Engine: Engine operations
        else Общая команда
            App->>App: ProcessCommonCommand()
            App->>Engine: Engine operations
        end
        
        Engine-->>App: Результат
        App-->>Decoder: Результат выполнения
        Decoder->>Dispatcher: PushToProcessedQueue(result)
        Dispatcher->>Transport: SendResponse(result)
        Transport-->>Client: Ответ клиенту
    else Команда не поддерживается
        Decoder-->>Dispatcher: false
        Dispatcher->>Transport: SendError("Command not supported")
        Transport-->>Client: Ошибка
    end
```

```mermaid
sequenceDiagram
    participant Main as main()
    participant AppCore as UAppCore
    participant App as UApplication
    participant Storage as UStorage
    participant Engine as UEngine
    participant Server as UServerControl
    participant Dispatcher as URpcDispatcher
    
    Main->>AppCore: Init(application_file, ini_file, log_dir, user_name, argc, argv)
    AppCore->>AppCore: Initialize components
    AppCore->>App: SetApplicationFileName()
    AppCore->>App: SetWorkDirectory()
    AppCore->>App: SetConfigsMainPath()
    AppCore->>App: SetLibrariesPath()
    AppCore->>Storage: LoadLibraries()
    Storage->>Storage: Build()
    AppCore->>Dispatcher: SetApplication(app)
    AppCore->>Server: SetApplication(app)
    AppCore->>Server: SetRpcDispatcher(dispatcher)
    AppCore->>App: PostInit()
    App->>App: Initialize logging
    App->>App: Load configuration
    App->>App: Initialize GUI controllers
    App-->>AppCore: Initialization complete
    AppCore-->>Main: Success
```

```mermaid
sequenceDiagram
    participant App as UApplication
    participant Project as UProject
    participant Storage as UStorage
    participant Engine as UEngine
    participant Env as UEnvironment
    
    App->>Project: LoadProject(project_path)
    Project->>Project: LoadConfig()
    Project->>Project: LoadChannelsConfig()
    
    loop Для каждого канала
        Project->>Storage: CreateComponent(channel_class_name)
        Storage-->>Project: UEPtr~UContainer~
        Project->>Env: CreateEnvironment()
        Project->>Env: SetModel(component)
        Project->>Env: LoadParameters(parameters_file)
        Project->>Env: LoadStates(states_file)
        Project->>Env: Build()
        Project->>Project: AddEnvironment(env)
    end
    
    Project->>Project: ProjectOpenFlag = true
    Project-->>App: true
    App->>App: UpdateInterface()
```
