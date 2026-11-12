#include <gtest/gtest.h>

#include <cstdlib>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

#include "Context/UDocSearchIndex.h"
#include "NmsdkBuiltinKnowledgeCatalog.h"

#ifndef CMAKE_SOURCE_DIR
#define CMAKE_SOURCE_DIR "."
#endif

#ifndef RDK_LLM_RETRIEVAL_FIXTURES
#define RDK_LLM_RETRIEVAL_FIXTURES ""
#endif

namespace {

void ensureNmsdkRoot()
{
    ::setenv("NMSDK_ROOT", CMAKE_SOURCE_DIR, 1);
}

std::vector<nlohmann::json> loadExpectations()
{
    std::ifstream in(RDK_LLM_RETRIEVAL_FIXTURES);
    if(!in)
        return {};
    nlohmann::json arr = nlohmann::json::parse(in, nullptr, false);
    if(!arr.is_array())
        return {};
    std::vector<nlohmann::json> out;
    for(const auto& row : arr)
        out.push_back(row);
    return out;
}

} // namespace

TEST(LLMDocRetrieval, FixtureQueriesHitExpectedPaths)
{
    ensureNmsdkRoot();
    const auto expectations = loadExpectations();
    ASSERT_GE(expectations.size(), 15u) << "Fixture file missing or too small";

    NmsdkBuiltinKnowledgeCatalog catalog(nullptr);
    RDK::LLM::UDocSearchIndex index;
    index.buildFromCatalog(catalog, std::filesystem::path(CMAKE_SOURCE_DIR));
    ASSERT_FALSE(index.empty());

    int matched = 0;
    for(const nlohmann::json& row : expectations)
    {
        const std::string query = row.at("query").get<std::string>();
        const std::string expected = row.at("expected_subpath").get<std::string>();
        const std::string scope = row.value("scope", "docs");

        const auto hits = index.searchWithScope(query, 8, scope);
        bool found = false;
        for(const RDK::LLM::DocSnippet& sn : hits)
        {
            if(sn.path.find(expected) != std::string::npos)
                found = true;
        }
        if(found)
            ++matched;
    }
    EXPECT_GE(matched, static_cast<int>(expectations.size()) * 3 / 4)
        << "At least 75% of retrieval fixtures should match (got " << matched << "/"
        << expectations.size() << ")";
}
