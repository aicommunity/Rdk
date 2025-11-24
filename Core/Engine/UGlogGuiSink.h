#ifndef UGLOGGUISINK_H
#define UGLOGGUISINK_H

#include <deque>
#include <mutex>
#include <string>
#include <vector>

#ifdef RDK_USE_GLOG
#include <glog/logging.h>
#endif

namespace RDK
{

struct UGlogGuiMessage
{
 int Severity;
 std::string Text;
};

class UGlogGuiSink
#ifdef RDK_USE_GLOG
    : public google::LogSink
#endif
{
public:
 static UGlogGuiSink& Instance();

#ifdef RDK_USE_GLOG
 void send(google::LogSeverity severity,
           const char* full_filename,
           const char* base_filename,
           int line,
           const struct ::tm* tm_time,
           const char* message,
           size_t message_len) override;
#endif

 std::vector<UGlogGuiMessage> ReadMessages(std::size_t max_count = 256);
 void Clear();
 void SetMaxMessages(std::size_t value);
 std::size_t PendingMessages() const;
 std::size_t GetTotalMessages() const;

private:
 UGlogGuiSink();
 UGlogGuiSink(const UGlogGuiSink&) = delete;
 UGlogGuiSink& operator=(const UGlogGuiSink&) = delete;

 void PushMessage(int severity, const std::string& text);

 mutable std::mutex QueueMutex;
 std::deque<UGlogGuiMessage> Messages;
 std::size_t MaxMessages;
 std::size_t TotalMessages;
};

}

#endif

