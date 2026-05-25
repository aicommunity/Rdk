#ifndef RDK_ULLM_POLICY_LIMITS_H
#define RDK_ULLM_POLICY_LIMITS_H

namespace RDK::LLM {

struct LLMPolicyLimits {
    int max_tool_rounds = 8;
    int max_tool_invocations_per_message = 24;
    int max_cloud_provider_rounds_per_session = 40;
    /// HITL write confirmation validity (TD-025).
    int confirmation_ttl_seconds = 600;
};

inline constexpr int kDefaultMaxToolRounds = 8;
inline constexpr int kDefaultMaxToolInvocationsPerMessage = 24;

inline const LLMPolicyLimits& defaultPolicyLimits()
{
    static const LLMPolicyLimits kLimits;
    return kLimits;
}

} // namespace RDK::LLM

#endif
