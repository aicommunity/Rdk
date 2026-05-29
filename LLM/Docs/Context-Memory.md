# Context and memory (LLM assistant)

Normative overview for **ephemeral context**, **session store v2**, and **long-term memory files**. Implementation: `ULLMContextAssembler`, `ULLMConversationStore`, `ULLMLongTermMemoryLoader`.

**See also:** [Conversation-State.md](Conversation-State.md), [Knowledge-Sources.md](Knowledge-Sources.md) § Long-term context memory, [GUI-Integration.md](GUI-Integration.md).

---

## Ephemeral vs persisted

| Layer | Storage | Examples |
|-------|---------|----------|
| Ephemeral (per provider round) | Not in `messages` (DD-CTX-001) | GUI focus hint, retriever summary, manifest, agent notes block, query prefetch |
| Session transcript | `LLM/sessions/{session_id}.json` | `messages`, HITL `pending`, `pending_plan` |
| Session metadata v2 | Same JSON file | `last_gui_context`, `resolved_entities`, `agent_notes`, `session_summary`, `last_session_context` |
| Long-term | `LLM/memory/` | User preferences, project `rules.md` |

---

## Environment flags (context)

| Variable | Default | Effect |
|----------|---------|--------|
| `NMSDK_LLM_SESSION_BOOTSTRAP` | off | One-time read `get_net_snapshot` on new chat |
| `NMSDK_LLM_QUERY_PREFETCH_DOCS` | off | Query intent: prefetch top doc snippets (DD-CTX-003) |
| `NMSDK_LLM_CONTEXT_COMPACT` | off | Rule-based compaction + `.full.jsonl` sidecar |
| `NMSDK_LLM_MASK_STALE_TOOLS` | off | Mask old tool message bodies |

---

## GUI session safety

While a session is **busy** (async user message or plan execution), the orchestrator rejects overlapping `confirmPending` / plan ops and skips `discardSession`. The dock **defers** new chat, project-load reset, and archive switches until the in-flight request finishes (`flushDeferredUiActions`).

---

## Resume parity (TD-088)

`last_session_context` in session JSON stores the latest `LLMSessionContext` from orchestrator entry points. **Continue chat** reloads it so write/autonomy flags match the saved session; live GUI still supplies current user, channel, and project-open state.
