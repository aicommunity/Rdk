#ifndef RDK_LOGGING_H
#define RDK_LOGGING_H

#include "rdk_init.h"

#include <atomic>
#include <cstdint>
#include <functional>
#include <ctime>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#ifdef RDK_USE_GLOG
#include <glog/logging.h>
DECLARE_int32(minloglevel);
DECLARE_int32(v);

// Map RDK log levels to glog levels
#define RDK_LOG_FATAL(msg) LOG(FATAL) << msg
#define RDK_LOG_ERROR(msg) LOG(ERROR) << msg
#define RDK_LOG_WARNING(msg) LOG(WARNING) << msg
#define RDK_LOG_INFO(msg) LOG(INFO) << msg
#define RDK_LOG_APP(msg) LOG(INFO) << "[APP] " << msg

// Debug logging with conditional compilation
// Note: DebugMode check should be done at call site if needed
#define RDK_LOG_DEBUG(msg) VLOG(1) << msg

// Helper macros for logging with object/method context
#define RDK_LOG_FATAL_EX(obj, method, msg) LOG(FATAL) << "[" << obj << "::" << method << "] " << msg
#define RDK_LOG_ERROR_EX(obj, method, msg) LOG(ERROR) << "[" << obj << "::" << method << "] " << msg
#define RDK_LOG_WARNING_EX(obj, method, msg) LOG(WARNING) << "[" << obj << "::" << method << "] " << msg
#define RDK_LOG_INFO_EX(obj, method, msg) LOG(INFO) << "[" << obj << "::" << method << "] " << msg
#define RDK_LOG_DEBUG_EX(obj, method, msg) VLOG(1) << "[" << obj << "::" << method << "] " << msg
#define RDK_LOG_APP_EX(obj, method, msg) LOG(INFO) << "[APP][" << obj << "::" << method << "] " << msg

// Map RDK exception types to glog levels
inline void RDK_LOG_BY_LEVEL(int msg_level, const std::string& msg) {
  switch (msg_level) {
    case RDK_EX_FATAL:
      LOG(FATAL) << msg;
      break;
    case RDK_EX_ERROR:
      LOG(ERROR) << msg;
      break;
    case RDK_EX_WARNING:
      LOG(WARNING) << msg;
      break;
    case RDK_EX_INFO:
      LOG(INFO) << msg;
      break;
    case RDK_EX_DEBUG:
      VLOG(1) << msg;
      break;
    case RDK_EX_APP:
      LOG(INFO) << "[APP] " << msg;
      break;
    default:
      LOG(INFO) << msg;
      break;
  }
}

inline void RDK_LOG_BY_LEVEL_EX(int msg_level, const std::string& object_name, const std::string& method_name, const std::string& msg) {
  std::string prefix = "[" + object_name + "::" + method_name + "] ";
  switch (msg_level) {
    case RDK_EX_FATAL:
      LOG(FATAL) << prefix << msg;
      break;
    case RDK_EX_ERROR:
      LOG(ERROR) << prefix << msg;
      break;
    case RDK_EX_WARNING:
      LOG(WARNING) << prefix << msg;
      break;
    case RDK_EX_INFO:
      LOG(INFO) << prefix << msg;
      break;
    case RDK_EX_DEBUG:
      VLOG(1) << prefix << msg;
      break;
    case RDK_EX_APP:
      LOG(INFO) << "[APP]" << prefix << msg;
      break;
    default:
      LOG(INFO) << prefix << msg;
      break;
  }
}

#else
// Fallback implementation when glog is not available
// These will be replaced with old logging system calls
#include <iostream>
#include <string>

#define RDK_LOG_FATAL(msg) std::cerr << "[FATAL] " << msg << std::endl
#define RDK_LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl
#define RDK_LOG_WARNING(msg) std::cerr << "[WARNING] " << msg << std::endl
#define RDK_LOG_INFO(msg) std::cout << "[INFO] " << msg << std::endl
#define RDK_LOG_DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#define RDK_LOG_APP(msg) std::cout << "[APP] " << msg << std::endl

#define RDK_LOG_FATAL_EX(obj, method, msg) std::cerr << "[FATAL][" << obj << "::" << method << "] " << msg << std::endl
#define RDK_LOG_ERROR_EX(obj, method, msg) std::cerr << "[ERROR][" << obj << "::" << method << "] " << msg << std::endl
#define RDK_LOG_WARNING_EX(obj, method, msg) std::cerr << "[WARNING][" << obj << "::" << method << "] " << msg << std::endl
#define RDK_LOG_INFO_EX(obj, method, msg) std::cout << "[INFO][" << obj << "::" << method << "] " << msg << std::endl
#define RDK_LOG_DEBUG_EX(obj, method, msg) std::cout << "[DEBUG][" << obj << "::" << method << "] " << msg << std::endl
#define RDK_LOG_APP_EX(obj, method, msg) std::cout << "[APP][" << obj << "::" << method << "] " << msg << std::endl

