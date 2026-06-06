# Entity Resolution

## 1. Проблема

Пользователь пишет: «модель», «нейрон 3», «Arduino» — неоднозначно. **Write запрещён** без canonical `long_name`.

**GuiTurnPin + canonical paths (TD-135–136):** relative hints resolve under `ConversationState::active_turn_pin->at_user_message` (diagram scope / current component at user Send). Tool JSON Schema may mark fields with `"x-llm-semantic": "entity_long_name"`; gateway runs `canonicalizeEntityPaths` before handler invoke. `add_component` parent defaults use `resolveValidAddParent` when `Model` is not a valid container.

---

## 2. `URdkEntityResolver`

```cpp
class URdkEntityResolver {
public:
    explicit URdkEntityResolver(URdkDomainAccess& domain);

    ResolutionResult resolveComponent(const std::string& query,
                                      int channel_index,
                                      const std::optional<std::string>& class_filter);

    ResolutionResult resolveClassName(const std::string& query);
};
```

```cpp
struct ResolutionResult {
    enum class Status { Resolved, Ambiguous, NotFound };
    Status status;
    std::string canonical_long_name;  // if Resolved
    std::vector<Candidate> candidates;
};

struct Candidate {
    std::string long_name;
    std::string class_name;
    std::string short_name;
    double score;  // 0..1
};
```

---

## 3. Алгоритм `resolveComponent` (MVP)

1. Exact match `long_name` или `short_name` (case-sensitive как в RDK)
2. Prefix / substring match по `short_name`, `long_name`
3. Если `class_filter` — фильтр кандидатов
4. Scoring:
   - exact short_name: 1.0
   - exact long_name tail: 0.95
   - substring: 0.5–0.8
5. **Resolved** если top score ≥ 0.9 и (top - second) ≥ 0.15
6. **Ambiguous** если ≥2 кандидата с близкими scores
7. **NotFound** если пусто

---

## 4. Интеграция с orchestrator

Состояния ответа LLM (structured):

```json
{
  "assistant_state": "ask_clarification",
  "clarification": {
    "question": "Найдено несколько компонентов. Уточните:",
    "options": [
      { "label": "Net1 (UNet)", "canonical_long_name": "Model.Ch0.Net1" },
      { "label": "Net2 (UNet)", "canonical_long_name": "Model.Ch0.Net2" }
    ]
  }
}
```

GUI: кнопки выбора option → обновить `ConversationState.resolved_entities`.

---

## 5. Tool `find_component`

Основной read-tool для resolution. Write-tools **вызывают** resolver внутри gateway:

```cpp
// Before add_component to existing sibling — N/A
// Before set_property — resolve long_name argument
auto res = resolver.resolveComponent(args["long_name"], channel);
if (res.status != Resolved) return error AmbiguousOrNotFound;
args["long_name"] = res.canonical_long_name;
```

---

## 6. Class name resolution

`resolveClassName("arduino")` → fuzzy against `list_registered_classes` + ClDesc headers.

Порог: Levenshtein / contains, top match ≥ 0.85.

---

## 7. GUI focus shortcut

Если `LLMGuiContext.focused_component_long_name` не пуст:

- Запросы «этот компонент», «здесь», «выделенный» → auto-bind canonical без поиска
- Audit: `resolution_source: "gui_focus"`

---

## 8. Тесты

| Case | Expected |
|------|----------|
| unique short_name | Resolved |
| two "Source" | Ambiguous |
| empty graph | NotFound |
| gui focus + «измени свойство» | Resolved to focus |

Fixtures: `Rdk/Tests/Fixtures/LLM/entity/`.
