# Developer architecture — RDK LLM assistant

## RU

Normative overview for implementers and coding agents. Contract details remain in topic-specific docs; this file is the **single source of truth** for structure and request flow.

**See also:** [Extension-Guide.md](Extension-Guide.md) (how to add tools/knowledge), [Architecture.md](Architecture.md) (short index + CMake), [Post-MVP-Implementation-Plan.md](Post-MVP-Implementation-Plan.md) (deferred features).

---

## EN

## 1. Role of the LLM

The LLM is a **planner and intent interpreter**, not the source of truth and not a direct executor.

```
User → UI → Orchestrator → ILLMProvider
              ↓
        Tool Gateway → Policy
              ↓
        URdkApplicationCommands → UApplication   (configuration lifecycle)
        URdkDomainAccess        → UEngine/UNet   (graph read/write)
              ↓
        ILLMPresentationSink (shell refresh + GUI-thread host commands via `LLMPresentationEvent`)
              ↓
        ILLMProjectContextProvider + UDocSearchIndex (NMSDK knowledge)
```

The model must **never**:

- Apply SQL/XML or raw project files bypassing the gateway.
- Call `UEngine` / `UNet` / `UApplication` directly from provider output.
- Escalate permissions beyond `ULLMPolicyEngine` and session flags.

**Invariants** (full list): [README.md](README.md) § strict invariants.

---
