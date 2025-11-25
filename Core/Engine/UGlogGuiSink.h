#ifndef UGLOGGUISINK_H
#define UGLOGGUISINK_H

#include <deque>
#include <memory>
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

class UGlogFileTail;

class UGlogGuiSink
{
public:
 static UGlogGuiSink& Instance();

 void StartSession(const std::string& directory, const std::string& base_name, std::time_t session_start);
 void AddDirectory(const std::string& directory);
 std::vector<UGlogGuiMessage> ReadMessages(std::size_t max_count = 256) const;
 void Clear();
 void SetMaxMessages(std::size_t value);
 std::size_t PendingMessages() const;
 std::size_t GetTotalMessages() const;

private:
 UGlogGuiSink();
 UGlogGuiSink(const UGlogGuiSink&) = delete;
 UGlogGuiSink& operator=(const UGlogGuiSink&) = delete;

 void DrainFileMessages() const;
 void PushMessage(int severity, const std::string& text) const;
 std::string FormatTimestamp(std::time_t timestamp) const;
 std::string SeverityToString(int severity) const;

 mutable std::mutex QueueMutex;
 mutable std::deque<UGlogGuiMessage> Messages;
 std::size_t MaxMessages;
 mutable std::size_t TotalMessages;

 mutable std::mutex FileMutex;
 mutable std::unique_ptr<UGlogFileTail> FileTail;
};

}

#endif

