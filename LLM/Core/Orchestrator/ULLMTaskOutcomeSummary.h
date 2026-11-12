#ifndef RDK_ULLM_TASK_OUTCOME_SUMMARY_H
#define RDK_ULLM_TASK_OUTCOME_SUMMARY_H

#include <nlohmann/json.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace RDK::LLM {

/// Leaf component name from a long_name (after last '/').
std::string taskOutcomeShortName(const std::string& long_name);

/// One line: `FromLeaf.FromPort → ToLeaf.ToPort` from connect_components args.
std::string formatConnectLinkOutcomeLine(const nlohmann::json& args);

/// One line for a successful add_component (class + short/long name).
std::string formatAddComponentOutcomeLine(const nlohmann::json& args,
                                          const nlohmann::json& result);

/// Human-readable TaskExecutor summary: list links/adds, plan_id only in the tail.
std::string buildTaskExecuteSummary(const std::vector<std::string>& connect_lines,
                                    const std::vector<std::string>& add_lines,
                                    const std::string& plan_id, std::size_t completed_steps,
                                    bool ok, const std::string& fail_reason);

} // namespace RDK::LLM

#endif
