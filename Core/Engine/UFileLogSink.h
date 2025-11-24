#ifndef UFILELOGSINK_H
#define UFILELOGSINK_H

#include <fstream>
#include <mutex>
#include <string>

#include "../../Deploy/Include/rdk_logging.h"

namespace RDK
{

class UFileLogSink: public Logging::ILogSink
{
public:
 static UFileLogSink& Instance();

 void Configure(const std::string& directory, const std::string& base_name);
 void Disable();
 bool IsEnabled() const;

 void Consume(const Logging::LogItem& item) override;
 void Flush() override;

private:
 UFileLogSink();
 ~UFileLogSink();

 void OpenStreamLocked();
 void CloseStreamLocked();
 std::string BuildFilePathUnlocked() const;
 std::string FormatMessage(const Logging::LogItem& item) const;
 std::string SeverityToString(int severity) const;

 mutable std::mutex SinkMutex;
 std::string TargetDirectory;
 std::string BaseName;
 bool Enabled;

 std::ofstream Stream;
};

}

#endif

