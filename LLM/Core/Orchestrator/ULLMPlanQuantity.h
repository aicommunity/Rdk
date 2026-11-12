#ifndef RDK_ULLM_PLAN_QUANTITY_H
#define RDK_ULLM_PLAN_QUANTITY_H

#include <optional>
#include <string>

#include "ULLMExecutionPlan.h"

namespace RDK::LLM {

/// Raises `repeat_count` on repeatable plan steps when the goal or session quantity requests N>1.
void applyGoalQuantityToExecutionPlan(ULLMExecutionPlan& plan,
                                      std::optional<int> session_primary_quantity = std::nullopt);

std::string uniqueShortNameForAddRepeat(const std::string& base_short_name, int repeat_index);

} // namespace RDK::LLM

#endif
