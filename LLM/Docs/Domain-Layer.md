# Domain Layer — UApplication, UNet

## RU

## 1. Определение «домена» в RDK LLM

**Домен** — не отдельный микросервис, а **слой приложения RDK**:

```
UApplication
  └── UEngineControl / UEngine
        └── UEnvironment (каналы, корневые контейнеры)
              └── UStorage (классы, фабрики)
              └── UNet / UContainer / UComponent (граф модели)
```

**Наиболее полный источник данных о компоненте и связях — `UNet`**, т.к. он агрегирует:
- дочерние компоненты (`GetComponentByIndex`, `FindComponentByNameAndType`);
- связи (`GetComponentInputLinks`, `GetComponentOutputLinks`, `GetPersonalLinks`);
- алиасы свойств (`PropertyAliases`).

`UGuiModelSnapshot` — **кэш для GUI**, может использоваться read-tools как быстрый путь, но **источник истины для write** — Domain через `URdkDomainAccess`.

---

## EN

## 2. Класс `URdkDomainAccess`

### 2.1 Назначение

Единственный фасад для `ULLMToolExecutor`. Запрещено вызывать `UEngine`, `UNet` из других пакетов LLM.

### 2.2 Конструкция

```cpp
namespace RDK::LLM {

class URdkDomainAccess {
public:
    explicit URdkDomainAccess(RDK::UApplication* app);

    RDK::UApplication* application() const;
    RDK::UEngine* engine() const;           // через UEngineControl, может быть null
    RDK::UEnvironment* environment(int channel_index = 0) const;
    RDK::UStorage* storage() const;

    /// Корневой UNet активного канала (или nullptr)
    RDK::UEPtr<RDK::UNet> rootNet(int channel_index = 0) const;

    // --- Read (фаза 1) ---
    DomainStatus listComponents(NetSnapshot& out, int channel_index = 0) const;
    DomainStatus getComponentDetail(const std::string& long_name,
                                    ComponentDetail& out, int channel_index = 0) const;
    DomainStatus listLinks(const std::string& long_name, LinkList& out,
                           int channel_index = 0) const;
    DomainStatus listRegisteredClassNames(std::vector<std::string>& out) const;

    // --- Write (фаза 2) ---
    DomainStatus addComponent(const AddComponentRequest& req, std::string& out_long_name);
    DomainStatus removeComponent(const std::string& long_name);
    DomainStatus setProperty(const SetPropertyRequest& req);
    DomainStatus connectComponents(const ConnectRequest& req);

    // --- Project ---
    DomainStatus loadProject(const std::string& xml_path);
    DomainStatus saveProject(const std::string& xml_path);
    DomainStatus validateProjectDryRun(std::vector<std::string>& warnings) const;

    DomainSessionInfo sessionInfo() const;  // проект открыт, read-only, channel count
};

} // namespace RDK::LLM
```

### 2.3 Получение указателей (нормативно)

| Метод | Источник в RDK |
|-------|----------------|
| `application()` | Конструктор (из GUI / NeuroModeler) |
| `engine()` | `app->GetEngineControl()` → `GetEngine()` (проверить null) |
| `environment(ch)` | `engine->GetEnvironment()` + выбор канала |
| `storage()` | `engine->GetStorage()` после Init |
| `rootNet(ch)` | Корневой контейнер канала, `dynamic_pointer_cast<UNet>` |

**Если `engine() == nullptr`:** все tools возвращают `DomainStatus::NotInitialized` — orchestrator переводит в `cannot_execute`.

### 2.4 `NetSnapshot` (формат для LLM, не полный dump)

```json
{
  "channel_index": 0,
  "components": [
    {
      "long_name": "Model.Channel0.Net1",
      "short_name": "Net1",
      "class_name": "UNet",
      "parent_long_name": "Model.Channel0",
      "child_count": 3
    }
  ],
  "links": [
    {
      "from_long_name": "/ch0/MModel.Neuron1",
      "from_property": "Output",
      "to_long_name": "/ch0/MModel.Neuron2",
      "to_property": "Input"
    }
  ],
  "links_truncated": false,
  "truncated": false,
  "max_components_applied": 500
}
```

