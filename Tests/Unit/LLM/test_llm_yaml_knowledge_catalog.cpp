#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>

#include "Knowledge/ILLMYamlKnowledgeCatalog.h"
#include "NmsdkBuiltinKnowledgeCatalog.h"

namespace fs = std::filesystem;

namespace {

fs::path tempRepoDir()
{
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    return fs::temp_directory_path() / ("nmsdk_llm_yaml_catalog_" + std::to_string(stamp));
}

void writeFile(const fs::path& p, const std::string& content)
{
    fs::create_directories(p.parent_path());
    std::ofstream out(p);
    out << content;
}

} // namespace

TEST(LLMYamlKnowledgeCatalog, ParsesManifestSources)
{
    const fs::path repo = tempRepoDir();
    const fs::path manifest = repo / "Docs/llm-knowledge.yaml";
    writeFile(repo / "CMakeLists.txt", "cmake_minimum_required(VERSION 3.16)\n");
    writeFile(repo / "Docs/Extra/a.md", "# Doc\n");
    writeFile(manifest,
              "prebuilt_index_directory: Bin/LLM/yaml-index\n"
              "sources:\n"
              "  - source_id: yaml.docs\n"
              "    root: Docs/Extra\n"
              "    kind: ProductDocs\n"
              "    extensions: [.md, .txt]\n"
              "    exclude_globs:\n"
              "      - Draft/**\n");

    std::string err;
    const auto loaded = RDK::LLM::UYamlKnowledgeCatalog::load(manifest, repo, &err);
    ASSERT_TRUE(loaded.has_value()) << err;
    ASSERT_EQ(loaded->sources().size(), 1u);
    EXPECT_EQ(loaded->sources()[0].source_id, "yaml.docs");
    EXPECT_EQ(loaded->sources()[0].extensions.size(), 2u);
    EXPECT_EQ(loaded->prebuiltIndexDirectory(), repo / "Bin/LLM/yaml-index");

    fs::remove_all(repo);
}

TEST(LLMYamlKnowledgeCatalog, BuiltinCatalogFederatesYamlSources)
{
    const fs::path repo = tempRepoDir();
    const fs::path manifest = repo / "Docs/llm-knowledge.yaml";
    writeFile(repo / "CMakeLists.txt", "cmake_minimum_required(VERSION 3.16)\n");
    writeFile(repo / "Docs/Extra/readme.md", "# Extra\n");
    writeFile(manifest,
              "prebuilt_index_directory: Bin/LLM/yaml-index\n"
              "sources:\n"
              "  - source_id: yaml.docs\n"
              "    root: Docs/Extra\n"
              "    kind: ProductDocs\n"
              "    extensions:\n"
              "      - .md\n");

    ::setenv("NMSDK_ROOT", repo.string().c_str(), 1);
    ::setenv("NMSDK_LLM_KNOWLEDGE_CATALOG_YAML", manifest.string().c_str(), 1);

    NmsdkBuiltinKnowledgeCatalog catalog(nullptr);
    const auto sources = catalog.sources();
    const auto it = std::find_if(sources.begin(), sources.end(), [](const RDK::LLM::LLMKnowledgeSource& s) {
        return s.source_id == "yaml.docs";
    });
    EXPECT_TRUE(it != sources.end());
    EXPECT_EQ(catalog.prebuiltIndexDirectory(), repo / "Bin/LLM/yaml-index");

    ::unsetenv("NMSDK_LLM_KNOWLEDGE_CATALOG_YAML");
    fs::remove_all(repo);
}
