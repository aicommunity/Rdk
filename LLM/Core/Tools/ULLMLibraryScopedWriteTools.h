#ifndef RDK_ULLM_LIBRARY_SCOPED_WRITE_TOOLS_H
#define RDK_ULLM_LIBRARY_SCOPED_WRITE_TOOLS_H

#include <string>
#include <unordered_set>

#include "../Domain/URdkDomainAccess.h"
#include "ULLMToolRegistry.h"

namespace RDK::LLM {

inline LLMToolDefinition makeLibraryWriteDef(const std::string& name, const std::string& desc,
                                             nlohmann::json input, bool confirm = true)
{
    LLMToolDefinition d;
    d.name = name;
    d.kind = LLMToolKind::Write;
    d.description = desc;
    d.input_schema = std::move(input);
    d.output_schema = {{"type", "object"}};
    d.requires_confirmation = confirm;
    return d;
}

inline ToolGatewayResult validateLibraryClassName(const std::string& class_name,
                                                const std::unordered_set<std::string>& allowed,
                                                const char* library_label)
{
    ToolGatewayResult r;
    if(allowed.count(class_name) == 0)
    {
        r.ok = false;
        r.error_code = "CLASS_NOT_IN_LIBRARY";
        r.message = std::string(class_name) + " is not a " + library_label + " component class";
    }
    return r;
}

inline ToolGatewayResult invokeLibraryAddComponent(URdkDomainAccess& domain,
                                                 const nlohmann::json& args,
                                                 const std::unordered_set<std::string>& allowed,
                                                 const char* library_label)
{
    ToolGatewayResult r;
    const std::string class_name = args.at("class_name").get<std::string>();
    if(ToolGatewayResult denied = validateLibraryClassName(class_name, allowed, library_label);
       !denied.ok)
        return denied;

    std::string out_name;
    const DomainStatus st = domain.addComponent(
        class_name, args.at("parent_long_name").get<std::string>(),
        args.at("short_name").get<std::string>(), args.value("channel_index", 0), out_name);
    r.ok = st.ok();
    r.result["long_name"] = out_name;
    r.result["class_name"] = class_name;
    r.result["library"] = library_label;
    if(!r.ok)
    {
        r.error_code = "DomainError";
        r.message = st.message;
    }
    return r;
}

inline ToolGatewayResult invokeLibrarySetProperty(URdkDomainAccess& domain,
                                                  const nlohmann::json& args,
                                                  const std::unordered_set<std::string>& allowed,
                                                  const char* library_label)
{
    ToolGatewayResult r;
    const std::string long_name = args.at("long_name").get<std::string>();
    const int ch = args.value("channel_index", 0);
    nlohmann::json props;
    if(const DomainStatus meta = domain.getComponentProperties(long_name, props, ch); !meta.ok())
    {
        r.ok = false;
        r.error_code = "DomainError";
        r.message = meta.message;
        return r;
    }
    const std::string class_name = props.value("class_name", "");
    if(ToolGatewayResult denied = validateLibraryClassName(class_name, allowed, library_label);
       !denied.ok)
        return denied;

    std::string previous;
    const DomainStatus st =
        domain.setProperty(long_name, args.at("property_name").get<std::string>(),
                           args.at("value").get<std::string>(), ch, &previous);
    r.ok = st.ok();
    r.result["long_name"] = long_name;
    r.result["property_name"] = args.at("property_name");
    r.result["library"] = library_label;
    if(!previous.empty())
        r.result["previous_value"] = previous;
    if(!r.ok)
    {
        r.error_code = "DomainError";
        r.message = st.message;
    }
    return r;
}

} // namespace RDK::LLM

#endif
