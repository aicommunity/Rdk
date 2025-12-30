#ifndef RDK_LOGGING_H
#define RDK_LOGGING_H

#include "rdk_init.h"

#include <atomic>
#include <cstdint>
#include <ctime>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#ifndef RDK_USE_GLOG
#error "RDK logging now requires Google glog. Define RDK_USE_GLOG before including rdk_logging.h"
#endif

#include <glog/logging.h>
DECLARE_int32(minloglevel);
DECLARE_int32(v);

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

inline const char* ChannelTag(int channel_index)
{
  if(channel_index == RDK_SYS_MESSAGE)
    return kSystemChannel.Name;
  if(channel_index == RDK_GLOB_MESSAGE)
    return kGlobalChannel.Name;
  return nullptr;
}

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
  std::string label;
  if(channel.Index == kSystemChannel.Index)
  {
    label = "S";
  }
  else if(channel.Index == kGlobalChannel.Index)
  {
    label = "G";
  }
  else
  {
    label = std::to_string(channel.Index);
  }

  std::string prefix{"["};
  prefix.append(label);
  prefix.append("] ");
  return prefix;
}

inline void EmitToGlog(int msg_level, const std::string& text)
{
  // Если идет инициализация, downgrade FATAL до ERROR чтобы не вызывать abort()
  if (msg_level == RDK_EX_FATAL)
  {
   // Проверяем флаг режима инициализации через внешнюю C-функцию
   // Объявлена в rdk_init.h (включен через rdk_init.h в начале файла)
   if (RDK_IsInitializationMode())
   {
    msg_level = RDK_EX_ERROR;
   }
  }
  
  switch (msg_level)
  {
    case RDK_EX_FATAL:
      LOG(FATAL) << text;
      break;
    case RDK_EX_ERROR:
      LOG(ERROR) << text;
      break;
    case RDK_EX_WARNING:
      LOG(WARNING) << text;
      break;
    case RDK_EX_INFO:
      LOG(INFO) << text;
      break;
    case RDK_EX_DEBUG:
      VLOG(1) << text;
      break;
    case RDK_EX_APP:
      LOG(INFO) << "[APP] " << text;
      break;
    default:
      LOG(INFO) << text;
      break;
  }
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
  auto prefix = detail::BuildChannelPrefix(channel);
  std::string final_message = prefix + payload;
  detail::EmitToGlog(msg_level, final_message);
}

template <typename Builder>
inline void DispatchLazy(const ChannelDescriptor& channel, int msg_level, Builder&& builder)
{
  DispatchLazy(channel, msg_level, 0, std::forward<Builder>(builder));
}

template <typename Message>
inline void ChannelLog(int channel_index, int msg_level, Message&& message)
{
  const ChannelDescriptor descriptor{channel_index, ChannelTag(channel_index)};
  DispatchLazy(descriptor, msg_level, [&]() -> std::string {
    return detail::ToString(std::forward<Message>(message));
  });
}

template <typename Message>
inline void SystemLog(int msg_level, Message&& message)
{
  ChannelLog(kSystemChannel.Index, msg_level, std::forward<Message>(message));
}

template <typename Message>
inline void GlobalLog(int msg_level, Message&& message)
{
  ChannelLog(kGlobalChannel.Index, msg_level, std::forward<Message>(message));
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

#endif // RDK_LOGGING_H

