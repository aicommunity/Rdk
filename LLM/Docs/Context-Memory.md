# Context and memory (LLM assistant)

## RU

Normative overview for **ephemeral context**, **session store v2**, and **long-term memory files**. Implementation: `ULLMContextAssembler`, `ULLMConversationStore`, `ULLMLongTermMemoryLoader`.

**See also:** [Conversation-State.md](Conversation-State.md), [Knowledge-Sources.md](Knowledge-Sources.md) § Long-term context memory, [GUI-Integration.md](GUI-Integration.md).

---

## EN

## Ephemeral vs persisted

| Layer | Storage | Examples |
|-------|---------|----------|
| Ephemeral (per provider round) | Not in `messages` (DD-CTX-001) | GUI focus hint, retriever summary (live net when `project_loaded`), manifest, agent notes, query/explain inspect decision tree, query prefetch |
| Session transcript | `LLM/sessions/{session_id}.json` | `messages`, HITL `pending`, `pending_plan` |
| Session metadata v2 | Same JSON file | `last_gui_context`, `resolved_entities`, `agent_notes`, `session_summary`, `last_session_context` |
| Long-term | `LLM/memory/` | User preferences, project `rules.md` |

### Query / Explain inspect hints

Ephemeral system block `## Inspect live project vs docs` (Query and Explain):

- Live graph / current model / components / diagram → `get_net_snapshot` (use GUI `channel_index`; drill → `root_long_name=diagram_scope_long_name`). Prefer the injected **Project context snapshot** when present.
- Selected component → `find_component` / `get_component_properties`
- Class / ClDesc → `describe_class`
- How-to docs → `search_project_docs`
- Recent configs on disk → `list_recent_configurations` (not live graph)

When `project_loaded`, `URdkContextRetriever` injects a live net snapshot (scoped by diagram drill when set) so the model need not guess tools for “о текущей модели”.

---
