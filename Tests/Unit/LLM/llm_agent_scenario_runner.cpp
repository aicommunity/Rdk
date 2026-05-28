#include "llm_agent_scenario_runner.h"

#include <cstdlib>
#include <memory>
#include <sstream>

#include "Domain/URdkDomainAccess.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Providers/ULLMMockProvider.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/RegisterApplicationTools.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMToolArgumentValidator.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"
#include "Gui/ULLMNoopPresentationSink.h"
#include "LlmPublicApi.h"
#include "Settings/ULLMSettingsStore.h"
#include "llm_e2e_lab_harness.h"

#ifdef CMAKE_SOURCE_DIR
#include "RegisterHardwareLibLlmTools.h"
#include "RegisterPulseLibLlmTools.h"
#include "RegisterMotionControlLibLlmTools.h"
#endif

namespace RDK::LLM::AgentScenarios {

namespace {

struct EnvRestore {
    std::map<std::string, std::optional<std::string>> prior;
    ~EnvRestore()
    {
        for(const auto& [key, value] : prior)
        {
            if(value)
                setenv(key.c_str(), value->c_str(), 1);
            else
                unsetenv(key.c_str());
        }
    }
};

void applyEnv(const std::map<std::string, std::string>& env, EnvRestore& restore)
{
    for(const auto& [key, value] : env)
    {
        const char* old = std::getenv(key.c_str());
        restore.prior[key] = old ? std::optional<std::string>(old) : std::nullopt;
        if(value.empty())
            unsetenv(key.c_str());
        else
            setenv(key.c_str(), value.c_str(), 1);
    }
}

LLMCompletionResult completionFromJson(const nlohmann::json& step)
{
    LLMCompletionResult r;
    r.ok = step.value("ok", true);
    r.text = step.value("text", std::string());
    if(step.contains("tool_calls") && step["tool_calls"].is_array())
    {
        int idx = 0;
        for(const auto& tc : step["tool_calls"])
        {
            if(!tc.is_object())
                continue;
            LLMToolCall call;
            call.id = tc.value("id", std::string("call_") + std::to_string(idx++));
            call.name = tc.value("name", std::string());
            call.arguments = tc.value("arguments", nlohmann::json::object());
            r.tool_calls.push_back(std::move(call));
        }
    }
    return r;
}

std::string collectEphemeralSystemText(const ULLMConversationStore& store,
                                       const std::string& session_id)
{
    std::ostringstream oss;
    const ConversationState* state = store.findSession(session_id);
    if(!state)
        return {};
    for(const LLMMessage& m : state->messages)
    {
        if(m.role == LLMMessage::Role::System)
            oss << m.content << '\n';
    }
    return oss.str();
}

LLMGuiContextSnapshot guiFromSpec(const std::optional<AgentGuiSpec>& gui,
                                  const AgentSessionSpec& session)
{
    LLMGuiContextSnapshot snap;
    snap.channel_index = session.active_channel_index;
    if(!gui)
        return snap;
    snap.channel_index = gui->channel_index;
    snap.focused_component_long_name = gui->focused_component_long_name;
    snap.focused_class_name = gui->focused_class_name;
    snap.project_xml_path = gui->project_xml_path;
    return snap;
}

} // namespace

void registerToolsForProfile(const RegistryProfile profile, ULLMToolRegistry& registry)
{
    URdkDomainAccess domain(nullptr);
    RegisterCoreRdkTools(registry, domain, nullptr);
#ifdef CMAKE_SOURCE_DIR
    if(profile == RegistryProfile::Full)
    {
        RegisterPulseLibLlmTools(registry, nullptr, domain);
        RegisterHardwareLibLlmTools(registry, nullptr, domain);
        RegisterMotionControlLibLlmTools(registry, nullptr, domain);
    }
#endif
}

void ensureLlmServicesForHeadlessTools()
{
    static bool ready = false;
    if(ready)
        return;
    LLMServices& svc = LLMServices::instance();
    if(!svc.isInitialized())
        svc.initialize(nullptr, nullptr);
    if(!svc.presentationSink())
        svc.setPresentationSink(std::make_unique<ULLMNoopPresentationSink>());
    ready = true;
}

void applySessionRuntimeOverrides(const AgentSessionSpec& spec)
{
    if(!LLMServices::instance().isInitialized())
        return;
    ULLMSettingsStore& settings = LLMServices::instance().settings();
    settings.setLlmWriteEnabled(spec.llm_write_enabled);
    settings.setLlmAutoApplyWrites(spec.auto_apply_writes);
}

AgentScenarioHarness makeHarness(const RegistryProfile profile)
{
    ensureLlmServicesForHeadlessTools();
    AgentScenarioHarness h;
    h.mock_provider = std::make_unique<ULLMMockProvider>();
    (void)profile;
    return h;
}

LLMSessionContext sessionFromSpec(const AgentSessionSpec& spec, const std::string& session_id)
{
    LLMSessionContext s;
    s.session_id = session_id;
    s.project_loaded = spec.project_loaded;
    s.llm_write_enabled = spec.llm_write_enabled;
    s.auto_apply_writes = spec.auto_apply_writes;
    s.active_channel_index = spec.active_channel_index;
    if(spec.autonomous_mode == "strict")
        s.autonomous_mode = LLMAutonomousMode::Strict;
    else if(spec.autonomous_mode == "semi_auto")
        s.autonomous_mode = LLMAutonomousMode::SemiAuto;
    else
        s.autonomous_mode = LLMAutonomousMode::Off;
    return s;
}

void enqueueMockScript(ULLMMockProvider& provider, const std::vector<nlohmann::json>& script)
{
    for(const nlohmann::json& step : script)
        provider.enqueue(completionFromJson(step));
}

AgentScenarioRun runDeterministicScenario(AgentScenarioHarness& harness,
                                          const AgentScenarioCase& scenario)
{
    AgentScenarioRun out;
    EnvRestore env_restore;
    applyEnv(scenario.env, env_restore);
    if(scenario.env.find("NMSDK_LLM_INTENT_LLM") == scenario.env.end())
        unsetenv("NMSDK_LLM_INTENT_LLM");
    if(scenario.env.find("NMSDK_LLM_TRANSLATE_QUERIES") == scenario.env.end())
        setenv("NMSDK_LLM_TRANSLATE_QUERIES", "0", 1);

    ULLMToolRegistry registry;
    registerToolsForProfile(scenario.registry_profile, registry);
    applySessionRuntimeOverrides(scenario.session);

    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(*harness.mock_provider, registry, gateway, store);

    const std::string session_id = "agent-scenario-" + scenario.id;
    LLMSessionContext session = sessionFromSpec(scenario.session, session_id);
    const LLMGuiContextSnapshot gui_snap = guiFromSpec(scenario.gui, scenario.session);

    auto run_one_turn = [&](const std::string& user_text,
                            const std::vector<nlohmann::json>& mock_script, bool confirm_pending) {
        harness.mock_provider->resetQueue();
        enqueueMockScript(*harness.mock_provider, mock_script);
        LLMRequestEnvelope req;
        req.session_id = session_id;
        req.trace_id = "trace-" + scenario.id;
        req.user_text = user_text;
        req.session = session;
        req.gui = gui_snap;
        out.final_response = orch.handleUserMessage(req);
        if(confirm_pending && out.final_response.pending_confirmation
           && !out.final_response.pending_confirmation_id.empty())
        {
            out.final_response =
                orch.confirmPending(session_id, out.final_response.pending_confirmation_id);
        }
        out.provider_invoke_count += harness.mock_provider->invokeCount();
        out.mock_queue_remaining = harness.mock_provider->remaining();
    };

    if(!scenario.turns.empty())
    {
        for(const AgentScenarioTurn& turn : scenario.turns)
            run_one_turn(turn.user_text, turn.mock_script, turn.confirm_pending);
    }
    else
    {
        run_one_turn(scenario.user_text, scenario.mock_script, scenario.confirm_pending);
    }

    out.digest = E2eLab::digestConversation(store, session_id, out.final_response, &registry);
    out.ephemeral_system_text = collectEphemeralSystemText(store, session_id);

    ConversationState& state = store.getOrCreate(session_id);
    for(const LLMMessage& m : state.messages)
    {
        if(m.role == LLMMessage::Role::Tool)
            ++out.tool_message_count;
    }

    return out;
}

AgentScenarioRun runE2eScenario(E2eLab::Harness& lab, const AgentScenarioCase& scenario)
{
    ensureLlmServicesForHeadlessTools();
    AgentScenarioRun out;
    EnvRestore env_restore;
    applyEnv(scenario.env, env_restore);
    setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);

    const std::string session_id = "agent-e2e-" + scenario.id;
    LLMSessionContext session = sessionFromSpec(scenario.session, session_id);

    E2eLab::E2eScenarioRun run =
        E2eLab::runNaturalLanguageScenario(lab, session_id, scenario.user_text, session,
                                           scenario.confirm_pending);

    out.final_response = run.final_response;
    out.digest = run.digest;
    if(lab.provider)
    {
        if(auto* mock = dynamic_cast<ULLMMockProvider*>(lab.provider.get()))
        {
            out.provider_invoke_count = mock->invokeCount();
            out.mock_queue_remaining = mock->remaining();
        }
    }
    return out;
}

} // namespace RDK::LLM::AgentScenarios
