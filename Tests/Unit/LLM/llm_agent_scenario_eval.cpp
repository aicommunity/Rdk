#include "llm_agent_scenario_eval.h"

#include "Orchestrator/ULLMIntentParser.h"
#include "llm_e2e_analyzer.h"

#include <algorithm>
#include <sstream>

namespace RDK::LLM::AgentScenarios {

namespace {

bool containsAll(const std::string& hay, const std::string& needle)
{
    return hay.find(needle) != std::string::npos;
}

bool containsAny(const std::string& hay, const std::vector<std::string>& needles)
{
    for(const std::string& n : needles)
    {
        if(containsAll(hay, n))
            return true;
    }
    return false;
}

bool setContains(const std::vector<std::string>& hay, const std::string& needle)
{
    return std::find(hay.begin(), hay.end(), needle) != hay.end();
}

bool setContainsAny(const std::vector<std::string>& hay, const std::vector<std::string>& needles)
{
    for(const std::string& n : needles)
    {
        if(setContains(hay, n))
            return true;
    }
    return false;
}

bool setContainsAll(const std::vector<std::string>& hay, const std::vector<std::string>& needles)
{
    for(const std::string& n : needles)
    {
        if(!setContains(hay, n))
            return false;
    }
    return true;
}

} // namespace

AgentScenarioVerdict evaluateDeterministic(const AgentScenarioCase& scenario,
                                           const AgentScenarioRun& run)
{
    AgentScenarioVerdict v;
    const AgentScenarioExpect& e = scenario.expect;
    const LLMFinalResponse& f = run.final_response;
    const E2eLab::E2eConversationDigest& d = run.digest;

    if(e.expect_intent)
    {
        ULLMIntentParser parser;
        const IntentParseResult parsed = parser.parseDetailed(scenario.user_text);
        if(parsed.kind != *e.expect_intent)
        {
            v.diagnosis = "Intent mismatch for phrase.";
            return v;
        }
    }

    if(e.orchestrator_ok && *e.orchestrator_ok != f.ok)
    {
        v.diagnosis = "orchestrator_ok expected " + std::string(*e.orchestrator_ok ? "true" : "false")
                      + " got " + (f.ok ? "true" : "false") + " err=" + f.error;
        return v;
    }

    if(e.no_suitable_tool && *e.no_suitable_tool != f.no_suitable_tool)
    {
        v.diagnosis = "no_suitable_tool flag mismatch.";
        return v;
    }

    if(e.needs_argument_clarification
       && *e.needs_argument_clarification != f.needs_argument_clarification)
    {
        v.diagnosis = "needs_argument_clarification mismatch.";
        return v;
    }

    if(e.needs_entity_clarification && *e.needs_entity_clarification != f.needs_entity_clarification)
    {
        v.diagnosis = "needs_entity_clarification mismatch.";
        return v;
    }

    if(e.pending_confirmation && *e.pending_confirmation != f.pending_confirmation)
    {
        v.diagnosis = "pending_confirmation mismatch.";
        return v;
    }

    if(e.pending_plan_execution && *e.pending_plan_execution != f.pending_plan_execution)
    {
        v.diagnosis = "pending_plan_execution mismatch.";
        return v;
    }

    if(!e.tools_invoked.empty() && d.tools_invoked != e.tools_invoked)
    {
        std::ostringstream oss;
        oss << "tools_invoked exact mismatch. expected=[";
        for(size_t i = 0; i < e.tools_invoked.size(); ++i)
        {
            if(i)
                oss << ", ";
            oss << e.tools_invoked[i];
        }
        oss << "] got=[";
        for(size_t i = 0; i < d.tools_invoked.size(); ++i)
        {
            if(i)
                oss << ", ";
            oss << d.tools_invoked[i];
        }
        oss << "]";
        v.diagnosis = oss.str();
        return v;
    }

    if(!e.tools_invoked_any.empty() && !setContainsAny(d.tools_invoked, e.tools_invoked_any))
    {
        v.diagnosis = "None of expected tools_invoked_any were invoked.";
        return v;
    }

    if(!e.tools_invoked_all.empty() && !setContainsAll(d.tools_invoked, e.tools_invoked_all))
    {
        v.diagnosis = "Not all tools_invoked_all present.";
        return v;
    }

    if(!e.tools_forbidden.empty() && setContainsAny(d.tools_invoked, e.tools_forbidden))
    {
        v.diagnosis = "Forbidden tool was invoked.";
        return v;
    }

    if(e.tools_invoked_count_min
       && static_cast<int>(d.tools_invoked.size()) < *e.tools_invoked_count_min)
    {
        v.diagnosis = "Too few tools invoked.";
        return v;
    }

    if(e.tools_invoked_count_max
       && static_cast<int>(d.tools_invoked.size()) > *e.tools_invoked_count_max)
    {
        v.diagnosis = "Too many tools invoked.";
        return v;
    }

    for(const std::string& req : e.final_text_contains)
    {
        if(!containsAll(f.text, req))
        {
            v.diagnosis = "final_text missing required substring: " + req;
            return v;
        }
    }

    if(!e.final_text_contains_any.empty() && !containsAny(f.text, e.final_text_contains_any))
    {
        v.diagnosis = "final_text missing any of final_text_contains_any.";
        return v;
    }

    for(const std::string& bad : e.final_text_not_contains)
    {
        if(containsAll(f.text, bad))
        {
            v.diagnosis = "final_text contains forbidden substring: " + bad;
            return v;
        }
    }

    if(e.provider_rounds_max && run.provider_invoke_count > static_cast<size_t>(*e.provider_rounds_max))
    {
        v.diagnosis = "Too many provider rounds (uncontrolled dialog).";
        return v;
    }

    if(e.tool_messages_max && run.tool_message_count > static_cast<size_t>(*e.tool_messages_max))
    {
        v.diagnosis = "Too many tool messages in store.";
        return v;
    }

    if(e.mock_queue_empty && *e.mock_queue_empty && run.mock_queue_remaining != 0)
    {
        v.diagnosis = "Mock provider queue not empty after run.";
        return v;
    }

    if(!e.system_prompt_contains_any.empty()
       && !containsAny(run.ephemeral_system_text, e.system_prompt_contains_any))
    {
        v.diagnosis = "ephemeral system prompt missing required substring.";
        return v;
    }

    v.passed = true;
    v.diagnosis = "OK";
    return v;
}

E2eLab::E2eScenarioEvaluation evaluateE2e(const AgentScenarioCase& scenario,
                                          const AgentScenarioRun& run)
{
    if(!scenario.e2e)
    {
        E2eLab::E2eScenarioEvaluation ev;
        ev.diagnosis = "Missing e2e spec.";
        return ev;
    }

    E2eLab::E2eScenarioExpectation exp;
    exp.id = scenario.id;
    exp.goal = scenario.e2e->goal;
    exp.expected_tools_any = scenario.e2e->expected_tools_any;
    exp.forbidden_tools = scenario.e2e->forbidden_tools;
    exp.require_orchestrator_ok = scenario.e2e->require_orchestrator_ok;
    exp.allow_app_unavailable_result = scenario.e2e->allow_app_unavailable_result;

    E2eLab::E2eScenarioEvaluation ev = E2eLab::evaluateScenario(run.digest, exp);

    if(scenario.expect.provider_rounds_max
       && run.provider_invoke_count > static_cast<size_t>(*scenario.expect.provider_rounds_max))
    {
        ev.passed = false;
        ev.diagnosis = "Too many provider rounds for e2e scenario.";
    }

    for(const std::string& bad : scenario.expect.final_text_not_contains)
    {
        if(containsAll(run.final_response.text, bad))
        {
            ev.passed = false;
            ev.diagnosis = "final_text contains forbidden: " + bad;
            break;
        }
    }

    return ev;
}

} // namespace RDK::LLM::AgentScenarios
