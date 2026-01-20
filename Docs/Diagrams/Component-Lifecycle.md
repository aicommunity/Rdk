# Жизненный цикл компонента

## RU

### Диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Created: Создание компонента
    Created --> Default: ADefault()
    Default --> Built: ABuild()
    Built --> Ready: Готов к работе
    Ready --> Reset: AReset()
    Reset --> Calculate: ACalculate()
    Calculate --> Calculate: Повтор вычислений
    Calculate --> Reset: Новый цикл
    Ready --> [*]: Удаление компонента
    
    Built --> Error: Ошибка сборки
    Calculate --> Error: Ошибка вычисления
    Error --> [*]: Удаление
```

### Последовательность методов

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Factory as UComponentFactory
    participant Component as UComponent
    participant Env as UEnvironment
    
    Storage->>Factory: CreateComponent()
    Factory->>Component: new Component()
    Component->>Component: ADefault()
    Component->>Component: ABuild()
    
    Env->>Component: Reset()
    Component->>Component: AReset()
    
    loop Каждый шаг времени
        Env->>Component: Calculate()
        Component->>Component: ACalculate()
        Component->>Component: Обработка данных
    end
```

---

## EN

### State Diagram

### Method Sequence
