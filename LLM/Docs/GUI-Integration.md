# GUI Integration — Qt, слабая связность

## RU

## 1. Принцип

GUI RDK — **набор слабосвязанных виджетов**. LLM **не** монолитно встраивается в `UGEngineControlWidget.cpp`.

| Роль | Компонент |
|------|-----------|
| **Host** | `UGEngineControlWidget` — регистрация, меню, владение `UApplication*` |
| **Bridge** | `ULlmGuiContextBridge` — сбор контекста из сигналов |
| **Views** | `ULlmAssistantDockWidget`, `ULlmChangePreviewWidget`, settings |
| **Bootstrap** | `LlmGuiBootstrap.cpp` — `#ifdef RDK_USE_LLM` |

---

## EN

## 2. `LLMGuiContext` (data structure)

```cpp
struct LLMGuiContext {
    RDK::UApplication* application = nullptr;
    int channel_index = 0;
    QString project_xml_path;           // текущий открытый проект (если есть)
    QString focused_component_long_name; // из диаграммы / списка
    QString focused_class_name;
    QRectF diagram_viewport_scene_rect; // опционально, фаза 2
    qint64 snapshot_fingerprint = 0;    // UGuiModelSnapshot
};
```

**Selection vs diagram scope (DD-AG-001 / TD-111 reverted):** GUI selection and diagram drill update `focused_*` / `diagram_scope_long_name` only. They must **not** call `Env_SelectCurrentComponent` / `syncEngineCurrentComponent` — that rebases `FindComponent` / all `Model_*` string paths off Model and breaks absolute longName C-API usage (delete, props, links). Engine `CurrentComponent` stays at Model in normal NeuroModeler use (as BCB did for years).

`guiSnapshotFromContext` sets `current_component_long_name` from `diagram_scope_long_name`, else `focused_component_long_name` — not from live `Env_GetCurrentComponent*`.

Write tools resolve `parent_long_name` as a **full model path** (`GetComponentL` under model lock); defaults come from pinned `diagram_scope_long_name`.

Update: bridge subscribes to signals, **does not** poll the engine on a timer unless needed.

При Send dock converts `LLMGuiContext` → `LLMGuiContextSnapshot` и fills `LLMRequestEnvelope::gui`. Orchestrator stores snapshot в `ConversationState::last_gui_context`, **freezes** его в `ConversationState::active_turn_pin` (`beginGuiTurnPin`) на entire user turn, и adds ephemeral hint `## GUI focus` via `ULLMContextAssembler`. Write tools и normalizer use pin, not live navigation during a long LLM response.

### Tool trace в chat (TD-134)

After each reply assistant dock shows collapsible block **Tools (N)** из `LLMFinalResponse::tool_trace` (sanitized arguments, ok/error, duration). Тот же HTML is written в `Bin/AiChats/.../*.html` via `formatTurnToolTraceHtml`.

### Presentation scope guard (TD-136)

Before write invoke gateway can temporarily open drill-level из pin (`ILLMPresentationSink::navigateToDiagramScope`) и restore вид user after invoke (`captureNavigationToken` / `restoreNavigationToken`). Controlled `LLMRuntimeProviderSettings::pin_diagram_for_writes` (GUI default: true).

`seedSessionContext()` called из `startNewChat` / `onProjectLoaded` when project open (bootstrap system message; optional `get_net_snapshot` при `NMSDK_LLM_SESSION_BOOTSTRAP=1`).

---
