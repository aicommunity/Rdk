# Pending CMake / vcpkg changes

**Status: applied** (фаза 0, 2026-05-25). Этот файл сохранён как чеклист; новые изменения CMake — в [Build.md](Build.md).

Реализовано в репозитории:

- `cmake/RdkDefines.cmake` — `RDK_USE_LLM`, `RDK_LLM_BUILD_EMBEDDED`
- `vcpkg.json` — feature `llm`
- `Rdk/CMakeLists.txt` — `add_subdirectory(LLM)`
- `Rdk/LLM/CMakeLists.txt` — target `rdk.llm.core`, `llm_audit_verify`