inline void RDK_LOG_BY_LEVEL(int msg_level, const std::string& msg) {
  switch (msg_level) {
    case RDK_EX_FATAL:
      std::cerr << "[FATAL] " << msg << std::endl;
      break;
    case RDK_EX_ERROR:
      std::cerr << "[ERROR] " << msg << std::endl;
      break;
    case RDK_EX_WARNING:
      std::cerr << "[WARNING] " << msg << std::endl;
      break;
    case RDK_EX_INFO:
      std::cout << "[INFO] " << msg << std::endl;
      break;
    case RDK_EX_DEBUG:
      std::cout << "[DEBUG] " << msg << std::endl;
      break;
    case RDK_EX_APP:
      std::cout << "[APP] " << msg << std::endl;
      break;
    default:
      std::cout << "[UNKNOWN] " << msg << std::endl;
      break;
  }
}

inline void RDK_LOG_BY_LEVEL_EX(int msg_level, const std::string& object_name, const std::string& method_name, const std::string& msg) {
  std::string prefix = "[" + object_name + "::" + method_name + "] ";
  switch (msg_level) {
    case RDK_EX_FATAL:
      std::cerr << "[FATAL]" << prefix << msg << std::endl;
      break;
    case RDK_EX_ERROR:
      std::cerr << "[ERROR]" << prefix << msg << std::endl;
      break;
    case RDK_EX_WARNING:
      std::cerr << "[WARNING]" << prefix << msg << std::endl;
      break;
    case RDK_EX_INFO:
      std::cout << "[INFO]" << prefix << msg << std::endl;
      break;
    case RDK_EX_DEBUG:
      std::cout << "[DEBUG]" << prefix << msg << std::endl;
      break;
    case RDK_EX_APP:
      std::cout << "[APP]" << prefix << msg << std::endl;
      break;
    default:
      std::cout << "[UNKNOWN]" << prefix << msg << std::endl;
      break;
  }
}

#endif // RDK_USE_GLOG

namespace RDK::Logging
{

struct ChannelDescriptor
{
  int Index;
  const char* Name;
};

struct ChannelRuntimeConfig
{
  int MinSeverity;
  int MaxVerbosity;
};

struct LogItem
{
  int Severity;
  int Verbosity;
  std::string BaseFilename;
  int Line;
  std::time_t Timestamp;
  std::string Message;
};

class ILogSink
{
public:
  virtual ~ILogSink() = default;
  virtual void Consume(const LogItem& item) = 0;
  virtual void Flush() {}
};

inline constexpr ChannelDescriptor kSystemChannel{RDK_SYS_MESSAGE, "sys"};
inline constexpr ChannelDescriptor kGlobalChannel{RDK_GLOB_MESSAGE, "glob"};
inline constexpr ChannelDescriptor kDefaultChannel{kSystemChannel.Index, kSystemChannel.Name};

namespace detail
{

inline std::string ToString(const std::string& value)
{
  return value;
}

inline std::string ToString(std::string&& value)
{
  return std::move(value);
}

inline std::string ToString(const char* value)
{
  return value ? std::string(value) : std::string();
}

template <typename T>
inline std::string ToString(T&& value)
{
  std::ostringstream stream;
  stream << std::forward<T>(value);
  return stream.str();
}

inline bool IsSeverityEnabled(int msg_level, int verbose_level)
{
#ifdef RDK_USE_GLOG
  switch (msg_level)
  {
    case RDK_EX_FATAL:
      return true;
    case RDK_EX_ERROR:
      return FLAGS_minloglevel <= google::GLOG_ERROR;
    case RDK_EX_WARNING:
      return FLAGS_minloglevel <= google::GLOG_WARNING;
    case RDK_EX_INFO:
    case RDK_EX_APP:
      return FLAGS_minloglevel <= google::GLOG_INFO;
    case RDK_EX_DEBUG:
    default:
    {
      const int required = (verbose_level <= 0) ? 1 : verbose_level;
      return FLAGS_v >= required;
    }
  }
#else
  (void)msg_level;
  (void)verbose_level;
  return true;
#endif
}

inline std::string BuildChannelPrefix(const ChannelDescriptor& channel)
{
  if(channel.Index == 0 && (!channel.Name || channel.Name[0] == '\0'))
    return {};

  std::string prefix{"["};
  if(channel.Name && channel.Name[0] != '\0')
  {
    prefix.append(channel.Name);
    if(channel.Index != 0)
      prefix.push_back('#');
  }
  if(channel.Index != 0)
  {
    prefix.append(std::to_string(channel.Index));
  }
  prefix.append("] ");
  return prefix;
}

} // namespace detail

void ResetChannelRuntimeConfig(const ChannelRuntimeConfig& default_config);
void SetChannelRuntimeConfig(int channel_index, const ChannelRuntimeConfig& config);
void RegisterLogSink(const std::shared_ptr<ILogSink>& sink);
void UnregisterLogSink(const std::shared_ptr<ILogSink>& sink);
bool ChannelFilterAllows(const ChannelDescriptor& channel, int msg_level, int verbose_level = 0);

template <typename Builder>
inline void DispatchLazy(const ChannelDescriptor& channel, int msg_level, int verbose_level, Builder&& builder)
{
  if(!detail::IsSeverityEnabled(msg_level, verbose_level))
    return;
  if(!ChannelFilterAllows(channel, msg_level, verbose_level))
    return;

  auto payload = detail::ToString(builder());
  if(channel.Index != 0 || (channel.Name && channel.Name[0] != '\0'))
  {
    auto prefix = detail::BuildChannelPrefix(channel);
    RDK_LOG_BY_LEVEL(msg_level, prefix + payload);
  }
  else
  {
    RDK_LOG_BY_LEVEL(msg_level, payload);
  }
}

template <typename Builder>
inline void DispatchLazy(const ChannelDescriptor& channel, int msg_level, Builder&& builder)
{
  DispatchLazy(channel, msg_level, 0, std::forward<Builder>(builder));
}

} // namespace RDK::Logging

