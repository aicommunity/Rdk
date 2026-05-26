#include "ULLMPolicyEngine.h"

#include "../Domain/ULLMWriteArgumentNormalizer.h"
#include "../Domain/URdkApplicationCommands.h"
#include "../Tools/ULLMToolRegistry.h"
#include "ULLMPathPolicy.h"
#include "ULLMUserRole.h"

namespace RDK::LLM {

namespace {

bool isPathPolicyTool(const std::string& name)
{
    static const char* kTools[] = {"load_configuration",
                                   "load_project",
                                   "save_configuration_as",
                                   "save_project",
                                   "create_configuration",
                                   "copy_configuration",
                                   "rename_configuration",
                                   "validate_configuration",
                                   nullptr};
    for(const char** p = kTools; *p; ++p)
    {
        if(name == *p)
            return true;
    }
    return false;
}

bool isSavePolicyTool(const std::string& name)
{
    return name == "save_configuration" || name == "save_configuration_as"
           || name == "save_project" || name == "save_project_metadata";
}

/// URdkApplicationCommands tools (File-menu parity); allowed for local guest (UserId < 0).
bool isApplicationCommandTool(const std::string& name)
{
    static const char* kTools[] = {"create_configuration",
                                   "load_configuration",
                                   "load_project",
                                   "save_configuration",
                                   "save_configuration_as",
                                   "save_project",
                                   "close_configuration",
                                   "update_configuration",
                                   "save_project_metadata",
                                   "reload_configuration_parameters",
                                   "copy_configuration",
                                   "rename_configuration",
                                   "start_channel_calculation",
                                   "pause_channel_calculation",
                                   "reset_channel_calculation",
                                   "step_channel_calculation",
                                   nullptr};
    for(const char** p = kTools; *p; ++p)
    {
        if(name == *p)
            return true;
    }
    return false;
}

std::string extractPathArg(const std::string& tool_name, const nlohmann::json& args,
                           RDK::UApplication* app)
{
    if(tool_name == "rename_configuration" && args.contains("new_directory_path"))
        return args.at("new_directory_path").get<std::string>();
    if(tool_name == "copy_configuration" && args.contains("destination_directory"))
        return args.at("destination_directory").get<std::string>();
    if(tool_name == "create_configuration")
    {
        std::string err;
        return URdkApplicationCommands::resolveProjectIniPath(args, app, err);
    }
    std::string err;
    return URdkApplicationCommands::resolveConfigurationPathFromArgs(args, err);
}

} // namespace

PolicyDecision ULLMPolicyEngine::checkToolInvoke(const ToolInvokeRequest& req,
                                                  const LLMToolDefinition& tool,
                                                  const URdkDomainAccess& domain) const
{
    const LLMUserRole role = resolveUserRole(req.session.user_id);
    DomainSessionInfo info = domain.sessionInfo();
    const bool project_loaded = info.project_loaded || req.session.project_loaded;

    if(tool.kind == LLMToolKind::Write && tool.requires_project_loaded && !project_loaded)
    {
        return {false, "PROJECT_NOT_LOADED", "Project must be loaded for write tools"};
    }
    if(tool.kind == LLMToolKind::Write && !req.session.llm_write_enabled)
    {
        return {false, "WRITE_DISABLED", "LLM write operations disabled in settings"};
    }
    if(tool.kind == LLMToolKind::Write && role == LLMUserRole::Guest
       && !isApplicationCommandTool(tool.name))
    {
        return {false, "RBAC_GUEST_DENIED",
                "Write tools require operator or admin (GetUserId() >= 0)"};
    }
    if(isSavePolicyTool(tool.name) && !req.session.allow_save)
    {
        return {false, "SAVE_DISABLED", "Saving configurations is disabled for this session"};
    }
    if(tool.name == "create_configuration")
    {
        std::string resolve_err;
        const std::string ini =
            URdkApplicationCommands::resolveProjectIniPath(req.arguments, domain.application(),
                                                         resolve_err);
        if(ini.empty())
        {
            return {false, "ARGS_REQUIRED",
                    resolve_err.empty()
                        ? "create_configuration requires a resolvable path (parent_directory, "
                          "project_ini_path, or autocreate under default configs)"
                        : resolve_err};
        }
    }
    if(isPathPolicyTool(tool.name))
    {
        const std::string path = extractPathArg(tool.name, req.arguments, domain.application());
        if(!path.empty())
        {
            std::string path_err;
            if(!ULLMPathPolicy::isAllowed(path, domain.application(), path_err))
                return {false, "PATH_NOT_ALLOWED", path_err};
        }
        else if(tool.name == "load_configuration" || tool.name == "load_project"
                || tool.name == "validate_configuration")
        {
            return {false, "PATH_NOT_ALLOWED", "configuration path is required"};
        }
    }
    if(tool.requires_confirmation && !req.confirmed)
    {
        return {true, "", ""};
    }
    return {true, "", ""};
}

PolicyDecision ULLMPolicyEngine::checkPlan(const ULLMExecutionPlan& plan, const LLMSessionContext& session,
                                           const ULLMToolRegistry& registry) const
{
    if(plan.steps.empty())
        return {false, "PLAN_EMPTY", "Execution plan has no steps"};
    if(static_cast<int>(plan.steps.size()) > 12)
        return {false, "PLAN_TOO_LARGE", "Execution plan exceeds 12 steps"};

    const LLMUserRole role = resolveUserRole(session.user_id);
    for(const ExecutionPlanStep& step : plan.steps)
    {
        const LLMToolDefinition* def = registry.find(step.tool_name);
        if(!def)
            return {false, "PLAN_UNKNOWN_TOOL", "Unknown tool in plan: " + step.tool_name};
        if(def->kind == LLMToolKind::Write && role == LLMUserRole::Guest
           && !isApplicationCommandTool(def->name))
            return {false, "PLAN_RBAC_DENIED", "Plan contains write tools for guest user"};
        if(def->kind == LLMToolKind::Write && !session.llm_write_enabled)
            return {false, "PLAN_WRITE_DISABLED", "Plan contains write tools but LLM write is disabled"};
        if(isSavePolicyTool(def->name) && !session.allow_save)
            return {false, "SAVE_DISABLED", "Plan contains save tools but allow_save is false"};
    }

    bool saw_snapshot = false;
    for(const ExecutionPlanStep& step : plan.steps)
    {
        const LLMToolDefinition* def = registry.find(step.tool_name);
        if(!def)
            continue;
        if(step.tool_name == "get_net_snapshot")
            saw_snapshot = true;
        if(isNetGraphWriteTool(step.tool_name) && !saw_snapshot)
        {
            return {false,
                    "PLAN_NEEDS_SNAPSHOT",
                    "Execution plan must call get_net_snapshot before graph write steps"};
        }
    }
    return {true, "", ""};
}

} // namespace RDK::LLM
