# Project Context — NMSDK (конечный проект)

## 1. Проблема

`Rdk/LLM` — **ядро**, переиспользуемое. Пути `Bin/`, набор библиотек и документация **специфичны для корневого репозитория NMSDK** (NeuroModeler).

Жёстко прошивать `/home/.../Bin` в `Rdk/LLM` **запрещено**.

---

## 2. Интерфейс `ILLMProjectContextProvider`

```cpp
namespace RDK::LLM {

struct ProjectPaths {
    std::filesystem::path repository_root;   // корень Nmsdk (где CMakeLists.txt)
    std::filesystem::path bin_root;          // обычно <root>/Bin
    std::filesystem::path cl_desc_root;      // <bin>/ClDesc или app->GetClDescPath()
    std::filesystem::path configs_root;      // <bin>/Configs
    std::filesystem::path docs_root;         // <root>/Docs и <bin>/Docs
};

class ILLMProjectContextProvider {
public:
    virtual ~ILLMProjectContextProvider() = default;

    virtual ProjectPaths paths() const = 0;

    /// Имена библиотек как в RdkLoadPredefinedLibraries (BasicLib → "Rdk-BasicLib" mapping)
    virtual std::vector<LibraryDescriptor> loadedLibraries() const = 0;

    /// Фрагмент ClDesc XML для ClassName (пусто если нет)
    virtual std::string clDescFragment(const std::string& class_name,
                                       const std::string& locale = "ru-RU") const = 0;

    /// Поиск по документации (Markdown), top-K фрагментов, max_chars
    virtual std::vector<DocSnippet> searchDocs(const std::string& query,
                                               int top_k = 5,
                                               int max_chars = 8000) const = 0;

    /// Опционально: регистрация доп. tools библиотеки
    virtual void registerExtraTools(ULLMToolRegistry& registry) {}
};

class ILLMProjectContextProviderRegistry {
public:
    void setPrimary(std::shared_ptr<ILLMProjectContextProvider> p);
    ILLMProjectContextProvider* primary() const;
};

} // namespace RDK::LLM
```

---

## 3. Реализация NMSDK: `NmsdkLlmProjectContext`

**Файлы (нормативные пути):**
- `App/NeuroModeler/NmsdkLlmProjectContext.h`
- `App/NeuroModeler/NmsdkLlmProjectContext.cpp`
- `App/NeuroModeler/NmsdkRegisterLlm.cpp`

### 3.1 Инициализация путей

```cpp
ProjectPaths NmsdkLlmProjectContext::paths() const {
    // repository_root: из app->GetWorkDirectory() подняться к корню ИЛИ env NMSDK_ROOT
    // cl_desc: app->GetClDescPath() относительно work dir → абсолютный
    // bin_root: work_dir + "/../Bin" или фиксированная схема NeuroModeler deploy
}
```

**Приоритет:**
1. `UApplication::GetClDescPath()`, `GetConfigsMainPath()`, `GetWorkDirectory()`
2. Переменная окружения `NMSDK_ROOT` (опционально, для CI)
3. Fallback: `repository_root / "Bin"`

### 3.2 `loadedLibraries()`

Статический список, **синхронизированный** с `Libraries/Libraries.cpp`:

| Library global | ClDesc folder | Docs |
|----------------|---------------|------|
| `RDK::BasicLibrary` | `BasicLibrary` / `Rdk-BasicLib` | — |
| `RDK::CvBasicLibrary` | `CvBasicLibrary` | — |
| `RDK::HardwareLibrary` | `HardwareLibrary` | `Libraries/Rdk-HardwareLib/Docs/` |
| `NMSDK::PulseLibrary` | `PulseLibrary` | `Libraries/Nmsdk-PulseLib/Docs/` |
| `NMSDK::MotionControlLibrary` | `MotionControlLibrary` | `Libraries/Nmsdk-MotionControlLib/Docs/` |

**Правило:** при добавлении библиотеки в `RdkLoadPredefinedLibraries` — **обязательно** обновить таблицу в этом документе и в `NmsdkLlmProjectContext.cpp`.

