# GUI Integration — Qt, слабая связность

## 1. Принцип

GUI RDK — **набор слабосвязанных виджетов**. LLM **не** монолитно встраивается в `UGEngineControlWidget.cpp`.

| Роль | Компонент |
|------|-----------|
| **Host** | `UGEngineControlWidget` — регистрация, меню, владение `UApplication*` |
| **Bridge** | `ULlmGuiContextBridge` — сбор контекста из сигналов |
| **Views** | `ULlmAssistantDockWidget`, `ULlmChangePreviewWidget`, settings |
| **Bootstrap** | `LlmGuiBootstrap.cpp` — `#ifdef RDK_USE_LLM` |

---

## 2. `LLMGuiContext` (структура данных)

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

**Selection vs diagram scope:** on component selection, GUI may call `syncEngineCurrentComponent` (`UEngineSelectionSync.cpp`) for legacy C APIs. `diagram_scope_long_name` is updated on schematic drill **without** changing engine `CurrentComponent`. Write tools resolve `parent_long_name` as a **full model path** (`GetComponentL` under model lock); defaults come from pinned `diagram_scope_long_name`, not from `Env_GetCurrentComponentName()`.

Обновление: bridge подписан на сигналы, **не** опрашивает engine в timer без необходимости.

При Send dock конвертирует `LLMGuiContext` → `LLMGuiContextSnapshot` и заполняет `LLMRequestEnvelope::gui`. Orchestrator сохраняет snapshot в `ConversationState::last_gui_context`, **замораживает** его в `ConversationState::active_turn_pin` (`beginGuiTurnPin`) на весь user turn, и добавляет ephemeral hint `## GUI focus` через `ULLMContextAssembler`. Write tools и нормализатор используют pin, а не live navigation во время долгого LLM-ответа.

### Tool trace в чате (TD-134)

После каждого ответа ассистента dock показывает collapsible блок **Tools (N)** из `LLMFinalResponse::tool_trace` (санитизированные аргументы, ok/error, duration). Тот же HTML пишется в `Bin/AiChats/.../*.html` через `formatTurnToolTraceHtml`.

### Presentation scope guard (TD-136)

Перед write invoke gateway может временно открыть drill-level из pin (`ILLMPresentationSink::navigateToDiagramScope`) и восстановить вид пользователя после invoke (`captureNavigationToken` / `restoreNavigationToken`). Управляется `LLMRuntimeProviderSettings::pin_diagram_for_writes` (GUI default: true).

`seedSessionContext()` вызывается из `startNewChat` / `onProjectLoaded` когда проект открыт (bootstrap system message; optional `get_net_snapshot` при `NMSDK_LLM_SESSION_BOOTSTRAP=1`).

---

## 3. `ULlmGuiContextBridge`

### 3.1 API

```cpp
class ULlmGuiContextBridge : public QObject {
    Q_OBJECT
public:
    explicit ULlmGuiContextBridge(RDK::UApplication* app, QObject* parent = nullptr);

    LLMGuiContext currentContext() const;

public slots:
    void onDiagramSelectionChanged(const UComponentGuiContext& ctx);
    void onProjectLoaded(const QString& path);
    void onProjectClosed();
    void onChannelChanged(int index);

signals:
    void contextChanged(const LLMGuiContext& ctx);
};
```

### 3.2 Подключения (нормативные)

| Источник | Сигнал | Слот bridge |
|----------|--------|-------------|
| `UModernDiagramWidget` / container | `openComponentGuiRequested` / selection | `onDiagramSelectionChanged` |
| `UGEngineControlWidget` | `loadProjectExternal` / close | project slots |
| Channels widget | channel switch | `onChannelChanged` |
| `UGuiModelSnapshot` | internal update (optional) | обновить fingerprint |

**Реализация host** (`UGEngineControlWidget`):

```cpp
#ifdef RDK_USE_LLM
    m_llmBridge = new ULlmGuiContextBridge(application, this);
    LlmGui::RegisterLlmUi(this, application, m_llmBridge);
    connect(modernDiagram, &UModernDiagramContainerWidget::openComponentGuiFromScheme,
            m_llmBridge, &ULlmGuiContextBridge::onDiagramSelectionChanged);
#endif
```

