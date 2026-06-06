# Context and memory (LLM assistant)

## RU

Normative overview for **ephemeral context**, **session store v2**, and **long-term memory files**. Implementation: `ULLMContextAssembler`, `ULLMConversationStore`, `ULLMLongTermMemoryLoader`.

**See also:** [Conversation-State.md](Conversation-State.md), [Knowledge-Sources.md](Knowledge-Sources.md) § Long-term context memory, [GUI-Integration.md](GUI-Integration.md).

---

## EN

## Ephemeral vs persisted

| Layer | Storage | Examples |
|-------|---------|----------|
| Ephemeral (per provider round) | Not in `messages` (DD-CTX-001) | GUI focus hint, retriever summary, manifest, agent notes block, query prefetch |
| Session transcript | `LLM/sessions/{session_id}.json` | `messages`, HITL `pending`, `pending_plan` |
| Session metadata v2 | Same JSON file | `last_gui_context`, `resolved_entities`, `agent_notes`, `session_summary`, `last_session_context` |
| Long-term | `LLM/memory/` | User preferences, project `rules.md` |

---
