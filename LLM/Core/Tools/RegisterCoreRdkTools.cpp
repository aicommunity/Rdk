#include "RegisterCoreRdkTools.h"

#include "../Context/ILLMProjectContextProvider.h"
#include "../Domain/URdkDomainAccess.h"
#include "../Domain/URdkEntityResolver.h"
#include "ULLMToolRegistry.h"

#include <fstream>

namespace RDK::LLM {

static LLMToolDefinition makeDef(const std::string& name, LLMToolKind kind,
                                 const std::string& desc, nlohmann::json input,
                                 nlohmann::json output, bool confirm = false)
{
    LLMToolDefinition d;
    d.name = name;
    d.kind = kind;
    d.description = desc;
    d.input_schema = std::move(input);
    d.output_schema = std::move(output);
    d.requires_confirmation = confirm;
    d.idempotent = (name == "add_component");
    return d;
}

void RegisterCoreRdkTools(ULLMToolRegistry& registry, URdkDomainAccess& domain,
                          ILLMProjectContextProvider* project_context)
{
    registry.registerTool(
        makeDef("get_net_snapshot", LLMToolKind::Read, "Returns component graph for active channel",
                {{"type", "object"},
                 {"properties",
                  {{"channel_index", {{"type", "integer"}, {"minimum", 0}}},
                   {"max_components", {{"type", "integer"}, {"minimum", 1}, {"maximum", 500}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [&](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            int ch = args.value("channel_index", 0);
            int max_c = args.value("max_components", 200);
            DomainStatus st = domain.listNetSnapshot(r.result, ch, max_c);
            r.ok = st.ok();
            if(!r.ok)
            {
                r.error_code = "DomainError";
                r.message = st.message;
            }
            return r;
        });

    registry.registerTool(
        makeDef("list_registered_classes", LLMToolKind::Read,
                "Lists registered component class names",
                {{"type", "object"}, {"additionalProperties", false}},
                {{"type", "object"}}),
        [&](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            ToolGatewayResult r;
            std::vector<std::string> names;
            DomainStatus st = domain.listRegisteredClassNames(names);
            r.ok = st.ok();
            r.result["classes"] = nlohmann::json::array();
            for(const auto& n : names)
                r.result["classes"].push_back({{"class_name", n}});
            if(!r.ok)
            {
                r.error_code = "DomainError";
                r.message = st.message;
            }
            return r;
        });

    registry.registerTool(
        makeDef("describe_class", LLMToolKind::Read, "Returns ClDesc fragment for a class",
                {{"type", "object"},
                 {"required", {"class_name"}},
                 {"properties", {{"class_name", {{"type", "string"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [&](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            const std::string cn = args.at("class_name").get<std::string>();
            if(project_context)
                r.result["cl_desc_xml_fragment"] = project_context->clDescFragment(cn);
            else
                r.result["cl_desc_xml_fragment"] = "";
            r.result["class_name"] = cn;
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeDef("find_component", LLMToolKind::Read, "Find components by query string",
                {{"type", "object"},
                 {"required", {"query"}},
                 {"properties",
                  {{"query", {{"type", "string"}}},
                   {"channel_index", {{"type", "integer"}, {"minimum", 0}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [&](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            const std::string query = args.at("query").get<std::string>();
            const int ch = args.value("channel_index", 0);
            URdkEntityResolver resolver(domain);
            const EntityResolutionResult resolved = resolver.resolveComponent(query, ch);
            r.result = resolver.toToolJson(resolved);
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeDef("get_component_properties", LLMToolKind::Read, "Get component metadata",
                {{"type", "object"},
                 {"required", {"long_name"}},
                 {"properties",
                  {{"long_name", {{"type", "string"}}},
                   {"channel_index", {{"type", "integer"}, {"minimum", 0}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [&](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            int ch = args.value("channel_index", 0);
            DomainStatus st =
                domain.getComponentProperties(args.at("long_name").get<std::string>(), r.result, ch);
            r.ok = st.ok();
            if(!r.ok)
            {
                r.error_code = "DomainError";
                r.message = st.message;
            }
            return r;
        });

    registry.registerTool(
        makeDef("search_project_docs", LLMToolKind::Read, "Search NMSDK documentation",
                {{"type", "object"},
                 {"required", {"query"}},
                 {"properties", {{"query", {{"type", "string"}}}, {"top_k", {{"type", "integer"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [&](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            const std::string query = args.at("query").get<std::string>();
            int top_k = args.value("top_k", 5);
            if(project_context)
            {
                auto snippets = project_context->searchDocs(query, top_k);
                r.result["snippets"] = nlohmann::json::array();
                for(const auto& s : snippets)
                {
                    r.result["snippets"].push_back(
                        {{"path", s.path}, {"title", s.title}, {"excerpt", s.excerpt}, {"score", s.score}});
                }
            }
            else
                r.result["snippets"] = nlohmann::json::array();
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeDef("validate_project", LLMToolKind::Read, "Dry-run project validation",
                {{"type", "object"}, {"additionalProperties", false}},
                {{"type", "object"}}),
        [&](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            ToolGatewayResult r;
            std::vector<std::string> warnings;
            DomainStatus st = domain.validateProjectDryRun(warnings);
            r.ok = st.ok();
            r.result["warnings"] = warnings;
            r.result["ok"] = warnings.empty();
            return r;
        });

    registry.registerTool(
        makeDef("add_component", LLMToolKind::Write, "Add a component to the model",
                {{"type", "object"},
                 {"required", {"class_name", "parent_long_name", "short_name"}},
                 {"properties",
                  {{"class_name", {{"type", "string"}}},
                   {"parent_long_name", {{"type", "string"}}},
                   {"short_name", {{"type", "string"}}},
                   {"channel_index", {{"type", "integer"}}},
                   {"client_request_id", {{"type", "string"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}},
                true),
        [&](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            std::string out_name;
            DomainStatus st = domain.addComponent(
                args.at("class_name").get<std::string>(),
                args.at("parent_long_name").get<std::string>(),
                args.at("short_name").get<std::string>(), args.value("channel_index", 0), out_name);
            r.ok = st.ok();
            r.result["long_name"] = out_name;
            if(!r.ok)
            {
                r.error_code = "DomainError";
                r.message = st.message;
            }
            return r;
        });

    registry.registerTool(
        makeDef("set_property", LLMToolKind::Write, "Set a component property",
                {{"type", "object"},
                 {"required", {"long_name", "property_name", "value"}},
                 {"properties",
                  {{"long_name", {{"type", "string"}}},
                   {"property_name", {{"type", "string"}}},
                   {"value", {{"type", "string"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}},
                true),
        [&](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            DomainStatus st = domain.setProperty(args.at("long_name").get<std::string>(),
                                                 args.at("property_name").get<std::string>(),
                                                 args.at("value").get<std::string>(),
                                                 args.value("channel_index", 0));
            r.ok = st.ok();
            if(!r.ok)
            {
                r.error_code = "DomainError";
                r.message = st.message;
            }
            return r;
        });

    registry.registerTool(
        makeDef("remove_component", LLMToolKind::Write, "Remove a component from the model",
                {{"type", "object"},
                 {"required", {"long_name"}},
                 {"properties",
                  {{"long_name", {{"type", "string"}}},
                   {"channel_index", {{"type", "integer"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}},
                true),
        [&](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            DomainStatus st = domain.removeComponent(args.at("long_name").get<std::string>(),
                                                     args.value("channel_index", 0));
            r.ok = st.ok();
            r.result["removed"] = args.at("long_name").get<std::string>();
            if(!r.ok)
            {
                r.error_code = "DomainError";
                r.message = st.message;
            }
            return r;
        });
}

} // namespace RDK::LLM