### 3.3 `clDescFragment(class_name)`

1. Перебор `paths().cl_desc_root / <lib> / ru-RU / <ClassName>.xml`
2. Если файл > 32 KiB — извлечь только `<Properties>` (простой XML slice, без полного DOM если тяжело)
3. Кэш в памяти на сессию (LRU 100 классов)

### 3.4 `searchDocs(query)`

Индекс (MVP — простой):
- Обход glob: `Docs/**/*.md`, `Bin/Docs/**/*.md`, `Libraries/*/Docs/**/*.md`
- Scoring: keyword match (MVP); post-MVP — embeddings offline
- Возврат: `{path, title, excerpt, score}`

**Не индексировать:** `Rdk/ThirdParty`, бинарники, `*.xml` проектов целиком.

---

## 4. Расширения per-library (`Libraries/*/Llm/`)

### 4.1 Когда создавать

Если библиотека добавляет **специфичные** tools или контекст (Hardware pinout, Pulse SNN params).

### 4.2 Структура (пример HardwareLib)

```
Libraries/Rdk-HardwareLib/Llm/
  HardwareLibLlmRegistration.cpp
  HardwareLibLlmTools.cpp
```

```cpp
void RegisterHardwareLibLlmTools(RDK::LLM::ULLMToolRegistry& reg,
                                 RDK::LLM::ILLMProjectContextProvider& ctx) {
    reg.registerTool(/* describe_arduino_board */, ...);
}
```

Вызов из `NmsdkRegisterLlm()` после primary provider.

### 4.3 Разделение: ядро vs библиотека

| Категория | Где |
|-----------|-----|
| Универсальные (net, component, project) | `Rdk/LLM` — **всегда** |
| Домен библиотеки (Arduino, NModel STDP) | `Libraries/*/Llm/` |
| ClDesc / Docs пути | `NmsdkLlmProjectContext` + library docs roots |

**Запрещено:** дублировать `add_component` в каждой библиотеке.

---

## 5. Retrieval в orchestrator

```cpp
struct RetrievedContext {
    std::string net_snapshot_json;       // from URdkDomainAccess
    std::vector<DocSnippet> doc_snippets;
    std::string cl_desc_for_focus_class; // if GUI selection known
    int estimated_tokens;
};
```

**Бюджет токенов (default):**
- net_snapshot: ≤ 4000 tokens (truncate)
- doc_snippets: ≤ 3000 tokens
- cl_desc fragment: ≤ 2000 tokens
- system prompt: ≤ 1500 tokens

Порядок урезания при переполнении: doc_snippets → net_snapshot (оставить только выбранный компонент + соседи) → cl_desc.

---

## 6. Регистрация при старте NeuroModeler

```cpp
#ifdef RDK_USE_LLM
void NmsdkRegisterLlm(RDK::UApplication* app) {
    auto ctx = std::make_shared<NmsdkLlmProjectContext>(app);
    RDK::LLM::GetContextRegistry().setPrimary(ctx);

    auto& tools = RDK::LLM::GetToolRegistry();
    RDK::LLM::RegisterCoreRdkTools(tools);  // в Rdk/LLM

    RegisterHardwareLibLlmTools(tools, *ctx);  // опционально, фаза 2
    // ...
}
#endif
```

Вызов: из `UGEngineControlWidget` constructor **после** `application` valid, **один раз** (std::call_once).

---

## 7. CMake (NMSDK)

```cmake
if(RDK_USE_LLM)
  target_sources(NeuroModeler PRIVATE
    App/NeuroModeler/NmsdkLlmProjectContext.cpp
    App/NeuroModeler/NmsdkRegisterLlm.cpp
  )
  target_link_libraries(NeuroModeler PRIVATE rdk.llm.core)
endif()
```

Per-library Llm — добавлять в `Libraries/Rdk-HardwareLib/CMakeLists.txt` только если `RDK_USE_LLM` и линковка через NeuroModeler (как `.gui`).
