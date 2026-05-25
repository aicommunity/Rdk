#ifndef RDK_ULLM_CONFIRMATION_EXPIRY_H
#define RDK_ULLM_CONFIRMATION_EXPIRY_H

#include <chrono>
#include <cstdint>

#include "../Policy/ULLMPolicyLimits.h"
#include "ULLMConversationStore.h"

namespace RDK::LLM {

inline int64_t confirmationNowUnixSec()
{
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

inline bool isPendingConfirmationExpired(const PendingConfirmation& pending, int ttl_seconds)
{
    if(pending.created_at_unix_sec <= 0 || ttl_seconds <= 0)
        return false;
    return confirmationNowUnixSec() - pending.created_at_unix_sec >= ttl_seconds;
}

inline int defaultConfirmationTtlSeconds()
{
    return defaultPolicyLimits().confirmation_ttl_seconds;
}

} // namespace RDK::LLM

#endif