**Лимиты (policy + domain):**
- `max_components_applied`: 500 по умолчанию (конфиг `llm.context.max_components`);
- `links_truncated`: true когда достигнут лимит `NMSDK_LLM_SNAPSHOT_MAX_LINKS` (default 2000);
- при превышении компонентов `truncated: true` и сообщение модели «показана часть схемы».

Обход: DFS от `rootNet`, без сериализации значений матриц целиком (только scalar/meta в `get_component_properties`).

**Пагинация связей:** для полного обхода без snapshot cap используйте read-tool `list_model_links` (`offset`, `limit`, optional `root_long_name`). Реализация: `ULLMModelLinkWalker` + `URdkDomainAccess::listModelLinks`.

### 2.5 Write-операции и `rdk_init`

| Операция | Предпочтительный путь | Запасной |
|----------|----------------------|----------|
| `add_component` | `GetEngineLockTimeout` → `model->GetComponentL(parent_path)` → `Storage->TakeObject` → `AddComponent`; `out_long_name` via `GetLongName` | — |
| `set_property` | Прямой доступ к `UComponent` property по LongName | — |
| `load/save project` | `UApplication::OpenProject` / `SaveProject(As)` | — |

**Инвариант:** перед write `URdkDomainAccess` проверяет `sessionInfo().project_loaded == true`.

---

## 3. Маппинг tools → Domain

| Tool | Domain метод |
|------|----------------|
| `get_net_snapshot` | `listComponents` + links (capped) |
| `list_model_links` | `listModelLinks` (paginated strict quads) |
| `find_component` | обход + fuzzy match (см. Entity-Resolution) |
| `get_component_properties` | `getComponentDetail` |
| `list_registered_classes` | `listRegisteredClassNames` + ClDesc provider |
| `add_component` | `addComponent` |
| `set_property` | `setProperty` |
| `connect_components` | `connectComponents` → `MModel_CreateLinkByName` |
| `disconnect_components` | internal rollback → `Model_BreakLinkByName` |
| `load_project` / `load_configuration` | `URdkApplicationCommands::loadConfiguration` → `OpenProject` |
| `save_project` / `save_configuration` | `URdkApplicationCommands::save*` → `SaveProject` / `SaveProjectAs` |
| `create_configuration` / `close_configuration` | `URdkApplicationCommands` lifecycle |

---

## 4. Каналы (multi-channel)

- `UApplication` / `UEngine` поддерживают несколько calculation channels.
- `LLMGuiContext` передаёт `channel_index` из UI (активная вкладка канала).
- Все domain-методы принимают `channel_index`; default `0`.
- Tools: опциональное поле `channel_index` в schema (integer, minimum 0).

---

## 5. Ошибки Domain (enum)

```cpp
enum class DomainStatusCode {
    Ok,
    NotInitialized,
    ProjectNotLoaded,
    ComponentNotFound,
    AmbiguousComponent,
    ClassNotFound,
    PropertyNotFound,
    InvalidPropertyValue,
    LinkFailed,
    PolicyDenied,
    IOError
};

struct DomainStatus {
    DomainStatusCode code;
    std::string message;  // для tool_result, не для сырого throw в GUI thread
};
```

Gateway преобразует в JSON:

```json
{ "ok": false, "error_code": "ComponentNotFound", "message": "..." }
```

---

## 6. Threading

| API | Thread |
|-----|--------|
| `URdkDomainAccess` (UNet read/write) | Orchestrator **worker** thread (NeuroModeler: `QtConcurrent` from dock) |
| `URdkApplicationCommands` (create/load/save) | **GUI thread** when `ULlmQtPresentationSink` is set (`invokeHostSynchronized`); else worker (headless) |

- Мутации engine должны использовать существующий lock API (`UEngine` lock / `GetEngineLock`) — **тот же**, что RPC и `rdk_init`.
- Application handlers must use `invokeApplicationTool` — never call `CreateProject` / `OpenProject` from a tool handler on the worker thread when a GUI sink exists.

See [Application-Commands.md](Application-Commands.md), [Developer-Architecture.md](Developer-Architecture.md) §4.

---

## 7. Тестирование Domain

- Mock `UApplication` с тестовым `UStorage` + минимальный `UNet` (см. `Rdk/Tests/Unit/Engine/`).
- Без реального LLM: прямые вызовы `URdkDomainAccess` из unit-тестов.
