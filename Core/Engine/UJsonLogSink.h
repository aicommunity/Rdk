#ifndef UJSONLOGSINK_H
#define UJSONLOGSINK_H

#include "../../Deploy/Include/rdk_logging.h"

#include <fstream>
#include <memory>
#include <mutex>
#include <string>

namespace RDK
{

class UJsonLogSink : public Logging::ILogSink
{
public:
  static std::shared_ptr<UJsonLogSink> Create(const std::string& file_path);

  void Consume(const Logging::LogItem& item) override;
  void Flush() override;

private:
  explicit UJsonLogSink(const std::string& file_path);

  bool OpenStream();
  std::string Escape(const std::string& value) const;
  std::string SeverityToString(int severity) const;

  std::mutex SinkMutex;
  std::ofstream Stream;
  std::string TargetPath;
};

}

#endif


