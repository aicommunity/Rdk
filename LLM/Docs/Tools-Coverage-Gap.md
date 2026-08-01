# Tools coverage gap (menu / C-API / domain)

Catalog of high-value capabilities **not** yet exposed as LLM tools (or filter gaps).  
Calc start/pause/reset/step **are already tools** — routing is DD-CALC-001, not a missing tool.

Sources: Qt main menu (`UGEngineControllWidget`), C-API (`rdk_init.h` / `nmsdk.h`), `UEnvironment` / `UEngine` / `UNet` / `UStorage`, `URdkDomainAccess` / `URdkApplicationCommands`.

## Already covered (do not re-register)

| Area | Tools |
|------|--------|
| Graph | `get_net_snapshot`, `list_model_links`, `get_component_ports`, add/remove/connect/set_property |
| Classes | `list_registered_classes`, `describe_class`, library `list_*_classes` |
| Config lifecycle | create/load/save/close/copy/rename/validate/recent |
| Channel calc | `start/pause/reset/step_channel_calculation`, `list_channels`, `set_active_channel` |
| UI | `list_ui_panels`, `show_ui_panel`, `open_component_gui_tab` |

## Gaps (priority)

| Priority | Gap | Source | Notes |
|----------|-----|--------|--------|
| High | `run_n_steps` | Calculate → Run N Steps | Have single `step`; need N |
| High | Channel CRUD (`add` / `delete` / `clone` channel) | Channels menu; `Core_AddChannel` / `Core_DelChannel` | Only list/select + calc today |
| Med | `clone` / `move` / `rename` / reorder component | C-API `Model_CloneComponent`, `Model_MoveComponent`, …; context menu | Common graph edits |
| Med | Export / import component (XML/file) | `Model_SaveComponent(ToFile)`, `Model_LoadComponent(FromFile)` | Subgraph transfer |
| Med | Per-component `Env_Calculate` / `Env_Reset` / `Env_Default` | UEnvironment | Debug without full channel run |
| Med | Select / navigate current component | `Env_SelectCurrentComponent`, Up/Down | Sync with diagram focus |
| Low | Storage build / class-library ops | `Storage_BuildStorage`, load/save collections | Build Storage Mode menu |
| Filter | `save_configuration_as` in `kMutateTools` | Registered but missing from mutate allowlist | Progressive disclosure only today |

## Out of scope for LLM (for now)

Broadcasters, video, FTP, Help, theme/skin — low agent value.

## Implementation policy

- New tools: separate PRs after domain facade methods exist (no direct Qt from tool handlers).
- Prefer `RecordedToolInvoke` for any pre-LLM / FastPath path (DD-MEM-001).
- Update this table when a gap is closed.
