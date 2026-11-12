# Tools Contracts — JSON Schema

## RU

`TOOL_REGISTRY_VERSION`: **1.0.0**

Все tools используют `additionalProperties: false` где указано (strict).

---

## EN

## Read tools (phase 1)

### `list_registered_classes`

**kind:** Read | **confirmation:** false | **idempotent:** N/A

**input:**
```json
{
  "type": "object",
  "properties": {
    "library_filter": {
      "type": "string",
      "description": "Optional ULibrary::GetName() filter (PulseLibrary, BasicLib, HardwareLibrary, MotionControlLibrary, CvBasicLib). Short aliases (PulseLib, HardwareLib, …) are resolved."
    }
  },
  "additionalProperties": false
}
```

**output:** `{ "classes": [ { "class_name", "library?" } ], "library_filter_resolved?" }`

---

### `get_net_snapshot`

**input:**
```json
{
  "type": "object",
  "properties": {
    "channel_index": { "type": "integer", "minimum": 0, "default": 0 },
    "max_components": { "type": "integer", "minimum": 1, "maximum": 500, "default": 200 },
    "root_long_name": {
      "type": "string",
      "description": "Optional subtree root. Empty/omitted = entire Model root. Unknown root → auto-retry Model root."
    }
  },
  "additionalProperties": false
}
```

**output:** see `NetSnapshot` в [Domain-Layer.md](Domain-Layer.md); may include `retried_without_root` when a bad root was discarded.

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

### `list_model_links`

**input:**
```json
{
  "type": "object",
  "properties": {
    "channel_index": { "type": "integer", "minimum": 0, "default": 0 },
    "root_long_name": { "type": "string" },
    "component_long_name": { "type": "string" },
    "from_long_name": { "type": "string" },
    "to_long_name": { "type": "string" },
    "offset": { "type": "integer", "minimum": 0, "default": 0 },
    "limit": { "type": "integer", "minimum": 1, "maximum": 2000, "default": 500 }
  },
  "additionalProperties": false
}
```

**Subtree filters:** `component_long_name` / `from_long_name` / `to_long_name` are **anchors**. An endpoint matches if it equals the anchor or is nested under it (`Anchor.Child…`). `component_long_name` matches incident links (either end). Combine filters with AND.

**output:** `links[]` (strict 4-tuple), `offset`, `limit`, `returned_count`, `total_links_seen`, `total_matching`, `no_matching_links`, `truncated`, `next_offset`.

---

### `get_component_ports`

**input:**
```json
{
  "type": "object",
  "required": ["long_name"],
  "properties": {
    "long_name": { "type": "string", "minLength": 1 },
    "channel_index": { "type": "integer", "minimum": 0, "default": 0 },
    "include_nested": { "type": "boolean", "default": true }
  },
  "additionalProperties": false
}
```

**output:** `outputs[]` / `inputs[]` for the named component; `ports[]` entries `{ owner_long_name, port_name, direction }` including descendants when `include_nested` is true.

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

If `candidates.length > 1` && score close → `resolved: false`; write-tools **must** reject до clarifications.

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

**output:** `{ "long_name", "class_name", "properties": [ { "name", "type", "value_repr" } ] }` — без large matrices (if matrix — `value_repr: "<Matrix 100x100>"`).

---

### `search_project_docs`

**input:**
```json
{
  "type": "object",
  "required": ["query"],
  "properties": {
    "query": { "type": "string", "minLength": 2 },
    "top_k": { "type": "integer", "minimum": 1, "maximum": 10, "default": 5 },
    "scope": { "type": "string", "enum": ["docs", "sources", "all"], "default": "docs" },
    "match": { "type": "string", "enum": ["tfidf", "literal"], "default": "tfidf" }
  },
  "additionalProperties": false
}
```

**output:** `{ "match", "snippets": [ { "path", "title", "excerpt", "score", "source_id", "doc_uri?", ... } ] }`  
`doc_uri` is `nmsdk-doc:<repo-relative>` when the path is under indexed Docs roots (for chat links / `open_documentation`).

---

### Documentation open / Help (DD-DOC-001)

| Tool | Kind | Purpose |
|------|------|---------|
| `list_help_topics` | Read | List `Bin/Help/{locale}/*.html` → `{topics:[{topic, help_uri, title}]}` |
| `open_help` | Read | Open GUI User Guide; optional `topic` (default index) |
| `open_class_docs` | Read | Open `UClassDescriptionDisplay` for `class_name` (ambiguous → candidates) |
| `open_documentation` | Read | Open markdown Docs via `path` or `doc_uri` (allowlisted roots only) |

Chat markdown may use the same URIs: `nmsdk-doc:…`, `nmsdk-help:…`, `nmsdk-class:…` (clickable in the assistant dock).

See [Agent-Documentation.md](Agent-Documentation.md).

---

### Project artifacts / config inspect (path policy, read-only)

| Tool | Purpose |
|------|---------|
| `inspect_configuration` | Compact classes+links from model XML. Omit `configuration_path` for open configuration; bare `project.ini` / `model.xml` / sentinels (`open project`, `.`) rewrite to the open config root when a project is open. |
| `search_configuration_links` | Filter links in a sample/project XML |
| `list_project_files` | List sidecar files under open project |
| `read_text_artifact` | Capped text/csv/json/xml/md read |
| `stat_project_file` | Size/mtime/ext; optional image dims (no vision) |

---

### `validate_project`

**input:** `{ "channel_index": 0 }`  
**output:** `{ "ok", "warnings": [ "string" ] }`

---
