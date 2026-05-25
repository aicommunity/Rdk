# Архитектура RDK LLM

## 1. Роль LLM в системе

LLM — **планировщик и интерпретатор намерения**, не исполнитель и не источник истины.

```
Пользователь → UI → Orchestrator → ILLMProvider (модель)
                      ↓
              Tool Gateway → Policy → URdkDomainAccess (UApplication…UNet)
                      ↓
              ILLMProjectContextProvider (NMSDK: Bin, ClDesc, Docs)
```

Модель **никогда** не:
- генерирует SQL/XML для прямого применения;
- читает файлы проекта в обход gateway;
- расширяет права пользователя.

---

## 2. Три плоскости

```mermaid
flowchart TB
    subgraph nmsdk [NMSDK]
        Provider[NmsdkLlmProjectContext]
        LibLlm[Libraries_star_Llm_optional]
    end
    subgraph gui [Rdk_GUI_Qt_Llm]
        Host[UGEngineControlWidget]
        Bridge[ULlmGuiContextBridge]
        Dock[ULlmAssistantDockWidget]
    end
    subgraph core [Rdk_LLM_Core]
        Orch[ULLMAgentOrchestrator]
        GW[ULLMToolGateway]
        Dom[URdkDomainAccess]
    end
    subgraph rdk [Rdk_UApplication_Layer]
        App[UApplication]
        Eng[UEngine]
        Env[UEnvironment]
        Stor[UStorage]
        Net[UNet]
    end
    Provider --> Orch
    LibLlm --> GW
    Host --> Bridge --> Dock --> Orch
    Orch --> GW --> Dom
    Dom --> App --> Eng
    Eng --> Env
    Eng --> Stor
    Env --> Net
```

| Плоскость | Путь | Зависимости |
|-----------|------|-------------|
| Core | `Rdk/LLM/` | `rdk.static.qt`, CURL, nlohmann_json **только если** `RDK_USE_LLM` |
| GUI | `Rdk/GUI/Qt/Llm/` | Qt, `rdk.llm.core`, **только если** `RDK_USE_LLM` |
| NMSDK | `App/NeuroModeler/`, `Libraries/*/Llm/` | Регистрация контекста и доп. tools |

---

## 3. Слои (нумерация для кода)

| # | Слой | Namespace / префикс классов | Каталог |
|---|------|------------------------------|---------|
| 1 | UI / API | `ULlm*` (Qt) | `Rdk/GUI/Qt/Llm/` |
| 2 | Intent | `ULLMIntent*` | `Rdk/LLM/Core/Intent/` |
| 3 | Orchestrator | `ULLMAgentOrchestrator` | `Rdk/LLM/Core/Orchestrator/` |
| 4 | Tool Gateway | `ULLMToolGateway` | `Rdk/LLM/Core/Tools/` |
| 5 | Policy | `ULLMPolicyEngine` | `Rdk/LLM/Core/Policy/` |
| 6 | Domain | `URdkDomainAccess` | `Rdk/LLM/Core/Domain/` |
| 6b | Project context | `ILLMProjectContextProvider` | `Rdk/LLM/Core/Context/` + NMSDK impl |
| 7 | Observability | `ULLMAuditLog` | `Rdk/LLM/Core/Observability/` |
| — | Providers | `ILLMProvider` | `Rdk/LLM/Core/Providers/` |

**Правило зависимостей:** слои 1–7 не включают заголовки Qt в `Rdk/LLM/Core` (кроме опционального bridge interface в Deploy).

---

## 4. Поток одного запроса пользователя

```mermaid
sequenceDiagram
    participant U as User
    participant UI as ULlmAssistantDockWidget
    participant Br as ULlmGuiContextBridge
    participant O as ULLMAgentOrchestrator
    participant P as ILLMProvider
    participant Pol as ULLMPolicyEngine
    participant GW as ULLMToolGateway
    participant D as URdkDomainAccess
    participant Ctx as ILLMProjectContextProvider

    U->>UI: текст запроса
    UI->>Br: snapshot GUI context
    Br->>O: LLMRequestEnvelope
    O->>O: load ULLMConversationStore
    O->>Ctx: retrieve static snippets
    O->>D: get_net_snapshot summary
    O->>P: chat + tool definitions subset
    P-->>O: tool_calls or text
    alt tool_calls
        O->>Pol: validateToolCalls
        O->>UI: preview if write
        U->>UI: Confirm
        O->>GW: execute
        GW->>Pol: per-call
        GW->>D: domain method
        GW-->>O: tool_results JSON
        O->>P: continue loop
    end
    O->>O: audit append
    O-->>UI: final message
```

---

## 5. Сценарии зрелости

| ID | Имя | Фаза | Документ |
|----|-----|------|----------|
| A | Chat over data | 1 | [MVP-Roadmap.md](MVP-Roadmap.md) |
| B | Copilot for actions | 2 | MVP-Roadmap |
| C | Workflow operator | Post-MVP | Orchestrator (design only) |
| D | Autonomous agent | **Не в scope** | Anti-Patterns |

---

## 6. CMake targets (итог)

| Target | Условие |
|--------|---------|
| `rdk.llm.core` | `RDK_USE_LLM=ON` |
| `rdk.llm.embedded` | `RDK_USE_LLM=ON` && `RDK_LLM_BUILD_EMBEDDED=ON` |
| NeuroModeler + GUI Llm sources | `RDK_USE_LLM=ON` |

См. [Build.md](Build.md).

---

## 7. Точки регистрации NMSDK

```cpp
// App/NeuroModeler/NmsdkRegisterLlm.h — вызывается после UApplication::Init, до показа главного окна
void NmsdkRegisterLlm(RDK::UApplication* app,
                      ULLMToolRegistry& tools,
                      ILLMProjectContextProviderRegistry& ctx);
```

Аналоги:
- `RdkLoadPredefinedLibraries` → список библиотек для `list_registered_classes`
- `RegisterComponentGuiForms` → паттерн для `Libraries/*/Llm/*LibLlmRegistration.cpp`

---

## 8. Ссылки на код RDK (якоря)

| Сущность | Файл |
|----------|------|
| UApplication | `Rdk/Core/Application/UApplication.h` |
| UEngine | `Rdk/Core/Engine/UEngine.h` |
| UNet | `Rdk/Core/Engine/UNet.h` |
| UGuiModelSnapshot | `Rdk/GUI/Qt/UGuiModelSnapshot.h` |
| UGEngineControlWidget | `Rdk/GUI/Qt/UGEngineControlWidget.h` |
| Libraries load | `Libraries/Libraries.cpp` |
| C API | `Rdk/Deploy/Include/rdk_init.h` |
