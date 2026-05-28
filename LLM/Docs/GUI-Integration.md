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

Обновление: bridge подписан на сигналы, **не** опрашивает engine в timer без необходимости.

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
| Settings | **Allow LLM write tools**; **Apply write tools automatically** (`LLM/llm_auto_apply_writes`); **Autonomous mode** (`LLM/autonomous_mode`: off / strict / semi_auto) |

**Потоки:** orchestrator в `QThread` worker или `QtConcurrent::run` + signals `finished` — **запрещено** блокировать GUI на curl/LLM.

**Presentation sink:** `ULlmQtPresentationSink::invokeHostSynchronized` выполняет configuration tools на **GUI thread** (обязательно при auto-apply и worker orchestrator). После успеха `apply` вызывает `refreshLlmPresentationShell()`, `onProjectLoaded` / `onProjectClosed`, recent configs. Headless: sink не регистрируется.

**Таймаут:** `NMSDK_LLM_PRESENTATION_TIMEOUT_MS` (default 30000) — worker ждёт `BlockingQueuedConnection` не дольше этого значения; при таймауте UI refresh пропускается (без падения tool).

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
