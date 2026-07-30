#include "RegisterCoreRdkTools.h"

#include "../Context/ILLMProjectContextProvider.h"
#include "../Context/UDocSearchIndex.h"
#include "../LlmPublicApi.h"
#include "../Domain/URdkDomainAccess.h"
#include "../Domain/URdkEntityResolver.h"
#include "../Domain/ULLMNameResolution.h"
#include "RegisterApplicationTools.h"
#include "RegisterObservabilityTools.h"
#include "ULLMToolRegistry.h"

#include <algorithm>
#include <fstream>

namespace RDK::LLM {

namespace {

bool requireStringField(const nlohmann::json& args, const char* key, ToolGatewayResult& r,
                        std::string& out)
{
    if(!args.contains(key) || !args[key].is_string())
    {
        r.ok = false;
        r.error_code = "ARGS_REQUIRED";
        r.message = std::string("Missing required field: ") + key;
        return false;
    }
    out = args[key].get<std::string>();
    return true;
}

} // namespace

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
    URdkDomainAccess* const domain_access = &domain;
    ILLMProjectContextProvider* const project_ctx = project_context;

    registry.registerTool(
        makeDef("get_net_snapshot", LLMToolKind::Read,
                "Use when you need the component graph / topology of the active channel. "
                "Requires an open project/channel. Do not use for docs or class metadata.",
                {{"type", "object"},
                 {"properties",
                  {{"channel_index", {{"type", "integer"}, {"minimum", 0}}},
                   {"max_components", {{"type", "integer"}, {"minimum", 1}, {"maximum", 500}}},
                   {"root_long_name", {{"type", "string"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            int ch = args.value("channel_index", 0);
            int max_c = args.value("max_components", 200);
            const std::string root =
                args.contains("root_long_name") && args["root_long_name"].is_string()
                    ? args["root_long_name"].get<std::string>()
                    : std::string();
            DomainStatus st = domain_access->listNetSnapshot(r.result, ch, max_c, root);
            r.ok = st.ok();
            if(!r.ok)
            {
                r.error_code = "DomainError";
                r.message = st.message;
            }
            return r;
        });

    registry.registerTool(
        makeDef("list_model_links", LLMToolKind::Read,
                "Lists model links with pagination (strict 4-tuple identity)",
                {{"type", "object"},
                 {"properties",
                  {{"channel_index", {{"type", "integer"}, {"minimum", 0}}},
                   {"root_long_name", {{"type", "string"}}},
                   {"offset", {{"type", "integer"}, {"minimum", 0}, {"default", 0}}},
                   {"limit",
                    {{"type", "integer"}, {"minimum", 1}, {"maximum", 2000}, {"default", 500}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [domain_access](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            const int ch = args.value("channel_index", 0);
            const int offset = args.value("offset", 0);
            const int limit = args.value("limit", 500);
            const std::string root =
                args.contains("root_long_name") && args["root_long_name"].is_string()
                    ? args["root_long_name"].get<std::string>()
                    : std::string();
            const DomainStatus st =
                domain_access->listModelLinks(r.result, ch, root, offset, limit);
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
                "Use when discovering which component classes exist (optionally by library). "
                "Do not use to inspect an instance already on the net — use find_component / "
                "get_component_properties instead.",
                {{"type", "object"},
                 {"properties", {{"library_filter", {{"type", "string"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            (void)project_ctx;
            ToolGatewayResult r;
            const std::string library_filter =
                args.contains("library_filter") && args["library_filter"].is_string()
                    ? args["library_filter"].get<std::string>()
                    : std::string();
            DomainStatus st = domain_access->listRegisteredClasses(r.result, library_filter);
            r.ok = st.ok();
            if(!r.ok)
            {
                r.error_code = "DomainError";
                r.message = st.message;
            }
            return r;
        });

    registry.registerTool(
        makeDef("describe_class", LLMToolKind::Read,
                "Use when you need ClDesc / property schema for a registered class name. "
                "Requires a concrete class_name. Do not use for live instance property values.",
                {{"type", "object"},
                 {"required", {"class_name"}},
                 {"properties", {{"class_name", {{"type", "string"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            std::string cn = args.at("class_name").get<std::string>();
            std::vector<std::string> registered;
            if(domain_access->listRegisteredClassNames(registered).ok() && !registered.empty())
            {
                const RegisteredClassResolution resolved = resolveRegisteredClassName(cn, registered);
                if(resolved.status == RegisteredClassResolution::Status::Resolved)
                    cn = resolved.class_name;
                else if(resolved.status == RegisteredClassResolution::Status::Ambiguous)
                {
                    r.ok = true;
                    r.result["ambiguous"] = true;
                    r.result["kind"] = "class";
                    r.result["query"] = cn;
                    r.result["candidates"] = nlohmann::json::array();
                    for(const auto& [name, score] : resolved.candidates)
                        r.result["candidates"].push_back({{"class_name", name}, {"score", score}});
                    return r;
                }
            }
            if(project_ctx)
                r.result["cl_desc_xml_fragment"] = project_ctx->clDescFragment(cn);
            else
                r.result["cl_desc_xml_fragment"] = "";
            r.result["class_name"] = cn;
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeDef("find_component", LLMToolKind::Read,
                "Use when locating components on the net by name/query. "
                "Do not use for class catalog listing — use list_registered_classes.",
                {{"type", "object"},
                 {"required", {"query"}},
                 {"properties",
                  {{"query", {{"type", "string"}}},
                   {"channel_index", {{"type", "integer"}, {"minimum", 0}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            const std::string query = args.at("query").get<std::string>();
            const int ch = args.value("channel_index", 0);
            URdkEntityResolver resolver(*domain_access);
            const EntityResolutionResult resolved = resolver.resolveComponent(query, ch);
            r.result = resolver.toToolJson(resolved);
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeDef("get_component_properties", LLMToolKind::Read,
                "Use when reading property values/metadata of an existing component instance. "
                "Requires a resolved component identity. Do not use for class-level ClDesc.",
                {{"type", "object"},
                 {"required", {"long_name"}},
                 {"properties",
                  {{"long_name", {{"type", "string"}}},
                   {"channel_index", {{"type", "integer"}, {"minimum", 0}}},
                   {"property_names",
                    {{"type", "array"}, {"items", {{"type", "string"}}}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            int ch = args.value("channel_index", 0);
            std::vector<std::string> property_names;
            if(args.contains("property_names") && args["property_names"].is_array())
            {
                for(const nlohmann::json& pn : args["property_names"])
                {
                    if(pn.is_string())
                        property_names.push_back(pn.get<std::string>());
                }
            }
            DomainStatus st = domain_access->getComponentProperties(
                args.at("long_name").get<std::string>(), r.result, ch, property_names);
            r.ok = st.ok();
            if(!r.ok)
            {
                r.error_code = "DomainError";
                r.message = st.message;
            }
            return r;
        });

    registry.registerTool(
        makeDef("search_project_docs", LLMToolKind::Read,
                "Use when answering how-to / conceptual questions from NMSDK docs. "
                "Prefer over guessing. Do not use for live model graph — use get_net_snapshot.",
                {{"type", "object"},
                 {"required", {"query"}},
                 {"properties",
                  {{"query", {{"type", "string"}}},
                   {"top_k", {{"type", "integer"}}},
                   {"scope",
                    {{"type", "string"},
                     {"enum", nlohmann::json::array({"docs", "sources", "all"})},
                     {"default", "docs"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}}),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            (void)domain_access;
            ToolGatewayResult r;
            const std::string query = args.at("query").get<std::string>();
            const int top_k = args.value("top_k", 5);
            const std::string scope = args.value("scope", std::string("docs"));
            std::vector<DocSnippet> snippets;
            if(LLMServices::instance().isInitialized())
                snippets = LLMServices::instance().searchIndex().searchWithScope(query, top_k, scope);
            else if(project_ctx)
                snippets = project_ctx->searchDocs(query, top_k);
            snippets.erase(std::remove_if(snippets.begin(), snippets.end(),
                                            [](const DocSnippet& sn) {
                                                return sn.score < kMinRetrievalScore;
                                            }),
                           snippets.end());
            r.result["snippets"] = nlohmann::json::array();
            for(const DocSnippet& s : snippets)
            {
                r.result["snippets"].push_back({{"source_id", s.source_id},
                                                {"path", s.path},
                                                {"title", s.title},
                                                {"excerpt", s.excerpt},
                                                {"score", s.score},
                                                {"content_kind",
                                                 s.content_kind == LLMContentKind::Source ? "source"
                                                 : s.content_kind == LLMContentKind::RuntimeXml
                                                     ? "runtime_xml"
                                                     : "doc"},
                                                {"start_line", s.start_line}});
            }
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeDef("validate_project", LLMToolKind::Read,
                "Use when dry-running validation of the currently loaded project. "
                "Do not use for a configuration path on disk — use validate_configuration.",
                {{"type", "object"}, {"additionalProperties", false}},
                {{"type", "object"}}),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            ToolGatewayResult r;
            std::vector<std::string> warnings;
            DomainStatus st = domain_access->validateProjectDryRun(warnings);
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
                  {{"class_name", {{"type", "string"}, {"x-llm-semantic", "registered_class"}}},
                   {"parent_long_name",
                    {{"type", "string"}, {"x-llm-semantic", "entity_long_name"}}},
                   {"short_name", {{"type", "string"}}},
                   {"channel_index", {{"type", "integer"}}},
                   {"client_request_id", {{"type", "string"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}},
                true),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            std::string class_name;
            std::string parent_long_name;
            std::string short_name;
            if(!requireStringField(args, "class_name", r, class_name)
               || !requireStringField(args, "parent_long_name", r, parent_long_name)
               || !requireStringField(args, "short_name", r, short_name))
                return r;
            std::string out_name;
            DomainStatus st = domain_access->addComponent(
                class_name, parent_long_name, short_name, args.value("channel_index", 0), out_name);
            r.ok = st.ok();
            r.result["long_name"] = out_name;
            r.result["class_name"] = class_name;
            r.result["parent_long_name"] = parent_long_name;
            nlohmann::json found;
            if(r.ok && domain_access->findComponentByLongName(out_name, found, args.value("channel_index", 0)).ok())
            {
                if(found.contains("short_name"))
                    r.result["short_name"] = found["short_name"];
                if(found.contains("long_name"))
                    r.result["long_name"] = found["long_name"];
            }
            else if(r.ok)
                r.result["short_name"] = short_name;
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
                  {{"long_name", {{"type", "string"}, {"x-llm-semantic", "entity_long_name"}}},
                   {"property_name", {{"type", "string"}}},
                   {"value", {{"type", "string"}}},
                   {"channel_index", {{"type", "integer"}, {"minimum", 0}, {"default", 0}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}},
                true),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            std::string previous;
            DomainStatus st = domain_access->setProperty(args.at("long_name").get<std::string>(),
                                                 args.at("property_name").get<std::string>(),
                                                 args.at("value").get<std::string>(),
                                                 args.value("channel_index", 0), &previous);
            r.ok = st.ok();
            r.result["long_name"] = args.at("long_name");
            r.result["property_name"] = args.at("property_name");
            r.result["had_previous"] = !previous.empty();
            if(!previous.empty())
                r.result["previous_value"] = previous;
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
                  {{"long_name", {{"type", "string"}, {"x-llm-semantic", "entity_long_name"}}},
                   {"channel_index", {{"type", "integer"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}},
                true),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            DomainStatus st = domain_access->removeComponent(args.at("long_name").get<std::string>(),
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

    registry.registerTool(
        makeDef("connect_components", LLMToolKind::Write,
                "Create a data/control link between two component properties",
                {{"type", "object"},
                 {"required",
                  {"from_long_name", "from_property", "to_long_name", "to_property"}},
                 {"properties",
                  {{"from_long_name",
                    {{"type", "string"}, {"x-llm-semantic", "entity_long_name"}}},
                   {"from_property", {{"type", "string"}}},
                   {"to_long_name",
                    {{"type", "string"}, {"x-llm-semantic", "entity_long_name"}}},
                   {"to_property", {{"type", "string"}}},
                   {"channel_index", {{"type", "integer"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}},
                true),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            bool already_existed = false;
            DomainStatus st = domain_access->connectComponents(
                args.at("from_long_name").get<std::string>(),
                args.at("from_property").get<std::string>(),
                args.at("to_long_name").get<std::string>(),
                args.at("to_property").get<std::string>(), args.value("channel_index", 0),
                &already_existed);
            r.ok = st.ok();
            r.result["from_long_name"] = args.at("from_long_name");
            r.result["from_property"] = args.at("from_property");
            r.result["to_long_name"] = args.at("to_long_name");
            r.result["to_property"] = args.at("to_property");
            if(already_existed)
                r.result["already_existed"] = true;
            if(!r.ok)
            {
                r.error_code = "DomainError";
                r.message = st.message;
            }
            return r;
        });

    registry.registerTool(
        makeDef("disconnect_components", LLMToolKind::Write,
                "Break a link between two component properties (undo connect)",
                {{"type", "object"},
                 {"required",
                  {"from_long_name", "from_property", "to_long_name", "to_property"}},
                 {"properties",
                  {{"from_long_name",
                    {{"type", "string"}, {"x-llm-semantic", "entity_long_name"}}},
                   {"from_property", {{"type", "string"}}},
                   {"to_long_name",
                    {{"type", "string"}, {"x-llm-semantic", "entity_long_name"}}},
                   {"to_property", {{"type", "string"}}},
                   {"channel_index", {{"type", "integer"}}}}},
                 {"additionalProperties", false}},
                {{"type", "object"}},
                true),
        [domain_access, project_ctx](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            DomainStatus st = domain_access->breakComponentLink(
                args.at("from_long_name").get<std::string>(),
                args.at("from_property").get<std::string>(),
                args.at("to_long_name").get<std::string>(),
                args.at("to_property").get<std::string>(), args.value("channel_index", 0));
            r.ok = st.ok();
            if(!r.ok)
            {
                r.error_code = "DomainError";
                r.message = st.message;
            }
            return r;
        });

    RegisterApplicationTools(registry);
    RegisterObservabilityTools(registry, domain);
}

} // namespace RDK::LLM
