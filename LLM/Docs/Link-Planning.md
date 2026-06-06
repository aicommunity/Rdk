# Link planning (TD-091)

## RU

This document describes how the assistant plans multi-link `connect_components` operations in a deterministic way (with an optional LLM fallback).

## Goals

- Support user goals like:
  - `connect Neuron1 and Neuron2`
  - `link 3 pairs`
  - `свяжи оставшиеся`
  - `connect rest like previous`
- Avoid silent success when the requested links were not created.
- Avoid creating duplicate links when the exact 4-tuple already exists.

## Link identity (strict 4-tuple)

A link is considered **already existing** only when all four fields match exactly:

- `from_long_name`
- `from_property`
- `to_long_name`
- `to_property`

Different ports or reversed direction are not duplicates.

## Snapshot links

`get_net_snapshot` returns a `links[]` array where each element is:

```json
{
  "from_long_name": "/ch0/MModel.Neuron1",
  "from_property": "Output",
  "to_long_name": "/ch0/MModel.Neuron2",
  "to_property": "Input"
}
```

The snapshot may set `links_truncated: true` when the max link limit is reached.

## Planning strategy (deterministic first)

1. Always plan `get_net_snapshot` first.
2. Parse the goal:
   - explicit endpoints: `Neuron1 and Neuron2`
   - explicit ports: `/ch0/A.Out -> /ch0/B.In`
   - count-only: `link 3 pairs`
   - remaining: `remaining/rest/оставшиеся`
   - analogous: `like previous/как предыдущий/по аналогии`
3. Resolve endpoint tokens to component `long_name`:
   - direct long_name (contains `/`) → `findComponentByLongName`
   - resolved entity store
   - exact short_name match in snapshot
4. Infer ports (or ask for clarification) using published ports and the link pattern catalog.
   When the user asks to connect «like previous», reuse `last_template` ports when component
   classes match (fallback to catalog inference otherwise).
5. For each target link quad:
   - if it is already present in snapshot `links[]`, skip creating a write step
   - otherwise, add a `connect_components` plan step with `success=link_exists`
   For count-only goals, fail with `insufficient_new_links` when fewer new links remain
   after skipping snapshot duplicates than requested.
6. `goal_success` uses:
   - `link_exists` for a single link
   - `multi_link_exists` for multiple links

## Idempotent connect tool behavior

`connect_components` is idempotent with respect to the strict 4-tuple. If the link already exists, it returns `ok=true` and adds `already_existed: true`.

## Remaining scope (TD-092)

`parseConnectGoal` sets `remaining_scope`:

- `SessionDelta` (default): endpoints from session graph memory (`added` not yet linked in session).
- `ModelGraph`: all snapshot components with no incident link in the full model walk (`ULLMConnectEndpoints`).

If the model link walk is truncated while computing global remaining, planning fails with `links_incomplete_for_global_remaining`.

## Topologies (TD-092)

| Topology | Behavior |
|----------|----------|
| `Sequential` | Pair endpoints FIFO (`e0→e1`, `e2→e3`, …) |
| `Chain` | Ordered chain `e0→e1→…→eN` (catalog score ≥ 0.6 per step) |
| `Tree` | Hub → each leaf; hub from `hub_token` or auto-pick (`ULLMConnectPairing`) |

Routing to task path also triggers for `ModelGraph`, non-sequential topology, or multi-link connect goals.

## Truncated snapshot dedup (TD-093)

When `links_truncated: true`, duplicate detection uses `planSnapshotOrModelHasLink` (snapshot first, then `linkExistsInModel` via paginated walk). `connect_components` uses the same check before creating a link.

## Container vs internal ports (TD-096)

Users often mean **published ports on components** (for example `LTZone` → `Soma1.ExcSynapse1`), not abstract container boxes or generic `Output`/`Input` shell names. See [Connect-Semantics.md](Connect-Semantics.md) for the machine index and phrasing hints.

## Connect semantics (TD-096)

Port inference prefers machine index `connect-semantics.json` (see `Connect-Semantics.md`) over generic `Output`/`Input` when connecting neurons. Ephemeral system hint is injected for connect goals via `ULLMContextAssembler`.

## Optional LLM fallback

When enabled, the orchestrator can attempt an LLM-based JSON extraction path if deterministic planning fails. The fallback is gated by:

