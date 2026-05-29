# Link planning (TD-091)

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
5. For each target link quad:
   - if it is already present in snapshot `links[]`, skip creating a write step
   - otherwise, add a `connect_components` plan step with `success=link_exists`
6. `goal_success` uses:
   - `link_exists` for a single link
   - `multi_link_exists` for multiple links

## Idempotent connect tool behavior

`connect_components` is idempotent with respect to the strict 4-tuple. If the link already exists, it returns `ok=true` and adds `already_existed: true`.

## Optional LLM fallback

When enabled, the orchestrator can attempt an LLM-based JSON extraction path if deterministic planning fails. The fallback is gated by:

- `NMSDK_LLM_CONNECT_PLAN_LLM=1` or runtime setting `connect_plan_llm_fallback`.

