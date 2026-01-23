# Обзорные диаграммы архитектуры Rdk Core

## RU

### Обзор

Этот документ содержит обзорные диаграммы архитектуры Rdk Core на высоком уровне: диаграммы компонентов, развертывания, пакетов и активности.

### Диаграмма компонентов (Component Diagram)

Диаграмма показывает основные модули Rdk Core и их взаимосвязи.

```mermaid
flowchart TB
    subgraph "Rdk Core"
        subgraph "Application Layer"
            App[UApplication]
            RPC[URpcDispatcher]
            Server[UServerControl]
            Project[UProject]
        end
        
        subgraph "Engine Layer"
            Engine[UEngine]
            Env[UEnvironment]
            Storage[UStorage]
            Component[UComponent]
            Container[UContainer]
        end
        
        subgraph "Graphics Layer"
            Graphics[UGraphics]
            DrawEngine[UDrawEngine]
            Bitmap[UBitmap]
        end
        
        subgraph "Serialize Layer"
            SerStorage[USerStorage]
            XMLSer[UXMLStdSerialize]
            BinSer[UBinaryStdSerialize]
        end
        
        subgraph "System Layer"
            Mutex[UGenericMutex]
            DllLoader[UDllLoader]
            Event[UGenericEvent]
        end
        
        subgraph "Math Layer"
            Matrix[MMatrix]
            Vector[MVector]
            Kalman[MKalmanFilter]
        end
        
        subgraph "Utilities Layer"
            Exception[UException]
            IniFile[UIniFile]
            TimeStamp[UTimeStamp]
        end
    end
    
    App --> Engine
    App --> RPC
    App --> Server
    App --> Project
    
    Engine --> Env
    Engine --> Storage
    Env --> Component
    Component --> Container
    
    Graphics --> DrawEngine
    DrawEngine --> Bitmap
    
    SerStorage --> XMLSer
    SerStorage --> BinSer
    
    Component --> Graphics
    Component --> SerStorage
    Component --> Mutex
    
    Engine --> Matrix
    Engine --> Vector
    Component --> Exception
    Component --> IniFile
```

### Диаграмма развертывания (Deployment Diagram)

Диаграмма показывает развертывание приложения с Rdk Core и связи с внешними библиотеками.

```mermaid
flowchart TB
    subgraph "Application Server"
        subgraph "Rdk Core Application"
            AppProcess[Application Process]
            subgraph "Rdk Core Libraries"
                CoreLib[Rdk Core Library]
                EngineLib[Engine Module]
                AppLib[Application Module]
                GraphicsLib[Graphics Module]
                SerializeLib[Serialize Module]
                SystemLib[System Module]
                MathLib[Math Module]
                UtilsLib[Utilities Module]
            end
        end
        
        subgraph "Component Libraries"
            BasicLib[Rdk-BasicLib]
            PulseLib[Nmsdk-PulseLib]
            HardwareLib[Rdk-HardwareLib]
            CvLib[Rdk-CvBasicLib]
        end
    end
    
    subgraph "External Dependencies"
        QtLib[Qt5 Core]
        BoostLib[Boost Thread]
        GlogLib[Google Glog]
        OpenCVLib[OpenCV]
    end
    
    subgraph "Storage"
        ConfigFiles[Configuration Files]
        ProjectFiles[Project Files]
        LogFiles[Log Files]
    end
    
    subgraph "Network"
        TCPClient[TCP Clients]
        HTTPClient[HTTP Clients]
    end
    
    AppProcess --> CoreLib
    CoreLib --> EngineLib
    CoreLib --> AppLib
    CoreLib --> GraphicsLib
    CoreLib --> SerializeLib
    CoreLib --> SystemLib
    CoreLib --> MathLib
    CoreLib --> UtilsLib
    
    AppProcess --> BasicLib
    AppProcess --> PulseLib
    AppProcess --> HardwareLib
    AppProcess --> CvLib
    
    CoreLib --> QtLib
    CoreLib --> BoostLib
    CoreLib --> GlogLib
    CvLib --> OpenCVLib
    
    AppProcess --> ConfigFiles
    AppProcess --> ProjectFiles
    AppProcess --> LogFiles
    
    AppProcess --> TCPClient
    AppProcess --> HTTPClient
```

