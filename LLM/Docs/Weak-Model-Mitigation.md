# Weak-model mitigation (Capability Risk Gate)

## RU

Практика парирования слабых LLM в агентах хорошо изучена. NeuroModeler реализует **Capability Risk Gate (CRG)** — black-box контур без fine-tune базовой модели.

### Литература (семейства)

1. **Cascade / routing** — слабая модель отвечает, если «достаточно уверена»; иначе эскалация на сильную: [FrugalGPT](https://arxiv.org/abs/2305.05176), [AutoMix](https://arxiv.org/pdf/2310.12963), [HybridLLM](https://arxiv.org/pdf/2404.14618), [RouteLLM](https://arxiv.org/html/2406.18665), [Gatekeeper](https://arxiv.org/pdf/2502.19335), [SATER](https://arxiv.org/pdf/2510.05164).
2. **Abstention / refusal** — не отвечать, если риск ошибки выше порога: UniCR (evidence → calibrated p(correct) → refuse), conformal abstention, [AgentAbstain](https://arxiv.org/html/2607.10059v1); open rubrics (штраф за угадывание).
3. **Tool-agent confidence** — калибровать уверенность до tool call: [MICE for CATs](https://aclanthology.org/2025.naacl-long.615.pdf).

**Не делаем в v1:** Gatekeeper/SATER fine-tune, BERT RouteLLM, N-сэмпл self-consistency на каждый ход.

### Контракт CRG (DD-CAP-001 / TD-172)

Сигналы → score → `{Continue, Escalate, Abstain}`.

| Фаза | Сигналы |
|------|---------|
| Pre | Слабый Cortex-профиль/модель + сложная цель (Mutate write, description, structure/connect/calc); низкий intent confidence |
| Mid | Act-or-Clarify recovery без успеха; повторные tool errors (`PATH_NOT_ALLOWED`, schema); prose/exhaust без нужного write; cascade уже использован |

**Действия**

1. Recorded/FastPath (как раньше) — предпочтительный обход слабого ReAct.
2. **Pre-escalate / cascade once:** `model_override` на более сильный профиль (`ollama-thinking`), audit `capability_cascade_retry` / `capability_pre_escalate`.
3. **Abstain:** `error.model_too_weak_for_request` (ru/en); **запрет** `query_soft_fallback` при high risk; audit `capability_risk_abstain`.

Пороги: `NMSDK_LLM_CAPABILITY_RISK_TAU` (default `0.55`), `NMSDK_LLM_CAPABILITY_RISK_DISABLE=1` отключает gate.

Реализация: `ULLMCapabilityRiskGate`, хуки в `ULLMAgentOrchestrator`. См. [Unified-Turn-Contract.md](Unified-Turn-Contract.md).

---

## EN

Scientific practice for weak LLMs in agents is established. NeuroModeler ships a **Capability Risk Gate (CRG)**: black-box evidence fusion (UniCR-style) + one-shot cascade (FrugalGPT/AutoMix), without fine-tuning the base model.

### Literature families

1. **Cascade / routing** — defer hard queries to a stronger model: FrugalGPT, AutoMix, HybridLLM, RouteLLM, Gatekeeper, SATER.
2. **Abstention** — refuse when risk exceeds a budget: UniCR, conformal abstention, AgentAbstain; open rubrics discourage guessing.
3. **Tool-agent confidence** — calibrate before tool execution: MICE for CATs.

**Out of scope for v1:** Gatekeeper/SATER weight tuning, BERT RouteLLM, per-turn self-consistency sampling.

### CRG contract (DD-CAP-001 / TD-172)

Signals → score → `{Continue, Escalate, Abstain}`.

| Phase | Signals |
|-------|---------|
| Pre | Weak Cortex profile/model + complex goal (Mutate write, description, structure/connect/calc); low intent confidence |
| Mid | Act-or-Clarify recovery without success; repeated tool errors; exhaust without required write; cascade already used |

**Actions**

1. Prefer Recorded/FastPath when applicable.
2. **Pre-escalate / cascade once** via `model_override` to a stronger catalog profile (`ollama-thinking`); audit `capability_pre_escalate` / `capability_cascade_retry`.
3. **Abstain** with `error.model_too_weak_for_request`; block `query_soft_fallback` under high risk; audit `capability_risk_abstain`.

Env: `NMSDK_LLM_CAPABILITY_RISK_TAU` (default `0.55`), `NMSDK_LLM_CAPABILITY_RISK_DISABLE=1` disables the gate.

Code: `ULLMCapabilityRiskGate`, orchestrator hooks. See [Unified-Turn-Contract.md](Unified-Turn-Contract.md).
