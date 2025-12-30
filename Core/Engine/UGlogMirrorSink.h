#ifndef UGLOGMIRRORSINK_H
#define UGLOGMIRRORSINK_H

#include <fstream>
#include <map>
#include <mutex>
#include <string>

#include "../../Deploy/Include/rdk_logging.h"

namespace RDK
{

class UGlogMirrorSink : public Logging::ILogSink
{
public:
  static UGlogMirrorSink& Instance();

  void Configure(const std::string& directory,
                 const std::string& base_name,
                 std::time_t session_start);
  void Disable();

  void Consume(const Logging::LogItem& item) override;
  void Flush() override;

private:
  UGlogMirrorSink() = default;
  ~UGlogMirrorSink() override = default;
  UGlogMirrorSink(const UGlogMirrorSink&) = delete;
  UGlogMirrorSink& operator=(const UGlogMirrorSink&) = delete;

  struct FileHandle
  {
    std::ofstream Stream;
    std::string Path;
  };

  std::string BuildFileName(const std::string& level_tag) const;
  const char* SeverityTag(int severity) const;
  char SeverityLetter(int severity) const;
  std::string FormatLine(const Logging::LogItem& item) const;

  std::mutex Mutex;
  std::string TargetDirectory;
  std::string BaseName;
  std::string HostTag;
  std::string TimeTag;
  int ProcessId{0};
  bool Enabled{false};
  std::map<int, FileHandle> LevelFiles;
};

}

#endif

