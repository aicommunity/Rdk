# Entity Resolution

## RU

## 1. Проблема

Пользователь пишет: «модель», «нейрон 3», «Arduino» — неоднозначно. **Write запрещён** без canonical `long_name`.

**GuiTurnPin + canonical paths (TD-135–136):** relative hints resolve under `ConversationState::active_turn_pin->at_user_message` (diagram scope / current component at user Send). Tool JSON Schema may mark fields with `"x-llm-semantic": "entity_long_name"`; gateway runs `canonicalizeEntityPaths` before handler invoke. `add_component` parent defaults use `resolveValidAddParent` when `Model` is not a valid container.

---

## EN

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
