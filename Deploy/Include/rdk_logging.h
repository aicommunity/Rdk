#ifndef RDK_LOGGING_H
#define RDK_LOGGING_H

#include "rdk_init.h"

#ifdef RDK_USE_GLOG
#include <glog/logging.h>

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

#endif // RDK_LOGGING_H

