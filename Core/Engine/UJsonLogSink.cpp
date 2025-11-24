#include "UJsonLogSink.h"

#include <filesystem>
#include <iomanip>
#include <sstream>

namespace RDK
{

std::shared_ptr<UJsonLogSink> UJsonLogSink::Create(const std::string& file_path)
{
  auto sink = std::shared_ptr<UJsonLogSink>(new UJsonLogSink(file_path));
  if(!sink->OpenStream())
    return nullptr;
  return sink;
}

UJsonLogSink::UJsonLogSink(const std::string& file_path)
: TargetPath(file_path)
{
}

bool UJsonLogSink::OpenStream()
{
  if(TargetPath.empty())
    return false;
  std::error_code ec;
  std::filesystem::create_directories(std::filesystem::path(TargetPath).parent_path(), ec);
  Stream.open(TargetPath, std::ios::out | std::ios::app);
  return Stream.is_open();
}

void UJsonLogSink::Consume(const Logging::LogItem& item)
{
  std::lock_guard<std::mutex> lock(SinkMutex);
  if(!Stream.is_open() && !OpenStream())
    return;

  Stream << "{";
  Stream << "\"severity\":\"" << SeverityToString(item.Severity) << "\"";
  if(!item.BaseFilename.empty())
    Stream << ",\"file\":\"" << Escape(item.BaseFilename) << "\"";
  if(item.Line > 0)
    Stream << ",\"line\":" << item.Line;
  if(item.Timestamp != 0)
  {
    std::tm tm_value;
#ifdef _WIN32
    localtime_s(&tm_value, &item.Timestamp);
#else
    localtime_r(&item.Timestamp, &tm_value);
#endif
    std::ostringstream ts_stream;
    ts_stream << std::put_time(&tm_value, "%Y-%m-%dT%H:%M:%S");
    Stream << ",\"timestamp\":\"" << ts_stream.str() << "\"";
  }
  Stream << ",\"message\":\"" << Escape(item.Message) << "\"";
  Stream << "}" << std::endl;
}

void UJsonLogSink::Flush()
{
  std::lock_guard<std::mutex> lock(SinkMutex);
  if(Stream.is_open())
    Stream.flush();
}

std::string UJsonLogSink::Escape(const std::string& value) const
{
  std::string result;
  result.reserve(value.size());
  for(char ch : value)
  {
    switch(ch)
    {
      case '"': result += "\\\""; break;
      case '\\': result += "\\\\"; break;
      case '\b': result += "\\b"; break;
      case '\f': result += "\\f"; break;
      case '\n': result += "\\n"; break;
      case '\r': result += "\\r"; break;
      case '\t': result += "\\t"; break;
      default:
        if(static_cast<unsigned char>(ch) < 0x20)
        {
          std::ostringstream hex;
          hex << "\\u" << std::hex << std::setw(4) << std::setfill('0') << int(static_cast<unsigned char>(ch));
          result += hex.str();
        }
        else
        {
          result.push_back(ch);
        }
        break;
    }
  }
  return result;
}

std::string UJsonLogSink::SeverityToString(int severity) const
{
  switch(severity)
  {
    case RDK_EX_FATAL: return "fatal";
    case RDK_EX_ERROR: return "error";
    case RDK_EX_WARNING: return "warning";
    case RDK_EX_INFO: return "info";
    case RDK_EX_APP: return "app";
    case RDK_EX_DEBUG: return "debug";
    default: return "info";
  }
}

}


