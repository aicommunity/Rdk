# Архитектура RDK LLM (краткий индекс)

## RU

**Подробная normative-архитектура:** [Developer-Architecture.md](Developer-Architecture.md).  
**Как расширять:** [Extension-Guide.md](Extension-Guide.md).

## 1. Роль LLM в системе

LLM — **планировщик и интерпретатор намерения**, не исполнитель и не источник истины.

```
Пользователь → UI → Orchestrator → ILLMProvider (модель)
                      ↓
              Tool Gateway → Policy → URdkApplicationCommands → UApplication
                      ↓                    ↘ URdkDomainAccess (UNet write/read)
              ILLMPresentationSink (optional GUI refresh)
                      ↓
              ILLMProjectContextProvider (NMSDK: Bin, ClDesc, Docs)
```

Модель **никогда** не:
- генерирует SQL/XML для прямого применения;
- читает файлы проекта в обход gateway;
- расширяет права пользователя.

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
        Cmd[URdkApplicationCommands]
        Dom[URdkDomainAccess]
        Sink[ILLMPresentationSink]
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
    Orch --> GW --> Cmd
    GW --> Dom
    Cmd --> App
    Cmd --> Sink
    Sink --> Host
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
| 6 | Application commands | `URdkApplicationCommands` | `Rdk/LLM/Core/Domain/` |
| 6a | Domain (net/model) | `URdkDomainAccess` | `Rdk/LLM/Core/Domain/` |
| 6b | Presentation (optional) | `ILLMPresentationSink` | `Rdk/LLM/Core/Gui/` + `Rdk/GUI/Qt/Llm/` |
| 6c | Project context | `ILLMProjectContextProvider` | `Rdk/LLM/Core/Context/` + NMSDK impl |
| 7 | Observability | `ULLMAuditLog` | `Rdk/LLM/Core/Observability/` |
| — | Providers | `ILLMProvider` | `Rdk/LLM/Core/Providers/` |

**Правило зависимостей:** слои 1–7 не включают заголовки Qt в `Rdk/LLM/Core` (кроме опционального bridge interface в Deploy).

---

## 4. Поток одного запроса пользователя

