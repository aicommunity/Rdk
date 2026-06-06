#include "RegisterObservabilityTools.h"

#include "../Domain/URdkDomainAccess.h"
#include "../Observability/ULLMSystemLogPolicy.h"
#include "../Observability/ULLMSystemLogReader.h"
#include "ULLMToolRegistry.h"

#include <sstream>

namespace RDK::LLM {

namespace {

LLMToolDefinition makeDef(const std::string& name, const std::string& desc, nlohmann::json input,
                          nlohmann::json output)
{
    LLMToolDefinition d;
    d.name = name;
    d.kind = LLMToolKind::Read;
    d.description = desc;
    d.input_schema = std::move(input);
    d.output_schema = std::move(output);
    return d;
}

} // namespace

void RegisterObservabilityTools(ULLMToolRegistry& registry, URdkDomainAccess& domain)
{
    if(!ULLMSystemLogReader::isFeatureEnabled())
        return;

    RDK::UApplication* const app = domain.application();

    registry.registerTool(
        makeDef("get_system_log_policy",
                "Returns current project/channel logging policy (debug/events levels).",
                {{"type", "object"},
                 {"properties", {{"channel_index", {{"type", "integer"}, {"minimum", 0}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [app](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            const int channel = args.value("channel_index", 0);
            const SystemLogPolicySnapshot snap = buildSystemLogPolicySnapshot(app, channel);
            r.ok = true;
            r.result = {{"summary_for_model", snap.summary_for_model}, {"policy", snap.details}};
            return r;
        });

    registry.registerTool(
        makeDef("read_system_log",
                "Reads recent lines from NeuroModeler system log files on disk.",
                {{"type", "object"},
                 {"properties",
                  {{"max_lines", {{"type", "integer"}, {"minimum", 1}, {"maximum", 200}}},
                   {"min_severity",
                    {{"type", "string"},
                     {"enum", {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"}}}},
                   {"mode", {{"type", "string"}, {"enum", {"tail", "since_mark"}}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [app](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            if(!app)
            {
                r.ok = false;
                r.error_code = "NoApplication";
                r.message = "Application context is not available";
                return r;
            }

            ULLMSystemLogReader reader(app);
            reader.syncPaths();

            const int channel = args.value("channel_index", 0);
            const SystemLogPolicySnapshot pol = reader.policy(channel);
            const int default_sev = ULLMSystemLogReader::defaultMinSeverityForPolicy(pol);
            const std::string sev_token = args.value("min_severity", std::string());
            const int min_sev =
                sev_token.empty()
                    ? default_sev
                    : ULLMSystemLogReader::parseSeverityToken(sev_token, default_sev);

            std::size_t max_lines = args.value("max_lines", 80);
            if(max_lines > ULLMSystemLogReader::toolReadMaxLines())
                max_lines = ULLMSystemLogReader::toolReadMaxLines();

            const std::string mode = args.value("mode", "tail");
            const std::string excerpt =
                mode == "since_mark" ? reader.collectDeltaExcerpt(max_lines, min_sev)
                                     : reader.readTailExcerpt(max_lines, min_sev);

            std::vector<std::string> line_vec;
            if(!excerpt.empty())
            {
                std::istringstream stream(excerpt);
                std::string line;
                while(std::getline(stream, line))
                    line_vec.push_back(line);
            }

            std::string policy_reminder;
            if(min_sev == RDK_EX_DEBUG && !pol.project_debug_mode)
            {
                policy_reminder =
                    "DEBUG logging is disabled in project settings; only INFO and above are "
                    "recorded.";
            }

            const ApplicationLogReadPaths paths = app->GetApplicationLogReadPaths();
            nlohmann::json directories = nlohmann::json::array();
            for(const std::string& dir : paths.directories)
                directories.push_back(dir);

            r.ok = true;
            r.result = {{"lines", line_vec},
                          {"line_count", line_vec.size()},
                          {"truncated", line_vec.size() >= max_lines},
                          {"policy_reminder", policy_reminder},
                          {"log_directories", directories},
                          {"effective_min_severity", severityName(default_sev)}};
            return r;
        });
}

} // namespace RDK::LLM
