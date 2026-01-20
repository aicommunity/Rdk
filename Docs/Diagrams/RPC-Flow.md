# Поток обработки RPC команд

## RU

### Последовательность обработки RPC команды

```mermaid
sequenceDiagram
    participant Client as Клиент
    participant Transport as UServerTransport
    participant Dispatcher as URpcDispatcher
    participant Decoder as URpcDecoder
    participant App as UApplication
    participant Engine as UEngine
    
    Client->>Transport: Отправка команды (TCP/HTTP)
    Transport->>Dispatcher: PushCommand(command)
    Dispatcher->>Dispatcher: DispatchCommand()
    Dispatcher->>Decoder: IsCmdSupported()
    
    alt Команда поддерживается
        Dispatcher->>Decoder: PushCommand()
        Decoder->>Decoder: ProcessCommand()
        Decoder->>App: Выполнение команды
        App->>Engine: Операции с движком
        Engine-->>App: Результат
        App-->>Decoder: Результат выполнения
        Decoder-->>Dispatcher: PushToProcessedQueue()
        Dispatcher-->>Transport: Отправка ответа
        Transport-->>Client: Ответ клиенту
    else Команда не поддерживается
        Dispatcher-->>Transport: Ошибка
        Transport-->>Client: Ошибка
    end
```

### Архитектура RPC системы

```mermaid
flowchart TB
    clientNode["Клиент"]
    transportNode["UServerTransport TCP/HTTP"]
    dispatcherNode["URpcDispatcher (диспетчер)"]
    decoderNode["URpcDecoder (декодер)"]
    appNode["UApplication"]
    engineNode["UEngine"]
    
    clientNode --> transportNode
    transportNode --> dispatcherNode
    dispatcherNode --> decoderNode
    decoderNode --> appNode
    appNode --> engineNode
    engineNode --> appNode
    appNode --> decoderNode
    decoderNode --> dispatcherNode
    dispatcherNode --> transportNode
    transportNode --> clientNode
```

В совокупности эти две диаграммы иллюстрируют, как входящие RPC‑команды проходят через транспорт, диспетчер, декодер и приложение до движка и обратно к клиенту. Они непосредственно соответствуют коду в `URpcDispatcher`, `URpcDecoder`, `UServerTransport` и методам `UApplication`, вызываемым декодером.

---

## EN

### RPC Command Processing Sequence

The sequence diagram above shows how an incoming RPC command is processed:
- the client sends a command via TCP/HTTP transport,
- `UServerTransport` pushes the command into `URpcDispatcher`,
- the dispatcher selects a suitable decoder, which executes the command by calling `UApplication` / engine methods,
- the result is returned back to the client through the dispatcher and transport.

### RPC System Architecture

The flowchart describes the static architecture of the RPC system:
- the client only talks to `UServerTransport`,
- `URpcDispatcher` and `URpcDecoder` form the core of the routing/decoding logic,
- `UApplication` and `UEngine` are the ultimate handlers of most RPC commands.

