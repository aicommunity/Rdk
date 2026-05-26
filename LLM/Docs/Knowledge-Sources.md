# Knowledge sources and prebuilt index

## Builtin catalog

`NmsdkBuiltinKnowledgeCatalog` registers documentation and source roots under the repository
(see LLM-first architecture plan §11). At startup `LLMServices::initialize` loads or builds
`UDocSearchIndex`.

## `search_project_docs` scope

| scope | Indexed content |
|-------|-----------------|
| `docs` | Markdown documentation only |
| `sources` | C++/header excerpts |
| `all` | Documentation and sources |

Snippets include `source_id`, `content_kind`, `path`, `start_line`, and `score`.

## Prebuilt pack (`Bin/LLM/index`)

| File | Purpose |
|------|---------|
| `index-manifest.json` | `catalog_fingerprint`, `chunk_count` |
| `index.jsonl` | One JSON object per indexed chunk |

Build:

```bash
cmake --build build-llm-ci --target llm-index-pack
```

When the manifest fingerprint matches the runtime catalog, NeuroModeler loads the pack instead
of rebuilding the full index.
