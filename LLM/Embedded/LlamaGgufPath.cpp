#include "ULlamaRuntime.h"

#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

namespace RDK::LLM {

std::string resolveGgufPath(const LLMProviderProfile& profile)
{
    if(const char* env = std::getenv("NMSDK_LLM_GGUF_PATH"))
    {
        if(env[0] != '\0' && fs::exists(env))
            return env;
    }
    if(!profile.model.empty())
    {
        const fs::path model_path(profile.model);
        if(model_path.extension() == ".gguf" && fs::exists(model_path))
            return model_path.string();
        if(fs::exists(model_path))
            return model_path.string();
    }
    return {};
}

} // namespace RDK::LLM
