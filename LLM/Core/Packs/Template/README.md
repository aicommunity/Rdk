# Capability Pack template

Skeleton steps for a new pack (TD-165). Normative contract: [Capability-Packs.md](../../Docs/Capability-Packs.md).

## Steps

1. **Create** `UPackYourDomain.h` / `.cpp` (Builtin under `Core/Packs/Builtin/`, or library under `Libraries/*/Llm/`).
2. **Implement** `ILLMCapabilityPack`:
   - `id()` — stable string id (`your_domain`)
   - `match(snap)` — return `PackMatch{score, reason}` from user/planning text (and optional session signals)
   - `hints(snap)` — ephemeral markdown + `extra_tool_names` + `act_or_clarify_recovery_tools`
   - `tryRecorded(snap)` — optional; only for score band `>= 0.85`; must use `recordedToolInvoke`
3. **Register** in `RegisterBuiltinCapabilityPacks` or the library registration entry point.
4. **Tools** — register with `RegisterCoreRdkTools` / `Register*LibLlmTools` as usual; list names in the pack allowlist/recovery vectors.
5. **Test** match thresholds, Recorded short-circuit, and recovery tool list.

## Minimal stub

```cpp
class UPackYourDomain : public ILLMCapabilityPack {
public:
    const char* id() const override { return "your_domain"; }
    PackMatch match(const PackTurnSnapshot& snap) const override;
    PackHintContribution hints(const PackTurnSnapshot& snap) const override;
    // RecordedStrategyResult tryRecorded(PackTurnSnapshot& snap) override; // optional
};
```

## Score bands (DD-PACK-001)

| Score | Behavior |
|-------|----------|
| `>= 0.85` | Recorded short-circuit if implemented |
| `0.4 .. 0.85` | Hints + tool allowlist merge |
| `< 0.4` | Ignore |

## Library note

`Register*LibLlmTools` remains valid for tools. A pack is needed when the library owns a **scenario** (routing, FastPath, Act-or-Clarify recovery), not merely a catalog tool.
