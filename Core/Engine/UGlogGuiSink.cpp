#include "UGlogGuiSink.h"

#include "UGlogFileTail.h"

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

void UGlogGuiSink::StartSession(const std::string& directory, const std::string& base_name, std::time_t session_start)
{
 std::lock_guard<std::mutex> lock(FileMutex);
 if(directory.empty())
 {
  FileTail.reset();
  return;
 }
 if(!FileTail)
  FileTail = std::make_unique<UGlogFileTail>();
 FileTail->Reset(directory, base_name, session_start);
}

void UGlogGuiSink::AddDirectory(const std::string& directory)
{
 std::lock_guard<std::mutex> lock(FileMutex);
 if(!FileTail || directory.empty())
  return;
 FileTail->AddDirectory(directory);
}

void UGlogGuiSink::PushMessage(int severity, const std::string& text) const
{
 std::lock_guard<std::mutex> lock(QueueMutex);
 Messages.push_back({severity, text});
 if(Messages.size() > MaxMessages)
 {
  Messages.pop_front();
 }
 ++TotalMessages;
}

std::vector<UGlogGuiMessage> UGlogGuiSink::ReadMessages(std::size_t max_count) const
{
 DrainFileMessages();
 std::vector<UGlogGuiMessage> buffer;
 {
  std::lock_guard<std::mutex> lock(QueueMutex);
  if(max_count == 0 || max_count >= Messages.size())
  {
   buffer.assign(Messages.begin(), Messages.end());
   Messages.clear();
  }
  else
  {
   buffer.reserve(max_count);
   auto it = Messages.begin();
   for(std::size_t i=0; i<max_count && it != Messages.end(); ++i, ++it)
    buffer.push_back(*it);
   Messages.erase(Messages.begin(), Messages.begin() + static_cast<long>(max_count));
  }
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

void UGlogGuiSink::DrainFileMessages() const
{
 std::unique_lock<std::mutex> lock(FileMutex);
 if(!FileTail)
  return;
 auto new_entries = FileTail->CollectNewMessages();
 lock.unlock();

 for(const auto& entry : new_entries)
 {
  std::string line = FormatTimestamp(entry.Timestamp);
  line.append(" [");
  line.append(SeverityToString(entry.Severity));
  line.append("] ");
  line.append(entry.Text);
  PushMessage(entry.Severity, line);
 }
}

}

