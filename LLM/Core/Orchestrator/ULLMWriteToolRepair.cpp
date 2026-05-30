#include "ULLMWriteToolRepair.h"

namespace RDK::LLM {

namespace {

bool detailLooksRepairableForConnect(const std::string& detail)
{
    return detail.find("link_missing") != std::string::npos
           || detail.find("missing_quad") != std::string::npos
           || detail.find("component_missing") != std::string::npos;
}

bool detailLooksRepairableForAdd(const std::string& detail)
{
    return detail.find("component_missing") != std::string::npos
           || detail.find("component_under_parent count=0") != std::string::npos;
}

} // namespace

RepairAttemptResult attemptRepair(const std::string& tool_name, const nlohmann::json& args,
                                  const VerifyResult& verify, int attempt_index)
{
    RepairAttemptResult out;
    out.new_args = args;

    if(tool_name == "connect_components" && attempt_index == 0
       && detailLooksRepairableForConnect(verify.detail))
    {
        out.new_args["from_property"] = "";
        out.new_args["to_property"] = "";
        out.retry = true;
        return out;
    }

    if(tool_name == "add_component" && attempt_index == 0
       && verify.detail.find("component_under_parent count=0") != std::string::npos
       && out.new_args.contains("short_name") && out.new_args["short_name"].is_string())
    {
        const std::string base = out.new_args["short_name"].get<std::string>();
        out.new_args["short_name"] = base + std::to_string(attempt_index + 2);
        out.retry = true;
        return out;
    }

    out.user_message = verify.detail.empty() ? "Post-tool verification failed." : verify.detail;
    return out;
}

} // namespace RDK::LLM
