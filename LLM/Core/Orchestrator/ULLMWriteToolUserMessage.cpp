#include "ULLMWriteToolUserMessage.h"

namespace RDK::LLM {

nlohmann::json toolGatewayResultForProvider(const ToolGatewayResult& tr)
{
    nlohmann::json j = tr.result.is_object() ? tr.result : nlohmann::json::object();
    j["ok"] = tr.ok;
    if(!tr.ok)
    {
        if(!tr.error_code.empty())
            j["error_code"] = tr.error_code;
        if(!tr.message.empty())
            j["message"] = tr.message;
    }
    return j;
}

std::string formatWriteToolUserMessage(const std::string& tool_name, const ToolGatewayResult& tr)
{
    if(!tr.ok)
    {
        if(!tr.message.empty())
            return tr.message;
        return "Operation failed: " + tool_name;
    }

    if(tool_name == "add_component")
    {
        const std::string class_name = tr.result.value("class_name", "");
        const std::string long_name = tr.result.value("long_name", "");
        if(!class_name.empty() && !long_name.empty())
            return "Component added: " + class_name + " (" + long_name + ")";
        if(!class_name.empty())
            return "Component added: " + class_name;
        if(!long_name.empty())
            return "Component added: " + long_name;
        return "Component added to the model.";
    }
    if(tool_name == "remove_component")
        return "Component removed.";
    if(tool_name == "set_property")
        return "Property updated.";
    if(tool_name == "connect_components")
        return "Components connected.";
    if(tool_name == "disconnect_components")
        return "Link removed.";
    return "Change applied: " + tool_name;
}

} // namespace RDK::LLM
