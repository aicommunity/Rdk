#ifndef RDK_ULLM_TASK_PATH_MODE_H
#define RDK_ULLM_TASK_PATH_MODE_H

namespace RDK::LLM {

enum class LLMTaskPathMode {
    HintOnly,
    FastPath
};

/// Default HintOnly; FastPath when NMSDK_LLM_TASK_PATH_STRICT=1 or settings task_path_mode=fast_path.
LLMTaskPathMode resolveTaskPathMode();

bool taskPathStrictFastPathEnabled();

} // namespace RDK::LLM

#endif
