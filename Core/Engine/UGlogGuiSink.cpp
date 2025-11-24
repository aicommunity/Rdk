#include "UGlogGuiSink.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace RDK
{

UGlogGuiSink& UGlogGuiSink::Instance()
{
 static UGlogGuiSink sink_instance;
 return sink_instance;
}

UGlogGuiSink::UGlogGuiSink()
{
 MaxMessages = 2000;
 TotalMessages = 0;
}

void UGlogGuiSink::Consume(const Logging::LogItem& item)
{
 std::string formatted = FormatMessage(item);
 PushMessage(item.Severity, formatted);
}

void UGlogGuiSink::PushMessage(int severity, const std::string& text)
{
 std::lock_guard<std::mutex> lock(QueueMutex);
 Messages.push_back({severity, text});
 if(Messages.size() > MaxMessages)
 {
  Messages.pop_front();
 }
 ++TotalMessages;
}

std::vector<UGlogGuiMessage> UGlogGuiSink::ReadMessages(std::size_t max_count)
{
 std::vector<UGlogGuiMessage> buffer;
 std::lock_guard<std::mutex> lock(QueueMutex);
 if(Messages.empty())
  return buffer;

 const std::size_t take_count = std::min(max_count, Messages.size());
 buffer.reserve(take_count);
 for(std::size_t i=0; i<take_count; ++i)
 {
  buffer.push_back(Messages.front());
  Messages.pop_front();
 }
 return buffer;
}

void UGlogGuiSink::Clear()
{
 std::lock_guard<std::mutex> lock(QueueMutex);
 Messages.clear();
}

void UGlogGuiSink::SetMaxMessages(std::size_t value)
{
 std::lock_guard<std::mutex> lock(QueueMutex);
 MaxMessages = value;
 if(Messages.size() > MaxMessages)
 {
  const std::size_t overflow = Messages.size() - MaxMessages;
  for(std::size_t i=0; i<overflow; ++i)
   Messages.pop_front();
 }
}

std::size_t UGlogGuiSink::PendingMessages() const
{
 std::lock_guard<std::mutex> lock(QueueMutex);
 return Messages.size();
}

std::size_t UGlogGuiSink::GetTotalMessages() const
{
 std::lock_guard<std::mutex> lock(QueueMutex);
 return TotalMessages;
}

std::string UGlogGuiSink::FormatTimestamp(std::time_t timestamp) const
{
 std::tm* time_struct = std::localtime(&timestamp);
 if(!time_struct)
  return std::string();

 std::ostringstream oss;
 oss << std::setfill('0')
     << std::setw(2) << time_struct->tm_mday << "/"
     << std::setw(2) << (time_struct->tm_mon + 1) << "/"
     << std::setw(4) << (time_struct->tm_year + 1900) << " "
     << std::setw(2) << time_struct->tm_hour << ":"
     << std::setw(2) << time_struct->tm_min << ":"
     << std::setw(2) << time_struct->tm_sec;
 return oss.str();
}

std::string UGlogGuiSink::SeverityToString(int severity) const
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

std::string UGlogGuiSink::FormatMessage(const Logging::LogItem& item) const
{
 std::string line = FormatTimestamp(item.Timestamp) + " [" + SeverityToString(item.Severity) + "] ";
 line += item.Message;
 return line;
}

}

