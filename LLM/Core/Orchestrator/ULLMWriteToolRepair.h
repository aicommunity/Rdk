#ifndef RDK_ULLM_WRITE_TOOL_REPAIR_H
#define RDK_ULLM_WRITE_TOOL_REPAIR_H

#include "ULLMStepVerifier.h"

#include <nlohmann/json.hpp>
#include <string>

namespace RDK::LLM {

struct RepairAttemptResult {
    bool retry = false;
    nlohmann::json new_args;
    std::string user_message;
};

RepairAttemptResult attemptRepair(const std::string& tool_name, const nlohmann::json& args,
                                  const VerifyResult& verify, int attempt_index);

} // namespace RDK::LLM

#endif
