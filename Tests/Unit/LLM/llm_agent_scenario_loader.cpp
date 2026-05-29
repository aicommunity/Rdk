#include "llm_agent_scenario_loader.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace RDK::LLM::AgentScenarios {

namespace {

RegistryProfile parseRegistryProfileImpl(const std::string& s)
{
    if(s == "full")
        return RegistryProfile::Full;
    return RegistryProfile::Core;
}

AgentScenarioTier parseTierImpl(const std::string& s)
{
    if(s == "e2e")
        return AgentScenarioTier::E2e;
    return AgentScenarioTier::Deterministic;
}

std::optional<LLMIntentKind> parseIntentKindImpl(const std::string& s)
{
    if(s == "mutate")
        return LLMIntentKind::Mutate;
    if(s == "explain")
        return LLMIntentKind::Explain;
    if(s == "plan")
        return LLMIntentKind::Plan;
    if(s == "query")
        return LLMIntentKind::Query;
    return std::nullopt;
}

E2eLab::E2eGoalKind parseE2eGoal(const std::string& s)
{
    if(s == "OrchestratorOk")
        return E2eLab::E2eGoalKind::OrchestratorOk;
    if(s == "ValidateRan")
        return E2eLab::E2eGoalKind::ValidateRan;
    if(s == "ClassesListed")
        return E2eLab::E2eGoalKind::ClassesListed;
    if(s == "LifecycleToolUsed")
        return E2eLab::E2eGoalKind::LifecycleToolUsed;
    if(s == "LoadRequested")
        return E2eLab::E2eGoalKind::LoadRequested;
    if(s == "CreateRequested")
        return E2eLab::E2eGoalKind::CreateRequested;
    if(s == "InformativeAnswer")
        return E2eLab::E2eGoalKind::InformativeAnswer;
    if(s == "NoSuitableToolOrRefusal")
        return E2eLab::E2eGoalKind::NoSuitableToolOrRefusal;
    return E2eLab::E2eGoalKind::ToolInvoked;
}

void readStringArray(const nlohmann::json& j, const char* key, std::vector<std::string>& out)
{
    if(!j.contains(key) || !j[key].is_array())
        return;
    for(const auto& item : j[key])
    {
        if(item.is_string())
            out.push_back(item.get<std::string>());
    }
}

AgentSessionSpec parseSession(const nlohmann::json& j)
{
    AgentSessionSpec s;
    if(!j.is_object())
        return s;
    s.project_loaded = j.value("project_loaded", false);
    s.llm_write_enabled = j.value("llm_write_enabled", true);
    s.auto_apply_writes = j.value("auto_apply_writes", false);
    s.active_channel_index = j.value("active_channel_index", 0);
    s.autonomous_mode = j.value("autonomous_mode", std::string("off"));
    return s;
}

} // namespace

RegistryProfile parseRegistryProfile(const std::string& s)
{
    return parseRegistryProfileImpl(s);
}

AgentScenarioTier parseTier(const std::string& s)
{
    return parseTierImpl(s);
}

std::optional<LLMIntentKind> parseIntentKind(const std::string& s)
{
    return parseIntentKindImpl(s);
}

AgentScenarioExpect parseExpect(const nlohmann::json& j)
{
    AgentScenarioExpect e;
    if(!j.is_object())
        return e;
    if(j.contains("orchestrator_ok"))
        e.orchestrator_ok = j["orchestrator_ok"].get<bool>();
    if(j.contains("no_suitable_tool"))
        e.no_suitable_tool = j["no_suitable_tool"].get<bool>();
    if(j.contains("needs_argument_clarification"))
        e.needs_argument_clarification = j["needs_argument_clarification"].get<bool>();
    if(j.contains("needs_entity_clarification"))
        e.needs_entity_clarification = j["needs_entity_clarification"].get<bool>();
    if(j.contains("pending_confirmation"))
        e.pending_confirmation = j["pending_confirmation"].get<bool>();
    if(j.contains("pending_plan_execution"))
        e.pending_plan_execution = j["pending_plan_execution"].get<bool>();
    readStringArray(j, "tools_invoked", e.tools_invoked);
    readStringArray(j, "tools_invoked_any", e.tools_invoked_any);
    readStringArray(j, "tools_invoked_all", e.tools_invoked_all);
    readStringArray(j, "tools_forbidden", e.tools_forbidden);
    if(j.contains("tools_invoked_count_min"))
        e.tools_invoked_count_min = j["tools_invoked_count_min"].get<int>();
    if(j.contains("tools_invoked_count_max"))
        e.tools_invoked_count_max = j["tools_invoked_count_max"].get<int>();
    readStringArray(j, "final_text_contains", e.final_text_contains);
    readStringArray(j, "final_text_contains_any", e.final_text_contains_any);
    readStringArray(j, "final_text_not_contains", e.final_text_not_contains);
    readStringArray(j, "system_prompt_contains_any", e.system_prompt_contains_any);
    if(j.contains("provider_rounds_max"))
        e.provider_rounds_max = j["provider_rounds_max"].get<int>();
    if(j.contains("tool_messages_max"))
        e.tool_messages_max = j["tool_messages_max"].get<int>();
    if(j.contains("mock_queue_empty"))
        e.mock_queue_empty = j["mock_queue_empty"].get<bool>();
    if(j.contains("expect_intent") && j["expect_intent"].is_string())
        e.expect_intent = parseIntentKindImpl(j["expect_intent"].get<std::string>());
    return e;
}

