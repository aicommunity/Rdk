# Pending CMake / vcpkg changes

## RU

**Status: applied** (фаза 0, 2026-05-25). Этот файл сохранён как чеклист; новые изменения CMake — в [Build.md](Build.md).

Реализовано в репозитории:

- `cmake/RdkDefines.cmake` — `RDK_USE_LLM`, `RDK_LLM_BUILD_EMBEDDED`
- `vcpkg.json` — feature `llm`
- `Rdk/CMakeLists.txt` — `add_subdirectory(LLM)`
- `Rdk/LLM/CMakeLists.txt` — target `rdk.llm.core`, `llm_audit_verify`

---

## EN

**Status: applied** (phase 0, 2026-05-25). This file is kept as a checklist; new CMake changes go in [Build.md](Build.md).

Implemented in the repository:

- `cmake/RdkDefines.cmake` — `RDK_USE_LLM`, `RDK_LLM_BUILD_EMBEDDED`
- `vcpkg.json` — feature `llm`
- `Rdk/CMakeLists.txt` — `add_subdirectory(LLM)`
- `Rdk/LLM/CMakeLists.txt` — target `rdk.llm.core`, `llm_audit_verify`
