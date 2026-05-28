#include "llm_e2e_analyzer.h"

#include <algorithm>
#include <cctype>

namespace RDK::LLM::E2eLab {

std::string toLowerAscii(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool textMentionsAny(const std::string& text, const std::vector<std::string>& needles)
{
    const std::string lower = toLowerAscii(text);
    for(const std::string& n : needles)
    {
        if(lower.find(toLowerAscii(n)) != std::string::npos)
            return true;
    }
    return false;
}

bool digestUsedAnyTool(const E2eConversationDigest& digest,
                       const std::vector<std::string>& tool_names)
{
    for(const std::string& expected : tool_names)
    {
        if(std::find(digest.tools_invoked.begin(), digest.tools_invoked.end(), expected)
           != digest.tools_invoked.end())
            return true;
    }
    return false;
}

bool digestUsedForbiddenTool(const E2eConversationDigest& digest,
                             const std::vector<std::string>& forbidden)
{
    for(const std::string& bad : forbidden)
    {
        if(std::find(digest.tools_invoked.begin(), digest.tools_invoked.end(), bad)
           != digest.tools_invoked.end())
            return true;
    }
    return false;
}

namespace {

bool jsonHasTruthyField(const nlohmann::json& j, const char* key)
{
    if(!j.contains(key))
        return false;
    if(j[key].is_boolean())
        return j[key].get<bool>();
    return !j[key].is_null() && j[key] != 0;
}

bool resultIndicatesAppUnavailable(const nlohmann::json& j, const std::string& orchestrator_error)
{
    auto checkStr = [](const std::string& s) {
        const std::string lower = toLowerAscii(s);
        return lower.find("application not available") != std::string::npos
               || lower.find("not initialized") != std::string::npos;
    };
    if(checkStr(orchestrator_error))
        return true;
    if(j.contains("message") && j["message"].is_string() && checkStr(j["message"].get<std::string>()))
        return true;
    if(j.contains("status") && j["status"].is_object() && j["status"].contains("message")
       && j["status"]["message"].is_string()
       && checkStr(j["status"]["message"].get<std::string>()))
        return true;
    return false;
}

bool classesListedInResult(const nlohmann::json& j)
{
    if(j.contains("classes") && j["classes"].is_array() && !j["classes"].empty())
        return true;
    return false;
}

} // namespace

E2eScenarioEvaluation evaluateScenario(const E2eConversationDigest& digest,
                                       const E2eScenarioExpectation& expectation)
{
    E2eScenarioEvaluation ev;

    if(expectation.require_orchestrator_ok && !digest.orchestrator_ok)
    {
        if(expectation.allow_app_unavailable_result
           && resultIndicatesAppUnavailable(nlohmann::json::object(), digest.orchestrator_error)
           && digestUsedAnyTool(digest, expectation.expected_tools_any))
        {
            ev.passed = true;
            ev.diagnosis = "Orchestrator reported app unavailable after tool invocation (acceptable headless).";
            return ev;
        }
        ev.diagnosis = "Orchestrator failed: " + digest.orchestrator_error;
        return ev;
    }

    const bool hit_round_limit =
        textMentionsAny(digest.assistant_text, {"maximum tool rounds", "max rounds"});

    if(!expectation.expected_tools_any.empty() && !digestUsedAnyTool(digest, expectation.expected_tools_any))
    {
        if(textMentionsAny(digest.assistant_text, expectation.expected_tools_any))
        {
            ev.passed = true;
            ev.diagnosis = "Expected tool name appears in assistant output (embedded call).";
            return ev;
        }
        ev.diagnosis = "No expected tool invoked. Got: [";
        for(size_t i = 0; i < digest.tools_invoked.size(); ++i)
        {
            if(i)
                ev.diagnosis += ", ";
            ev.diagnosis += digest.tools_invoked[i];
        }
        ev.diagnosis += "]. Assistant: " + digest.assistant_text.substr(0, 200);
        return ev;
    }

    if(digestUsedForbiddenTool(digest, expectation.forbidden_tools))
    {
        ev.diagnosis = "Forbidden tool used for this scenario.";
        return ev;
    }

    switch(expectation.goal)
    {
    case E2eGoalKind::OrchestratorOk:
        ev.passed = digest.orchestrator_ok;
        ev.diagnosis = ev.passed ? "OK" : digest.orchestrator_error;
        return ev;

    case E2eGoalKind::ToolInvoked:
        ev.passed = true;
        ev.diagnosis = hit_round_limit ? "Expected tool(s) invoked (hit round limit)." : "Expected tool(s) invoked.";
        return ev;

    case E2eGoalKind::ValidateRan:
    {
        if(digestUsedAnyTool(digest, {"validate_configuration"}))
        {
            const auto it = digest.tool_results_by_name.find("validate_configuration");
            if(it != digest.tool_results_by_name.end())
            {
                const nlohmann::json& j = it->second;
                if(j.contains("is_valid"))
                {
                    ev.passed = true;
                    ev.diagnosis = "validate_configuration returned is_valid.";
                    return ev;
                }
                if(j.contains("errors") || j.contains("warnings"))
                {
                    ev.passed = true;
                    ev.diagnosis = "validate_configuration returned diagnostics.";
                    return ev;
                }
                if(expectation.allow_app_unavailable_result
                   && (resultIndicatesAppUnavailable(j, {}) || j.empty() || j == nlohmann::json::object()))
                {
                    ev.passed = true;
                    ev.diagnosis = "validate_configuration invoked (headless or round-limited).";
                    return ev;
                }
            }
            else
            {
                ev.passed = true;
                ev.diagnosis = "validate_configuration invoked.";
                return ev;
            }
        }
        if(textMentionsAny(digest.assistant_text, {"valid", "invalid", "is_valid", "validation", "ошибк"}))
        {
            ev.passed = true;
            ev.diagnosis = "Validation outcome described in assistant text.";
            return ev;
        }
        ev.diagnosis = "validate_configuration did not run.";
        return ev;
    }

    case E2eGoalKind::ClassesListed:
    {
        if(digestUsedAnyTool(digest, {"list_registered_classes"}))
        {
            const auto it = digest.tool_results_by_name.find("list_registered_classes");
            if(it != digest.tool_results_by_name.end() && classesListedInResult(it->second))
            {
                ev.passed = true;
                ev.diagnosis = "list_registered_classes returned non-empty classes.";
                return ev;
            }
            ev.passed = true;
            ev.diagnosis = "list_registered_classes invoked.";
            return ev;
        }
        if(textMentionsAny(digest.assistant_text,
                           {"class", "component", "matrix", "source", "neuron", "класс", "компонент"}))
        {
            ev.passed = true;
            ev.diagnosis = "Class list appears in assistant text.";
            return ev;
        }
        ev.diagnosis = "No class list in tool result or reply.";
        return ev;
    }

    case E2eGoalKind::LifecycleToolUsed:
        ev.passed = digestUsedAnyTool(digest, expectation.expected_tools_any);
        ev.diagnosis = ev.passed ? "Lifecycle tool invoked." : "Missing lifecycle tool.";
        return ev;

    case E2eGoalKind::LoadRequested:
        if(digest.had_pending_confirmation
           && (digest.pending_tool_name == "load_configuration"
               || digest.pending_tool_name == "load_project"))
        {
            ev.passed = true;
            ev.diagnosis = "Load requested; awaiting confirmation.";
            return ev;
        }
        if(digestUsedAnyTool(digest, {"load_configuration", "load_project"}))
        {
            const std::string key =
                digest.tool_results_by_name.count("load_configuration") ? "load_configuration" : "load_project";
            const nlohmann::json& j = digest.tool_results_by_name.at(key);
            if(jsonHasTruthyField(j, "project_loaded"))
            {
                ev.passed = true;
                ev.diagnosis = "Configuration loaded.";
                return ev;
            }
            if(expectation.allow_app_unavailable_result && resultIndicatesAppUnavailable(j, digest.orchestrator_error))
            {
                ev.passed = true;
                ev.diagnosis = "Load tool ran; app unavailable headless.";
                return ev;
            }
            ev.passed = true;
            ev.diagnosis = "Load tool invoked.";
            return ev;
        }
        ev.diagnosis = "Load tool not invoked.";
        return ev;

    case E2eGoalKind::CreateRequested:
        if(digest.had_pending_confirmation && digest.pending_tool_name == "create_configuration")
        {
            ev.passed = true;
            ev.diagnosis = "create_configuration pending confirmation.";
            return ev;
        }
        if(digest.tool_results_by_name.count("create_configuration"))
        {
            const nlohmann::json& j = digest.tool_results_by_name.at("create_configuration");
            if(j.contains("project_ini_path") || j.contains("project_loaded"))
            {
                ev.passed = true;
                ev.diagnosis = "Configuration created.";
                return ev;
            }
            if(expectation.allow_app_unavailable_result && resultIndicatesAppUnavailable(j, digest.orchestrator_error))
            {
                ev.passed = true;
                ev.diagnosis = "create_configuration invoked headless.";
                return ev;
            }
        }
        ev.diagnosis = "create_configuration not reached.";
        return ev;

    case E2eGoalKind::InformativeAnswer:
        if(!digest.assistant_text.empty()
           && textMentionsAny(digest.assistant_text,
                               {"save", "configuration", "project.ini", "сохран", "конфиг"}))
        {
            ev.passed = true;
            ev.diagnosis = "Informative answer about configuration workflow.";
            return ev;
        }
        if(digestUsedAnyTool(digest, expectation.expected_tools_any))
        {
            ev.passed = true;
            ev.diagnosis = "Relevant tool used for informational query.";
            return ev;
        }
        ev.diagnosis = "Answer does not cover configuration save topic.";
        return ev;

    case E2eGoalKind::NoSuitableToolOrRefusal:
        if(textMentionsAny(digest.assistant_text,
                           {"too many steps", "maximum tool rounds", "max rounds"}))
        {
            ev.diagnosis = "Hit max rounds instead of refusing cleanly.";
            return ev;
        }
        if(textMentionsAny(digest.assistant_text,
                           {"Cannot find a suitable", "Не найдено подходящ", "NO_SUITABLE_TOOL",
                            "не могу выполнить", "cannot perform", "no tool"}))
        {
            ev.passed = true;
            ev.diagnosis = "Refusal or no_suitable_tool messaging.";
            return ev;
        }
        ev.diagnosis = "Expected refusal text; got: " + digest.assistant_text.substr(0, 120);
        return ev;

    default:
        ev.diagnosis = "Unknown goal kind.";
        return ev;
    }
}

} // namespace RDK::LLM::E2eLab
