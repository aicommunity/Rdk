#ifndef RDK_LLM_TEST_OLLAMA_LAB_H
#define RDK_LLM_TEST_OLLAMA_LAB_H

#include <gtest/gtest.h>

#include <string>

namespace RDK::LLM::Test {

/// Lab Ollama host (explicit integration tests).
constexpr const char* kLabOllamaHost = "http://10.245.1.12:11434";
constexpr const char* kLabOllamaOpenAiV1 = "http://10.245.1.12:11434/v1";

/// Returns true if GET /api/tags succeeds (2xx).
bool isLabOllamaReachable(int timeout_ms = 5000);

/// Model for chat tests: env `NMSDK_LLM_OLLAMA_MODEL` or default `qwen2.5:14b`.
std::string labOllamaModelName();

/// Thinking-capable model: env `NMSDK_LLM_OLLAMA_THINKING_MODEL` or default `qwen3`.
std::string labOllamaThinkingModelName();

/// True when thinking model (or qwen3*) appears in /api/tags.
bool labHasThinkingModel();

/// Prefer these macros from TEST bodies — GTEST_SKIP must return from the test function.
#define RDK_LLM_SKIP_IF_LAB_OLLAMA_UNREACHABLE()                                                       \
    do                                                                                                 \
    {                                                                                                  \
        if(!::RDK::LLM::Test::isLabOllamaReachable())                                                  \
            GTEST_SKIP() << "Lab Ollama unreachable at " << ::RDK::LLM::Test::kLabOllamaHost;         \
    } while(0)

#define RDK_LLM_SKIP_IF_LAB_THINKING_MODEL_MISSING()                                                   \
    do                                                                                                 \
    {                                                                                                  \
        RDK_LLM_SKIP_IF_LAB_OLLAMA_UNREACHABLE();                                                      \
        if(!::RDK::LLM::Test::labHasThinkingModel())                                                   \
            GTEST_SKIP() << "Thinking model not on lab Ollama: "                                       \
                         << ::RDK::LLM::Test::labOllamaThinkingModelName();                            \
    } while(0)

/// Legacy name used by existing tests (macro expands GTEST_SKIP in the caller when inlined...).
/// Prefer RDK_LLM_SKIP_IF_LAB_OLLAMA_UNREACHABLE from new code.
inline void skipIfLabOllamaUnreachable()
{
    RDK_LLM_SKIP_IF_LAB_OLLAMA_UNREACHABLE();
}

inline void skipIfLabThinkingModelMissing()
{
    RDK_LLM_SKIP_IF_LAB_THINKING_MODEL_MISSING();
}

} // namespace RDK::LLM::Test

#endif
