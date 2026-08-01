# Tools coverage gap (menu / C-API / domain)

Catalog of high-value capabilities **not** yet exposed as LLM tools (or filter gaps).  
Calc start/pause/reset/step **are already tools** — routing is DD-CALC-001, not a missing tool.

Sources: Qt main menu (`UGEngineControllWidget`), C-API (`rdk_init.h` / `nmsdk.h`), `UEnvironment` / `UEngine` / `UNet` / `UStorage`, `URdkDomainAccess` / `URdkApplicationCommands`.

## Already covered (do not re-register)

| Area | Tools |
|------|--------|
| Graph | `get_net_snapshot`, `list_model_links`, `get_component_ports`, add/remove/connect/set_property |
| Graph edits | `clone_component`, `move_component`, `rename_component`, `reorder_component` |
| Component I/O | `export_component`, `import_component` (path policy) |
| Per-component env | `calculate_component`, `reset_component`, `default_component` |
| Navigation | `select_component` (diagram scope via presentation sink; no `Env_Select`) |
| Classes | `list_registered_classes`, `describe_class`, library `list_*_classes` |
| Config lifecycle | create/load/save/`save_configuration_as`/close/copy/rename/validate/recent |
| Channel calc | `start/pause/reset/step_channel_calculation`, `run_n_steps`, `list_channels`, `set_active_channel` |
| Channel CRUD | `add_channel`, `delete_channel`, `clone_channel` |
| UI | `list_ui_panels`, `show_ui_panel`, `open_component_gui_tab` |

## Gaps (priority)

| Priority | Gap | Source | Notes |
|----------|-----|--------|--------|
| ~~High~~ | ~~`run_n_steps`~~ | Calculate → Run N Steps | **closed** — `run_n_steps` |
| ~~High~~ | ~~Channel CRUD~~ | Channels menu; `Core_AddChannel` / `Core_DelChannel` | **closed** — `add/delete/clone_channel` |
| ~~Med~~ | ~~clone / move / rename / reorder~~ | C-API `Model_*` | **closed** |
| ~~Med~~ | ~~Export / import component~~ | `Model_Save/LoadComponent(To/From)File` | **closed** |
| ~~Med~~ | ~~Per-component Env_*~~ | UEnvironment | **closed** |
| ~~Med~~ | ~~Select / navigate~~ | diagram scope | **closed** via `select_component` (not `Env_Select`; DD-AG-001) |
| Low | Storage build / class-library ops | `Storage_BuildStorage`, load/save collections | Build Storage Mode menu — deferred (risky) |
| ~~Filter~~ | ~~`save_configuration_as` in `kMutateTools`~~ | allowlist | **closed** |

## Out of scope for LLM (for now)

Broadcasters, video, FTP, Help, theme/skin — low agent value.

## Implementation policy

- New tools: separate PRs after domain facade methods exist (no direct Qt from tool handlers).
- Prefer `RecordedToolInvoke` for any pre-LLM / FastPath path (DD-MEM-001).
- Update this table when a gap is closed.
