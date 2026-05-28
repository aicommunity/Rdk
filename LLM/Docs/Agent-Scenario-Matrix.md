# Agent scenario matrix (typовые запросы ассистента)

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

Обязательные поля: `schema_version`, `id`, `tier`, `user_text`, `expect`. Для `tier=e2e` — блок `e2e` с `goal` ([`E2eGoalKind`](../Tests/Unit/LLM/llm_e2e_analyzer.h)).

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