AgentScenarioCase parseScenarioFile(const nlohmann::json& root, const std::string& source_path)
{
    AgentScenarioCase c;
    c.source_path = source_path;
    c.schema_version = root.value("schema_version", 1);
    if(c.schema_version != 1)
        throw std::runtime_error("Unsupported schema_version in " + source_path);

    c.id = root.at("id").get<std::string>();
    c.tier = parseTierImpl(root.value("tier", "deterministic"));
    c.suite = root.value("suite", std::string());
    c.registry_profile = parseRegistryProfileImpl(root.value("registry_profile", "core"));
    c.user_text = root.at("user_text").get<std::string>();
    c.session = parseSession(root.value("session", nlohmann::json::object()));
    c.confirm_pending = root.value("confirm_pending", false);

    if(root.contains("tags") && root["tags"].is_array())
    {
        for(const auto& t : root["tags"])
            if(t.is_string())
                c.tags.push_back(t.get<std::string>());
    }

    if(root.contains("env") && root["env"].is_object())
    {
        for(auto it = root["env"].begin(); it != root["env"].end(); ++it)
            c.env[it.key()] = it.value().is_string() ? it.value().get<std::string>() : "";
    }

    if(root.contains("expect_intent") && root["expect_intent"].is_string())
        c.expect_intent = parseIntentKindImpl(root["expect_intent"].get<std::string>());

    if(root.contains("mock_script") && root["mock_script"].is_array())
    {
        for(const auto& step : root["mock_script"])
            c.mock_script.push_back(step);
    }

    c.expect = parseExpect(root.value("expect", nlohmann::json::object()));
    if(c.expect_intent && !c.expect.expect_intent)
        c.expect.expect_intent = c.expect_intent;

    if(root.contains("gui") && root["gui"].is_object())
    {
        AgentGuiSpec gui;
        const nlohmann::json& gj = root["gui"];
        gui.focused_component_long_name = gj.value("focused_component_long_name", "");
        gui.focused_class_name = gj.value("focused_class_name", "");
        gui.project_xml_path = gj.value("project_xml_path", "");
        gui.channel_index = gj.value("channel_index", 0);
        c.gui = gui;
    }

    if(root.contains("turns") && root["turns"].is_array())
    {
        for(const auto& turn_j : root["turns"])
        {
            AgentScenarioTurn turn;
            turn.user_text = turn_j.value("user_text", "");
            if(turn_j.contains("mock_script") && turn_j["mock_script"].is_array())
            {
                for(const auto& step : turn_j["mock_script"])
                    turn.mock_script.push_back(step);
            }
            turn.confirm_pending = turn_j.value("confirm_pending", false);
            turn.reload_persisted_session = turn_j.value("reload_persisted_session", false);
            if(turn_j.contains("expect"))
                turn.expect = parseExpect(turn_j["expect"]);
            c.turns.push_back(std::move(turn));
        }
    }

    if(root.contains("pre_resolved_entities") && root["pre_resolved_entities"].is_array())
    {
        for(const auto& ent : root["pre_resolved_entities"])
        {
            if(!ent.is_object())
                continue;
            AgentPreResolvedEntity rec;
            rec.kind = ent.value("kind", "");
            rec.query_key = ent.value("query_key", "");
            rec.canonical_value = ent.value("canonical_value", "");
            rec.channel_index = ent.value("channel_index", 0);
            if(!rec.kind.empty() && !rec.query_key.empty() && !rec.canonical_value.empty())
                c.pre_resolved_entities.push_back(std::move(rec));
        }
    }

    if(root.contains("e2e") && root["e2e"].is_object())
    {
        AgentE2eSpec e2e;
        const nlohmann::json& ej = root["e2e"];
        e2e.goal = parseE2eGoal(ej.value("goal", "ToolInvoked"));
        readStringArray(ej, "expected_tools_any", e2e.expected_tools_any);
        readStringArray(ej, "forbidden_tools", e2e.forbidden_tools);
        e2e.require_orchestrator_ok = ej.value("require_orchestrator_ok", true);
        e2e.allow_app_unavailable_result = ej.value("allow_app_unavailable_result", true);
        c.e2e = e2e;
    }

    if(c.tier == AgentScenarioTier::E2e && !c.e2e)
        throw std::runtime_error("e2e tier requires e2e object: " + source_path);

    return c;
}

