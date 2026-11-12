#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>

#include "../../../Core/Engine/UReadOnlyLogTail.h"
#include "../../../Deploy/Include/rdk_init.h"

namespace fs = std::filesystem;

namespace {

fs::path uniqueTempDir()
{
    const auto base = fs::temp_directory_path() / "rdk_read_only_log_tail";
    fs::create_directories(base);
    return base / std::to_string(
                      std::chrono::steady_clock::now().time_since_epoch().count());
}

void writeGlogLine(const fs::path& dir, const std::string& base, const std::string& severity,
                   const std::string& body)
{
    const std::time_t now = std::time(nullptr);
    std::tm local_tm{};
#if defined(_WIN32)
    localtime_s(&local_tm, &now);
#else
    localtime_r(&now, &local_tm);
#endif
    char stamp[32];
    std::strftime(stamp, sizeof(stamp), "%Y%m%d-%H%M%S", &local_tm);

    const std::string filename =
        base + ".log." + severity + "." + stamp + ".12345";
    std::ofstream out(dir / filename, std::ios::app);
    out << "I" << std::setfill('0') << std::setw(2) << (local_tm.tm_mon + 1) << std::setw(2)
        << local_tm.tm_mday << ' ' << std::setw(2) << local_tm.tm_hour << ':' << std::setw(2)
        << local_tm.tm_min << ':' << std::setw(2) << local_tm.tm_sec
        << ".000000  1000 file.cc:10] " << body << '\n';
}

} // namespace

TEST(ReadOnlyLogTail, CollectDelta_AfterMark)
{
    const fs::path dir = uniqueTempDir();
    fs::create_directories(dir);

    RDK::ApplicationLogReadPaths paths;
    paths.base_name = "TestApp";
    paths.session_start_unix = std::time(nullptr) - 60;
    paths.directories.push_back(dir.string());

    RDK::UReadOnlyLogTail tail;
    tail.syncFromPaths(paths);
    writeGlogLine(dir, "TestApp", "INFO", "line one");
    writeGlogLine(dir, "TestApp", "INFO", "line two");
    tail.mark();
    writeGlogLine(dir, "TestApp", "INFO", "line three");

    const auto delta = tail.collectDelta(10, RDK_EX_INFO);
    ASSERT_EQ(delta.size(), 1u);
    EXPECT_NE(delta[0].text.find("line three"), std::string::npos);
}

TEST(ReadOnlyLogTail, ReadTail_LastN)
{
    const fs::path dir = uniqueTempDir();
    fs::create_directories(dir);

    RDK::ApplicationLogReadPaths paths;
    paths.base_name = "TailN";
    paths.session_start_unix = std::time(nullptr) - 60;
    paths.directories.push_back(dir.string());

    RDK::UReadOnlyLogTail tail;
    tail.syncFromPaths(paths);
    for(int i = 1; i <= 5; ++i)
        writeGlogLine(dir, "TailN", "WARNING", "warn " + std::to_string(i));

    const auto lines = tail.readTail(2, RDK_EX_WARNING);
    ASSERT_EQ(lines.size(), 2u);
}

TEST(ReadOnlyLogTail, SeverityFilter)
{
    const fs::path dir = uniqueTempDir();
    fs::create_directories(dir);

    RDK::ApplicationLogReadPaths paths;
    paths.base_name = "SevFilter";
    paths.session_start_unix = std::time(nullptr) - 60;
    paths.directories.push_back(dir.string());

    RDK::UReadOnlyLogTail tail;
    tail.syncFromPaths(paths);
    writeGlogLine(dir, "SevFilter", "INFO", "info msg");
    writeGlogLine(dir, "SevFilter", "WARNING", "warn msg");

    const auto lines = tail.readTail(10, RDK_EX_WARNING);
    ASSERT_EQ(lines.size(), 1u);
    EXPECT_NE(lines[0].text.find("warn msg"), std::string::npos);
}