- `NMSDK_LLM_CONNECT_PLAN_LLM=1` or runtime setting `connect_plan_llm_fallback`.

---

## EN

This document describes how the assistant plans multi-link `connect_components` operations in a deterministic way (with an optional LLM fallback).

## Goals

- Support user goals like:
  - `connect Neuron1 and Neuron2`
  - `link 3 pairs`
  - `свяжи оставшиеся`
  - `connect rest like previous`
- Avoid silent success when the requested links were not created.
- Avoid creating duplicate links when the exact 4-tuple already exists.

## Link identity (strict 4-tuple)

A link is considered **already existing** only when all four fields match exactly:

- `from_long_name`
- `from_property`
- `to_long_name`
- `to_property`

Different ports or reversed direction are not duplicates.

## Snapshot links

`get_net_snapshot` returns a `links[]` array where each element is:

```json
{
  "from_long_name": "/ch0/MModel.Neuron1",
  "from_property": "Output",
  "to_long_name": "/ch0/MModel.Neuron2",
  "to_property": "Input"
}
```

The snapshot may set `links_truncated: true` when the max link limit is reached.

## Planning strategy (deterministic first)

1. Always plan `get_net_snapshot` first.
2. Parse the goal:
   - explicit endpoints: `Neuron1 and Neuron2`
   - explicit ports: `/ch0/A.Out -> /ch0/B.In`
   - count-only: `link 3 pairs`
   - remaining: `remaining/rest/оставшиеся`
   - analogous: `like previous/как предыдущий/по аналогии`
3. Resolve endpoint tokens to component `long_name`:
   - direct long_name (contains `/`) → `findComponentByLongName`
   - resolved entity store
   - exact short_name match in snapshot
4. Infer ports (or ask for clarification) using published ports and the link pattern catalog.
   When the user asks to connect «like previous», reuse `last_template` ports when component
   classes match (fallback to catalog inference otherwise).
5. For each target link quad:
   - if it is already present in snapshot `links[]`, skip creating a write step
   - otherwise, add a `connect_components` plan step with `success=link_exists`
   For count-only goals, fail with `insufficient_new_links` when fewer new links remain
   after skipping snapshot duplicates than requested.
6. `goal_success` uses:
   - `link_exists` for a single link
   - `multi_link_exists` for multiple links

## Idempotent connect tool behavior

`connect_components` is idempotent with respect to the strict 4-tuple. If the link already exists, it returns `ok=true` and adds `already_existed: true`.

## Remaining scope (TD-092)

`parseConnectGoal` sets `remaining_scope`:

- `SessionDelta` (default): endpoints from session graph memory (`added` not yet linked in session).
- `ModelGraph`: all snapshot components with no incident link in the full model walk (`ULLMConnectEndpoints`).

If the model link walk is truncated while computing global remaining, planning fails with `links_incomplete_for_global_remaining`.

## Topologies (TD-092)

| Topology | Behavior |
|----------|----------|
| `Sequential` | Pair endpoints FIFO (`e0→e1`, `e2→e3`, …) |
| `Chain` | Ordered chain `e0→e1→…→eN` (catalog score ≥ 0.6 per step) |
| `Tree` | Hub → each leaf; hub from `hub_token` or auto-pick (`ULLMConnectPairing`) |

Routing to task path also triggers for `ModelGraph`, non-sequential topology, or multi-link connect goals.

## Truncated snapshot dedup (TD-093)

When `links_truncated: true`, duplicate detection uses `planSnapshotOrModelHasLink` (snapshot first, then `linkExistsInModel` via paginated walk). `connect_components` uses the same check before creating a link.

## Container vs internal ports (TD-096)

Users often mean **published ports on components** (for example `LTZone` → `Soma1.ExcSynapse1`), not abstract container boxes or generic `Output`/`Input` shell names. See [Connect-Semantics.md](Connect-Semantics.md) for the machine index and phrasing hints.

## Connect semantics (TD-096)

Port inference prefers machine index `connect-semantics.json` (see `Connect-Semantics.md`) over generic `Output`/`Input` when connecting neurons. Ephemeral system hint is injected for connect goals via `ULLMContextAssembler`.

## Optional LLM fallback

When enabled, the orchestrator can attempt an LLM-based JSON extraction path if deterministic planning fails. The fallback is gated by:

- `NMSDK_LLM_CONNECT_PLAN_LLM=1` or runtime setting `connect_plan_llm_fallback`.
