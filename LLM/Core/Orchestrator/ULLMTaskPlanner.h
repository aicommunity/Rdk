#ifndef RDK_ULLM_TASK_PLANNER_H
#define RDK_ULLM_TASK_PLANNER_H

#include "ULLMExecutionPlan.h"
#include "ULLMQuantityTypes.h"
#include "../Context/ULinkPatternCatalog.h"

#include <optional>
#include "../Domain/URdkDomainAccess.h"
#include "../Providers/ILLMProvider.h"
#include "../Tools/ULLMToolRegistry.h"

namespace RDK::LLM {

struct ConversationState;

struct TaskPlanRequest {
    std::string goal_en;
    LLMSessionContext session;
    bool project_loaded = false;
    ULinkPatternCatalog* link_catalog = nullptr;
    std::optional<ResolvedUserQuantity> resolved_quantity;
    const ConversationState* state = nullptr;
};

struct TaskPlanResult {
    bool ok = false;
    ULLMExecutionPlan plan;
    float confidence = 0.f;
    std::vector<std::string> issues;
};

TaskPlanResult buildTaskPlan(ILLMProvider& provider,
                             ULLMToolRegistry& registry,
                             URdkDomainAccess& domain,
                             const TaskPlanRequest& req);

} // namespace RDK::LLM

#endif
