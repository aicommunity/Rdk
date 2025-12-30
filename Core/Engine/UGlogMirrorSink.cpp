#include "UGlogMirrorSink.h"

#include <chrono>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <thread>
#ifdef _WIN32
#include <winsock2.h>
#include <process.h>
#else
#include <unistd.h>
#endif

namespace RDK
{

namespace
{
std::string DetectHostTag()
{
  char buffer[256];
#ifdef _WIN32
  WSADATA wsaData;
  if(WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
  {
    if(gethostname(buffer, sizeof(buffer)) == 0)
    {
      WSACleanup();
      return std::string(buffer);
    }
    WSACleanup();
  }
#else
  if(gethostname(buffer, sizeof(buffer)) == 0)
    return std::string(buffer);
#endif
  return std::string("host");
}

std::string FormatSessionTag(std::time_t timestamp)
{
  std::tm tm_time{};
  if(std::tm* local = std::localtime(&timestamp))
    tm_time = *local;
  char buffer[32];
  std::snprintf(buffer, sizeof(buffer), "%04d%02d%02d-%02d%02d%02d",
                tm_time.tm_year + 1900,
                tm_time.tm_mon + 1,
                tm_time.tm_mday,
                tm_time.tm_hour,
                tm_time.tm_min,
                tm_time.tm_sec);
  return std::string(buffer);
}
}

UGlogMirrorSink& UGlogMirrorSink::Instance()
{
  static UGlogMirrorSink sink;
  return sink;
}

void UGlogMirrorSink::Configure(const std::string& directory,
                                const std::string& base_name,
                                std::time_t session_start)
{
  std::lock_guard<std::mutex> lock(Mutex);
  TargetDirectory = directory;
  BaseName = base_name.empty() ? std::string("rdk") : base_name;
  HostTag = DetectHostTag();
  TimeTag = FormatSessionTag(session_start);
#ifdef _WIN32
  ProcessId = static_cast<int>(_getpid());
#else
  ProcessId = static_cast<int>(::getpid());
#endif
  LevelFiles.clear();
  Enabled = !TargetDirectory.empty();
}

void UGlogMirrorSink::Disable()
{
  std::lock_guard<std::mutex> lock(Mutex);
  LevelFiles.clear();
  TargetDirectory.clear();
  Enabled = false;
}

void UGlogMirrorSink::Consume(const Logging::LogItem& item)
{
  std::lock_guard<std::mutex> lock(Mutex);
  if(!Enabled || TargetDirectory.empty())
    return;

  int severity = item.Severity;
  auto it = LevelFiles.find(severity);
  if(it == LevelFiles.end())
  {
    FileHandle handle;
    handle.Path = BuildFileName(SeverityTag(severity));
    handle.Stream.open(handle.Path, std::ios::out | std::ios::app);
    if(!handle.Stream.is_open())
    {
      severity = RDK_EX_INFO;
      it = LevelFiles.find(severity);
      if(it == LevelFiles.end())
      {
        handle.Path = BuildFileName(SeverityTag(severity));
        handle.Stream.open(handle.Path, std::ios::out | std::ios::app);
        if(!handle.Stream.is_open())
          return;
        it = LevelFiles.emplace(severity, std::move(handle)).first;
      }
    }
    else
    {
      it = LevelFiles.emplace(severity, std::move(handle)).first;
    }
  }
  std::string line = FormatLine(item);
  it->second.Stream << line << std::endl;
}

void UGlogMirrorSink::Flush()
{
  std::lock_guard<std::mutex> lock(Mutex);
  for(auto& kv : LevelFiles)
  {
    if(kv.second.Stream.is_open())
      kv.second.Stream.flush();
  }
}

std::string UGlogMirrorSink::BuildFileName(const std::string& level_tag) const
{
  std::string path = TargetDirectory;
  if(!path.empty() && path.back() != '/')
    path.push_back('/');
  path.append(BaseName);
  path.push_back('.');
  path.append(HostTag);
  path.append(".log.");
  path.append(level_tag);
  path.push_back('.');
  path.append(TimeTag);
  path.push_back('.');
  path.append(std::to_string(ProcessId));
  return path;
}

const char* UGlogMirrorSink::SeverityTag(int severity) const
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

char UGlogMirrorSink::SeverityLetter(int severity) const
{
  switch(severity)
  {
    case RDK_EX_FATAL: return 'F';
    case RDK_EX_ERROR: return 'E';
    case RDK_EX_WARNING: return 'W';
    case RDK_EX_INFO: return 'I';
    case RDK_EX_APP: return 'I';
    case RDK_EX_DEBUG: return 'D';
    default: return 'I';
  }
}

std::string UGlogMirrorSink::FormatLine(const Logging::LogItem& item) const
{
  std::ostringstream stream;
  char severity_letter = SeverityLetter(item.Severity);
  std::tm tm_time{};
  if(std::tm* local = std::localtime(&item.Timestamp))
    tm_time = *local;

  auto thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());

  stream << severity_letter
         << std::setfill('0') << std::setw(2) << tm_time.tm_mon + 1
         << std::setw(2) << tm_time.tm_mday << ' '
         << std::setw(2) << tm_time.tm_hour << ':'
         << std::setw(2) << tm_time.tm_min << ':'
         << std::setw(2) << tm_time.tm_sec << ".000000 "
         << thread_id << ' '
         << (item.BaseFilename.empty() ? "-" : item.BaseFilename.c_str())
         << ':' << item.Line << "] "
         << item.Message;
  return stream.str();
}

} // namespace RDK

