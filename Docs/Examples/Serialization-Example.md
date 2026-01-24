# Пример: Работа с сериализацией

## RU

### Описание задачи

Реализовать сохранение и загрузку компонента с несколькими свойствами через XML сериализацию.

### Решение

#### Шаг 1: Компонент с сериализацией

```cpp
#include "Rdk/Core/Engine/UContainer.h"
#include "Rdk/Core/Serialize/USerStorageXML.h"

class USerializableComponent : public RDK::UContainer {
public:
    USerializableComponent() : RDK::UContainer() {}
    
protected:
    RDK::UProperty<double, USerializableComponent, RDK::ptPubParam> Gain;
    RDK::UProperty<int, USerializableComponent, RDK::ptPubParam> WindowSize;
    RDK::UProperty<std::string, USerializableComponent, RDK::ptPubParam> Name;
    
    // Методы сериализации
    virtual bool Save(RDK::USerStorageXML &xml) override;
    virtual bool Load(RDK::USerStorageXML &xml) override;
};
```

#### Шаг 2: Реализация Save()

```cpp
bool USerializableComponent::Save(RDK::USerStorageXML &xml) {
    if (!UContainer::Save(xml))
        return false;
    
    // Сохранение версии для миграции
    xml.Add("Version", 1);
    
    // Сохранение параметров
    xml.Add("Gain", Gain());
    xml.Add("WindowSize", WindowSize());
    xml.Add("Name", Name().c_str());
    
    return true;
}
```

#### Шаг 3: Реализация Load()

```cpp
bool USerializableComponent::Load(RDK::USerStorageXML &xml) {
    if (!UContainer::Load(xml))
        return false;
    
    // Загрузка версии
    int version = xml.GetInt("Version", 1);
    
    // Загрузка параметров с проверкой версии
    if (version >= 1) {
        Gain = xml.GetDouble("Gain", 1.0);
        WindowSize = xml.GetInt("WindowSize", 10);
        Name = xml.GetString("Name", "DefaultName");
    }
    
    // Валидация загруженных данных
    if (Gain() < 0.0 || Gain() > 100.0) {
        Logger->LogMessageEx(RDK_EX_WARNING, GetName(), __FUNCTION__,
                            "Invalid Gain value, using default");
        Gain = 1.0;
    }
    
    if (WindowSize() < 1) {
        Logger->LogMessageEx(RDK_EX_WARNING, GetName(), __FUNCTION__,
                            "Invalid WindowSize, using default");
        WindowSize = 10;
    }
    
    return true;
}
```

#### Шаг 4: Использование сериализации

```cpp
// Сохранение компонента
RDK::USerStorageXML xml_writer;
if (component->Save(xml_writer)) {
    xml_writer.SaveToFile("component.xml");
}

// Загрузка компонента
RDK::USerStorageXML xml_reader;
if (xml_reader.LoadFromFile("component.xml")) {
    component->Load(xml_reader);
}
```

### Пример XML файла

```xml
<Component Name="MyComponent" Class="USerializableComponent">
    <Version>1</Version>
    <Gain>2.5</Gain>
    <WindowSize>20</WindowSize>
    <Name>MyComponentName</Name>
</Component>
```

### Объяснение ключевых моментов

1. **Версионирование** - добавление версии позволяет мигрировать старые форматы
2. **Валидация** - проверка загруженных данных на корректность
3. **Значения по умолчанию** - использование в GetDouble/GetInt при отсутствии значения
4. **Наследование Save/Load** - вызов базового метода для сохранения базовых данных

### Связанная документация

- [Guides/Serialization-Guide.md](../Guides/Serialization-Guide.md) - детальное руководство
- [Serialize-Detailed.md](../Serialize-Detailed.md) - детали сериализации

---

## EN

### Task Description

Implement saving and loading of a component with multiple properties through XML serialization.

### Solution

#### Step 1: Component with Serialization

```cpp
#include "Rdk/Core/Engine/UContainer.h"
#include "Rdk/Core/Serialize/USerStorageXML.h"

class USerializableComponent : public RDK::UContainer {
public:
    USerializableComponent() : RDK::UContainer() {}
    
protected:
    RDK::UProperty<double, USerializableComponent, RDK::ptPubParam> Gain;
    RDK::UProperty<int, USerializableComponent, RDK::ptPubParam> WindowSize;
    RDK::UProperty<std::string, USerializableComponent, RDK::ptPubParam> Name;
    
    // Serialization methods
    virtual bool Save(RDK::USerStorageXML &xml) override;
    virtual bool Load(RDK::USerStorageXML &xml) override;
};
```

#### Step 2: Save() Implementation

```cpp
bool USerializableComponent::Save(RDK::USerStorageXML &xml) {
    if (!UContainer::Save(xml))
        return false;
    
    // Save version for migration
    xml.Add("Version", 1);
    
    // Save parameters
    xml.Add("Gain", Gain());
    xml.Add("WindowSize", WindowSize());
    xml.Add("Name", Name().c_str());
    
    return true;
}
```

#### Step 3: Load() Implementation

```cpp
bool USerializableComponent::Load(RDK::USerStorageXML &xml) {
    if (!UContainer::Load(xml))
        return false;
    
    // Load version
    int version = xml.GetInt("Version", 1);
    
    // Load parameters with version check
    if (version >= 1) {
        Gain = xml.GetDouble("Gain", 1.0);
        WindowSize = xml.GetInt("WindowSize", 10);
        Name = xml.GetString("Name", "DefaultName");
    }
    
    // Validate loaded data
    if (Gain() < 0.0 || Gain() > 100.0) {
        Logger->LogMessageEx(RDK_EX_WARNING, GetName(), __FUNCTION__,
                            "Invalid Gain value, using default");
        Gain = 1.0;
    }
    
    if (WindowSize() < 1) {
        Logger->LogMessageEx(RDK_EX_WARNING, GetName(), __FUNCTION__,
                            "Invalid WindowSize, using default");
        WindowSize = 10;
    }
    
    return true;
}
```

#### Step 4: Using Serialization

```cpp
// Save component
RDK::USerStorageXML xml_writer;
if (component->Save(xml_writer)) {
    xml_writer.SaveToFile("component.xml");
}

// Load component
RDK::USerStorageXML xml_reader;
if (xml_reader.LoadFromFile("component.xml")) {
    component->Load(xml_reader);
}
```

### Example XML File

```xml
<Component Name="MyComponent" Class="USerializableComponent">
    <Version>1</Version>
    <Gain>2.5</Gain>
    <WindowSize>20</WindowSize>
    <Name>MyComponentName</Name>
</Component>
```

### Key Points Explanation

1. **Versioning** - adding version allows migration of old formats
2. **Validation** - checking loaded data for correctness
3. **Default Values** - using in GetDouble/GetInt when value is missing
4. **Inheriting Save/Load** - calling base method to save base data

### Related Documentation

- [Guides/Serialization-Guide.md](../Guides/Serialization-Guide.md) - detailed guide
- [Serialize-Detailed.md](../Serialize-Detailed.md) - serialization details
