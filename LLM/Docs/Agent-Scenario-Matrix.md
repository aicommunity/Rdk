# Agent scenario matrix (typовые запросы ассистента)

## RU

JSON-каталог сценариев для регрессии orchestrator, tool gateway и argument gate без GUI и без обязательного Ollama.

## Расположение

| Путь | Назначение |
|------|------------|
| `Rdk/Tests/Fixtures/LLM/agent-scenarios/` | Корень каталога |
| `deterministic/` | Mock-провайдер (`tier=deterministic`) |
| `e2e/` | Lab Ollama (`tier=e2e`, `GTEST_SKIP` если lab недоступен) |
| `mock-patterns/` | Переиспользуемые фрагменты `mock_script` |
| `manifest.json` | Список suite для CI |

## Запуск

```bash
cmake -S . -B build-llm-ci -DRDK_USE_LLM=ON -DBUILD_TESTING=ON
cmake --build build-llm-ci --target Test_LLM_AgentScenarios llm_scenario_runner -j"$(nproc)"

# Deterministic (обязательно в CI)
Bin/Platform/Linux/Test_LLM_AgentScenarios \
  --gtest_filter='Deterministic/*:AgentScenarioLoader.*:AgentScenarioEvalUnit.*'

# E2E (опционально; skip если lab Ollama недоступен)
Bin/Platform/Linux/Test_LLM_AgentScenarios --gtest_filter='E2e/*'

# JSON-отчёт локально
Bin/Platform/Linux/llm_scenario_runner --tier deterministic --root Rdk/Tests/Fixtures/LLM/agent-scenarios
```

`Scripts/ci-llm-agent-scenarios.sh` вызывается из `Scripts/ci-llm-linux.sh` после agentic-risk.

## Схема fixture (v1)

Обязательные поля: `schema_version`, `id`, `tier`, `user_text`, `expect`. Для `tier=e2e` — блок `e2e` с `goal` ([`E2eGoalKind`](../../Tests/Unit/LLM/llm_e2e_analyzer.h)).

Для deterministic: `mock_script[]` — очередь ответов mock-провайдера (`tool_calls` и/или `text`). Поле `registry_profile`: `core` (по умолчанию) или `full` (Pulse/Hardware/MotionControl Llm tools).

См. примеры в `deterministic/graph/graph_snapshot.json`, `deterministic/no-suitable-tool/no_tool_email_ru.json`.

## Обязательные suites

| Suite | Примеры id | Что проверяем |
|-------|------------|---------------|
| graph | `graph_snapshot`, `graph_snapshot__ru_modules` | `get_net_snapshot`, normalize args |
| links | `link_connect`, `link_connect_then_disconnect` | connect/disconnect |
| lifecycle | `life_load_recent`, `life_create` | configuration tools |
| no-suitable-tool | `no_tool_email_ru` | 2 раунда → `no_suitable_tool`, не `Done.` |
| observability | `obs_policy`, `obs_disabled` | system log tools / env disable |
| negative | `neg_write_disabled`, `neg_empty_done` | policy / пустой ответ |
| library (`full`) | `lib_pulse_search` | library doc tools |
| context | `ctx_gui_focus_find`, `ctx_gui_focus_set_property`, `ctx_multi_turn_entity`, `ctx_resume_gui_persist`, `ctx_query_prefetch` | GUI snapshot, entity cache, session reload, query prefetch hint |

## Как добавить сценарий

1. Выберите `tier` (`deterministic` / `e2e`) и `suite`.
2. Скопируйте фрагмент из `mock-patterns/` (для D).
3. Заполните `expect` / `e2e` (флаги, `tools_invoked_any`, `final_text_not_contains`).
4. Перегенерируйте каталог при массовых правках: `python3 Scripts/generate_agent_scenario_fixtures.py`
5. Прогон: `Test_LLM_AgentScenarios --gtest_filter='*your_id*'`

## Переменные окружения (runner)

| Переменная | Значение в тестах |
|------------|-------------------|
| `NMSDK_LLM_INTENT_LLM` | unset (rule-based intent) |
| `NMSDK_LLM_TRANSLATE_QUERIES` | `0` (не съедать mock-очередь переводом) |
| `NMSDK_LLM_DISABLE_SYSTEM_LOG` | только в `obs_disabled` |