#define RDK_CONCAT_IMPL(x, y) x##y
#define RDK_CONCAT(x, y) RDK_CONCAT_IMPL(x, y)
#define RDK_UNIQUE_NAME(base) RDK_CONCAT(base, __LINE__)

#define RLOG(level, channel_index, channel_tag, expr) \
  do { \
    const RDK::Logging::ChannelDescriptor RDK_UNIQUE_NAME(_rdk_channel_desc_){(channel_index), (channel_tag)}; \
    RDK::Logging::DispatchLazy(RDK_UNIQUE_NAME(_rdk_channel_desc_), (level), [&]() -> std::string { return (expr); }); \
  } while (0)

#define VRLOG(verbosity, channel_index, channel_tag, expr) \
  do { \
    const RDK::Logging::ChannelDescriptor RDK_UNIQUE_NAME(_rdk_channel_desc_v_){(channel_index), (channel_tag)}; \
    RDK::Logging::DispatchLazy(RDK_UNIQUE_NAME(_rdk_channel_desc_v_), RDK_EX_DEBUG, (verbosity), [&]() -> std::string { return (expr); }); \
  } while (0)

#define RLOG_IF(level, channel_index, channel_tag, condition, expr) \
  do { \
    if (condition) { \
      RLOG((level), (channel_index), (channel_tag), (expr)); \
    } \
  } while (0)

#define RLOG_COUNTED(level, channel_index, channel_tag, n, expr) \
  do { \
    auto& _rdk_counter = []() -> std::atomic<uint64_t>& { \
      static std::atomic<uint64_t> counter{0}; \
      return counter; \
    }(); \
    auto _rdk_value = ++_rdk_counter; \
    if (_rdk_value % (n) == 0) { \
      RLOG((level), (channel_index), (channel_tag), (expr)); \
    } \
  } while (0)

#define RDK_LOG_LAZY(level, expr) \
  RLOG((level), RDK::Logging::kSystemChannel.Index, RDK::Logging::kSystemChannel.Name, (expr))

#define RDK_LOG_IF(level, condition, expr) \
  RLOG_IF((level), RDK::Logging::kSystemChannel.Index, RDK::Logging::kSystemChannel.Name, (condition), (expr))

#define RDK_LOG_EVERY_N(level, n, expr) \
  RLOG_COUNTED((level), RDK::Logging::kSystemChannel.Index, RDK::Logging::kSystemChannel.Name, (n), (expr))

#define RDK_CHANNEL_LOG(level, channel_index, channel_name, expr) \
  RLOG((level), (channel_index), (channel_name), (expr))

#define RDK_CHANNEL_LOG_LAZY(level, channel_index, channel_name, expr) \
  RLOG((level), (channel_index), (channel_name), (expr))

#define RDK_VLOG_LAZY(verbosity, expr) \
  VRLOG((verbosity), RDK::Logging::kSystemChannel.Index, RDK::Logging::kSystemChannel.Name, (expr))

#define RDK_CHANNEL_VLOG_LAZY(verbosity, channel_index, channel_name, expr) \
  VRLOG((verbosity), (channel_index), (channel_name), (expr))


#endif // RDK_LOGGING_H

