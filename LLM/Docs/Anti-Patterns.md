# Anti-Patterns

Явные запреты для реализации и code review.

| # | Антипаттерн | Почему | Правильно |
|---|-------------|--------|-----------|
| 1 | LLM вызывает `UEngine`/`UNet` напрямую | Нет audit/policy | `ULLMToolGateway` |
| 2 | Hardcoded `Bin/` в `Rdk/LLM` | Ядро не переносимо | `ILLMProjectContextProvider` |
| 3 | Генерация SQL/XML для apply | Injection, corruption | High-level tools |
| 4 | Write без confirmation | Data loss | HITL preview; optional auto-apply in Settings; plans still need Run plan |
| 5 | Правила только в system prompt | Jailbreak | `ULLMPolicyEngine` |
| 6 | Write по fuzzy name | Wrong component | `URdkEntityResolver` |
| 7 | Нет idempotency on add | Duplicates on retry | `client_request_id` |
| 8 | Все tools в каждом запросе | Cost, confusion | `ToolFilter` |
| 9 | Парсинг JSON из markdown | Fragile | Native tool_calls |
| 10 | LLM в GUI thread | UI freeze | Worker + signals |
| 11 | Unbounded autonomous agent | Risk | [Post-MVP-Implementation-Plan.md](Post-MVP-Implementation-Plan.md) Scenario D with limits |
| 12 | Global `RDK_USE_LLM` on rdk.static.qt | Breaks OFF build | Target-level define |
| 13 | vcpkg required on Linux for LLM | Project policy | apt packages |
| 14 | Monolith LLM in UGEngineControlWidget | Unmaintainable | LlmGuiBootstrap + bridge |
| 15 | Skip tool_result after tool_use | Provider errors | Always pair results |
| 16 | Full matrix in get_properties | Token explosion | `value_repr` truncate |
| 17 | Trust model for permissions | Escalation | Policy + domain checks |
| 18 | No audit log | Unsupportable | `ULLMAuditLog` mandatory |
