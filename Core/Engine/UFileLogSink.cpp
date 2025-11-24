#include "UFileLogSink.h"

#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

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
#ifdef RDK_USE_GLOG
 std::error_code ec;
 if(!TargetDirectory.empty())
  fs::create_directories(TargetDirectory, ec);
 CloseStreamLocked();
 OpenStreamLocked();
#endif
}

void UFileLogSink::Disable()
{
 std::lock_guard<std::mutex> lock(SinkMutex);
 Enabled=false;
#ifdef RDK_USE_GLOG
 CloseStreamLocked();
#endif
}

bool UFileLogSink::IsEnabled() const
{
 std::lock_guard<std::mutex> lock(SinkMutex);
 return Enabled;
}

#ifdef RDK_USE_GLOG
void UFileLogSink::send(google::LogSeverity severity,
                        const char* full_filename,
                        const char* base_filename,
                        int line,
                        const struct ::tm* tm_time,
                        const char* message,
                        size_t message_len)
{
 std::lock_guard<std::mutex> lock(SinkMutex);
 if(!Enabled)
  return;
 if(!Stream.is_open())
  return;

 std::string formatted = google::LogSink::ToString(severity,
                                                   base_filename,
                                                   line,
                                                   tm_time,
                                                   message,
                                                   message_len);
 Stream << formatted << std::endl;
}

void UFileLogSink::WaitTillSent()
{
 std::lock_guard<std::mutex> lock(SinkMutex);
 if(Stream.is_open())
  Stream.flush();
}

void UFileLogSink::OpenStreamLocked()
{
 if(!Enabled)
  return;
 std::string file_path = BuildFilePathUnlocked();
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
 char last = normalized.back();
 if(last != '/' && last != '\\')
  normalized.push_back('/');
 normalized += BaseName;
 normalized += ".work.log";
 return normalized;
}
#endif

}


