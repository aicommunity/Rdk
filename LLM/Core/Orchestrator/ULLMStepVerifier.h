#ifndef RDK_ULLM_STEP_VERIFIER_H
#define RDK_ULLM_STEP_VERIFIER_H

#include "ULLMExecutionPlan.h"
#include "../Domain/URdkDomainAccess.h"

#include <string>

namespace RDK::LLM {

struct VerifyResult {
    bool satisfied = false;
    std::string detail;
};

VerifyResult verifySuccessCriteria(const SuccessCriteria& criteria,
                                   URdkDomainAccess& domain,
                                   int channel_index);

} // namespace RDK::LLM

#endif
