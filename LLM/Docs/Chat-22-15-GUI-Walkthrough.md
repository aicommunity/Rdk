# Chat 22-15 — manual GUI walkthrough (TD-119)

## RU

Automated coverage: `Test_LLM_TD041WalkthroughSim`, `Test_LLM_AgentScenarios` (`link_*` fixtures), `ctest -R Test_LLM_`.

Use this checklist for an optional **human** spot-check in NeuroModeler after chat-22-15 fixes.

**Prerequisites:** built `NeuroModeler`, Ollama or configured cloud profile, sample project with drill levels and `NSPNeuron` classes.

## 1. Diagram scope + add

1. Open a configuration and drill into a subsystem (double-click container on the diagram).
2. Open **AI Assistant Settings** → confirm **Context acquisition** = **Auto**.
3. Ask to add a neuron class (e.g. `NSPNeuron`) on the **current schematic level**.
4. **Expect:** action preview shows `parent` = drill container (`diagram_scope_long_name`), not project root.
5. With **Apply write tools automatically** off: preview appears before **Apply**; after Apply, component appears under drill.

## 2. Multi-turn connect

1. Without list selection, on a drilled schematic ask to connect two named neurons across 3–4 short messages (from → to → ports).
2. **Expect:** ephemeral hints mention catalog link patterns (no hardcoded `LTZone` / `ExcSynapse1` in assistant text).
3. **Expect:** preview lists normalized `from_long_name`, `to_long_name`, ports before invoke.
4. **Expect:** link exists after apply; chat HTML shows tool result rows.

## 3. Open recent (lifecycle)

1. New chat: ask to open a recent configuration.
2. **Expect:** orchestrator runs `list_recent_configurations` (tool row in history), then disambiguation or `open_recent` after index `1` — not prose «open manually».

## 4. HITL vs auto-apply

1. Disable **Apply write tools automatically** → mutating request → preview + Apply/Reject (no generic «confirmation required» only).
2. Enable auto-apply → same request → preview + outcome in one turn; post-verify line when applicable.

## 5. Verify / repair

1. Deliberately wrong parent or missing port (if reproducible).
2. **Expect:** verify failure message; at most two repair attempts, then clear user-facing error.

## 6. Context acquisition = Minimal

1. Set **Context acquisition** to **Minimal**.
2. Repeat connect intent without calling tools.
3. **Expect:** no link-pattern block in logs (`context_acquired` audit); model may still use tools — no silent auto-connect.

## Sign-off

| Step | Pass | Notes |
|------|------|-------|
| 1 Diagram scope + add | | |
| 2 Multi-turn connect | | |
| 3 Open recent | | |
| 4 HITL / auto-apply | | |
| 5 Verify / repair | | |
| 6 Minimal mode | | |

Tester / date: _______________

---

## EN

Automated coverage: `Test_LLM_TD041WalkthroughSim`, `Test_LLM_AgentScenarios` (`link_*` fixtures), `ctest -R Test_LLM_`.

Use this checklist for an optional **human** spot-check in NeuroModeler after chat-22-15 fixes.

**Prerequisites:** built `NeuroModeler`, Ollama or configured cloud profile, sample project with drill levels and `NSPNeuron` classes.

## 1. Diagram scope + add

1. Open a configuration and drill into a subsystem (double-click container on the diagram).
2. Open **AI Assistant Settings** → confirm **Context acquisition** = **Auto**.
3. Ask to add a neuron class (e.g. `NSPNeuron`) on the **current schematic level**.
4. **Expect:** action preview shows `parent` = drill container (`diagram_scope_long_name`), not project root.
5. With **Apply write tools automatically** off: preview appears before **Apply**; after Apply, component appears under drill.

## 2. Multi-turn connect

1. Without list selection, on a drilled schematic ask to connect two named neurons across 3–4 short messages (from → to → ports).
2. **Expect:** ephemeral hints mention catalog link patterns (no hardcoded `LTZone` / `ExcSynapse1` in assistant text).
3. **Expect:** preview lists normalized `from_long_name`, `to_long_name`, ports before invoke.
4. **Expect:** link exists after apply; chat HTML shows tool result rows.

## 3. Open recent (lifecycle)

1. New chat: ask to open a recent configuration.
2. **Expect:** orchestrator runs `list_recent_configurations` (tool row in history), then disambiguation or `open_recent` after index `1` — not prose «open manually».

## 4. HITL vs auto-apply

1. Disable **Apply write tools automatically** → mutating request → preview + Apply/Reject (no generic «confirmation required» only).
2. Enable auto-apply → same request → preview + outcome in one turn; post-verify line when applicable.

## 5. Verify / repair

1. Deliberately wrong parent or missing port (if reproducible).
2. **Expect:** verify failure message; at most two repair attempts, then clear user-facing error.

## 6. Context acquisition = Minimal

1. Set **Context acquisition** to **Minimal**.
2. Repeat connect intent without calling tools.
3. **Expect:** no link-pattern block in logs (`context_acquired` audit); model may still use tools — no silent auto-connect.

## Sign-off

| Step | Pass | Notes |
|------|------|-------|
| 1 Diagram scope + add | | |
| 2 Multi-turn connect | | |
| 3 Open recent | | |
| 4 HITL / auto-apply | | |
| 5 Verify / repair | | |
| 6 Minimal mode | | |

Tester / date: _______________
