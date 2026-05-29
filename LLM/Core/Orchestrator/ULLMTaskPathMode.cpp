#include "ULLMTaskPathMode.h"

#include "../LlmPublicApi.h"

#include <cstdlib>
#include <cstring>

namespace RDK::LLM {

namespace {

bool envTruthy(const char* name)
{
    const char* v = std::getenv(name);
    return v && v[0] != '0' && std::strcmp(v, "false") != 0 && std::strcmp(v, "FALSE") != 0;
}

} // namespace

LLMTaskPathMode resolveTaskPathMode()
{
    if(envTruthy("NMSDK_LLM_TASK_PATH_STRICT"))
        return LLMTaskPathMode::FastPath;
    if(LLMServices::instance().isInitialized()
       && LLMServices::instance().settings().runtime().task_path_mode == LLMTaskPathMode::FastPath)
        return LLMTaskPathMode::FastPath;
    return LLMTaskPathMode::HintOnly;
}

bool taskPathStrictFastPathEnabled()
{
    return resolveTaskPathMode() == LLMTaskPathMode::FastPath;
}

} // namespace RDK::LLM
