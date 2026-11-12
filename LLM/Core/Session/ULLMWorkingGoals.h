#ifndef RDK_ULLM_WORKING_GOALS_H
#define RDK_ULLM_WORKING_GOALS_H

#include <string>

#include "../LlmTypes.h"
#include "../Session/ULLMConversationStore.h"

namespace RDK::LLM {

nlohmann::json workingGoalsToJson(const std::vector<WorkingGoal>& goals);
std::vector<WorkingGoal> workingGoalsFromJson(const nlohmann::json& j);

std::string formatWorkingGoalsEphemeral(const std::vector<WorkingGoal>& goals,
                                        std::size_t max_chars = 1500);

/// Ensure a goal exists (by id); update title/criteria if provided.
void upsertWorkingGoal(ConversationState& state, const std::string& id, const std::string& title,
                       const std::string& success_criteria = {},
                       WorkingGoalStatus status = WorkingGoalStatus::InProgress);

void appendWorkingGoalEvidence(ConversationState& state, const std::string& goal_id,
                               const std::string& evidence_line);

void markWorkingGoalStatus(ConversationState& state, const std::string& goal_id,
                           WorkingGoalStatus status);

/// Seed a single goal from the user turn text when none exist yet.
void ensureTurnWorkingGoal(ConversationState& state, const std::string& user_text);

} // namespace RDK::LLM

#endif
