#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>

#include "Context/UDocSearchIndex.h"
#include "NmsdkBuiltinKnowledgeCatalog.h"

#ifndef CMAKE_SOURCE_DIR
#define CMAKE_SOURCE_DIR "."
#endif

namespace fs = std::filesystem;

namespace {

void ensureNmsdkRoot()
{
    ::setenv("NMSDK_ROOT", CMAKE_SOURCE_DIR, 1);
}

fs::path tempIndexDir()
{
    return fs::temp_directory_path() / ("nmsdk_llm_incr_" + std::to_string(
                                            std::chrono::steady_clock::now().time_since_epoch().count()));
}

} // namespace

TEST(LLMIndexIncremental, SyncUpdatesChangedFileOnly)
{
    ensureNmsdkRoot();
    const fs::path work = tempIndexDir();
    fs::create_directories(work / "Docs");
    const fs::path doc = work / "Docs" / "incr_test.md";
    {
        std::ofstream out(doc);
        out << "# Alpha\nversion one\n";
    }

    struct TestCatalog : RDK::LLM::ILLMKnowledgeCatalog {
        fs::path root;
        std::vector<RDK::LLM::LLMKnowledgeSource> sources() const override
        {
            RDK::LLM::LLMKnowledgeSource s;
            s.source_id = "test.docs";
            s.root = root / "Docs";
            s.kind = RDK::LLM::LLMKnowledgeKind::ProductDocs;
            s.extensions = {".md"};
            return {s};
        }
        fs::path prebuiltIndexDirectory() const override { return {}; }
        std::string catalogFingerprint() const override { return "test-incr"; }
    } catalog;
    catalog.root = work;

    RDK::LLM::UDocSearchIndex index;
    index.buildFromCatalog(catalog, work, 100);
    ASSERT_FALSE(index.empty());
    EXPECT_EQ(index.syncFromCatalog(catalog, work, 100), RDK::LLM::IndexSyncResult::UpToDate);

    const auto before = index.searchWithScope("version one", 3, "docs");
    ASSERT_FALSE(before.empty());

    {
        std::ofstream out(doc, std::ios::trunc);
        out << "# Beta\nversion two\n";
    }
    {
        std::error_code ec;
        const auto newer = fs::last_write_time(doc, ec) + std::chrono::hours(1);
        fs::last_write_time(doc, newer, ec);
    }

    const RDK::LLM::IndexSyncResult sync = index.syncFromCatalog(catalog, work, 100);
    EXPECT_EQ(sync, RDK::LLM::IndexSyncResult::IncrementalUpdated);

    const auto after = index.searchWithScope("version two", 3, "docs");
    ASSERT_FALSE(after.empty());
    EXPECT_NE(after.front().excerpt.find("version two"), std::string::npos);

    fs::remove_all(work);
}

TEST(LLMIndexIncremental, PrebuiltLoadAndSync)
{
    ensureNmsdkRoot();
    const fs::path work = tempIndexDir();
    fs::create_directories(work / "Docs");
    const fs::path doc = work / "Docs" / "prebuilt.md";
    {
        std::ofstream out(doc);
        out << "# Prebuilt\nstatic content\n";
    }

    struct TestCatalog : RDK::LLM::ILLMKnowledgeCatalog {
        fs::path root;
        std::vector<RDK::LLM::LLMKnowledgeSource> sources() const override
        {
            RDK::LLM::LLMKnowledgeSource s;
            s.source_id = "test.docs";
            s.root = root / "Docs";
            s.kind = RDK::LLM::LLMKnowledgeKind::ProductDocs;
            s.extensions = {".md"};
            return {s};
        }
        fs::path prebuiltIndexDirectory() const override { return root / "index"; }
        std::string catalogFingerprint() const override { return "fp-prebuilt"; }
    } catalog;
    catalog.root = work;

    {
        RDK::LLM::UDocSearchIndex built;
        built.buildFromCatalog(catalog, work, 100);
        built.savePrebuilt(work / "index", "fp-prebuilt");
    }

    RDK::LLM::UDocSearchIndex loaded;
    ASSERT_TRUE(loaded.loadPrebuilt(work / "index", "fp-prebuilt"));
    EXPECT_FALSE(loaded.fileMtimes().empty());
    EXPECT_EQ(loaded.syncFromCatalog(catalog, work, 100), RDK::LLM::IndexSyncResult::UpToDate);

    fs::remove_all(work);
}
