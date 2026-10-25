#include "RegisterProjectKnowledgeTools.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <optional>
#include <regex>
#include <sstream>
#include <unordered_set>

#include <filesystem>

#include "../Context/ULinkPatternCatalog.h"
#include "../Context/ULLMIndexCatalogs.h"
#include "../Domain/URdkDomainAccess.h"
#include "../LlmModuleInit.h"
#include "../LlmPublicApi.h"
#include "../Policy/ULLMPathPolicy.h"
#include "ULLMToolRegistry.h"

namespace fs = std::filesystem;

namespace RDK::LLM {
namespace {

LLMToolDefinition makeReadDef(const std::string& name, const std::string& desc, nlohmann::json input)
{
    LLMToolDefinition d;
    d.name = name;
    d.kind = LLMToolKind::Read;
    d.description = desc;
    d.input_schema = std::move(input);
    d.output_schema = {{"type", "object"}};
    return d;
}

RDK::UApplication* appOrNull()
{
    if(!LLMServices::instance().isInitialized())
        return nullptr;
    return LLMServices::instance().domain().application();
}

std::string defaultProjectRoot()
{
    RDK::UApplication* app = appOrNull();
    if(!app || !app->GetProjectOpenFlag())
        return {};
    return app->GetProjectPath();
}

bool pathAllowed(const std::string& path, std::string& err)
{
    return ULLMPathPolicy::isAllowed(path, appOrNull(), err);
}

fs::path resolveUnderPolicy(const std::string& path_arg, std::string& err)
{
    std::string path = path_arg;
    if(path.empty())
        path = defaultProjectRoot();
    else
        path = ULLMPathPolicy::rewriteRelativeConfigPath(path, appOrNull());
    if(path.empty())
    {
        err = "No path given and no project is open";
        return {};
    }
    path = ULLMPathPolicy::normalizePath(path, err);
    if(!err.empty())
        return {};
    if(!pathAllowed(path, err))
        return {};
    return fs::path(path);
}

fs::path findModelXml(const fs::path& config_dir)
{
    if(fs::is_regular_file(config_dir) && config_dir.extension() == ".xml")
        return config_dir;
    const fs::path dir = fs::is_directory(config_dir) ? config_dir : config_dir.parent_path();
    for(const char* name : {"model.xml", "Model.xml", "Model_00.xml", "model_00.xml"})
    {
        const fs::path candidate = dir / name;
        if(fs::is_regular_file(candidate))
            return candidate;
    }
    if(!fs::is_directory(dir))
        return {};
    for(const auto& ent : fs::directory_iterator(dir))
    {
        if(!ent.is_regular_file())
            continue;
        const std::string fname = ent.path().filename().string();
        if(fname.rfind("Model_", 0) == 0 && ent.path().extension() == ".xml")
            return ent.path();
    }
    return {};
}

std::string readTextCapped(const fs::path& file, size_t max_bytes, std::string& err)
{
    std::ifstream in(file, std::ios::binary);
    if(!in)
    {
        err = "Cannot open file";
        return {};
    }
    std::string data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if(data.size() > max_bytes)
        data.resize(max_bytes);
    // Reject obvious binary (NUL in first 512).
    const size_t check = std::min<size_t>(data.size(), 512);
    if(std::find(data.begin(), data.begin() + static_cast<std::ptrdiff_t>(check), '\0')
       != data.begin() + static_cast<std::ptrdiff_t>(check))
    {
        err = "Binary file rejected";
        return {};
    }
    return data;
}

nlohmann::json summarizeModelXml(const std::string& xml)
{
    nlohmann::json out;
    out["classes"] = nlohmann::json::array();
    out["links"] = nlohmann::json::array();
    std::unordered_set<std::string> classes;
    static const std::regex class_re(R"regex(Class="([^"]+)")regex");
    for(std::sregex_iterator it(xml.begin(), xml.end(), class_re), end; it != end; ++it)
        classes.insert((*it)[1].str());
    for(const std::string& c : classes)
        out["classes"].push_back(c);

    static const std::regex link_re(
        R"regex(<Item[^>]*Name="([^"]*)"[^>]*>([^<]+)</Item>\s*<Connector[^>]*Name="([^"]*)"[^>]*>([^<]+)</Connector>)regex");
    int link_count = 0;
    constexpr int kMaxLinks = 80;
    for(std::sregex_iterator it(xml.begin(), xml.end(), link_re), end; it != end; ++it)
    {
        if(link_count >= kMaxLinks)
            break;
        out["links"].push_back({{"from_port", (*it)[1].str()},
                                {"from", (*it)[2].str()},
                                {"to_port", (*it)[3].str()},
                                {"to", (*it)[4].str()}});
        ++link_count;
    }
    out["link_count"] = link_count;
    out["class_count"] = static_cast<int>(classes.size());
    return out;
}

bool looksLikeModelXmlName(const std::string& name)
{
    const std::string lower = [&]() {
        std::string s = name;
        for(char& c : s)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    }();
    return lower == "model.xml" || lower.rfind("model_", 0) == 0;
}

std::optional<std::pair<int, int>> tryImageDims(const fs::path& file)
{
    std::ifstream in(file, std::ios::binary);
    if(!in)
        return std::nullopt;
    unsigned char header[24] = {};
    in.read(reinterpret_cast<char*>(header), 24);
    const auto n = static_cast<size_t>(in.gcount());
    // PNG
    if(n >= 24 && header[0] == 0x89 && header[1] == 'P' && header[2] == 'N' && header[3] == 'G')
    {
        const int w = (header[16] << 24) | (header[17] << 16) | (header[18] << 8) | header[19];
        const int h = (header[20] << 24) | (header[21] << 16) | (header[22] << 8) | header[23];
        return {{w, h}};
    }
    // JPEG SOF0 rough scan
    if(n >= 2 && header[0] == 0xFF && header[1] == 0xD8)
    {
        in.clear();
        in.seekg(2);
        while(in)
        {
            unsigned char marker[2];
            if(!in.read(reinterpret_cast<char*>(marker), 2) || marker[0] != 0xFF)
                break;
            while(marker[1] == 0xFF && in.read(reinterpret_cast<char*>(&marker[1]), 1))
            {
            }
            if(marker[1] >= 0xC0 && marker[1] <= 0xC3)
            {
                unsigned char lenbuf[7];
                if(!in.read(reinterpret_cast<char*>(lenbuf), 7))
                    break;
                const int h = (lenbuf[3] << 8) | lenbuf[4];
                const int w = (lenbuf[5] << 8) | lenbuf[6];
                return {{w, h}};
            }
            unsigned char lenb[2];
            if(!in.read(reinterpret_cast<char*>(lenb), 2))
                break;
            const int len = (lenb[0] << 8) | lenb[1];
            if(len < 2)
                break;
            in.seekg(len - 2, std::ios::cur);
        }
    }
    return std::nullopt;
}

} // namespace

void RegisterProjectKnowledgeTools(ULLMToolRegistry& registry)
{
    registry.registerTool(
        makeReadDef(
            "inspect_configuration",
            "Summarize a configuration folder or model XML (classes + links) under path policy. "
            "Prefer this over dumping raw XML. For live loaded model use get_net_snapshot. "
            "Omit configuration_path (or leave empty) for the currently open configuration; "
            "never pass the words 'open project' or a bare '.' — those are rewritten only when "
            "a configuration is open; prefer omitting the argument.",
            {{"type", "object"},
             {"properties",
              {{"configuration_path",
                {{"type", "string"},
                 {"description",
                  "Config folder, project.ini, or model.xml. Empty/omitted = open configuration. "
                  "Do not pass 'open project' or '.' as the path."}}},
               {"max_links", {{"type", "integer"}, {"default", 80}}}}},
             {"additionalProperties", false}}),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            std::string err;
            const fs::path root =
                resolveUnderPolicy(args.value("configuration_path", std::string{}), err);
            if(root.empty())
            {
                r.ok = false;
                r.error_code = "PathPolicy";
                r.message = err;
                return r;
            }
            const fs::path model = findModelXml(root);
            if(model.empty())
            {
                r.ok = false;
                r.error_code = "NotFound";
                r.message = "No model.xml found under path";
                return r;
            }
            std::string read_err;
            const std::string xml = readTextCapped(model, 2 * 1024 * 1024, read_err);
            if(xml.empty())
            {
                r.ok = false;
                r.error_code = "ReadError";
                r.message = read_err;
                return r;
            }
            r.result = summarizeModelXml(xml);
            r.result["model_xml_path"] = model.string();
            r.result["configuration_path"] = root.string();
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeReadDef(
            "search_configuration_links",
            "Search known link patterns (offline link-patterns.json) and/or inspect a sample "
            "configuration for matching component/port tokens.",
            {{"type", "object"},
             {"required", {"query"}},
             {"properties",
              {{"query", {{"type", "string"}}},
               {"configuration_path", {{"type", "string"}}},
               {"top_k", {{"type", "integer"}, {"default", 10}}}}},
             {"additionalProperties", false}}),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            const std::string query = args.at("query").get<std::string>();
            const int top_k = args.value("top_k", 10);
            r.result["hits"] = nlohmann::json::array();

            const ULinkPatternCatalog& cat = defaultLinkPatternCatalog();
            r.result["link_patterns_loaded"] = !cat.empty();
            std::string err;
            const std::string path_arg = args.value("configuration_path", std::string{});
            if(!path_arg.empty() || !defaultProjectRoot().empty())
            {
                const fs::path root = resolveUnderPolicy(path_arg, err);
                if(!root.empty())
                {
                    const fs::path model = findModelXml(root);
                    if(!model.empty())
                    {
                        std::string read_err;
                        const std::string xml = readTextCapped(model, 2 * 1024 * 1024, read_err);
                        if(!read_err.empty())
                        {
                            r.ok = false;
                            r.error_code = "ReadError";
                            r.message = read_err;
                            return r;
                        }
                        const nlohmann::json summary = summarizeModelXml(xml);
                        const std::string qlower = [&]() {
                            std::string s = query;
                            for(char& c : s)
                                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                            return s;
                        }();
                        for(const auto& link : summary.value("links", nlohmann::json::array()))
                        {
                            const std::string blob =
                                link.dump();
                            std::string lower = blob;
                            for(char& c : lower)
                                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                            if(lower.find(qlower) == std::string::npos)
                                continue;
                            nlohmann::json hit = link;
                            hit["sample"] = model.string();
                            hit["source"] = "configuration_xml";
                            r.result["hits"].push_back(std::move(hit));
                            if(static_cast<int>(r.result["hits"].size()) >= top_k)
                                break;
                        }
                    }
                }
            }

            r.result["hint"] =
                "For cross-sample patterns use offline Bin/LLM/index/link-patterns.json via "
                "connect hints; pass configuration_path to search one sample/project.";
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeReadDef(
            "list_project_files",
            "List files under the open configuration folder (or allowlisted path). Read-only. "
            "Omit root_path for the open configuration; never pass '.' (cwd).",
            {{"type", "object"},
             {"properties",
              {{"root_path",
                {{"type", "string"},
                 {"description", "Empty/omitted = open configuration. Do not pass '.'."}}},
               {"extensions",
                {{"type", "array"},
                 {"items", {{"type", "string"}}},
                 {"description", "Optional filter e.g. [\".csv\",\".txt\"]"}}},
               {"max_depth", {{"type", "integer"}, {"default", 3}}},
               {"max_entries", {{"type", "integer"}, {"default", 200}}}}},
             {"additionalProperties", false}}),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            std::string err;
            const fs::path root = resolveUnderPolicy(args.value("root_path", std::string{}), err);
            if(root.empty() || !fs::is_directory(root))
            {
                r.ok = false;
                r.error_code = "PathPolicy";
                r.message = err.empty() ? "Not a directory" : err;
                return r;
            }
            std::unordered_set<std::string> exts;
            if(args.contains("extensions") && args["extensions"].is_array())
            {
                for(const auto& e : args["extensions"])
                {
                    if(!e.is_string())
                        continue;
                    std::string ext = e.get<std::string>();
                    if(!ext.empty() && ext[0] != '.')
                        ext = "." + ext;
                    for(char& c : ext)
                        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                    exts.insert(ext);
                }
            }
            const int max_depth = args.value("max_depth", 3);
            const int max_entries = args.value("max_entries", 200);
            r.result["root"] = root.string();
            r.result["files"] = nlohmann::json::array();
            std::error_code ec;
            for(auto it = fs::recursive_directory_iterator(root, ec);
                it != fs::recursive_directory_iterator(); it.increment(ec))
            {
                if(ec)
                    break;
                if(it.depth() > max_depth)
                {
                    it.disable_recursion_pending();
                    continue;
                }
                if(!it->is_regular_file())
                    continue;
                std::string ext = it->path().extension().string();
                for(char& c : ext)
                    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                if(!exts.empty() && !exts.count(ext))
                    continue;
                const fs::path rel = fs::relative(it->path(), root, ec);
                r.result["files"].push_back(
                    {{"path", rel.generic_string()},
                     {"size", static_cast<std::int64_t>(it->file_size(ec))},
                     {"extension", ext}});
                if(static_cast<int>(r.result["files"].size()) >= max_entries)
                    break;
            }
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeReadDef(
            "read_text_artifact",
            "Read a text artifact (txt/csv/json/xml/md/log) under path policy with size/line caps. "
            "For model.xml prefer inspect_configuration.",
            {{"type", "object"},
             {"required", {"path"}},
             {"properties",
              {{"path", {{"type", "string"}}},
               {"max_bytes", {{"type", "integer"}, {"default", 98304}}},
               {"head_lines", {{"type", "integer"}, {"default", 120}}},
               {"tail_lines", {{"type", "integer"}, {"default", 40}}}}},
             {"additionalProperties", false}}),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            std::string err;
            fs::path file = resolveUnderPolicy(args.at("path").get<std::string>(), err);
            if(file.empty())
            {
                // Relative to open project
                const std::string rel = args.at("path").get<std::string>();
                err.clear();
                const fs::path root = resolveUnderPolicy(defaultProjectRoot(), err);
                if(!root.empty())
                    file = resolveUnderPolicy((root / rel).string(), err);
            }
            if(file.empty() || !fs::is_regular_file(file))
            {
                r.ok = false;
                r.error_code = "PathPolicy";
                r.message = err.empty() ? "File not found" : err;
                return r;
            }
            if(looksLikeModelXmlName(file.filename().string()))
            {
                r.ok = false;
                r.error_code = "UseInspectConfiguration";
                r.message =
                    "Use inspect_configuration / get_net_snapshot for model XML instead of raw dump.";
                return r;
            }
            const size_t max_bytes = static_cast<size_t>(args.value("max_bytes", 98304));
            std::string read_err;
            std::string data = readTextCapped(file, max_bytes, read_err);
            if(data.empty() && !read_err.empty())
            {
                r.ok = false;
                r.error_code = "ReadError";
                r.message = read_err;
                return r;
            }
            const int head_n = args.value("head_lines", 120);
            const int tail_n = args.value("tail_lines", 40);
            std::vector<std::string> lines;
            {
                std::stringstream ss(data);
                std::string line;
                while(std::getline(ss, line))
                    lines.push_back(line);
            }
            nlohmann::json head = nlohmann::json::array();
            nlohmann::json tail = nlohmann::json::array();
            for(int i = 0; i < head_n && i < static_cast<int>(lines.size()); ++i)
                head.push_back(lines[static_cast<size_t>(i)]);
            const int start_tail =
                std::max(0, static_cast<int>(lines.size()) - tail_n);
            if(start_tail > head_n)
            {
                for(int i = start_tail; i < static_cast<int>(lines.size()); ++i)
                    tail.push_back(lines[static_cast<size_t>(i)]);
            }
            r.result["path"] = file.string();
            r.result["line_count"] = static_cast<int>(lines.size());
            r.result["bytes_read"] = static_cast<int>(data.size());
            r.result["truncated"] = fs::file_size(file) > max_bytes;
            r.result["head"] = std::move(head);
            r.result["tail"] = std::move(tail);
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeReadDef(
            "stat_project_file",
            "File metadata under path policy (size, mtime, extension; image dims when cheap). "
            "No vision/pixels. Use show_ui_panel(panel=images) to open Images UI.",
            {{"type", "object"},
             {"required", {"path"}},
             {"properties", {{"path", {{"type", "string"}}}}},
             {"additionalProperties", false}}),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            std::string err;
            fs::path file = resolveUnderPolicy(args.at("path").get<std::string>(), err);
            if(file.empty())
            {
                err.clear();
                const fs::path root = resolveUnderPolicy(defaultProjectRoot(), err);
                if(!root.empty())
                    file = resolveUnderPolicy((root / args.at("path").get<std::string>()).string(),
                                              err);
            }
            if(file.empty() || !fs::exists(file))
            {
                r.ok = false;
                r.error_code = "PathPolicy";
                r.message = err.empty() ? "Not found" : err;
                return r;
            }
            std::error_code ec;
            r.result["path"] = file.string();
            r.result["is_directory"] = fs::is_directory(file, ec);
            r.result["extension"] = file.extension().string();
            if(fs::is_regular_file(file, ec))
            {
                r.result["size"] = static_cast<std::int64_t>(fs::file_size(file, ec));
                const auto ft = fs::last_write_time(file, ec);
                if(!ec)
                {
                    const auto sec =
                        std::chrono::duration_cast<std::chrono::seconds>(ft.time_since_epoch())
                            .count();
                    r.result["mtime_unix"] = sec;
                }
                if(const auto dims = tryImageDims(file))
                {
                    r.result["image_width"] = dims->first;
                    r.result["image_height"] = dims->second;
                }
            }
            r.result["hint_images_ui"] = "show_ui_panel with panel=images";
            r.ok = true;
            return r;
        });
}

} // namespace RDK::LLM
