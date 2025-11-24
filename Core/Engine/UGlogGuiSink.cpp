#include "UGlogGuiSink.h"

#include <algorithm>

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
 PushMessage(item.Severity, item.Message);
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

}

