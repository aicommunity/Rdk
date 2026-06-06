# Extension guide — tools, knowledge, libraries

## RU

Step-by-step guide for adding assistant capabilities. Architecture context: [Developer-Architecture.md](Developer-Architecture.md).

---

## EN

## 1. Add a read-tool in core

1. Define JSON Schema in [Tools-Contracts.md](Tools-Contracts.md) (or append with PR note).
2. Register in [`RegisterCoreRdkTools.cpp`](../Core/Tools/RegisterCoreRdkTools.cpp):
   - `LLMToolKind::Read`
   - `requires_confirmation = false`
   - Handler: `URdkDomainAccess` or `searchDocsWithIndex` / project context.
3. Add tool name to `kQueryTools()` in [`ULLMToolFilterBuilder.cpp`](../Core/Orchestrator/ULLMToolFilterBuilder.cpp).
4. Unit test: [`test_llm_tool_gateway.cpp`](../../Tests/Unit/LLM/test_llm_tool_gateway.cpp) or domain test.

---
