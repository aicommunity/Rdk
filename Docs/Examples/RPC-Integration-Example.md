# Пример: Интеграция RPC

## RU

### Описание задачи

Добавить RPC команды для управления компонентом через удаленный интерфейс.

### Решение

#### Шаг 1: Компонент с RPC поддержкой

```cpp
#include "Rdk/Core/Application/URpcDispatcher.h"
#include "Rdk/Core/Engine/UContainer.h"

class URpcComponent : public RDK::UContainer {
public:
    URpcComponent() : RDK::UContainer() {}
    
protected:
    RDK::UProperty<double, URpcComponent, RDK::ptPubParam> Value;
    
    virtual bool ABuild(void) override;
    
    // RPC обработчики
    void HandleSetValue(const std::string& params);
    void HandleGetValue(const std::string& params);
};
```

#### Шаг 2: Регистрация RPC команд

```cpp
bool URpcComponent::ABuild(void) {
    if (!UContainer::ABuild())
        return false;
    
    // Получение RPC диспетчера
    RDK::UELockPtr<RDK::UEnvironment> env = RDK::GetEnvironmentLock();
    if (env) {
        RDK::URpcDispatcher* dispatcher = env->GetRpcDispatcher();
        if (dispatcher) {
            // Регистрация команд
            dispatcher->RegisterCommand(
                GetName() + ".SetValue",
                [this](const std::string& params) {
                    this->HandleSetValue(params);
                }
            );
            
            dispatcher->RegisterCommand(
                GetName() + ".GetValue",
                [this](const std::string& params) {
                    this->HandleGetValue(params);
                }
            );
        }
    }
    
    return true;
}
```

#### Шаг 3: Реализация обработчиков

```cpp
void URpcComponent::HandleSetValue(const std::string& params) {
    try {
        double newValue = std::stod(params);
        Value = newValue;
        
        Logger->LogMessageEx(RDK_EX_INFO, GetName(), __FUNCTION__,
                            "Value set to " + std::to_string(newValue));
    } catch (const std::exception& ex) {
        Logger->LogMessageEx(RDK_EX_ERROR, GetName(), __FUNCTION__,
                            "Failed to set value: " + std::string(ex.what()));
    }
}

void URpcComponent::HandleGetValue(const std::string& params) {
    std::string result = std::to_string(Value());
    
    // Отправка результата через RPC
    RDK::UELockPtr<RDK::UEnvironment> env = RDK::GetEnvironmentLock();
    if (env) {
        RDK::URpcDispatcher* dispatcher = env->GetRpcDispatcher();
        if (dispatcher) {
            dispatcher->SendResponse(GetName() + ".GetValue", result);
        }
    }
}
```

#### Шаг 4: Использование RPC

```cpp
// Запуск RPC сервера
RDK::URpcServer server;
server.Start(8080);

// Клиент может вызывать команды:
// POST /rpc/MyComponent.SetValue
// Body: "2.5"
//
// GET /rpc/MyComponent.GetValue
// Response: "2.5"
```

### Объяснение ключевых моментов

1. **Регистрация команд** - команды регистрируются в ABuild()
2. **Именование** - использование имени компонента для уникальности команд
3. **Обработка ошибок** - валидация параметров и обработка исключений
4. **Отправка ответов** - использование SendResponse для возврата результатов

### Связанная документация

- [Guides/RPC-Integration.md](../Guides/RPC-Integration.md) - детальное руководство
- [Application-Detailed.md](../Application-Detailed.md) - детали приложения

---

## EN

### Task Description

Add RPC commands to control a component through a remote interface.

### Solution

#### Step 1: Component with RPC Support

```cpp
#include "Rdk/Core/Application/URpcDispatcher.h"
#include "Rdk/Core/Engine/UContainer.h"

class URpcComponent : public RDK::UContainer {
public:
    URpcComponent() : RDK::UContainer() {}
    
protected:
    RDK::UProperty<double, URpcComponent, RDK::ptPubParam> Value;
    
    virtual bool ABuild(void) override;
    
    // RPC handlers
    void HandleSetValue(const std::string& params);
    void HandleGetValue(const std::string& params);
};
```

#### Step 2: RPC Command Registration

```cpp
bool URpcComponent::ABuild(void) {
    if (!UContainer::ABuild())
        return false;
    
    // Get RPC dispatcher
    RDK::UELockPtr<RDK::UEnvironment> env = RDK::GetEnvironmentLock();
    if (env) {
        RDK::URpcDispatcher* dispatcher = env->GetRpcDispatcher();
        if (dispatcher) {
            // Register commands
            dispatcher->RegisterCommand(
                GetName() + ".SetValue",
                [this](const std::string& params) {
                    this->HandleSetValue(params);
                }
            );
            
            dispatcher->RegisterCommand(
                GetName() + ".GetValue",
                [this](const std::string& params) {
                    this->HandleGetValue(params);
                }
            );
        }
    }
    
    return true;
}
```

#### Step 3: Handler Implementation

```cpp
void URpcComponent::HandleSetValue(const std::string& params) {
    try {
        double newValue = std::stod(params);
        Value = newValue;
        
        Logger->LogMessageEx(RDK_EX_INFO, GetName(), __FUNCTION__,
                            "Value set to " + std::to_string(newValue));
    } catch (const std::exception& ex) {
        Logger->LogMessageEx(RDK_EX_ERROR, GetName(), __FUNCTION__,
                            "Failed to set value: " + std::string(ex.what()));
    }
}

void URpcComponent::HandleGetValue(const std::string& params) {
    std::string result = std::to_string(Value());
    
    // Send result via RPC
    RDK::UELockPtr<RDK::UEnvironment> env = RDK::GetEnvironmentLock();
    if (env) {
        RDK::URpcDispatcher* dispatcher = env->GetRpcDispatcher();
        if (dispatcher) {
            dispatcher->SendResponse(GetName() + ".GetValue", result);
        }
    }
}
```

#### Step 4: Using RPC

```cpp
// Start RPC server
RDK::URpcServer server;
server.Start(8080);

// Client can call commands:
// POST /rpc/MyComponent.SetValue
// Body: "2.5"
//
// GET /rpc/MyComponent.GetValue
// Response: "2.5"
```

### Key Points Explanation

1. **Command Registration** - commands registered in ABuild()
2. **Naming** - using component name for command uniqueness
3. **Error Handling** - parameter validation and exception handling
4. **Sending Responses** - using SendResponse to return results

### Related Documentation

- [Guides/RPC-Integration.md](../Guides/RPC-Integration.md) - detailed guide
- [Application-Detailed.md](../Application-Detailed.md) - application details
