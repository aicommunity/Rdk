#ifndef RDK_ULLM_CLARIFICATION_FORMAT_H
#define RDK_ULLM_CLARIFICATION_FORMAT_H

#include <nlohmann/json.hpp>
#include <string>

namespace RDK::LLM {

/// Human-readable clarification prompt from a disambiguation payload.
/// When include_candidate_list is false, omit the numbered dump (UI shows
/// user_choice_options separately).
std::string formatClarificationMessage(const nlohmann::json& payload,
                                       bool include_candidate_list = true);

/// Prepend session last_add class as candidate #1 (same-as-last). No-op if empty/duplicate.
void enrichClassDisambiguationWithLastAdd(nlohmann::json& disambiguation,
                                          const std::string& last_added_class);

} // namespace RDK::LLM

#endif
