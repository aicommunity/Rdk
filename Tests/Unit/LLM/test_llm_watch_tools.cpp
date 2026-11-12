#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Gui/ILLMPresentationSink.h"
#include "Tools/ULLMToolRegistry.h"
#include "Tools/RegisterApplicationTools.h"

using namespace RDK::LLM;

namespace {

struct CountingWatchSink : ILLMPresentationSink {
    int add_count = 0;
    int create_mdi_count = 0;
    int list_mdi_count = 0;
    int close_mdi_count = 0;
    int next_mdi_id = 1;

    void apply(const LLMPresentationEvent& ev) override { (void)ev; }

    nlohmann::json watchAddSeries(const LLMWatchSeriesArgs& args) override
    {
        ++add_count;
        if(args.surface == "mdi" && args.mdi_id < 0)
            return {{"ok", false}, {"error", "mdi_id required"}};
        return {{"ok", true},
                {"surface", args.surface},
                {"mdi_id", args.mdi_id},
                {"long_name", args.long_name},
                {"property_name", args.property_name},
                {"serie_index", add_count - 1}};
    }

    nlohmann::json watchMdiList() override
    {
        ++list_mdi_count;
        return {{"ok", true},
                {"items", nlohmann::json::array({{{"mdi_id", 1}, {"title", "Watches_1"}}})}};
    }

    nlohmann::json watchMdiCreate(int grid_rows, int grid_cols, const std::string& title) override
    {
        ++create_mdi_count;
        (void)grid_rows;
        (void)grid_cols;
        const int id = next_mdi_id++;
        return {{"ok", true},
                {"mdi_id", id},
                {"title", title.empty() ? "Watches_" + std::to_string(id) : title}};
    }

    nlohmann::json watchMdiClose(int mdi_id) override
    {
        ++close_mdi_count;
        return {{"ok", true}, {"mdi_id", mdi_id}};
    }
};

} // namespace

TEST(LLMWatchTools, SchemaRegistered)
{
    ULLMToolRegistry registry;
    RegisterApplicationTools(registry);
    EXPECT_NE(registry.find("add_watch_series"), nullptr);
    EXPECT_NE(registry.find("list_watch_series"), nullptr);
    EXPECT_NE(registry.find("remove_watch_series"), nullptr);
    EXPECT_NE(registry.find("clear_watch_series"), nullptr);
    EXPECT_NE(registry.find("list_watch_mdi"), nullptr);
    EXPECT_NE(registry.find("create_watch_mdi"), nullptr);
    EXPECT_NE(registry.find("focus_watch_mdi"), nullptr);
    EXPECT_NE(registry.find("close_watch_mdi"), nullptr);

    const LLMToolDefinition* add = registry.find("add_watch_series");
    ASSERT_NE(add, nullptr);
    EXPECT_EQ(add->kind, LLMToolKind::Write);
    EXPECT_TRUE(add->input_schema.contains("properties"));
    EXPECT_TRUE(add->input_schema["properties"].contains("long_name"));
    EXPECT_TRUE(add->input_schema["properties"].contains("surface"));
}

TEST(LLMPresentationSink, WatchMockCreateListAddClose)
{
    CountingWatchSink sink;
    const nlohmann::json created = sink.watchMdiCreate(1, 1, "");
    ASSERT_TRUE(created.value("ok", false));
    const int mdi_id = created.value("mdi_id", -1);
    EXPECT_EQ(mdi_id, 1);

    const nlohmann::json listed = sink.watchMdiList();
    EXPECT_TRUE(listed.value("ok", false));
    EXPECT_EQ(listed["items"].size(), 1u);

    LLMWatchSeriesArgs args;
    args.surface = "mdi";
    args.mdi_id = mdi_id;
    args.long_name = "Root.PNeuron";
    args.property_name = "Output";
    const nlohmann::json added = sink.watchAddSeries(args);
    EXPECT_TRUE(added.value("ok", false));
    EXPECT_EQ(sink.add_count, 1);

    EXPECT_TRUE(sink.watchMdiClose(mdi_id).value("ok", false));
    EXPECT_EQ(sink.create_mdi_count, 1);
    EXPECT_EQ(sink.close_mdi_count, 1);
}

TEST(LLMDomainAccess, ValidateWatchPropertyRequiresEngine)
{
    URdkDomainAccess domain(nullptr);
    const DomainStatus st = domain.validateWatchProperty("MModel.PNeuron", "Output", 0, 0, 0);
    EXPECT_FALSE(st.ok());
    EXPECT_TRUE(st.code == DomainStatusCode::NotInitialized
                || st.code == DomainStatusCode::ProjectNotLoaded);

    const DomainStatus empty =
        domain.validateWatchProperty("", "Output", 0, 0, 0);
    EXPECT_EQ(empty.code, DomainStatusCode::ComponentNotFound);

    const DomainStatus no_prop =
        domain.validateWatchProperty("MModel.PNeuron", "", 0, 0, 0);
    EXPECT_EQ(no_prop.code, DomainStatusCode::PropertyNotFound);

    const DomainStatus bad_cell =
        domain.validateWatchProperty("MModel.PNeuron", "Output", 0, -1, 0);
    EXPECT_EQ(bad_cell.code, DomainStatusCode::InvalidPropertyValue);
}
