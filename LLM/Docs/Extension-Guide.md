# Extension guide — tools, knowledge, libraries, packs

## RU

Пошаговое руководство по расширению ассистента. Архитектура: [Developer-Architecture.md](Developer-Architecture.md), packs: [Capability-Packs.md](Capability-Packs.md).

**Основной путь (packs-first):** доменный сценарий = Capability Pack (`match` / hints / optional `tryRecorded` / recovery tools). Сырые tools регистрируются отдельно и подключаются через allowlist пака.

**Вторичный путь:** прямой `registerTool` без пака — для узких read/write tools без сценария.

---

## EN

## 1. Primary path — add a Capability Pack (TD-165)

1. Copy skeleton from [`Core/Packs/Template/README.md`](../Core/Packs/Template/README.md).
2. Implement `ILLMCapabilityPack` under `Core/Packs/Builtin/` **or** `Libraries/*/Llm/` (library-owned packs).
3. Register:
   - Builtin → [`RegisterBuiltinPacks.cpp`](../Core/Packs/RegisterBuiltinPacks.cpp)
   - Library → call `packs.registerPack(...)` from the library LLM registration hook (same place as `Register*LibLlmTools`).
4. Score bands (DD-PACK-001): `>= 0.85` Recorded; `0.4–0.85` hints + tools; `< 0.4` ignore.
5. Prefer `recordedToolInvoke` for any pre-LLM short-circuit (DD-MEM-001).
6. List Act-or-Clarify recovery tools in `PackHintContribution::act_or_clarify_recovery_tools`.
7. Unit-test match / Recorded / recovery contribution.

Template checklist: [Template/README.md](../Core/Packs/Template/README.md).

## 2. Secondary path — raw `registerTool`

Use when you need a tool without a domain scenario pack yet.

### 2.1 Core read tool

1. Define JSON Schema in [Tools-Contracts.md](Tools-Contracts.md) (or append with PR note).
2. Register in [`RegisterCoreRdkTools.cpp`](../Core/Tools/RegisterCoreRdkTools.cpp):
   - `LLMToolKind::Read`
   - `requires_confirmation = false`
   - Handler: `URdkDomainAccess` or `searchDocsWithIndex` / project context.
3. Add tool name to `kQueryTools()` in [`ULLMToolFilterBuilder.cpp`](../Core/Orchestrator/ULLMToolFilterBuilder.cpp).
4. Unit test: [`test_llm_tool_gateway.cpp`](../../Tests/Unit/LLM/test_llm_tool_gateway.cpp) or domain test.

### 2.2 Library tools (remain valid)

`Register*LibLlmTools` remains valid for tools. Packs wrap **domain scenarios** (match/hints/Recorded/recovery) and may list those tool names in `extra_tool_names` / recovery lists. Example library adapter: `Libraries/Nmsdk-PulseLib/Llm/UPackPulseDocs` (registered from `RegisterPulseLibLlmTools` when LLMServices is ready).

Examples: [`Libraries/Rdk-HardwareLib/Llm/`](../../../Libraries/Rdk-HardwareLib/Llm/), PulseLib / MotionControlLib similarly.

## 3. Knowledge / docs

- Project docs and index: [Project-Context-NMSDK.md](Project-Context-NMSDK.md).
- Ephemeral hints: prefer pack `hints()` markdown over hardcoding in the orchestrator.

## 4. Agent tools

`ask_user`, `propose_plan`, `search_tools`, `spawn_explore_subagent` — [`RegisterAgentTools.cpp`](../Core/Tools/RegisterAgentTools.cpp). Subagent explore runs via gateway + `ULLMSubagentRunner` (TD-164); nested spawn is denied inside the runner.

## 5. See also

| Topic | Doc |
|-------|-----|
| Pack contract | [Capability-Packs.md](Capability-Packs.md) |
| Tool gateway | [Tool-Gateway-and-Registry.md](Tool-Gateway-and-Registry.md) |
| Turn contract | [Unified-Turn-Contract.md](Unified-Turn-Contract.md) |
| Tech debt | [TECH-DEBT.md](../TECH-DEBT.md) |
