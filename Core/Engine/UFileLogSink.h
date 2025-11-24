#ifndef UFILELOGSINK_H
#define UFILELOGSINK_H

#include <fstream>
#include <mutex>
#include <string>

#ifdef RDK_USE_GLOG
#include <glog/logging.h>
#endif

namespace RDK
{

class UFileLogSink
#ifdef RDK_USE_GLOG
    : public google::LogSink
#endif
{
public:
 static UFileLogSink& Instance();

 void Configure(const std::string& directory, const std::string& base_name);
 void Disable();
 bool IsEnabled() const;

#ifdef RDK_USE_GLOG
 void send(google::LogSeverity severity,
           const char* full_filename,
           const char* base_filename,
           int line,
           const struct ::tm* tm_time,
           const char* message,
           size_t message_len) override;

 void WaitTillSent() override;
#endif

private:
 UFileLogSink();
 ~UFileLogSink();

#ifdef RDK_USE_GLOG
 void OpenStreamLocked();
 void CloseStreamLocked();
 std::string BuildFilePathUnlocked() const;
#endif

 mutable std::mutex SinkMutex;
 std::string TargetDirectory;
 std::string BaseName;
 bool Enabled;

#ifdef RDK_USE_GLOG
 std::ofstream Stream;
#endif
};

}

#endif

