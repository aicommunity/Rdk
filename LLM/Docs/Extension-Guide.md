# Extension guide — tools, knowledge, libraries

Step-by-step guide for adding assistant capabilities. Architecture context: [Developer-Architecture.md](Developer-Architecture.md).

---

## 1. Add a read-tool in core

1. Define JSON Schema in [Tools-Contracts.md](Tools-Contracts.md) (or append with PR note).
2. Register in [`RegisterCoreRdkTools.cpp`](../Core/Tools/RegisterCoreRdkTools.cpp):
   - `LLMToolKind::Read`
   - `requires_confirmation = false`
   - Handler: `URdkDomainAccess` or `searchDocsWithIndex` / project context.
3. Add tool name to `kQueryTools()` in [`ULLMToolFilterBuilder.cpp`](../Core/Orchestrator/ULLMToolFilterBuilder.cpp).
4. Unit test: [`test_llm_tool_gateway.cpp`](../../Tests/Unit/LLM/test_llm_tool_gateway.cpp) or domain test.

---

## 2. Add a write-tool on UNet

1. Implement domain method in [`URdkDomainAccess`](../Core/Domain/URdkDomainAccess.cpp).
2. Register write def with `requires_confirmation = true` unless low-risk property (see [`ULLMWriteToolPolicy`](../Core/Policy/ULLMWriteToolPolicy.cpp)).
3. If args use `long_name` / `parent_long_name`, register fields in [`ULLMWriteArgumentNormalizer`](../Core/Domain/ULLMWriteArgumentNormalizer.cpp).
4. Add to `kMutateTools()` in `ULLMToolFilterBuilder.cpp`.
5. Policy: project loaded, `llm_write_enabled`, path/save gates — [Policy-and-Safety.md](Policy-and-Safety.md).
6. Tests: gateway HITL, policy deny, optional [`Test_LLM_WriteToolsP1`](../../Tests/Unit/LLM/test_llm_write_tools_p1.cpp).

**Do not** call `MModel_*` or `UApplication` from the handler; only domain/application facades.

---

## 3. Add library tools (`Libraries/*/Llm/`)

Pattern (Pulse/Motion/Hardware):

| Kind | Allowed | Example |
|------|---------|---------|
| Read | Yes | `search_pulse_docs`, `list_pulse_component_classes` |
| Write duplicate | **No** | Do not add `add_pulse_component` |

Steps:

1. Create `Libraries/MyLib/Llm/RegisterMyLibLlmTools.cpp` + header.
2. Register read tools with `mutation_hint`: “Use `add_component` with class from this list.”
3. Wire in [`NmsdkLlmProjectContext::registerExtraTools`](../../../App/NeuroModeler/NmsdkLlmProjectContext.cpp).
4. Add docs roots to [`NmsdkBuiltinKnowledgeCatalog`](../../../App/NeuroModeler/NmsdkBuiltinKnowledgeCatalog.cpp) and `loadedLibraries()`.
5. Add assist tool names to `kLibraryAssistTools()` in `ULLMToolFilterBuilder.cpp` if the model should see them in query/mutate filters.

Link library in NeuroModeler `CMakeLists.txt` like existing Pulse/Motion Llm sources.

---

## 4. Library focus (`ULLMLibraryScopeHint`)

When the user mentions a library (RU/EN keywords), the orchestrator:

- Injects a manifest section via `buildAgentManifest(..., user_text)`.
- Optionally fuzzy-matches class tokens in `ULLMLifecycleArgumentGate` for `add_component`.

To extend:

- Add keywords in [`ULLMLibraryScopeHint.cpp`](../Core/Orchestrator/ULLMLibraryScopeHint.cpp) (`detectLibraryScopeFromUserText`).
- Add class allowlist for fuzzy match (`componentClassAllowlistForScope`).
- Add aliases (e.g. `NPLNeuron` → `NPulseNeuron`) in `resolveKnownClassAlias`.

**Rule:** mutations stay on core `add_component` / `set_property`; library layer only narrows **read** hints and class resolution.

---

## 5. Knowledge and index

| Step | Action |
|------|--------|
| Catalog | Register doc/source roots in `NmsdkBuiltinKnowledgeCatalog` |
| Libraries table | Update `loadedLibraries()` + [Project-Context-NMSDK.md](Project-Context-NMSDK.md) |
| Rebuild index | `cmake --build build-llm-ci --target llm-index-pack` |
| Verify | `Test_LLM_DocRetrieval`, `Test_LLM_KnowledgeIndex` |

See [Knowledge-Sources.md](Knowledge-Sources.md). Post-MVP: YAML catalog (TD-031), ctags chunks (TD-034) — [Post-MVP-Implementation-Plan.md](Post-MVP-Implementation-Plan.md).

---

## 6. Entity resolution

- Implement synonyms or scoring in [`URdkEntityResolver`](../Core/Domain/URdkEntityResolver.cpp).
- Orchestrator returns `needs_entity_clarification` with JSON candidates.
- GUI: assistant dock shows choices (Post-MVP: dedicated flag TD-035).

See [Entity-Resolution.md](Entity-Resolution.md).

---

## 7. Lifecycle tools and GUI thread

All handlers that call `URdkApplicationCommands` must use:

```cpp
return invokeApplicationTool(activeSink(), [&]() {
    return commands->createConfiguration(args);
});
```

With `ULlmQtPresentationSink`, `invokeApplicationTool` → `invokeHostSynchronized` runs on the **GUI thread**. Never call `CreateProject` / `OpenProject` directly from a registry handler on the worker thread.

See [Application-Commands.md](Application-Commands.md), [GUI-Integration.md](GUI-Integration.md).

**UI panel tools:** `show_ui_panel` / `open_component_gui_tab` are GUI actions triggered through `LLMPresentationEvent.show_panel` (no direct Qt calls from the tool handler). The Qt sink applies the event on the GUI thread.

---

## 8. Manifest and intent

- **Intent:** extend rules in [`ULLMIntentParser`](../Core/Orchestrator/ULLMIntentParser.cpp) or `NMSDK_LLM_INTENT_LLM=1`.
- **Manifest:** [`buildAgentManifest`](../Core/Orchestrator/ULLMAgentManifestBuilder.cpp) — pass `user_text` for library focus sections.
- **Filter:** `buildToolFilter` — keep mutate/query sets small.

---

## 9. PR checklist

- [ ] JSON Schema + `additionalProperties: false` where required
- [ ] Tool in correct filter set (query vs mutate)
- [ ] Policy paths / project_loaded flags
- [ ] Audit events on invoke (gateway automatic)
- [ ] `Test_LLM_ToolSchemaRegression` / gateway unit test
- [ ] Breaking schema → bump `TOOL_REGISTRY_VERSION` + audit note
- [ ] Update [Tools-Contracts.md](Tools-Contracts.md) and [Extension-Guide.md](Extension-Guide.md) if pattern is new
- [ ] [TECH-DEBT.md](../TECH-DEBT.md) row if deferring follow-up
- [ ] No hardcoded `Bin/` paths in `Rdk/LLM/Core`
