#pragma once

#include <string>

namespace RDK::LLM {

/** Strip/neutralize untrusted tool and retrieval payloads before re-prompting the model. */
std::string sanitizeUntrustedToolContent(const std::string& raw);

} // namespace RDK::LLM