Файл bootstrap **не** должен include весь `UGEngineControlWidget.h` — только forward declarations где возможно.

---

## 4. `LlmGuiBootstrap`

```cpp
namespace LlmGui {
void RegisterLlmUi(UGEngineControlWidget* host,
                   RDK::UApplication* app,
                   ULlmGuiContextBridge* bridge);
}
```

Действия:
1. `host->registerCustomWidget` — dock «AI Assistant»
2. Menu: верхний уровень **AI Assistant** (перед **Window**): AI Assistant, AI Change Preview, separator, AI Assistant Settings
3. Shortcut: `Ctrl+Shift+A` (dock), `Ctrl+Shift+L` (settings)
4. Создать `ULlmAssistantDockWidget`, передать `bridge`, `ULLMAgentOrchestrator` facade

---

## 5. `ULlmAssistantDockWidget`

| UI элемент | Поведение |
|------------|-----------|
| Chat history | `QTextEdit` (HTML escaped) |
| Input | `QPlainTextEdit`, Send; отправка по **Ctrl+Enter** (default) или **Enter** (настройка); подпись Send: `Send (Ctrl+Enter)` / `Send (Enter)` |
| Request status | `QLabel` + indeterminate `QProgressBar` при ожидании ответа («Waiting for model response…») |
| Provider combo | Профили из `ULLMSettingsStore` + Settings |
| Status | Local / Cloud / key hint |
| Ответ LLM | Token stream (`LLMStreamHandlers`) для OpenAI-compat / Ollama / EmbeddedLlama (TD-045) |
| Cancel | `orchestrator.cancel()` во время запроса |
| HITL TTL | `QTimer` 10 min → `rejectPending`; реальный `pending_confirmation_id` из ответа |
| Run plan | `confirmPlanExecution()` |
| Resume / Rollback plan | После checkpoint (`resumePlanExecution` / `rollbackPlanExecution`); UI показывает `LLMFinalResponse.rollback_status` (`rolled_back`, `partial_rollback`, `rollback_failed`, …) |
| Apply / Reject | `confirmPending()` / `rejectPending()` (skipped when auto-apply enabled) |
| Settings | **Allow LLM write tools**; **Apply write tools automatically** (`LLM/llm_auto_apply_writes`); **Context acquisition** (`LLM/context_acquisition_mode`: `auto` / `minimal`); **Autonomous mode** (`LLM/autonomous_mode`: off / strict / semi_auto) |

**Потоки:** orchestrator в `QThread` worker или `QtConcurrent::run` + signals `finished` — **запрещено** блокировать GUI на curl/LLM.

**Presentation sink:** `ULlmQtPresentationSink::invokeHostSynchronized` выполняет configuration tools на **GUI thread** (обязательно при auto-apply и worker orchestrator). После успеха `apply` вызывает `refreshLlmPresentationShell()`, `onProjectLoaded` / `onProjectClosed`, recent configs. Headless: sink не регистрируется.

**Таймаут:** `NMSDK_LLM_PRESENTATION_TIMEOUT_MS` (default 30000) — worker ждёт `BlockingQueuedConnection` не дольше этого значения; при таймауте UI refresh пропускается (без падения tool).

**Session busy / deferred UI:** пока orchestrator держит `session_id` busy (async message или plan execute/resume/rollback), `discardSession` не вызывается; **New chat**, смена проекта и переключение архива чата откладываются до `flushDeferredUiActions()` после завершения запроса. Plan/Apply при busy показывают ошибку «Session busy». См. [Context-Memory.md](Context-Memory.md).

---

## 6. `ULlmChangePreviewWidget` (HITL)

Показывает текст превью плана (`formatExecutionPlanPreview` — шаги и статусы).

Открывается через `LlmGui::showPlanPreview()` при `pending_plan` / paused plan.

Write HITL для одиночных tools — кнопки **Apply/Reject** в assistant dock (`confirmPending` / `rejectPending`).