namespace {

AgentScenarioCase mergeVariant(const AgentScenarioCase& base, const nlohmann::json& variant,
                               const std::string& source_path)
{
    AgentScenarioCase c = base;
    if(variant.contains("id_suffix"))
        c.id = base.id + "__" + variant["id_suffix"].get<std::string>();
    if(variant.contains("user_text"))
        c.user_text = variant["user_text"].get<std::string>();
    if(variant.contains("session"))
        c.session = parseSession(variant["session"]);
    if(variant.contains("mock_script") && variant["mock_script"].is_array())
    {
        c.mock_script.clear();
        for(const auto& step : variant["mock_script"])
            c.mock_script.push_back(step);
    }
    if(variant.contains("expect"))
    {
        AgentScenarioExpect over = parseExpect(variant["expect"]);
        if(over.orchestrator_ok)
            c.expect.orchestrator_ok = over.orchestrator_ok;
        if(over.no_suitable_tool)
            c.expect.no_suitable_tool = over.no_suitable_tool;
        if(over.needs_argument_clarification)
            c.expect.needs_argument_clarification = over.needs_argument_clarification;
        if(over.needs_entity_clarification)
            c.expect.needs_entity_clarification = over.needs_entity_clarification;
        if(over.pending_confirmation)
            c.expect.pending_confirmation = over.pending_confirmation;
        if(over.pending_plan_execution)
            c.expect.pending_plan_execution = over.pending_plan_execution;
        if(!over.tools_invoked.empty())
            c.expect.tools_invoked = over.tools_invoked;
        if(!over.tools_invoked_any.empty())
            c.expect.tools_invoked_any = over.tools_invoked_any;
        if(!over.tools_invoked_all.empty())
            c.expect.tools_invoked_all = over.tools_invoked_all;
        if(!over.tools_forbidden.empty())
            c.expect.tools_forbidden = over.tools_forbidden;
        if(over.tools_invoked_count_min)
            c.expect.tools_invoked_count_min = over.tools_invoked_count_min;
        if(over.tools_invoked_count_max)
            c.expect.tools_invoked_count_max = over.tools_invoked_count_max;
        if(!over.final_text_contains.empty())
            c.expect.final_text_contains = over.final_text_contains;
        if(!over.final_text_contains_any.empty())
            c.expect.final_text_contains_any = over.final_text_contains_any;
        if(!over.final_text_not_contains.empty())
            c.expect.final_text_not_contains = over.final_text_not_contains;
        if(over.provider_rounds_max)
            c.expect.provider_rounds_max = over.provider_rounds_max;
        if(over.tool_messages_max)
            c.expect.tool_messages_max = over.tool_messages_max;
        c.expect.mock_queue_empty = over.mock_queue_empty;
        if(over.expect_intent)
            c.expect.expect_intent = over.expect_intent;
    }
    c.source_path = source_path;
    return c;
}

void expandScenario(const nlohmann::json& root, const std::string& path,
                    std::vector<AgentScenarioCase>& out)
{
    if(root.contains("variants") && root["variants"].is_array())
    {
        const AgentScenarioCase base = parseScenarioFile(root, path);
        for(const auto& v : root["variants"])
        {
            if(!v.is_object())
                continue;
            out.push_back(mergeVariant(base, v, path));
        }
        return;
    }
    out.push_back(parseScenarioFile(root, path));
}

} // namespace

std::vector<AgentScenarioCase> loadAllAgentScenarios(const std::string& root_dir)
{
    std::vector<AgentScenarioCase> cases;
    const fs::path root(root_dir);
    if(!fs::is_directory(root))
        return cases;

    for(const auto& entry : fs::recursive_directory_iterator(root))
    {
        if(!entry.is_regular_file() || entry.path().extension() != ".json")
            continue;
        if(entry.path().filename() == "manifest.json")
            continue;
        if(entry.path().parent_path().filename() == "schema")
            continue;
        if(entry.path().parent_path().filename() == "mock-patterns")
            continue;

        std::ifstream in(entry.path());
        if(!in)
            continue;
        nlohmann::json j;
        try
        {
            in >> j;
        }
        catch(...)
        {
            continue;
        }
        if(!j.is_object() || !j.contains("id"))
            continue;
        try
        {
            expandScenario(j, entry.path().string(), cases);
        }
        catch(...)
        {
            continue;
        }
    }
    return cases;
}

} // namespace RDK::LLM::AgentScenarios
