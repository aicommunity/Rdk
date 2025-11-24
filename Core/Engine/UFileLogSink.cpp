#include "UFileLogSink.h"

#include <filesystem>
#include <system_error>

namespace RDK
{

UFileLogSink& UFileLogSink::Instance()
{
 static UFileLogSink sink_instance;
 return sink_instance;
}

UFileLogSink::UFileLogSink()
: Enabled(false)
{
}

UFileLogSink::~UFileLogSink()
{
 Disable();
}

void UFileLogSink::Configure(const std::string& directory, const std::string& base_name)
{
 std::lock_guard<std::mutex> lock(SinkMutex);
 TargetDirectory = directory;
 BaseName = base_name.empty() ? std::string("rdk") : base_name;
 Enabled = true;
 std::error_code ec;
 if(!TargetDirectory.empty())
  std::filesystem::create_directories(TargetDirectory, ec);
 CloseStreamLocked();
 OpenStreamLocked();
}

void UFileLogSink::Disable()
{
 std::lock_guard<std::mutex> lock(SinkMutex);
 Enabled=false;
 CloseStreamLocked();
}

bool UFileLogSink::IsEnabled() const
{
 std::lock_guard<std::mutex> lock(SinkMutex);
 return Enabled;
}

void UFileLogSink::Consume(const Logging::LogItem& item)
{
 std::lock_guard<std::mutex> lock(SinkMutex);
 if(!Enabled)
  return;

 if(!Stream.is_open())
  OpenStreamLocked();
 if(!Stream.is_open())
  return;

 Stream << FormatMessage(item) << std::endl;
}

void UFileLogSink::Flush()
{
 std::lock_guard<std::mutex> lock(SinkMutex);
 if(Stream.is_open())
  Stream.flush();
}

void UFileLogSink::OpenStreamLocked()
{
 if(!Enabled)
  return;
 const std::string file_path = BuildFilePathUnlocked();
 if(file_path.empty())
  return;
 Stream.open(file_path, std::ios::out | std::ios::app);
}

void UFileLogSink::CloseStreamLocked()
{
 if(Stream.is_open())
  Stream.close();
}

std::string UFileLogSink::BuildFilePathUnlocked() const
{
 if(TargetDirectory.empty())
  return std::string();
 std::string normalized = TargetDirectory;
 if(!normalized.empty())
 {
  char last = normalized.back();
  if(last != '/' && last != '\\')
   normalized.push_back('/');
 }
 normalized += BaseName.empty() ? std::string("rdk") : BaseName;
 normalized += ".work.log";
 return normalized;
}

std::string UFileLogSink::SeverityToString(int severity) const
{
 switch(severity)
 {
  case RDK_EX_FATAL: return "FATAL";
  case RDK_EX_ERROR: return "ERROR";
  case RDK_EX_WARNING: return "WARNING";
  case RDK_EX_INFO: return "INFO";
  case RDK_EX_APP: return "APP";
  case RDK_EX_DEBUG: return "DEBUG";
  default: return "INFO";
 }
}

std::string UFileLogSink::FormatMessage(const Logging::LogItem& item) const
{
 std::string line = "[" + SeverityToString(item.Severity) + "]";
 if(!item.BaseFilename.empty())
  line += "[" + item.BaseFilename + ":" + std::to_string(item.Line) + "]";
 line += " ";
 line += item.Message;
 return line;
}

}


