#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "NmsdkBuiltinKnowledgeCatalog.h"
#include "../../Core/Context/UDocSearchIndex.h"
#include "../../Core/Context/ULinkPatternCatalog.h"
#include "../../Core/Context/ULLMConnectSemanticsCatalog.h"

namespace fs = std::filesystem;

static fs::path resolveRepositoryRoot(int argc, char** argv)
{
    if(argc > 1)
        return fs::absolute(argv[1]);
    if(const char* env = std::getenv("NMSDK_ROOT"))
        return fs::path(env);
    return fs::current_path();
}

static int countModelConfigFiles(const fs::path& configs_root)
{
    if(!fs::exists(configs_root))
        return 0;
    int count = 0;
    for(const auto& entry : fs::recursive_directory_iterator(configs_root))
    {
        if(!entry.is_regular_file())
            continue;
        const std::string name = entry.path().filename().string();
        if(name.rfind("Model_", 0) == 0 && entry.path().extension() == ".xml")
            ++count;
    }
    return count;
}

int main(int argc, char** argv)
{
    const fs::path repo = resolveRepositoryRoot(argc, argv);
    if(!fs::exists(repo / "CMakeLists.txt"))
    {
        std::cerr << "Repository root not found: " << repo << '\n';
        return 1;
    }

    const std::string repo_str = repo.string();
#ifdef _WIN32
    _putenv_s("NMSDK_ROOT", repo_str.c_str());
#else
    ::setenv("NMSDK_ROOT", repo_str.c_str(), 1);
#endif
    NmsdkBuiltinKnowledgeCatalog catalog(nullptr);
    RDK::LLM::UDocSearchIndex index;
    index.buildFromCatalog(catalog, repo);

    const fs::path out_dir = catalog.prebuiltIndexDirectory();
    index.savePrebuilt(out_dir, catalog.catalogFingerprint());
    std::cout << "Wrote prebuilt index to " << out_dir << '\n';

    const fs::path configs_root = repo / "Bin/Configs";
    const auto link_patterns = RDK::LLM::buildLinkPatternsFromConfigs(configs_root);
    const fs::path link_json = out_dir / "link-patterns.json";
    const fs::path link_manifest = out_dir / "link-patterns-manifest.json";
    if(RDK::LLM::writeLinkPatterns(link_json, link_manifest, link_patterns,
                                   catalog.catalogFingerprint(),
                                   countModelConfigFiles(configs_root)))
    {
        std::cout << "Wrote link patterns to " << link_json << '\n';
    }
    else
    {
        std::cerr << "Warning: failed to write link patterns\n";
    }

    const auto semantics = RDK::LLM::buildConnectSemanticsFromConfigs(configs_root);
    const fs::path sem_json = out_dir / "connect-semantics.json";
    const fs::path sem_manifest = out_dir / "connect-semantics-manifest.json";
    if(RDK::LLM::writeConnectSemantics(sem_json, sem_manifest, semantics,
                                        catalog.catalogFingerprint(),
                                        countModelConfigFiles(configs_root)))
    {
        std::cout << "Wrote connect semantics to " << sem_json << '\n';
    }
    else
    {
        std::cerr << "Warning: failed to write connect semantics\n";
    }

    if(catalog.writeLlmsTxt(repo))
        std::cout << "Wrote Docs/llms.txt\n";
    else
        std::cerr << "Warning: failed to write Docs/llms.txt\n";

    return index.empty() ? 2 : 0;
}
