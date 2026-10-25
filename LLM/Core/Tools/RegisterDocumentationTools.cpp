#include "RegisterDocumentationTools.h"

#include "../Context/ILLMProjectContextProvider.h"
#include "../Context/ULLMDocOpenPolicy.h"
#include "../Domain/ULLMNameResolution.h"
#include "../Domain/URdkDomainAccess.h"
#include "../Gui/ILLMPresentationSink.h"
#include "../LlmPublicApi.h"
#include "../Settings/ULLMSettingsStore.h"
#include "ULLMToolRegistry.h"

#include <filesystem>

namespace RDK::LLM {
namespace {

LLMToolDefinition makeDocDef(const std::string& name, const std::string& desc, nlohmann::json input)
{
    LLMToolDefinition d;
    d.name = name;
    d.kind = LLMToolKind::Read;
    d.description = desc;
    d.input_schema = std::move(input);
    d.output_schema = {{"type", "object"}};
    return d;
}

ILLMPresentationSink* activeSink()
{
    if(!LLMServices::instance().isInitialized())
        return nullptr;
    return LLMServices::instance().presentationSink();
}

std::filesystem::path repoRootOf(ILLMProjectContextProvider* ctx)
{
    if(!ctx)
        return {};
    return ctx->paths().repository_root;
}

std::string helpLocaleHint(const std::string& preferred)
{
    if(preferred.rfind("ru", 0) == 0)
        return "ru";
    return "en";
}

} // namespace

void RegisterDocumentationTools(ULLMToolRegistry& registry, URdkDomainAccess& domain,
                                ILLMProjectContextProvider* project_context)
{
    URdkDomainAccess* domain_access = &domain;

    registry.registerTool(
        makeDocDef("list_help_topics",
                   "List GUI User Guide HTML topics under Bin/Help/{locale}. "
                   "Use before open_help when the user asks what help pages exist.",
                   {{"type", "object"},
                    {"properties",
                     {{"locale",
                       {{"type", "string"},
                        {"enum", nlohmann::json::array({"ru", "en"})},
                        {"default", "en"}}}}},
                    {"additionalProperties", false}}),
        [project_context](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            const std::filesystem::path root = repoRootOf(project_context);
            if(root.empty())
            {
                r.ok = false;
                r.error_code = "NOT_FOUND";
                r.message = "Repository root unavailable";
                return r;
            }
            std::string locale = args.value("locale", std::string("en"));
            if(locale != "ru" && locale != "en")
                locale = "en";
            const std::filesystem::path dir = root / "Bin" / "Help" / locale;
            r.result["locale"] = locale;
            r.result["topics"] = nlohmann::json::array();
            if(std::filesystem::is_directory(dir))
            {
                for(const auto& ent : std::filesystem::directory_iterator(dir))
                {
                    if(!ent.is_regular_file())
                        continue;
                    const auto ext = ent.path().extension().string();
                    if(ext != ".html" && ext != ".htm")
                        continue;
                    const std::string topic = ent.path().filename().string();
                    r.result["topics"].push_back(
                        {{"topic", topic},
                         {"help_uri", makeHelpUri(topic)},
                         {"title", ent.path().stem().string()}});
                }
            }
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeDocDef("open_help",
                   "Open the NeuroModeler GUI User Guide (HTML Help). "
                   "Optional topic filename e.g. getting-started.html; omit for index.",
                   {{"type", "object"},
                    {"properties", {{"topic", {{"type", "string"}}}}},
                    {"additionalProperties", false}}),
        [project_context](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            const std::string topic = args.value("topic", std::string());
            const std::filesystem::path root = repoRootOf(project_context);
            std::string locale = "en";
            if(LLMServices::instance().isInitialized())
            {
                locale = helpLocaleHint(
                    LLMServices::instance().settings().runtime().preferred_response_language);
            }
            if(!root.empty())
            {
                const DocOpenResolve resolved = resolveHelpTopic(topic, root, locale);
                if(!resolved.ok && resolved.error_code == "NOT_FOUND")
                {
                    r.ok = false;
                    r.error_code = resolved.error_code;
                    r.message = "Help topic not found";
                    r.result["topic"] = topic.empty() ? "index.html" : topic;
                    return r;
                }
            }
            ILLMPresentationSink* sink = activeSink();
            if(!sink)
            {
                r.ok = false;
                r.error_code = "HOST_UNAVAILABLE";
                r.message = "Presentation host unavailable";
                return r;
            }
            r.result = sink->openHelpTopic(topic);
            r.ok = r.result.value("ok", false);
            if(!r.ok)
            {
                r.error_code = "HOST_UNAVAILABLE";
                r.message = r.result.value("error", "Failed to open help");
            }
            return r;
        });

    registry.registerTool(
        makeDocDef("open_class_docs",
                   "Open the GUI Class Description (ClDesc) panel for a registered class. "
                   "Prefer over describe_class when the user asks to open/show class help.",
                   {{"type", "object"},
                    {"required", {"class_name"}},
                    {"properties", {{"class_name", {{"type", "string"}}}}},
                    {"additionalProperties", false}}),
        [domain_access](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            std::string cn = args.at("class_name").get<std::string>();
            std::vector<std::string> registered;
            if(domain_access->listRegisteredClassNames(registered).ok() && !registered.empty())
            {
                const RegisteredClassResolution resolved =
                    resolveRegisteredClassName(cn, registered);
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
                        r.result["candidates"].push_back(
                            {{"class_name", name},
                             {"score", score},
                             {"class_uri", makeClassUri(name)}});
                    return r;
                }
            }
            ILLMPresentationSink* sink = activeSink();
            if(!sink)
            {
                r.ok = false;
                r.error_code = "HOST_UNAVAILABLE";
                r.message = "Presentation host unavailable";
                r.result["class_name"] = cn;
                return r;
            }
            r.result = sink->openClassDescription(cn);
            r.result["class_name"] = cn;
            r.result["class_uri"] = makeClassUri(cn);
            r.ok = r.result.value("ok", false);
            if(!r.ok)
            {
                r.error_code = "HOST_UNAVAILABLE";
                r.message = r.result.value("error", "Failed to open class description");
            }
            return r;
        });

    registry.registerTool(
        makeDocDef(
            "open_documentation",
            "Open a markdown documentation file from indexed Docs roots (repo Docs/, "
            "Libraries/*/Docs, Bin/Docs, Rdk/Docs). Pass path, doc_uri (nmsdk-doc:...), or "
            "source_id+path from search_project_docs hits.",
            {{"type", "object"},
             {"properties",
              {{"path", {{"type", "string"}}},
               {"doc_uri", {{"type", "string"}}},
               {"source_id", {{"type", "string"}}},
               {"title", {{"type", "string"}}}}},
             {"additionalProperties", false}}),
        [project_context](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            const std::filesystem::path root = repoRootOf(project_context);
            if(root.empty())
            {
                r.ok = false;
                r.error_code = "NOT_FOUND";
                r.message = "Repository root unavailable";
                return r;
            }
            DocOpenResolve resolved;
            if(args.contains("doc_uri") && args["doc_uri"].is_string())
                resolved = resolveDocUri(args["doc_uri"].get<std::string>(), root);
            else if(args.contains("path") && args["path"].is_string())
                resolved = resolveMarkdownPath(args["path"].get<std::string>(), root);
            else
            {
                r.ok = false;
                r.error_code = "INVALID_URI";
                r.message = "Provide path or doc_uri";
                return r;
            }
            if(!resolved.ok || resolved.kind != "markdown")
            {
                r.ok = false;
                r.error_code = resolved.error_code.empty() ? "PATH_NOT_ALLOWED" : resolved.error_code;
                r.message = "Documentation path not allowed or not found";
                r.result["error_code"] = r.error_code;
                return r;
            }
            ILLMPresentationSink* sink = activeSink();
            if(!sink)
            {
                r.ok = false;
                r.error_code = "HOST_UNAVAILABLE";
                r.message = "Presentation host unavailable";
                r.result["doc_uri"] = resolved.doc_uri;
                return r;
            }
            const std::string title = args.value("title", std::string());
            r.result =
                sink->openMarkdownDocument(resolved.abs_path.string(), title);
            r.result["doc_uri"] = resolved.doc_uri;
            r.result["path"] = resolved.abs_path.string();
            if(args.contains("source_id"))
                r.result["source_id"] = args["source_id"];
            r.ok = r.result.value("ok", false);
            if(!r.ok)
            {
                r.error_code = "HOST_UNAVAILABLE";
                r.message = r.result.value("error", "Failed to open documentation");
            }
            return r;
        });
}

} // namespace RDK::LLM
