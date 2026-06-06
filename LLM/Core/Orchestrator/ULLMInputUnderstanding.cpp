#include "ULLMInputUnderstanding.h"

#include "ULLMModelRouter.h"

#include <cctype>
#include <cstdlib>
#include <cstring>

namespace RDK::LLM {

namespace {

bool inputEnsembleEnabled()
{
    const char* v = std::getenv("NMSDK_LLM_INPUT_ENSEMBLE");
    if(!v)
        return true;
    return v[0] != '0' && std::strcmp(v, "false") != 0;
}

bool inputLlmClassifierEnabled()
{
    const char* v = std::getenv("NMSDK_LLM_INPUT_ENSEMBLE_LLM");
    if(!v)
        return true;
    return v[0] != '0' && std::strcmp(v, "false") != 0;
}

LLMIntentKind intentFromLabel(const std::string& label_raw)
{
    std::string label = label_raw;
    for(char& c : label)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    if(label.find("mutate") != std::string::npos)
        return LLMIntentKind::Mutate;
    if(label.find("plan") != std::string::npos)
        return LLMIntentKind::Plan;
    if(label.find("explain") != std::string::npos)
        return LLMIntentKind::Explain;
    return LLMIntentKind::Query;
}

} // namespace

InputUnderstandingResult understandUserInput(ILLMProvider* provider, const std::string& text_en,
                                             const IntentParseResult& heuristic,
                                             const LLMProviderProfile* active_profile,
                                             const std::string& trace_id,
                                             const std::string& session_id)
{
    InputUnderstandingResult out;
    out.intent = heuristic.kind;
    out.confidence = heuristic.confidence;
    out.method = heuristic.method.empty() ? "heuristic" : heuristic.method;
    if(!inputEnsembleEnabled())
        return out;

    if(provider && inputLlmClassifierEnabled())
    {
        std::vector<LLMMessage> msgs;
        LLMMessage system;
        system.role = LLMMessage::Role::System;
        system.content =
            "Classify the user message. Reply with JSON only: "
            "{\"intent\":\"query|mutate|explain|plan\",\"confidence\":0.0-1.0,"
            "\"needs_clarification\":false}";
        msgs.push_back(system);
        LLMMessage user;
        user.role = LLMMessage::Role::User;
        user.content = text_en;
        msgs.push_back(user);

        LLMCompletionOptions opts;
        opts.max_tokens = 64;
        opts.temperature = 0.f;
        opts.tools_for_api.clear();
        if(active_profile)
        {
            const ModelRoute route = routeModelForPhase(ModelTier::Router, *active_profile);
            applyModelRouteAudit(route, trace_id, session_id);
            if(const std::optional<std::string> model = modelOverrideForRoute(route))
                opts.model_override = *model;
        }

        const LLMCompletionResult completion = provider->chat(msgs, opts);
        if(completion.ok)
        {
            try
            {
                const nlohmann::json j = nlohmann::json::parse(completion.text);
                out.intent = intentFromLabel(j.value("intent", "query"));
                out.confidence = j.value("confidence", 0.75f);
                out.needs_clarification = j.value("needs_clarification", false);
                out.method = "llm_json";
            }
            catch(...)
            {
                out.intent = intentFromLabel(completion.text);
                out.confidence = 0.7f;
                out.method = "llm_text";
            }
        }
    }

    if(heuristic.confidence >= 0.75f && heuristic.kind != out.intent)
    {
        out.intent = heuristic.kind;
        out.confidence = heuristic.confidence;
        out.method = "heuristic_override";
    }

    if(out.confidence < 0.35f)
    {
        out.needs_clarification = true;
        out.rationale = "low_confidence_intent";
    }
    return out;
}

} // namespace RDK::LLM
