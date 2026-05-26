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
| `index-manifest.json` | `catalog_fingerprint`, `chunk_count`, `file_mtimes` (repo path → unix sec) |
| `index.jsonl` | One JSON object per indexed chunk |

Build:

```bash
cmake --build build-llm-ci --target llm-index-pack
```

When the manifest fingerprint matches the runtime catalog, NeuroModeler loads the pack instead
of rebuilding the full index.

## Incremental sync (TD-033)

After loading prebuilt or dev cache (`<repo>/LLM/index/`), `UDocSearchIndex::syncFromCatalog`
compares each indexed file's `mtime` to `file_mtimes` in the manifest. Changed, added, or removed
files are patched in memory; if more than 25% of chunks would change, a full rebuild runs instead.

Runtime never writes to `Bin/LLM/index`; dev cache is updated under `LLM/index/` when incremental
sync applies. Rebuild the Bin pack after catalog changes: `cmake --build build-llm-ci --target llm-index-pack`.

## `Docs/llms.txt` (TD-032)

`llm-index-pack` also writes [`Docs/llms.txt`](../../../Docs/llms.txt) listing library ClDesc folders and
indexed `source_id` roots for external LLM crawlers. Regenerate with the index pack target; do not hand-edit.

## Library list (TD-039)

`NmsdkBuiltinKnowledgeCatalog::loadedLibraries()` is the single source for `library_id` → `cl_desc_folder`
used by `NmsdkLlmProjectContext` and library doc paths. Keep in sync with `Libraries.cpp` when adding libs.
