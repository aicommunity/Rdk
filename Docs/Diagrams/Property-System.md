# Система свойств компонентов

## RU

### Диаграмма классов свойств

Диаграмма ниже обобщает иерархию классов свойств, которая реализована в шаблонных классах `UProperty`, `UVProperty`, `UVBaseDataProperty` и интерфейсах `UIPropertyInput` / `UIPropertyOutput`. В коде свойства регистрируются внутри `UComponent` через таблицу `PropertiesLookupTable`, а коннектор `UConnector` использует интерфейсы ввода/вывода для установки связей между компонентами.

```mermaid
classDiagram
    class UProperty {
        <<abstract>>
        +getValue()
        +setValue()
        +GetId()
        +GetName()
    }
    
    class UPropertyInput {
        +getData()
        +IsConnected()
    }
    
    class UPropertyOutput {
        +setData()
        +ConnectTo()
    }
    
    class UPropertyIO {
        +getData()
        +setData()
    }
    
    class UComponent {
        +PropertiesLookupTable
        +GetProperty()
        +SetProperty()
    }
    
    class UConnector {
        +Connect()
        +Disconnect()
    }
    
    UProperty <|-- UPropertyInput
    UProperty <|-- UPropertyOutput
    UProperty <|-- UPropertyIO
    UComponent "1" o-- "*" UProperty
    UConnector --> UPropertyInput
    UConnector --> UPropertyOutput
```

### Соединение свойств

Следующая диаграмма деталирует, как данные передаются между двумя компонентами через свойства и коннектор:
- компонент‑источник записывает значение в выходное свойство (`setData`),
- выходное свойство уведомляет коннектор об обновлении,
- коннектор обновляет входное свойство целевого компонента,
- целевой компонент использует новое значение во время выполнения `ACalculate()`.

Эта последовательность соответствует логике методов `UConnector::ConnectToItem`, `UIPropertyInput::SetPointer` и связанных маршрутов чтения/записи данных.

```mermaid
sequenceDiagram
    participant Comp1 as Component1
    participant PropOut as OutputProperty
    participant Connector as UConnector
    participant PropIn as InputProperty
    participant Comp2 as Component2
    
    Comp1->>PropOut: setData(value)
    PropOut->>Connector: NotifyUpdate()
    Connector->>PropIn: UpdateValue(value)
    PropIn->>Comp2: Data available
    Comp2->>Comp2: ACalculate()
```

---

## EN

### Property Classes Diagram

The class diagram above summarizes the property class hierarchy implemented by templated `UProperty`, `UVProperty`, `UVBaseDataProperty` and the `UIPropertyInput` / `UIPropertyOutput` interfaces. In code, properties are registered inside `UComponent` via the `PropertiesLookupTable`, and the `UConnector` uses input/output interfaces to wire components together.

### Property Connection

The sequence diagram explains how data flows between two components via properties and a connector:
- the source component writes a value to its output property (`setData`),
- the output property notifies the connector about the update,
- the connector updates the target input property,
- the target component consumes the new value during its `ACalculate()` call.

This sequence follows the logic of `UConnector::ConnectToItem`, `UIPropertyInput::SetPointer` and related data access paths in the engine.