## Масштаб v1

~**61** deterministic кейсов (с `variants`) + **15** e2e. Расширение — новые JSON в подпапках; loader подхватывает `**/*.json` рекурсивно.

---

## EN

JSON scenario catalog for orchestrator, tool gateway, and argument gate regression without GUI and without mandatory Ollama.

## Location

| Path | Purpose |
|------|---------|
| `Rdk/Tests/Fixtures/LLM/agent-scenarios/` | Catalog root |
| `deterministic/` | Mock provider (`tier=deterministic`) |
| `e2e/` | Lab Ollama (`tier=e2e`, `GTEST_SKIP` if lab unavailable) |
| `mock-patterns/` | Reusable `mock_script` fragments |
| `manifest.json` | Suite list for CI |

## Running

```bash
cmake -S . -B build-llm-ci -DRDK_USE_LLM=ON -DBUILD_TESTING=ON
cmake --build build-llm-ci --target Test_LLM_AgentScenarios llm_scenario_runner -j"$(nproc)"

# Deterministic (required in CI)
Bin/Platform/Linux/Test_LLM_AgentScenarios \
  --gtest_filter='Deterministic/*:AgentScenarioLoader.*:AgentScenarioEvalUnit.*'

# E2E (optional; skip if lab Ollama unavailable)
Bin/Platform/Linux/Test_LLM_AgentScenarios --gtest_filter='E2e/*'

# JSON report locally
Bin/Platform/Linux/llm_scenario_runner --tier deterministic --root Rdk/Tests/Fixtures/LLM/agent-scenarios
```

`Scripts/ci-llm-agent-scenarios.sh` is invoked from `Scripts/ci-llm-linux.sh` after agentic-risk.

## Fixture schema (v1)

Required fields: `schema_version`, `id`, `tier`, `user_text`, `expect`. For `tier=e2e` — `e2e` block with `goal` ([`E2eGoalKind`](../../Tests/Unit/LLM/llm_e2e_analyzer.h)).

For deterministic: `mock_script[]` — mock provider response queue (`tool_calls` and/or `text`). Field `registry_profile`: `core` (default) or `full` (Pulse/Hardware/MotionControl Llm tools).

See examples in `deterministic/graph/graph_snapshot.json`, `deterministic/no-suitable-tool/no_tool_email_ru.json`.

## Required suites

| Suite | Example ids | What we verify |
|-------|-------------|----------------|
| graph | `graph_snapshot`, `graph_snapshot__ru_modules` | `get_net_snapshot`, normalize args |
| links | `link_connect`, `link_connect_then_disconnect` | connect/disconnect |
| lifecycle | `life_load_recent`, `life_create` | configuration tools |
| no-suitable-tool | `no_tool_email_ru` | 2 rounds → `no_suitable_tool`, not `Done.` |
| observability | `obs_policy`, `obs_disabled` | system log tools / env disable |
| negative | `neg_write_disabled`, `neg_empty_done` | policy / empty response |
| library (`full`) | `lib_pulse_search` | library doc tools |
| context | `ctx_gui_focus_find`, `ctx_gui_focus_set_property`, `ctx_multi_turn_entity`, `ctx_resume_gui_persist`, `ctx_query_prefetch` | GUI snapshot, entity cache, session reload, query prefetch hint |

## How to add a scenario

1. Choose `tier` (`deterministic` / `e2e`) and `suite`.
2. Copy a fragment from `mock-patterns/` (for D).
3. Fill in `expect` / `e2e` (flags, `tools_invoked_any`, `final_text_not_contains`).
4. Regenerate the catalog for bulk edits: `python3 Scripts/generate_agent_scenario_fixtures.py`
5. Run: `Test_LLM_AgentScenarios --gtest_filter='*your_id*'`

## Environment variables (runner)

| Variable | Value in tests |
|------------|----------------|
| `NMSDK_LLM_INTENT_LLM` | unset (rule-based intent) |
| `NMSDK_LLM_TRANSLATE_QUERIES` | `0` (do not consume mock queue with translation) |
| `NMSDK_LLM_DISABLE_SYSTEM_LOG` | only in `obs_disabled` |

## v1 scale

~**61** deterministic cases (with `variants`) + **15** e2e. Extension — new JSON in subfolders; loader picks up `**/*.json` recursively.
