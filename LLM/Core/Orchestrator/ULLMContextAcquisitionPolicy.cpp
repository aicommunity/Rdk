#include "ULLMContextAcquisitionPolicy.h"

#include <sstream>

namespace RDK::LLM {

ContextAcquisitionPlan defaultContextAcquisitionPlan(const ConversationState& state,
                                                       const LLMSessionContext& session)
{
    ContextAcquisitionPlan plan;
    plan.bootstrap_session = !state.session_context_seeded;
    plan.prefetch_snapshot = session.project_loaded && state.known_facts.empty();
    plan.prefetch_docs = false;
    return plan;
}

std::string formatKnownFactsBlock(const std::vector<std::string>& facts)
{
    if(facts.empty())
        return {};
    std::ostringstream oss;
    oss << "## Known facts (session)\n";
    for(const std::string& f : facts)
        oss << "- " << f << "\n";
    return oss.str();
}

} // namespace RDK::LLM