См. **[Developer-Architecture.md §4](Developer-Architecture.md#4-request-flow-current)** (worker thread, optional HITL, auto-apply, GUI-thread lifecycle tools, plans).

### 4.1 Thinking-first Cortex (Ollama)

Каждый ReAct-раунд Cortex (профиль `ollama-thinking`, модель по умолчанию **`qwen3:14b`**):

1. Ephemeral capability catalog + `tools` schemas для текущего allowlist  
2. `think: true` (если `LLM/enable_ollama_thinking` и `supports_thinking`)  
3. Primary transport — native `/api/chat` (`UOllamaNativeProvider`)  
4. `message.thinking` сохраняется на assistant turns с `tool_calls` (без mid-loop rewrite)  
5. После Observe следующий round снова с think ON  
6. При think ON нет forced `tool_choice`  
7. `search_tools` расширяет allowlist на следующие раунды (progressive disclosure)

User-facing ответ не содержит raw thinking; GUI показывает collapsible **Reasoning** (`on_thinking_token` / `final.thinking`).

См. [Providers.md](Providers.md), [Model-Capability-Matrix.md](Model-Capability-Matrix.md), [Agent-Interaction.md](Agent-Interaction.md).

---

## 5. Сценарии зрелости

| ID | Имя | Фаза | Документ |
|----|-----|------|----------|
| A | Chat over data | 1 | [MVP-Roadmap.md](MVP-Roadmap.md) |
| B | Copilot for actions | 2 | MVP-Roadmap |
| C | Workflow operator | Post-MVP | [Orchestrator.md](Orchestrator.md), `ULLMPlanExecutor` |
| D | Autonomous agent | Post-MVP | [Post-MVP-Implementation-Plan.md](Post-MVP-Implementation-Plan.md) |

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

---

## EN

**Detailed normative architecture:** [Developer-Architecture.md](Developer-Architecture.md).  
**How to extend:** [Extension-Guide.md](Extension-Guide.md).

## 1. Role of LLM in the system

LLM is a **planner and intent interpreter**, not an executor or source of truth.

```
User → UI → Orchestrator → ILLMProvider (model)
                      ↓
              Tool Gateway → Policy → URdkApplicationCommands → UApplication
                      ↓                    ↘ URdkDomainAccess (UNet write/read)
              ILLMPresentationSink (optional GUI refresh)
                      ↓
              ILLMProjectContextProvider (NMSDK: Bin, ClDesc, Docs)
```

The model **never**:
- generates SQL/XML for direct application;
- reads project files bypassing the gateway;
- expands user privileges.

## 2. Three planes

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
        Cmd[URdkApplicationCommands]
        Dom[URdkDomainAccess]
        Sink[ILLMPresentationSink]
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
    Orch --> GW --> Cmd
    GW --> Dom
    Cmd --> App
    Cmd --> Sink
    Sink --> Host
    Dom --> App --> Eng
    Eng --> Env
    Eng --> Stor
    Env --> Net
```

| Plane | Path | Dependencies |
|-------|------|--------------|
| Core | `Rdk/LLM/` | `rdk.static.qt`, CURL, nlohmann_json **only if** `RDK_USE_LLM` |
| GUI | `Rdk/GUI/Qt/Llm/` | Qt, `rdk.llm.core`, **only if** `RDK_USE_LLM` |
| NMSDK | `App/NeuroModeler/`, `Libraries/*/Llm/` | Context registration and extra tools |

---

## 3. Layers (numbering for code)

| # | Layer | Namespace / class prefix | Directory |
|---|-------|--------------------------|-----------|
| 1 | UI / API | `ULlm*` (Qt) | `Rdk/GUI/Qt/Llm/` |
| 2 | Intent | `ULLMIntent*` | `Rdk/LLM/Core/Intent/` |
| 3 | Orchestrator | `ULLMAgentOrchestrator` | `Rdk/LLM/Core/Orchestrator/` |
| 4 | Tool Gateway | `ULLMToolGateway` | `Rdk/LLM/Core/Tools/` |
| 5 | Policy | `ULLMPolicyEngine` | `Rdk/LLM/Core/Policy/` |
| 6 | Application commands | `URdkApplicationCommands` | `Rdk/LLM/Core/Domain/` |
| 6a | Domain (net/model) | `URdkDomainAccess` | `Rdk/LLM/Core/Domain/` |
| 6b | Presentation (optional) | `ILLMPresentationSink` | `Rdk/LLM/Core/Gui/` + `Rdk/GUI/Qt/Llm/` |
| 6c | Project context | `ILLMProjectContextProvider` | `Rdk/LLM/Core/Context/` + NMSDK impl |
| 7 | Observability | `ULLMAuditLog` | `Rdk/LLM/Core/Observability/` |
| — | Providers | `ILLMProvider` | `Rdk/LLM/Core/Providers/` |

**Dependency rule:** layers 1–7 do not include Qt headers in `Rdk/LLM/Core` (except optional bridge interface in Deploy).

---

## 4. Flow of a single user request

See **[Developer-Architecture.md §4](Developer-Architecture.md#4-request-flow-current)** (worker thread, optional HITL, auto-apply, GUI-thread lifecycle tools, plans).

### 4.1 Thinking-first Cortex (Ollama)

Each Cortex ReAct round (profile `ollama-thinking`, default model **`qwen3:14b`**):

1. Ephemeral capability catalog + `tools` schemas for the current allowlist  
2. `think: true` when `LLM/enable_ollama_thinking` and `supports_thinking`  
3. Primary transport — native `/api/chat` (`UOllamaNativeProvider`)  
4. Preserve `message.thinking` on assistant turns with `tool_calls`  
5. After Observe, the next round thinks again  
6. No forced `tool_choice` while thinking is on  
7. `search_tools` expands the allowlist for later rounds  

User-facing answers omit raw thinking; the GUI shows a collapsible **Reasoning** block.

See [Providers.md](Providers.md), [Model-Capability-Matrix.md](Model-Capability-Matrix.md), [Agent-Interaction.md](Agent-Interaction.md).

---

## 5. Maturity scenarios

| ID | Name | Phase | Document |
|----|------|-------|----------|
| A | Chat over data | 1 | [MVP-Roadmap.md](MVP-Roadmap.md) |
| B | Copilot for actions | 2 | MVP-Roadmap |
| C | Workflow operator | Post-MVP | [Orchestrator.md](Orchestrator.md), `ULLMPlanExecutor` |
| D | Autonomous agent | Post-MVP | [Post-MVP-Implementation-Plan.md](Post-MVP-Implementation-Plan.md) |

---

## 6. CMake targets (summary)

| Target | Condition |
|--------|-----------|
| `rdk.llm.core` | `RDK_USE_LLM=ON` |
| `rdk.llm.embedded` | `RDK_USE_LLM=ON` && `RDK_LLM_BUILD_EMBEDDED=ON` |
| NeuroModeler + GUI Llm sources | `RDK_USE_LLM=ON` |

See [Build.md](Build.md).

---

## 7. NMSDK registration points

```cpp
// App/NeuroModeler/NmsdkRegisterLlm.h — called after UApplication::Init, before showing the main window
void NmsdkRegisterLlm(RDK::UApplication* app,
                      ULLMToolRegistry& tools,
                      ILLMProjectContextProviderRegistry& ctx);
```

Analogues:
- `RdkLoadPredefinedLibraries` → library list for `list_registered_classes`
- `RegisterComponentGuiForms` → pattern for `Libraries/*/Llm/*LibLlmRegistration.cpp`

---

## 8. RDK code references (anchors)

| Entity | File |
|--------|------|
| UApplication | `Rdk/Core/Application/UApplication.h` |
| UEngine | `Rdk/Core/Engine/UEngine.h` |
| UNet | `Rdk/Core/Engine/UNet.h` |
| UGuiModelSnapshot | `Rdk/GUI/Qt/UGuiModelSnapshot.h` |
| UGEngineControlWidget | `Rdk/GUI/Qt/UGEngineControlWidget.h` |
| Libraries load | `Libraries/Libraries.cpp` |
| C API | `Rdk/Deploy/Include/rdk_init.h` |
