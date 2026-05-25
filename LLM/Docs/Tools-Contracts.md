# Tools Contracts — JSON Schema

`TOOL_REGISTRY_VERSION`: **1.0.0**

Все tools используют `additionalProperties: false` где указано (strict).

---

## Read tools (фаза 1)

### `list_registered_classes`

**kind:** Read | **confirmation:** false | **idempotent:** N/A

**input:**
```json
{
  "type": "object",
  "properties": {
    "library_filter": { "type": "string", "description": "Optional ClDesc library folder name" }
  },
  "additionalProperties": false
}
```

**output:**
```json
{
  "type": "object",
  "required": ["classes"],
  "properties": {
    "classes": {
      "type": "array",
      "items": {
        "type": "object",
        "required": ["class_name", "library"],
        "properties": {
          "class_name": { "type": "string" },
          "library": { "type": "string" },
          "header": { "type": "string" }
        }
      }
    }
  }
}
```

---

### `describe_class`

**input:**
```json
{
  "type": "object",
  "required": ["class_name"],
  "properties": {
    "class_name": { "type": "string" },
    "locale": { "type": "string", "default": "ru-RU" }
  },
  "additionalProperties": false
}
```

**output:** `{ "class_name", "cl_desc_xml_fragment", "properties_summary": [...] }`

---

### `get_net_snapshot`

**input:**
```json
{
  "type": "object",
  "properties": {
    "channel_index": { "type": "integer", "minimum": 0, "default": 0 },
    "root_long_name": { "type": "string", "description": "Optional subtree root" },
    "max_components": { "type": "integer", "minimum": 1, "maximum": 500, "default": 200 }
  },
  "additionalProperties": false
}
```

**output:** см. `NetSnapshot` в [Domain-Layer.md](Domain-Layer.md)

---

### `find_component`

**input:**
```json
{
  "type": "object",
  "required": ["query"],
  "properties": {
    "query": { "type": "string", "minLength": 1 },
    "channel_index": { "type": "integer", "minimum": 0, "default": 0 },
    "class_name_filter": { "type": "string" }
  },
  "additionalProperties": false
}
```

**output:**
```json
{
  "type": "object",
  "required": ["candidates"],
  "properties": {
    "candidates": {
      "type": "array",
      "items": {
        "type": "object",
        "required": ["long_name", "class_name", "score"],
        "properties": {
          "long_name": { "type": "string" },
          "class_name": { "type": "string" },
          "score": { "type": "number" }
        }
      }
    },
    "resolved": { "type": "boolean" },
    "canonical_long_name": { "type": "string" }
  }
}
```

Если `candidates.length > 1` && score близки → `resolved: false`; write-tools **должны** отказать до уточнения.

---

### `get_component_properties`

**input:**
```json
{
  "type": "object",
  "required": ["long_name"],
  "properties": {
    "long_name": { "type": "string" },
    "channel_index": { "type": "integer", "minimum": 0, "default": 0 },
    "property_names": {
      "type": "array",
      "items": { "type": "string" },
      "description": "Empty = pub parameters summary only"
    }
  },
  "additionalProperties": false
}
```

**output:** `{ "long_name", "class_name", "properties": [ { "name", "type", "value_repr" } ] }` — без больших матриц (если matrix — `value_repr: "<Matrix 100x100>"`).

---

### `search_project_docs`

**input:**
```json
{
  "type": "object",
  "required": ["query"],
  "properties": {
    "query": { "type": "string", "minLength": 2 },
    "top_k": { "type": "integer", "minimum": 1, "maximum": 10, "default": 5 }
  },
  "additionalProperties": false
}
```

**output:** `{ "snippets": [ { "path", "title", "excerpt", "score" } ] }`

---

### `validate_project`

**input:** `{ "channel_index": 0 }`  
**output:** `{ "ok", "warnings": [ "string" ] }`

---

## Write tools (фаза 2)

### `add_component`

**kind:** Write | **confirmation:** true | **idempotent:** true (with `client_request_id`)

**input:**
```json
{
  "type": "object",
  "required": ["class_name", "parent_long_name", "short_name"],
  "properties": {
    "class_name": { "type": "string" },
    "parent_long_name": { "type": "string" },
    "short_name": { "type": "string", "pattern": "^[A-Za-z][A-Za-z0-9_]*$" },
    "channel_index": { "type": "integer", "minimum": 0, "default": 0 },
    "client_request_id": { "type": "string", "format": "uuid" }
  },
  "additionalProperties": false
}
```

**output:** `{ "ok", "long_name" }`

**Preconditions (policy):** `project_loaded`, class exists in storage, parent exists.

---

### `remove_component`

**confirmation:** true

**input:** `{ "long_name", "channel_index" }`

---

### `set_property`

**confirmation:** true (false для «безопасных» свойств — whitelist в policy, фаза 3)

**input:**
```json
{
  "type": "object",
  "required": ["long_name", "property_name", "value"],
  "properties": {
    "long_name": { "type": "string" },
    "property_name": { "type": "string" },
    "value": { "type": "string", "description": "Serialized scalar; complex types via structured sub-object in v2" },
    "channel_index": { "type": "integer", "minimum": 0, "default": 0 },
    "client_request_id": { "type": "string" }
  },
  "additionalProperties": false
}
```

---

### `connect_components`

**confirmation:** true | **Domain:** `MModel_CreateLinkByName`

**input:**
```json
{
  "type": "object",
  "required": ["from_long_name", "from_property", "to_long_name", "to_property"],
  "properties": {
    "from_long_name": { "type": "string" },
    "from_property": { "type": "string", "description": "Output property on source" },
    "to_long_name": { "type": "string" },
    "to_property": { "type": "string", "description": "Input property on target" },
    "channel_index": { "type": "integer", "minimum": 0, "default": 0 }
  },
  "additionalProperties": false
}
```

**output:** `{ "from_long_name", "from_property", "to_long_name", "to_property" }`

**Internal (not exposed to LLM API):** `disconnect_components` — undo link (`Model_BreakLinkByName`).

---

### Configuration lifecycle (`URdkApplicationCommands`)

See [Application-Commands.md](Application-Commands.md) for full schemas.

| Tool | Aliases | Notes |
|------|---------|--------|
| `create_configuration` | — | `requires_project_loaded: false` |
| `load_configuration` | `load_project` | P04 path whitelist |
| `save_configuration` | — | P03 `allow_save` |
| `save_configuration_as` | `save_project` (if path set) | |
| `close_configuration` | — | |
| `validate_configuration` | — | Read; on-disk validate |
| `update_configuration` | — | |
| `copy_configuration` / `rename_configuration` | — | |
| `reload_configuration_parameters` | — | |
| `*_channel_calculation` | — | `channel_index` default -1 |

After success, optional `ILLMPresentationSink` refreshes GUI (NeuroModeler).

---

## OpenAI tool definition mapping

```json
{
  "type": "function",
  "function": {
    "name": "get_net_snapshot",
    "description": "Returns component graph and links for the active model channel.",
    "parameters": { }
  }
}
```

Поле `parameters` = `input_schema` из registry. `strict: true` в API request если провайдер поддерживает.

---

## Тестовые fixtures

Хранить в `Rdk/Tests/Fixtures/LLM/tools/`:
- `valid_add_component.json`
- `invalid_add_component_missing_parent.json`
- `find_component_ambiguous_response.json`
