#include "ULLMPathPolicy.h"

#include <cstdlib>
#include <filesystem>

#include <rdk_application.h>

namespace RDK::LLM {

namespace {

bool hasParentTraversal(const std::filesystem::path& p)
{
    for(const auto& part : p)
    {
        if(part == "..")
            return true;
    }
    return false;
}

std::string ensureTrailingSlash(std::string s)
{
    if(!s.empty() && s.back() != '/' && s.back() != '\\')
        s.push_back('/');
    return s;
}

bool isUnderRoot(const std::filesystem::path& path, const std::filesystem::path& root)
{
    std::error_code ec;
    const auto norm_path = std::filesystem::weakly_canonical(path, ec);
    const auto norm_root = std::filesystem::weakly_canonical(root, ec);
    if(ec)
        return false;
    const auto rel = std::filesystem::relative(norm_path, norm_root, ec);
    if(ec || rel.empty())
        return norm_path == norm_root;
    const std::string rel_s = rel.generic_string();
    return rel_s.empty() || rel_s == "." || rel_s.rfind("..", 0) != 0;
}

} // namespace

bool ULLMPathPolicy::isStrictEnabled()
{
    const char* env = std::getenv("NMSDK_LLM_PATH_POLICY_STRICT");
    if(env && env[0] == '0')
        return false;
    return true;
}

std::string ULLMPathPolicy::normalizePath(const std::string& path, std::string& err)
{
    err.clear();
    if(path.empty())
    {
        err = "path is empty";
        return {};
    }
    std::error_code ec;
    std::filesystem::path p(path);
    if(hasParentTraversal(p))
    {
        err = "path must not contain .. segments";
        return {};
    }
    const std::filesystem::path norm = std::filesystem::weakly_canonical(p, ec);
    if(ec)
    {
        err = "failed to normalize path";
        return {};
    }
    return norm.generic_string();
}

std::vector<std::string> ULLMPathPolicy::allowedRoots(const RDK::UApplication* app)
{
    std::vector<std::string> roots;
    if(!app)
        return roots;

    auto add_root = [&](const std::string& candidate) {
        std::error_code ec;
        const std::filesystem::path canon =
            std::filesystem::weakly_canonical(std::filesystem::path(candidate), ec);
        if(!ec && !canon.empty())
            roots.push_back(canon.generic_string());
    };

    const std::string wd = app->GetWorkDirectory();
    add_root(wd + "/../../Configs");
    add_root(wd + "/../../../Configs");
    add_root(wd);

    const std::string user_rel = app->GetUserConfigPath();
    if(!app->GetUserName().empty() && !user_rel.empty())
    {
        std::string configs = wd + "/../../Configs";
        std::error_code ec;
        if(!std::filesystem::exists(configs, ec))
            configs = wd + "/../../../Configs";
        add_root(configs + "/" + user_rel);
    }

    if(app->GetProjectOpenFlag())
        add_root(app->GetProjectPath());

    return roots;
}

bool ULLMPathPolicy::isAllowed(const std::string& path, const RDK::UApplication* app, std::string& err)
{
    err.clear();
    if(!isStrictEnabled())
        return true;

    std::string norm_err;
    const std::string norm = normalizePath(path, norm_err);
    if(norm.empty())
    {
        err = norm_err.empty() ? "invalid path" : norm_err;
        return false;
    }

    const auto roots = allowedRoots(app);
    if(roots.empty())
    {
        err = "no allowed path roots configured";
        return false;
    }

    const std::filesystem::path p(norm);
    for(const std::string& root_s : roots)
    {
        if(isUnderRoot(p, std::filesystem::path(root_s)))
            return true;
    }

    err = "path outside allowed configuration roots";
    return false;
}

std::string ULLMPathPolicy::rewriteRelativeConfigPath(const std::string& path,
                                                      const std::string& open_project_root)
{
    if(path.empty() || open_project_root.empty())
        return path;
    const std::filesystem::path p(path);
    if(p.is_absolute() || hasParentTraversal(p))
        return path;
    const auto parent = p.parent_path();
    if(!parent.empty() && parent != "." && parent.generic_string() != "./")
        return path;
    const std::string fname = p.filename().generic_string();
    static const char* kKnown[] = {"project.ini", "model.xml", "Model.xml", "Model_00.xml",
                                   "model_00.xml", nullptr};
    bool known = false;
    for(const char** k = kKnown; *k; ++k)
    {
        if(fname == *k)
        {
            known = true;
            break;
        }
    }
    if(!known)
        return path;
    return (std::filesystem::path(open_project_root) / fname).generic_string();
}

std::string ULLMPathPolicy::rewriteRelativeConfigPath(const std::string& path,
                                                      const RDK::UApplication* app)
{
    if(!app || !app->GetProjectOpenFlag())
        return path;
    const std::string root = app->GetProjectPath();
    if(root.empty())
        return path;
    return rewriteRelativeConfigPath(path, root);
}

} // namespace RDK::LLM
