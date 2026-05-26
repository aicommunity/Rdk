# Agent interaction (LLM-first)

## Turn loop

1. Classify intent (`query`, `mutate`, `explain`, `plan`).
2. Build tool filter and agent manifest (tools + knowledge hints).
3. Provider round: assistant may return tool calls or text.
4. **Mutate recovery:** if no tool call on the first round, a system recovery message asks for
   exactly one tool or `NO_SUITABLE_TOOL`; a second empty round sets `no_suitable_tool`.
5. Tool results are appended; lifecycle write tools may return argument clarification or HITL
   confirmation.

## Configuration lifecycle (PR1)

Russian phrases such as «создай новый проект» map to `create_configuration` via the LLM tool
path. Pre-LLM `preflightLifecycleArguments` bypass was removed; the model must emit tool calls.

## Human-in-the-loop

Write tools with `requires_confirmation` (for example `create_configuration`) return
`pending_confirmation` until the user applies the change in the assistant panel.
