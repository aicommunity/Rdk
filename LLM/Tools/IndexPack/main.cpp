#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "NmsdkBuiltinKnowledgeCatalog.h"
#include "../../Core/Context/UDocSearchIndex.h"

namespace fs = std::filesystem;

static fs::path resolveRepositoryRoot(int argc, char** argv)
{
    if(argc > 1)
        return fs::absolute(argv[1]);
    if(const char* env = std::getenv("NMSDK_ROOT"))
        return fs::path(env);
    return fs::current_path();
}

int main(int argc, char** argv)
{
    const fs::path repo = resolveRepositoryRoot(argc, argv);
    if(!fs::exists(repo / "CMakeLists.txt"))
    {
        std::cerr << "Repository root not found: " << repo << '\n';
        return 1;
    }

    ::setenv("NMSDK_ROOT", repo.string().c_str(), 1);
    NmsdkBuiltinKnowledgeCatalog catalog(nullptr);
    RDK::LLM::UDocSearchIndex index;
    index.buildFromCatalog(catalog, repo);

    const fs::path out_dir = catalog.prebuiltIndexDirectory();
    index.savePrebuilt(out_dir, catalog.catalogFingerprint());
    std::cout << "Wrote prebuilt index to " << out_dir << '\n';
    return index.empty() ? 2 : 0;
}