---

## 7. Feature flags

| Ключ QSettings | Env | Default |
|--------------|-----|---------|
| `LLM/Enabled` | `NMSDK_ENABLE_LLM` | `false` если не собрано; если собрано `true` |
| `LLM/ShowDockOnStartup` | — | `false` |
| `LLM/preferred_response_language` | — | `""` = Auto (system → en/ru/de/fr/zh → en) |
| `LLM/send_shortcut` | — | `ctrl_enter` (default) или `enter` |

Если бинарник **без** `RDK_USE_LLM` — пункты меню отсутствуют на этапе компиляции.

---

## 8. Файлы и CMake

```
Rdk/GUI/Qt/Llm/
  LlmGuiBootstrap.h
  LlmGuiBootstrap.cpp
  ULlmGuiContextBridge.h
  ULlmGuiContextBridge.cpp
  ULlmAssistantDockWidget.h
  ULlmAssistantDockWidget.cpp
  ULlmAssistantDockWidget.ui
  ULlmChatHistoryArchive.h
  ULlmChatHistoryArchive.cpp
  ULlmChatHistoryDialog.h
  ULlmChatHistoryDialog.cpp
  ULlmChangePreviewWidget.h
  ULlmChangePreviewWidget.cpp
  ULlmProviderSettingsWidget.h
  ULlmProviderSettingsWidget.cpp
```

`App/NeuroModeler/CMakeLists.txt` — добавить sources **только** `if(RDK_USE_LLM)`.

---

## 9. Логирование в UI

Категория glog/Qt: `LLM`. Дублировать краткие строки в `ULoggerWidget` (опционально, фаза 3):

```
[LLM] tool=get_net_snapshot ok components=42 trace=7f3a...
```

Без API keys и без полного system prompt.

---

## 10. Chat history (`Bin/AiChats`)

NeuroModeler persists a **human-readable HTML log** of the assistant dock separately from orchestrator JSON sessions.

| Store | Path | Purpose |
|-------|------|---------|
| HTML archive | `{bin_root}/AiChats/YYYY-MM-DD/YYYY-MM-DD_HH-MM-SS-NeuroModelerChat.html` | QTextEdit-compatible transcript (`<b>You:</b>`, `<b>Assistant:</b>`, …) |
| JSON session | `{repository_root}/LLM/sessions/{session_id}.json` | Tool loop, pending confirmation/plan, resume for **Continue** |

### 10.1 Components

| Class | Role |
|-------|------|
| `ULlmChatHistoryArchive` | Filesystem I/O (no Qt): create `AiChats/` tree, append HTML, list/load |
| `ULlmChatHistoryDialog` | History browser: **Open** (read-only) / **Continue** (if JSON exists) |
| `ULlmAssistantDockWidget` | Writes archive on send/stream; **History...** entry point |

`bin_root` comes from `ILLMProjectContextProvider::paths()` (NeuroModeler: `repository_root/Bin`). Directories are created automatically (`create_directories`).

### 10.2 User flows

1. **Live chat** — first **Send** creates the `.html` file; each turn appends before `</body>`. **New chat** finalizes the previous file (`nmsdk-archived=true`) and starts a new `gui-<uuid>` session.
2. **History → Open** — loads `<body>` HTML into the dock (read-only banner; Send disabled). Use **New chat** to return to a live session.
3. **History → Continue** — `ULLMAgentOrchestrator::tryResumeSession` loads JSON; dock rebuilds history from `ConversationState::messages` and restores pending HITL/plan buttons when present.

### 10.3 Environment

| Variable | Effect |
|----------|--------|
| `NMSDK_LLM_DISABLE_CHAT_ARCHIVE=1` | Disables archive writes and hides **History...** |

### 10.4 Core API (resume)

```cpp
ILLMProjectContextProvider* LLMServices::projectContext() const;
bool ULLMAgentOrchestrator::tryResumeSession(const std::string& session_id);
const ConversationState* LLMServices::conversationState(const std::string& session_id) const;
```

If JSON is missing, **Continue** is disabled; **Open** still works from HTML alone.
