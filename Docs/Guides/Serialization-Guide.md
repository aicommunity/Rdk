# Руководство по сериализации

## RU

### Обзор

Руководство по работе с сериализацией компонентов и данных.

### Сериализация компонента

```cpp
#include "Rdk/Core/Serialize/USerStorageXML.h"

RDK::USerStorageXML storage;
storage.Create("Component");
storage.AddNode("Name");
storage.Write("Name", component->Name.GetValue());
storage.SelectUp();
storage.AddNode("Properties");
// ... сохранение свойств ...
storage.SaveToFile("component.xml");
```

### Десериализация компонента

```cpp
RDK::USerStorageXML storage;
storage.LoadFromFile("component.xml");
storage.GetNode("Component");
std::string name = storage.Read("Name");
// ... загрузка свойств ...
```

### См. также

- [Serialize-Detailed.md](../Serialize-Detailed.md) - детальная документация сериализации

---

## EN

### Overview

Guide for working with component and data serialization.

### Serializing a Component

```cpp
#include "Rdk/Core/Serialize/USerStorageXML.h"

RDK::USerStorageXML storage;
storage.Create("Component");
storage.AddNode("Name");
storage.Write("Name", component->Name.GetValue());
storage.SelectUp();
storage.AddNode("Properties");
// ... save properties ...
storage.SaveToFile("component.xml");
```

### Deserializing a Component

```cpp
RDK::USerStorageXML storage;
storage.LoadFromFile("component.xml");
storage.GetNode("Component");
std::string name = storage.Read("Name");
// ... load properties ...
```

### See Also

- [Serialize-Detailed.md](../Serialize-Detailed.md) - detailed serialization documentation
