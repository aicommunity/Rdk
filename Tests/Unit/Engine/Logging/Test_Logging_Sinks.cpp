#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

#include "../../../../Core/Engine/UJsonLogSink.h"

using namespace RDK;

namespace
{

std::string ReadFile(const std::filesystem::path& path)
{
    std::ifstream stream(path);
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

}

TEST(LoggingSinks, JsonSinkWritesStructuredEntry)
{
    auto temp_dir = std::filesystem::temp_directory_path();
    auto log_path = temp_dir / "rdk_json_sink_test.log";
    std::filesystem::remove(log_path);

    auto sink = UJsonLogSink::Create(log_path.string());
    ASSERT_TRUE(sink) << "Failed to create JSON sink";

    Logging::LogItem item;
    item.Severity = RDK_EX_INFO;
    item.Verbosity = 0;
    item.BaseFilename = "TestLogging.cpp";
    item.Line = 123;
    item.Timestamp = std::time(nullptr);
    item.Message = "Test message";

    sink->Consume(item);
    sink->Flush();

    ASSERT_TRUE(std::filesystem::exists(log_path));
    auto content = ReadFile(log_path);
    EXPECT_NE(content.find("\"message\":\"Test message\""), std::string::npos);
    EXPECT_NE(content.find("\"file\":\"TestLogging.cpp\""), std::string::npos);

    // Smoke-test new macros compile and route without дополнительной настройки.
    RLOG(RDK_EX_INFO, RDK_SYS_MESSAGE, "sys", "JSON sink smoke log");
    VRLOG(1, RDK_SYS_MESSAGE, "sys", "Verbose smoke log");

    std::filesystem::remove(log_path);
}

