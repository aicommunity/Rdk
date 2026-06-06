#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <string>

#include "Context/UDocSearchIndex.h"
#include "NmsdkBuiltinKnowledgeCatalog.h"

#ifndef CMAKE_SOURCE_DIR
#define CMAKE_SOURCE_DIR "."
#endif

namespace {

void ensureNmsdkRoot()
{
    ::setenv("NMSDK_ROOT", CMAKE_SOURCE_DIR, 1);
}

} // namespace

TEST(LLMKnowledgeIndex, SearchUApplicationInAllScope)
{
    ensureNmsdkRoot();
    NmsdkBuiltinKnowledgeCatalog catalog(nullptr);
    RDK::LLM::UDocSearchIndex index;
    index.buildFromCatalog(catalog, std::filesystem::path(CMAKE_SOURCE_DIR));

    const auto hits = index.searchWithScope("UApplication", 24, "all");
    ASSERT_FALSE(hits.empty());
    bool found = false;
    for(const RDK::LLM::DocSnippet& sn : hits)
    {
        if(sn.path.find("UApplication") != std::string::npos)
            found = true;
    }
    EXPECT_TRUE(found);
}

TEST(LLMKnowledgeIndex, DocsScopeDoesNotReturnCpp)
{
    ensureNmsdkRoot();
    NmsdkBuiltinKnowledgeCatalog catalog(nullptr);
    RDK::LLM::UDocSearchIndex index;
    index.buildFromCatalog(catalog, std::filesystem::path(CMAKE_SOURCE_DIR));

    const auto hits = index.searchWithScope("HardwareLib", 10, "docs");
    for(const RDK::LLM::DocSnippet& sn : hits)
    {
        EXPECT_EQ(sn.content_kind, RDK::LLM::LLMContentKind::Doc);
        EXPECT_EQ(sn.path.rfind(".cpp"), std::string::npos);
    }
}
