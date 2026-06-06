#ifndef RDK_LLM_TEST_OLLAMA_LAB_H
#define RDK_LLM_TEST_OLLAMA_LAB_H

#include <string>

namespace RDK::LLM::Test {

/// Lab Ollama host (explicit integration tests).
constexpr const char* kLabOllamaHost = "http://10.245.1.12:11434";
constexpr const char* kLabOllamaOpenAiV1 = "http://10.245.1.12:11434/v1";

/// Returns true if GET /api/tags succeeds (2xx).
bool isLabOllamaReachable(int timeout_ms = 5000);

/// Model for chat tests: env `NMSDK_LLM_OLLAMA_MODEL` or default `qwen2.5:14b`.
std::string labOllamaModelName();

/// Skip current test when lab Ollama is down (does not fail the suite).
void skipIfLabOllamaUnreachable();

} // namespace RDK::LLM::Test

#endif
