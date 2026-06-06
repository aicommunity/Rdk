# Connect semantics (TD-096)

## RU

## Principle

`connect_components` links **published ports** on components, not abstract container boxes.
When a user says «connect Neuron1 and Neuron2», they usually mean a specific internal channel
(for example `LTZone` → `Soma1.ExcSynapse1`), not generic `Output`/`Input` on the neuron shell.

## Typical class pairs

| From class | To class | Suggested from port | Suggested to port |
|------------|----------|---------------------|-------------------|
| NSPNeuron | NSPNeuron | LTZone | Soma1.ExcSynapse1 |

Patterns are aggregated from `Bin/Configs/**/Model_*.xml` into `Bin/LLM/index/connect-semantics.json`.

## User phrasing (RU / EN)

- низкопороговая зона, LT zone, LTZone
- возбуждающий синапс, excitatory synapse, ExcSynapse

## When not to guess

- Multiple catalog/semantics candidates with similar scores → ask the user.
- Explicit port names in the request always win.
- Use `get_component_properties` when unsure.

## Regenerating the index

Run `llm-index-pack` from the repository root; outputs include `connect-semantics.json` and manifest.

See also [Link-Planning.md](Link-Planning.md) for goal parsing, remaining scope, and topologies.

---

## EN

## Principle

`connect_components` links **published ports** on components, not abstract container boxes.
When a user says «connect Neuron1 and Neuron2», they usually mean a specific internal channel
(for example `LTZone` → `Soma1.ExcSynapse1`), not generic `Output`/`Input` on the neuron shell.

## Typical class pairs

| From class | To class | Suggested from port | Suggested to port |
|------------|----------|---------------------|-------------------|
| NSPNeuron | NSPNeuron | LTZone | Soma1.ExcSynapse1 |

Patterns are aggregated from `Bin/Configs/**/Model_*.xml` into `Bin/LLM/index/connect-semantics.json`.

## User phrasing (RU / EN)

- низкопороговая зона, LT zone, LTZone
- возбуждающий синапс, excitatory synapse, ExcSynapse

## When not to guess

- Multiple catalog/semantics candidates with similar scores → ask the user.
- Explicit port names in the request always win.
- Use `get_component_properties` when unsure.

## Regenerating the index

Run `llm-index-pack` from the repository root; outputs include `connect-semantics.json` and manifest.

See also [Link-Planning.md](Link-Planning.md) for goal parsing, remaining scope, and topologies.
