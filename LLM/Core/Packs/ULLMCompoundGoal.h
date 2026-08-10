#ifndef RDK_LLM_COMPOUND_GOAL_H
#define RDK_LLM_COMPOUND_GOAL_H

#include <string>
#include <vector>

namespace RDK::LLM {

/// DD-PACK-003: conjunction + multiple actionable families in one user utterance.
bool isCompoundActionableGoal(const std::string& user_text);

/// Fixed Recorded dependency order (mutate before side-effect calc).
const std::vector<std::string>& packRecordedDependencyOrder();

int packDependencyRank(const std::string& pack_id);

const char* workingGoalIdForPack(const std::string& pack_id);
const char* workingGoalTitleForPack(const std::string& pack_id);

/// Map tool name → pack working goal id (empty if unknown).
std::string workingGoalIdForToolName(const std::string& tool_name);

} // namespace RDK::LLM

#endif