### Диаграмма пакетов (Package Diagram)

Диаграмма показывает структуру пакетов всех модулей Rdk Core.

```mermaid
flowchart TB
    subgraph "Rdk.Core"
        subgraph "Application"
            AppPkg[Application Package]
            RpcPkg[RPC Package]
            ServerPkg[Server Package]
            ProjectPkg[Project Package]
        end
        
        subgraph "Engine"
            EnginePkg[Engine Package]
            ComponentPkg[Component Package]
            StoragePkg[Storage Package]
            ControllerPkg[Controller Package]
            PropertyPkg[Property Package]
        end
        
        subgraph "Graphics"
            GraphicsPkg[Graphics Package]
            DrawPkg[Draw Engine Package]
            BitmapPkg[Bitmap Package]
            FontPkg[Font Package]
        end
        
        subgraph "Serialize"
            SerializePkg[Serialize Package]
            XMLPkg[XML Serialize Package]
            BinaryPkg[Binary Serialize Package]
        end
        
        subgraph "System"
            SystemPkg[System Package]
            MutexPkg[Mutex Package]
            DllPkg[DLL Loader Package]
            PlatformPkg[Platform Packages]
        end
        
        subgraph "Math"
            MathPkg[Math Package]
            MatrixPkg[Matrix Package]
            VectorPkg[Vector Package]
            FilterPkg[Filter Package]
        end
        
        subgraph "Utilities"
            UtilsPkg[Utilities Package]
            ExceptionPkg[Exception Package]
            FilePkg[File I/O Package]
        end
    end
    
    AppPkg --> EnginePkg
    AppPkg --> RpcPkg
    AppPkg --> ServerPkg
    AppPkg --> ProjectPkg
    
    EnginePkg --> ComponentPkg
    EnginePkg --> StoragePkg
    EnginePkg --> ControllerPkg
    EnginePkg --> PropertyPkg
    
    ComponentPkg --> GraphicsPkg
    ComponentPkg --> SerializePkg
    ComponentPkg --> SystemPkg
    ComponentPkg --> MathPkg
    ComponentPkg --> UtilsPkg
    
    GraphicsPkg --> DrawPkg
    GraphicsPkg --> BitmapPkg
    GraphicsPkg --> FontPkg
    
    SerializePkg --> XMLPkg
    SerializePkg --> BinaryPkg
    
    SystemPkg --> MutexPkg
    SystemPkg --> DllPkg
    SystemPkg --> PlatformPkg
    
    MathPkg --> MatrixPkg
    MathPkg --> VectorPkg
    MathPkg --> FilterPkg
    
    UtilsPkg --> ExceptionPkg
    UtilsPkg --> FilePkg
```

### Диаграмма активности (Activity Diagram) выполнения приложения

Диаграмма показывает жизненный цикл приложения от инициализации до завершения работы.

```mermaid
stateDiagram-v2
    [*] --> Initialization: Запуск приложения
    
    Initialization --> LoadingLibraries: Загрузка библиотек
    LoadingLibraries --> BuildingStorage: Сборка хранилища
    BuildingStorage --> InitializingLogging: Инициализация логирования
    InitializingLogging --> CreatingComponents: Создание компонентов
    CreatingComponents --> Ready: Готово к работе
    
    Ready --> OpeningProject: Открытие проекта
    OpeningProject --> LoadingProject: Загрузка проекта
    LoadingProject --> CreatingChannels: Создание каналов
    CreatingChannels --> BuildingModels: Сборка моделей
    BuildingModels --> ProjectReady: Проект готов
    
    ProjectReady --> StartingCalculation: Запуск расчета
    StartingCalculation --> Running: Расчет выполняется
    
    Running --> UpdatingInterface: Обновление интерфейса
    UpdatingInterface --> ProcessingRPC: Обработка RPC команд
    ProcessingRPC --> Running: Продолжение расчета
    
    Running --> StoppingCalculation: Остановка расчета
    StoppingCalculation --> SavingProject: Сохранение проекта
    SavingProject --> ClosingProject: Закрытие проекта
    ClosingProject --> Ready: Готово к работе
    
    Ready --> ShuttingDown: Завершение работы
    ShuttingDown --> CleaningUp: Очистка ресурсов
    CleaningUp --> [*]: Завершение
```

