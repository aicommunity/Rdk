#pragma once

#include <string>

namespace RDK::LLM {

/** Parse Retry-After header (seconds or delay-seconds integer). Returns 0 if absent/invalid. */
int parseRetryAfterDelayMs(const std::string& retry_after_header);

int providerRetryBackoffMs(int attempt);

/** max(backoff, retry-after), capped at max_delay_ms (default 60000). */
int providerRetryDelayMs(int attempt, const std::string& retry_after_header, int max_delay_ms = 60000);

} // namespace RDK::LLM
