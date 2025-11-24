#ifndef UGLOGGUISINK_H
#define UGLOGGUISINK_H

#include <deque>
#include <mutex>
#include <string>
#include <vector>

#include "../../Deploy/Include/rdk_logging.h"

namespace RDK
{

struct UGlogGuiMessage
{
 int Severity;
 std::string Text;
};

class UGlogGuiSink: public Logging::ILogSink
{
public:
 static UGlogGuiSink& Instance();

 void Consume(const Logging::LogItem& item) override;

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

