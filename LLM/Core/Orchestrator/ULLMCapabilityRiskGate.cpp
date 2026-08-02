#include "ULLMCapabilityRiskGate.h"

#include "../Settings/ULLMProviderCatalog.h"
#include "ULLMChannelCalcCommand.h"
#include "ULLMComponentStructureGoal.h"
#include "ULLMConnectPlanParsing.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace RDK::LLM {
namespace {

std::string toLowerAscii(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool containsCi(const std::string& hay, const char* needle)
{
    const std::string h = toLowerAscii(hay);
    const std::string n = toLowerAscii(needle);
    return h.find(n) != std::string::npos;
}

bool looksLikeSmallParamTag(const std::string& model)
{
    const std::string m = toLowerAscii(model);
    static const char* kTags[] = {":0.5b", ":1b", ":1.5b", ":2b", ":3b", ":4b",
                                  ":6b",   ":7b", ":8b",   ":9b"};
    for(const char* tag : kTags)
    {
        if(m.find(tag) != std::string::npos)
            return true;
    }
    return false;
}

bool isDescriptionLikeGoal(const std::string& text)
{
    const std::string t = toLowerAscii(text);
    return t.find("description") != std::string::npos || t.find("описан") != std::string::npos
           || t.find("project description") != std::string::npos;
}

} // namespace

bool isWeakCortexModel(const std::string& profile_id, const std::string& model_name)
{
    if(containsCi(profile_id, "lite") || containsCi(profile_id, "tiny")
       || containsCi(profile_id, "mini"))
        return true;
    if(containsCi(model_name, "tiny") || containsCi(model_name, "mini")
       || containsCi(model_name, "lite"))
        return true;
    return looksLikeSmallParamTag(model_name);
}

bool isComplexCapabilityGoal(const std::string& planning_text, LLMIntentKind intent,
                             bool filter_include_write, bool requires_pending_write)
{
    if(requires_pending_write)
        return true;
    if(intent == LLMIntentKind::Mutate && filter_include_write)
        return true;
    if(intent == LLMIntentKind::Plan)
        return true;
    if(isConnectGoalText(planning_text) || isComponentStructureGoal(planning_text)
       || isChannelCalcGoalText(planning_text) || isDescriptionLikeGoal(planning_text))
        return true;
    return false;
}

std::optional<std::string> findStrongerCortexModel(const std::string& current_model)
{
    const LLMProviderProfile* thinking = ULLMProviderCatalog::findById("ollama-thinking");
    if(!thinking || thinking->model.empty())
        return std::nullopt;
    const std::string cur = toLowerAscii(current_model);
    const std::string strong = toLowerAscii(thinking->model);
    if(!cur.empty() && cur == strong)
        return std::nullopt;
    // Escalate only when the active model is weak, empty, or a non-thinking local default.
    if(current_model.empty() || isWeakCortexModel("active", current_model)
       || (cur.find("qwen2.5") != std::string::npos && strong.find("qwen3") != std::string::npos))
        return thinking->model;
    return std::nullopt;
}

void accumulateToolFailureSignals(const std::vector<TurnToolInvocationView>& trace,
                                  int& tool_error_count, int& path_not_allowed_count)
{
    tool_error_count = 0;
    path_not_allowed_count = 0;
    for(const TurnToolInvocationView& row : trace)
    {
        if(row.ok)
            continue;
        ++tool_error_count;
        const std::string code = toLowerAscii(row.error_code);
        if(code.find("path_not_allowed") != std::string::npos
           || code.find("path_denied") != std::string::npos
           || code.find("invalid_argument") != std::string::npos
           || code.find("schema") != std::string::npos
           || code.find("validation") != std::string::npos)
            ++path_not_allowed_count;
    }
}

float capabilityRiskTau()
{
    if(const char* env = std::getenv("NMSDK_LLM_CAPABILITY_RISK_TAU"))
    {
        try
        {
            const float v = std::stof(env);
            if(v > 0.f && v < 1.f)
                return v;
        }
        catch(...)
        {
        }
    }
    return 0.55f;
}

bool capabilityRiskGateDisabled()
{
    if(const char* env = std::getenv("NMSDK_LLM_CAPABILITY_RISK_DISABLE"))
        return env[0] == '1' || env[0] == 't' || env[0] == 'T' || env[0] == 'y' || env[0] == 'Y';
    return false;
}

float scoreCapabilityRisk(const CapabilityRiskSignals& risk_signals)
{
    float score = 0.f;
    if(risk_signals.weak_cortex_profile && risk_signals.complex_goal)
        score += 0.55f;
    else if(risk_signals.weak_cortex_profile)
        score += 0.20f;
    else if(risk_signals.complex_goal)
        score += 0.10f;

    if(risk_signals.low_intent_confidence)
        score += 0.15f;
    else if(risk_signals.intent_confidence < 0.5f)
        score += 0.10f;

    if(risk_signals.requires_pending_write)
        score += 0.15f;

    if(risk_signals.aoc_recovery_count >= 2)
        score += 0.35f;
    else if(risk_signals.aoc_recovery_count >= 1)
        score += 0.20f;

    if(risk_signals.tool_error_count >= 2)
        score += 0.25f;
    else if(risk_signals.tool_error_count >= 1)
        score += 0.12f;

    if(risk_signals.path_not_allowed_count >= 1)
        score += 0.15f;

    if(risk_signals.aoc_exhausted)
        score += 0.30f;

    return std::min(score, 1.f);
}

CapabilityRiskDecision decideCapabilityRisk(const CapabilityRiskSignals& risk_signals)
{
    CapabilityRiskDecision d;
    if(capabilityRiskGateDisabled())
    {
        d.reason_code = "disabled";
        return d;
    }

    d.score = scoreCapabilityRisk(risk_signals);
    const float tau = capabilityRiskTau();
    if(d.score < tau)
    {
        d.action = CapabilityRiskAction::Continue;
        d.reason_code = "below_tau";
        return d;
    }

    if(risk_signals.stronger_model_available && !risk_signals.cascade_already_used)
    {
        d.action = CapabilityRiskAction::Escalate;
        d.reason_code = "escalate_stronger_model";
        return d;
    }

    d.action = CapabilityRiskAction::Abstain;
    d.reason_code =
        risk_signals.cascade_already_used ? "abstain_after_cascade" : "abstain_no_stronger";
    return d;
}

} // namespace RDK::LLM
