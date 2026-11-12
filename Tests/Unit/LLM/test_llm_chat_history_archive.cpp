#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <thread>

#include "../../../GUI/Qt/Llm/ULlmChatHistoryArchive.h"
#include "Observability/ULLMToolTrace.h"

namespace fs = std::filesystem;

namespace {

fs::path uniqueTempDir()
{
    const auto base = fs::temp_directory_path() / "rdk_llm_chat_archive_test";
    fs::create_directories(base);
    const auto dir = base / std::to_string(
                                std::chrono::steady_clock::now().time_since_epoch().count());
    fs::create_directories(dir);
    return dir;
}

ChatArchiveMeta sampleMeta()
{
    ChatArchiveMeta meta;
    meta.session_id = "gui-test-session-001";
    meta.provider_id = "ollama-local";
    meta.project_path = "/tmp/project.xml";
    meta.created_at_iso = "2026-05-28T14:30:05";
    return meta;
}

} // namespace

TEST(LLMChatHistoryArchive, StartNewChat_CreatesDirectories)
{
    const fs::path bin_root = uniqueTempDir();
    ASSERT_FALSE(fs::exists(bin_root / "AiChats"));

    ULlmChatHistoryArchive archive(bin_root);
    const auto file = archive.startNewChatFile(sampleMeta());
    ASSERT_TRUE(file.has_value());
    EXPECT_TRUE(fs::exists(bin_root / "AiChats"));
    EXPECT_TRUE(fs::exists(*file));
    EXPECT_EQ(file->extension(), ".html");
    EXPECT_NE(file->filename().string().find("NeuroModelerChat"), std::string::npos);
}

TEST(LLMChatHistoryArchive, Append_ToolTraceFragment)
{
    const fs::path bin_root = uniqueTempDir();
    ULlmChatHistoryArchive archive(bin_root);
    const auto file = archive.startNewChatFile(sampleMeta());
    ASSERT_TRUE(file.has_value());

    RDK::LLM::TurnToolInvocationView view;
    view.tool_name = "add_component";
    view.arguments = {{"class_name", "NSPNeuron"}};
    view.ok = true;
    const std::string trace_html = RDK::LLM::formatTurnToolTraceHtml({view});
    EXPECT_TRUE(archive.appendHtmlFragment(*file, trace_html));

    const std::string body = archive.loadChatBodyHtml(*file);
    EXPECT_NE(body.find("add_component"), std::string::npos);
}

TEST(LLMChatHistoryArchive, Append_TwoTurns)
{
    const fs::path bin_root = uniqueTempDir();
    ULlmChatHistoryArchive archive(bin_root);
    const auto file = archive.startNewChatFile(sampleMeta());
    ASSERT_TRUE(file.has_value());

    EXPECT_TRUE(archive.appendHtmlFragment(*file, "<b>You:</b> hello"));
    EXPECT_TRUE(archive.appendHtmlFragment(*file, "<b>Assistant:</b> hi there"));

    const std::string body = archive.loadChatBodyHtml(*file);
    EXPECT_NE(body.find("<b>You:</b>"), std::string::npos);
    EXPECT_NE(body.find("<b>Assistant:</b>"), std::string::npos);
}

TEST(LLMChatHistoryArchive, Finalize_SetsArchivedMeta)
{
    const fs::path bin_root = uniqueTempDir();
    ULlmChatHistoryArchive archive(bin_root);
    const auto file = archive.startNewChatFile(sampleMeta());
    ASSERT_TRUE(file.has_value());

    EXPECT_TRUE(archive.finalizeChat(*file));
    const ChatArchiveMeta meta = archive.parseMeta(*file);
    EXPECT_TRUE(meta.archived);
}

TEST(LLMChatHistoryArchive, List_SortedByMtime)
{
    const fs::path bin_root = uniqueTempDir();
    ULlmChatHistoryArchive archive(bin_root);

    auto meta1 = sampleMeta();
    meta1.session_id = "gui-a";
    const auto file1 = archive.startNewChatFile(meta1);
    ASSERT_TRUE(file1.has_value());
    archive.appendHtmlFragment(*file1, "<b>You:</b> first");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    auto meta2 = sampleMeta();
    meta2.session_id = "gui-b";
    const auto file2 = archive.startNewChatFile(meta2);
    ASSERT_TRUE(file2.has_value());
    archive.appendHtmlFragment(*file2, "<b>You:</b> second");

    const auto list = archive.listChats(10);
    ASSERT_GE(list.size(), 2u);
    EXPECT_GE(list[0].mtime_unix, list[1].mtime_unix);
}

TEST(LLMChatHistoryArchive, ParseSessionId)
{
    const fs::path bin_root = uniqueTempDir();
    ULlmChatHistoryArchive archive(bin_root);
    const auto file = archive.startNewChatFile(sampleMeta());
    ASSERT_TRUE(file.has_value());
    EXPECT_EQ(archive.parseSessionId(*file), sampleMeta().session_id);
}

TEST(LLMChatHistoryArchive, LoadBodyHtml)
{
    const fs::path bin_root = uniqueTempDir();
    ULlmChatHistoryArchive archive(bin_root);
    const auto file = archive.startNewChatFile(sampleMeta());
    ASSERT_TRUE(file.has_value());
    const std::string fragment = "<b>You:</b> load test";
    ASSERT_TRUE(archive.appendHtmlFragment(*file, fragment));

    const std::string body = archive.loadChatBodyHtml(*file);
    EXPECT_NE(body.find("load test"), std::string::npos);
}

TEST(LLMChatHistoryArchive, List_EmptyWhenNoRoot)
{
    const fs::path bin_root = uniqueTempDir();
    ULlmChatHistoryArchive archive(bin_root);
    EXPECT_TRUE(archive.listChats().empty());
}
