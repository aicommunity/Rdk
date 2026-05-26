# Agent interaction (LLM-first)

Architecture: [Developer-Architecture.md](Developer-Architecture.md).

## Turn loop

1. Classify intent (`query`, `mutate`, `explain`, `plan`).
2. Build tool filter and agent manifest (`buildAgentManifest(registry, filter, max_chars, user_text)` — library focus from `ULLMLibraryScopeHint`).
3. Provider round: assistant may return tool calls or text.
4. **Mutate recovery:** if no tool call on the first round, a system recovery message asks for
   exactly one tool or `NO_SUITABLE_TOOL`; a second empty round sets `no_suitable_tool`.
5. Tool results are appended; lifecycle tools may return argument clarification or HITL.
6. **Parallel reads:** when all tool calls in a round are Read, orchestrator may invoke them concurrently.

## Configuration lifecycle (PR1)

Russian phrases such as «создай новый проект» map to `create_configuration` via the LLM tool
path. Pre-LLM `preflightLifecycleArguments` bypass was removed; the model must emit tool calls.

Follow-up messages merge into pending args via `ULLMLifecycleArgumentGate` (paths, `add_component` class names).

## UI panel actions (phases D/E)

User phrases like «покажи логгер» / «show logger» trigger `show_ui_panel` with `panel: "logger"`.
UI panel actions are executed via the GUI presentation sink (`LLMPresentationEvent.show_panel`).

## Human-in-the-loop

Write tools with `requires_confirmation` return `pending_confirmation` until the user clicks **Apply**, unless:

| Condition | Behavior |
|-----------|----------|
| `session.auto_apply_writes` | From Settings **Apply write tools automatically**; gateway runs write immediately |
| Low-risk `set_property` | Exempt per `ULLMWriteToolPolicy` (Comment, Description, …) |
| **Plans** | Still require **Run plan**; auto-apply does not auto-execute plans |

See [Policy-and-Safety.md](Policy-and-Safety.md), [Conversation-State.md](Conversation-State.md).

## Graph mutations (library-agnostic)

Use core `add_component` / `set_property` for all libraries. Optional library read tools and manifest hints narrow class choice; see [Extension-Guide.md](Extension-Guide.md) §4.