### Диаграмма зависимостей модулей

Диаграмма показывает зависимости между модулями Rdk Core.

```mermaid
flowchart LR
    Application[Application Module]
    Engine[Engine Module]
    Graphics[Graphics Module]
    Serialize[Serialize Module]
    System[System Module]
    Math[Math Module]
    Utilities[Utilities Module]
    
    Application -->|использует| Engine
    Application -->|использует| System
    Application -->|использует| Utilities
    
    Engine -->|использует| Graphics
    Engine -->|использует| Serialize
    Engine -->|использует| System
    Engine -->|использует| Math
    Engine -->|использует| Utilities
    
    Graphics -->|использует| System
    Graphics -->|использует| Utilities
    
    Serialize -->|использует| System
    Serialize -->|использует| Utilities
    
    Math -->|использует| System
    Math -->|использует| Utilities
    
    System -->|использует| Utilities
```

### Диаграмма потоков данных (Data Flow Diagram)

Диаграмма показывает потоки данных в системе Rdk Core.

```mermaid
flowchart TB
    subgraph "Input Sources"
        ConfigInput[Configuration Files]
        ProjectInput[Project Files]
        NetworkInput[Network RPC]
    end
    
    subgraph "Application Layer"
        App[UApplication]
        Project[UProject]
        RPC[URpcDispatcher]
    end
    
    subgraph "Engine Layer"
        Engine[UEngine]
        Storage[UStorage]
        Env[UEnvironment]
        Component[UComponent]
    end
    
    subgraph "Output Destinations"
        LogOutput[Log Files]
        ResultOutput[Result Files]
        NetworkOutput[Network Response]
        GUIOutput[GUI Display]
    end
    
    ConfigInput --> App
    ProjectInput --> Project
    NetworkInput --> RPC
    
    App --> Engine
    Project --> Engine
    RPC --> Engine
    
    Engine --> Storage
    Engine --> Env
    Env --> Component
    
    Component --> LogOutput
    Component --> ResultOutput
    Component --> GUIOutput
    Component --> NetworkOutput
```

### Диаграмма взаимодействия компонентов

Диаграмма показывает взаимодействие основных компонентов системы.

```mermaid
sequenceDiagram
    participant Client as Клиент
    participant App as UApplication
    participant Project as UProject
    participant Engine as UEngine
    participant Env as UEnvironment
    participant Storage as UStorage
    participant Component as UComponent
    
    Client->>App: RPC Command
    App->>Project: Process Command
    Project->>Engine: Execute Operation
    Engine->>Storage: Get/Create Component
    Storage-->>Engine: Component
    Engine->>Env: Run Calculation
    Env->>Component: Calculate()
    Component-->>Env: Result
    Env-->>Engine: Result
    Engine-->>Project: Result
    Project-->>App: Result
    App-->>Client: Response
```

### См. также

- [Architecture.md](Architecture.md) - общая архитектура подсистем
- [Engine-Detailed.md](Engine-Detailed.md) - детальная документация движка
- [Application-Detailed.md](Application-Detailed.md) - детальная документация приложения

---

## EN

### Overview

This document contains high-level architecture diagrams of Rdk Core: component diagrams, deployment diagrams, package diagrams, and activity diagrams.

### Component Diagram

Shows the main modules of Rdk Core and their relationships.

### Deployment Diagram

Shows application deployment with Rdk Core and connections to external libraries.

### Package Diagram

Shows the package structure of all Rdk Core modules.

### Activity Diagram

Shows the application lifecycle from initialization to shutdown.

### Module Dependencies

Shows dependencies between Rdk Core modules.

### Data Flow Diagram

Shows data flows in the Rdk Core system.

### Component Interaction

Shows interaction between main system components.

### See Also

- [Architecture.md](Architecture.md) - general subsystem architecture
- [Engine-Detailed.md](Engine-Detailed.md) - engine detailed documentation
- [Application-Detailed.md](Application-Detailed.md) - application detailed documentation
