# Руководство по созданию свойств

## RU

### Обзор

Руководство по созданию и использованию свойств компонентов.

### Типы свойств

- `ptParameter` - параметр компонента
- `ptState` - переменная состояния
- `ptInput` - входное свойство
- `ptOutput` - выходное свойство
- `ptTemp` - временная переменная

### Регистрация свойств

```cpp
// В методе ABuild()
RegisterProperty<double>("MyParameter", ptPubParameter);
RegisterProperty<int>("MyState", ptPubState);
RegisterProperty<double>("MyInput", ptPubInput);
RegisterProperty<double>("MyOutput", ptPubOutput);
```

### Использование свойств

```cpp
// Получение свойства
auto prop = GetProperty<double>("MyParameter");
if (prop) {
    double value = prop->GetValue();
    prop->SetValue(value + 1.0);
}
```

### См. также

- [Engine-Detailed.md](../Engine-Detailed.md) - детальная документация системы свойств
- [Diagrams/Property-System.md](../Diagrams/Property-System.md) - диаграммы системы свойств

---

## EN

### Overview

Guide for creating and using component properties.

### Property Types

- `ptParameter` - component parameter
- `ptState` - state variable
- `ptInput` - input property
- `ptOutput` - output property

### See Also

- [Engine-Detailed.md](../Engine-Detailed.md) - property system details
- [Diagrams/Property-System.md](../Diagrams/Property-System.md) - property system diagrams
