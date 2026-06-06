#include "llm_e2e_lab_harness.h"

#include <algorithm>

#include "Orchestrator/ULLMEmbeddedToolCalls.h"

#include "Domain/URdkDomainAccess.h"
#include "Gui/ULLMNoopPresentationSink.h"
#include "LlmPublicApi.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Observability/ULLMAuditLog.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Providers/ULLMProviderFactory.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMToolArgumentValidator.h"

namespace RDK::LLM::E2eLab {

std::string repoPath(const std::string& relative)
{
#ifdef CMAKE_SOURCE_DIR
    return std::string(CMAKE_SOURCE_DIR) + "/" + relative;
#else
    return relative;
#endif
}

std::string sampleValidConfigurationIni()
{
    return repoPath("Bin/Configs/TestValidation/test_valid/project.ini");
}

Harness::Harness(RDK::UApplication* app)
    : domain(app)
    , gateway(registry, policy, domain, audit, idempotency, validator)
{
    static bool services_ready = false;
    if(!services_ready)
    {
        LLMServices& svc = LLMServices::instance();
        if(!svc.isInitialized())
            svc.initialize(app, nullptr);
        if(!svc.presentationSink())
            svc.setPresentationSink(std::make_unique<ULLMNoopPresentationSink>());
        services_ready = true;
    }

    RegisterCoreRdkTools(registry, domain, nullptr);

    profile.kind = LLMProviderKind::OllamaOpenAICompat;
    profile.base_url = kLabOllamaOpenAiV1;
    profile.model = labOllamaModelName();
    profile.profile_id = "e2e-lab-ollama";

    provider = ULLMProviderFactory::create(profile);
    orchestrator = std::make_unique<ULLMAgentOrchestrator>(*provider, registry, gateway, store);
}

LLMSessionContext defaultSession(bool project_loaded)
{
    LLMSessionContext s;
    s.session_id = "e2e-lab-session";
    s.user_id = 1;
    s.llm_write_enabled = true;
    s.allow_save = true;
    s.allow_cloud_llm = false;
    s.project_loaded = project_loaded;
    return s;
}

LLMRequestEnvelope makeRequest(const Harness& h, const std::string& session_id,
                               const std::string& user_text, const LLMSessionContext& session)
{
    LLMRequestEnvelope req;
    req.session_id = session_id;
    req.trace_id = "e2e-trace";
    req.user_text = user_text;
    req.session = session;
    req.provider_profile = h.profile;
    return req;
}

bool findToolResultJson(const ULLMConversationStore& store, const std::string& session_id,
                        const std::string& tool_name, nlohmann::json& out)
{
    ConversationState& state = const_cast<ULLMConversationStore&>(store).getOrCreate(session_id);
    for(const LLMMessage& m : state.messages)
    {
        if(m.role != LLMMessage::Role::Tool || !m.tool_name || *m.tool_name != tool_name)
            continue;
        if(m.content.empty())
            continue;
        try
        {
            out = nlohmann::json::parse(m.content);
            return true;
        }
        catch(...)
        {
            out = {{"raw", m.content}};
            return true;
        }
    }
    return false;
}

bool conversationUsedTool(const ULLMConversationStore& store, const std::string& session_id,
                          const std::string& tool_name)
{
    nlohmann::json discard;
    return findToolResultJson(store, session_id, tool_name, discard);
}

E2eConversationDigest digestConversation(const ULLMConversationStore& store,
                                         const std::string& session_id,
                                         const LLMFinalResponse& final_resp,
                                         const ULLMToolRegistry* registry)
{
    E2eConversationDigest d;
    d.orchestrator_ok = final_resp.ok;
    d.orchestrator_error = final_resp.error;
    d.assistant_text = final_resp.text;
    d.no_suitable_tool = final_resp.no_suitable_tool;
    d.had_pending_confirmation = final_resp.pending_confirmation;

    ConversationState& state = const_cast<ULLMConversationStore&>(store).getOrCreate(session_id);
    if(state.pending)
        d.pending_tool_name = state.pending->request.tool_name;

    for(const LLMMessage& m : state.messages)
    {
        if(m.role == LLMMessage::Role::Assistant && m.assistant_tool_calls)
        {
            for(const LLMToolCall& call : *m.assistant_tool_calls)
            {
                if(!call.name.empty()
                   && std::find(d.tools_invoked.begin(), d.tools_invoked.end(), call.name)
                          == d.tools_invoked.end())
                    d.tools_invoked.push_back(call.name);
            }
        }
        if(m.role == LLMMessage::Role::Tool && m.tool_name)
        {
            const std::string& name = *m.tool_name;
            if(std::find(d.tools_invoked.begin(), d.tools_invoked.end(), name) == d.tools_invoked.end())
                d.tools_invoked.push_back(name);

            nlohmann::json parsed = nlohmann::json::object();
            if(!m.content.empty())
            {
                try
                {
                    parsed = nlohmann::json::parse(m.content);
                }
                catch(...)
                {
                    parsed = {{"raw", m.content}};
                }
            }
            d.tool_results_by_name[name] = std::move(parsed);
        }
        if(registry && m.role == LLMMessage::Role::Assistant && !m.content.empty())
        {
            for(const LLMToolCall& embedded : tryExtractEmbeddedToolCalls(m.content, *registry))
            {
                if(std::find(d.tools_invoked.begin(), d.tools_invoked.end(), embedded.name)
                   == d.tools_invoked.end())
                    d.tools_invoked.push_back(embedded.name);
            }
        }
    }

    if(registry && !final_resp.text.empty())
    {
        for(const LLMToolCall& embedded : tryExtractEmbeddedToolCalls(final_resp.text, *registry))
        {
            if(std::find(d.tools_invoked.begin(), d.tools_invoked.end(), embedded.name)
               == d.tools_invoked.end())
                d.tools_invoked.push_back(embedded.name);
        }
    }

    if(!d.pending_tool_name.empty()
       && std::find(d.tools_invoked.begin(), d.tools_invoked.end(), d.pending_tool_name)
              == d.tools_invoked.end())
        d.tools_invoked.push_back(d.pending_tool_name);

    return d;
}

bool conversationAssistantRequestedTool(const ULLMConversationStore& store,
                                        const std::string& session_id,
                                        const std::string& tool_name)
{
    ConversationState& state = const_cast<ULLMConversationStore&>(store).getOrCreate(session_id);
    for(const LLMMessage& m : state.messages)
    {
        if(m.role != LLMMessage::Role::Assistant || !m.assistant_tool_calls)
            continue;
        for(const LLMToolCall& call : *m.assistant_tool_calls)
        {
            if(call.name == tool_name)
                return true;
        }
    }
    return false;
}

bool shouldSkipForProviderError(const LLMFinalResponse& resp, std::string* reason)
{
    if(resp.ok)
        return false;
    const bool skip = resp.error.find("404") != std::string::npos
                      || resp.error.find("tool") != std::string::npos
                      || resp.error.find("function") != std::string::npos
                      || resp.error.find("write tools are disabled") != std::string::npos;
    if(skip && reason)
        *reason = resp.error;
    return skip;
}

E2eScenarioRun runNaturalLanguageScenario(Harness& h, const std::string& session_id,
                                          const std::string& user_text,
                                          const LLMSessionContext& session,
                                          bool auto_confirm_pending)
{
    E2eScenarioRun run;
    const LLMRequestEnvelope req = makeRequest(h, session_id, user_text, session);
    run.final_response = h.orchestrator->handleUserMessage(req);

    if(auto_confirm_pending && run.final_response.pending_confirmation
       && !run.final_response.pending_confirmation_id.empty())
    {
        run.final_response =
            h.orchestrator->confirmPending(session_id, run.final_response.pending_confirmation_id);
    }

    run.digest = digestConversation(h.store, session_id, run.final_response, &h.registry);
    return run;
}

} // namespace RDK::LLM::E2eLab
